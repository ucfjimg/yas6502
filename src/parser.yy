/**
 * Copyright 2020 Jim Geist.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy 
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do 
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all 
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **/
%skeleton "lalr1.cc"
%require "3.6"
%defines

%define api.token.raw
%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
    #include <iomanip>
    #include <string>
    #include <vector>
    #include "ast.h"

    namespace yas6502 {
        class Assembler;
    }
}

%param{ yas6502::Assembler &asmb }

%locations

%define parse.trace
%define parse.error verbose
%define parse.lac full

%code {
#include "parser.h"
#include "ast.h"
using std::cerr;
using std::endl;

using std::make_unique;
using std::vector;

namespace ast = yas6502::ast;
using ast::Expression;
using ast::SymbolExpression;
using ast::ConstantExpression;
using ast::LocationExpression;
using ast::Operator;
using ast::BinaryOp;
using ast::UnaryOp;
using ast::IndexReg;
using ast::Address;
using ast::DataElement;
using ast::DataNode;
using ast::SpaceNode;
using ast::InstructionNode;
using ast::OrgNode;
using ast::SetNode;
using ast::NoopNode;
}

%define api.token.prefix {TOK_}

%token
  NEWLINE   "newline"
  EQUALS    "="
  COLON     ":"
  HASH      "#"
  SET       "set"
  ORG       "org"
  BYTE      "byte"
  WORD      "word"
  BYTES     "bytes"
  WORDS     "words"
  REP       "rep"
  END       "end"
  XINDEX    ",x"
  YINDEX    ",y"
  ACCUM     "a"
  LPAREN    "("
  RPAREN    ")"
  LBRACKET  "["
  RBRACKET  "]"
  PLUS      "+"
  MINUS     "-"
  TIMES     "*"
  DIVIDE    "/"
  MOD       "%"
  LSHIFT    "<<"
  RSHIFT    ">>"
  NEG       "~"
  AND       "&"
  XOR       "^"
  OR        "|"
  COMMA     ","
  DOT       "."
  ;

%token <std::string> OPCODE "opcode" 
%token <std::string> COMMENT "comment"
%token <std::string> IDENTIFIER "identifier"
%token <int> NUMBER "number"

%nterm <yas6502::ast::ExpressionPtr> expression
%nterm <yas6502::ast::ExpressionPtr> uexpr
%nterm <yas6502::ast::IndexReg> index
%nterm <yas6502::ast::IndexReg> yindex
%nterm <yas6502::ast::AddressPtr> addressing-mode
%nterm <std::vector<std::unique_ptr<yas6502::ast::DataElement>>> data-list;
%nterm <std::unique_ptr<yas6502::ast::DataElement>> data-element;
%nterm <yas6502::ast::DataSize> data-decl;
%nterm <yas6502::ast::DataSize> space-decl;
%nterm <std::unique_ptr<yas6502::ast::Node>> data-stmt;
%nterm <std::unique_ptr<yas6502::ast::Node>> space-stmt;
%nterm <std::unique_ptr<yas6502::ast::Node>> instr-stmt;
%nterm <std::unique_ptr<yas6502::ast::Node>> org-stmt;
%nterm <std::unique_ptr<yas6502::ast::Node>> set-stmt;
%nterm <std::unique_ptr<yas6502::ast::Node>> stmt;
%nterm <std::unique_ptr<yas6502::ast::Node>> line;
%nterm <std::vector<std::unique_ptr<yas6502::ast::Node>>> stmt-list;
%nterm <std::string> label;
%nterm <std::string> comment;

%left "|"
%left "^"
%left "&"
%left "<<" ">>"
%left "+" "-"
%left "*" "/" "%"
%left "~"

%%

%start program;

program: stmt-list { asmb.setProgram( std::move( $1 ) );  }

stmt-list:  
    line            { $$.push_back( std::move( $1 ) ); }
    | stmt-list line  { 
        $1.push_back( std::move( $2 ) );
        $$ = std::move( $1 );
    }

line: label stmt comment NEWLINE { 
    $$ = std::move( $2 ); 
    $$->setLine(@1.begin.line);
    $$->setLabel( $1 );
    $$->setComment( $3 );
}

stmt: 
    set-stmt     { $$ = std::move( $1 ); } 
    | org-stmt   { $$ = std::move( $1 ); }
    | end-stmt   { $$ = make_unique<NoopNode>(); } 
    | data-stmt  { $$ = std::move( $1 ); }
    | space-stmt { $$ = std::move( $1 ); }
    | instr-stmt { $$ = std::move( $1 ); } 
    | %empty     { $$ = make_unique<NoopNode>(); }

label: 
     %empty             {}
     | IDENTIFIER ":"   { $$ = $1; }

