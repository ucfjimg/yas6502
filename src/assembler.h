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
#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_

#include "ast.h"
#include "pass1.h"
#include "pass2.h"
#include "opcodes.h"
#include "symtab.h"

#include <string>
#include <map>
#include <memory>
#include <vector>

#include "parser.tab.hpp"

namespace yas6502
{
    class Assembler 
    {
    public:
        Assembler();

        void setTrace();
        void assemble(const std::string &file);

        int errors() const;
        int warnings() const; 
        std::vector<Message> messages() const;
        const Image &image() const;
        const std::vector<std::unique_ptr<ast::Node>> &program() const;

        void setProgram(std::vector<std::unique_ptr<ast::Node>> &&program);

        yy::location &loc();
        const yy::location &loc() const;

        const Opcode *opcode(const std::string &op) const;

    private:
        OpcodeMap opcodes_;

        yy::location location_;
        std::string file_;
        bool trace_;

        SymbolTable symtab_;
        std::unique_ptr<Pass1> pass1_;
        std::unique_ptr<Pass2> pass2_;

        std::vector<std::unique_ptr<ast::Node>> program_;

        void parse();
    };
}

# define YY_DECL yy::parser::symbol_type yylex(yas6502::Assembler &asmb)

YY_DECL;

#endif

