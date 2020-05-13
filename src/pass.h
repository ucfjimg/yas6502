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
#ifndef PASS_H_
#define PASS_H_

#include "except.h"
#include "opcodes.h"

#include <memory>
#include <string>
#include <vector>

namespace yas6502
{
    class SymbolTable;
    
    class Message 
    {
    public:
        Message();
        Message(bool warning, int line, const std::string &message);

        bool warning() const;
        int line() const;
        std::string message() const;

    private:
        bool warning_;
        int line_;
        std::string message_;
    };

    class Pass
    {
    public:
        Pass(SymbolTable &symtab, const opcodes::OpcodeMap &opcodes);
        virtual ~Pass();

        SymbolTable &symtab();
        const opcodes::Instruction &findInstruction(const std::string &op);

        int loc() const;
        void setLoc(int loc);

        void pushMessage(const Message &msg);

        int warnings() const;
        int errors() const;
        const std::vector<Message> &messages() const;

    protected:
        SymbolTable &symtab_;
        const opcodes::OpcodeMap &opcodes_;
        int loc_;
        int errors_;
        int warnings_;
        std::vector<Message> messages_;
    };
}
#endif

