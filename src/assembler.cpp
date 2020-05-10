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
#include "assembler.h"

#include "opcodes.h"

#include <cstring>
#include <sstream>
#include <stdexcept>

using std::runtime_error;

using ss = std::stringstream;

extern FILE *yyin;
extern int yy_flex_debug;

namespace yas6502
{
    Assembler::Assembler()
        : trace_(false)
    {
        opcodes_ = makeOpcodeMap();
    }

    yy::location &Assembler::loc()
    {
        return location_;
    }

    const yy::location &Assembler::loc() const
    {
        return location_;
    }

    const Opcode *Assembler::opcode(const std::string &op) const
    {
        // TODO strupr
        auto it = opcodes_.find(op);
        if (it != opcodes_.end()) {
            return &it->second;
        }
        return nullptr;    
    }

    void Assembler::assemble(const std::string &filename)
    {
        file_ = filename;
        location_.initialize(&file_);

        yy_flex_debug = trace_;

        yyin = fopen(file_.c_str(), "r");
        if (yyin == nullptr) {
            ss err{};
            err
                << "Could not open `" << file_ << "': " << strerror(errno) << std::endl;
            throw runtime_error{ err.str() };
        }

        yy::parser parse(*this);
        parse.set_debug_level(trace_);
        parse();

        fclose(yyin);
        yyin = nullptr;
    }
}