comment:
       %empty           {}
       | COMMENT        { $$ = $1; }

set-stmt: SET IDENTIFIER "=" expression { $$ = make_unique<SetNode>( $2, std::move( $4 ) ); }
org-stmt: ORG expression { $$ = make_unique<OrgNode>( std::move( $2 ) ); }
instr-stmt: OPCODE addressing-mode { $$ = make_unique<InstructionNode>( $1, std::move( $2 ) ); }

end-stmt: END 

data-stmt: data-decl data-list { $$ = make_unique<DataNode>( $1, std::move( $2 ) ); }
data-decl: 
    BYTE    { $$ = ast::DataSize::Byte; } 
    | WORD  { $$ = ast::DataSize::Word; }

data-element: 
    expression                          { $$ = make_unique<DataElement>(nullptr, std::move( $1 )); }
    | REP "(" expression ")" expression { $$ = make_unique<DataElement>(std::move( $3 ), std::move( $5 )); }

data-list: 
    data-element { $$.push_back( std::move( $1 ) ); }
    | data-list "," data-element { 
        $1.push_back(std::move( $3 ));
        $$ = std::move($1);
    }

space-stmt: space-decl expression { $$ = make_unique<SpaceNode>( $1, std::move( $2 ) ); }
space-decl:
    BYTES   { $$ = ast::DataSize::Byte; }
    | WORDS { $$ = ast::DataSize::Word; }

addressing-mode:
    %empty                        { $$ = make_unique<Address>( ast::AddrMode::Implied, nullptr ); }
    | "#" expression              { $$ = make_unique<Address>( ast::AddrMode::Immediate, std::move( $2 ) ); } 
    | "a"                         { $$ = make_unique<Address>( ast::AddrMode::Accumulator, nullptr ); }
    | expression index            { $$ = make_unique<Address>( ast::address( $2 ), std::move( $1 ) ); }
    | "[" expression "]" yindex   { $$ = make_unique<Address>( ast::indirect( $4 ), std::move( $2 ) ); }
    | "[" expression  ",x" "]"    { $$ = make_unique<Address>( ast::AddrMode::IndirectX, std::move( $2 ) ); }

index:
     %empty { $$ = ast::IndexReg::None; }
     | ",x" { $$ = ast::IndexReg::X; }
     | ",y" { $$ = ast::IndexReg::Y; }

yindex:
     %empty { $$ = ast::IndexReg::None; }
     | ",y" { $$ = ast::IndexReg::Y; }


uexpr:
    NUMBER                        { $$ = make_unique<ConstantExpression>( $1 ); }
    | IDENTIFIER                  { $$ = make_unique<SymbolExpression>( $1 ); }
    | "."                         { $$ = make_unique<LocationExpression>(); }
    | "-" uexpr                   { $$ = make_unique<UnaryOp>( Operator::Neg, std::move( $2 ) ); }
    | "~" uexpr                   { $$ = make_unique<UnaryOp>( Operator::BitNeg, std::move( $2 ) ); }

expression:
    uexpr                         { $$ = std::move( $1 ); }
    | expression "+" expression   { $$ = make_unique<BinaryOp>( Operator::Add, std::move( $1 ), std::move( $3 ) ); }
    | expression "-" expression   { $$ = make_unique<BinaryOp>( Operator::Sub, std::move( $1 ), std::move( $3 ) ); }
    | expression "*" expression   { $$ = make_unique<BinaryOp>( Operator::Mul, std::move( $1 ), std::move( $3 ) ); }
    | expression "/" expression   { $$ = make_unique<BinaryOp>( Operator::Div, std::move( $1 ), std::move( $3 ) ); }
    | expression "%" expression   { $$ = make_unique<BinaryOp>( Operator::Mod, std::move( $1 ), std::move( $3 ) ); }
    | expression "<<" expression  { $$ = make_unique<BinaryOp>( Operator::LShift, std::move( $1 ), std::move( $3 ) ); }
    | expression ">>" expression  { $$ = make_unique<BinaryOp>( Operator::RShift, std::move( $1 ), std::move( $3 ) ); }
    | expression "&" expression   { $$ = make_unique<BinaryOp>( Operator::And, std::move( $1 ), std::move( $3 ) ); }
    | expression "|" expression   { $$ = make_unique<BinaryOp>( Operator::Or, std::move( $1 ), std::move( $3 ) ); }
    | expression "^" expression   { $$ = make_unique<BinaryOp>( Operator::Xor, std::move( $1 ), std::move( $3 ) ); }
    | "(" expression ")"          { $$ = std::move( $2 ); $$->setParenthesized(); }

%%

void yy::parser::error(const location_type& l, const std::string& m)
{
  cerr << std::setw(5) << l.begin.line << ": Error: " << m << endl;
}

