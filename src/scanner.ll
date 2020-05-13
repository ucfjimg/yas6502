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
%{

#include "assembler.h"
#include "opcodes.h"

#include "parser.tab.hpp"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

using std::cout;
using std::endl;

using ss = std::stringstream;
using symtype = yy::parser::symbol_type;
using loctype = yy::parser::location_type;

const int DEC = 10;
const int HEX = 16;

void setInput(yas6502::Assembler &asmb);
#define YY_USER_INIT setInput(asmb)

namespace
{
    YY_BUFFER_STATE buffer_state;
}
%}

%option noyywrap nounput noinput batch debug caseless 

%{
symtype make_NUMBER(const std::string &s, int base, const loctype &loc);
symtype make_IdOrOpcode(const std::string &s, const yas6502::Assembler &asmb);
%}

id       [a-z_][a-z_0-9]+
int      [0-9]+
hexint   [0-9a-f]+
blank    [ \r\t]

%{
#define YY_USER_ACTION asmb.loc().columns(yyleng);
%}

%%

%{
  asmb.loc().step();
%}

{blank}+   asmb.loc().step();
\n+        asmb.loc().lines(yyleng); asmb.loc().step(); return yy::parser::make_NEWLINE(asmb.loc());

set        return yy::parser::make_SET(asmb.loc()); 
org        return yy::parser::make_ORG(asmb.loc()); 
byte       return yy::parser::make_BYTE(asmb.loc()); 
word       return yy::parser::make_WORD(asmb.loc()); 
"end"      return yy::parser::make_END(asmb.loc());
"="        return yy::parser::make_EQUALS(asmb.loc());
":"        return yy::parser::make_COLON(asmb.loc());
"#"        return yy::parser::make_HASH(asmb.loc());
",x"       return yy::parser::make_XINDEX(asmb.loc());
",y"       return yy::parser::make_YINDEX(asmb.loc());
","        return yy::parser::make_COMMA(asmb.loc());
"a"        return yy::parser::make_ACCUM(asmb.loc());
"("        return yy::parser::make_LPAREN(asmb.loc());
")"        return yy::parser::make_RPAREN(asmb.loc());
"+"        return yy::parser::make_PLUS(asmb.loc());
"-"        return yy::parser::make_MINUS(asmb.loc());
"*"        return yy::parser::make_TIMES(asmb.loc());
"/"        return yy::parser::make_DIVIDE(asmb.loc());


\${hexint} return make_NUMBER(yytext+1, HEX, asmb.loc());
{int}      return make_NUMBER(yytext, DEC, asmb.loc());
{id}       return make_IdOrOpcode(yytext, asmb);

;.*$       return yy::parser::make_COMMENT(yytext, asmb.loc()); 



.          {
    ss err{};
    err
        << "Invalid character(s) `"
        << yytext
        << "' in input.";
    throw yy::parser::syntax_error{
        asmb.loc(),
        err.str()
    };
}

<<EOF>> return yy::parser::make_YYEOF(asmb.loc());
%%

symtype make_NUMBER(const std::string &s, int base, const loctype &loc)
{
    // NB we don't need to check for end of string because the pattern will have
    // done that for us.
    //
    long n = strtol(s.c_str(), nullptr, base);
    return yy::parser::make_NUMBER((int)n, loc);
}

symtype make_IdOrOpcode(const std::string &s, const yas6502::Assembler &asmb)
{
    std::string upperS = s;
    std::transform(upperS.begin(), upperS.end(), upperS.begin(), ::toupper);

    if (asmb.isOpcode(upperS)) {
        return yy::parser::make_OPCODE(s, asmb.loc());
    }
    return yy::parser::make_IDENTIFIER(s, asmb.loc());
}

void setInput(yas6502::Assembler &asmb)
{
    char *src = asmb.source();
    int len = strlen(src);

    // TODO find the right place to clean this up.
    buffer_state = yy_scan_buffer(src, len+2);
}
