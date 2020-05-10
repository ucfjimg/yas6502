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
    #include <string>
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
#include "assembler.h"

using std::cerr;
using std::endl;
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
  END       "end"
  XINDEX    ",x"
  YINDEX    ",y"
  ACCUM     "a"
  LPAREN    "("
  RPAREN    ")"
  PLUS      "+"
  MINUS     "-"
  TIMES     "*"
  DIVIDE    "/"
  COMMA     ","
  ;

%token <std::string> OPCODE "opcode" 
%token <std::string> COMMENT "comment"
%token <std::string> IDENTIFIER "identifier"
%token <int> NUMBER "number"

%left "+" "-"
%left "*" "/"

%printer { yyo << $$; } <*> ;

%%

%start program;
program:  
    line
    | program line

line: label stmt comment NEWLINE

stmt: set-stmt | org-stmt | end-stmt | data-stmt | instr-stmt | %empty

label: 
     %empty
     | IDENTIFIER ":"

comment:
       %empty 
       | COMMENT

set-stmt: SET IDENTIFIER "=" expression
org-stmt: ORG expression
data-stmt: data-decl data-list
instr-stmt: OPCODE addressing-mode
end-stmt: END 

data-decl: BYTE | WORD
data-list: expression | data-list "," expression

addressing-mode:
    %empty
    | "#" expression
    | "a"
    | expression index
    | "(" expression ")" index

index:
     %empty
     | ",x"
     | ",y"

expression:
    NUMBER
    | IDENTIFIER
    | expression "+" expression
    | expression "-" expression
    | expression "*" expression
    | expression "/" expression


%%

void yy::parser::error(const location_type& l, const std::string& m)
{
  cerr << l << ": " << m << endl;
}

