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
#ifndef PASS2_H_
#define PASS2_H_

#include "ast.h"
#include "pass.h"
#include "opcodes.h"

#include <array>
#include <memory>
#include <vector>

namespace yas6502
{
    class SymbolTable;

    class Pass2 : public Pass
    {
    public:
        Pass2(SymbolTable &symtab, const opcodes::OpcodeMap &opcodes);
        void pass2(std::vector<std::unique_ptr<ast::Node>> &ast);
        
        const std::array<int, 65536> &image() const;

        // Interface for use by AST nodes assembling themselves
        void emit(unsigned byte);
        int evalCheckDefined(ast::Expression &expr);
        void checkByte(int value);



    private:
        // the address space of a 16-bit processor is so small that
        // it makes sense to just keep an image of all of memory
        // rather than try to build individual OMF records as we
        // would if the assembler was self-hosted.
        //
        std::array<int, 65536> image_;
    };
}

#endif

