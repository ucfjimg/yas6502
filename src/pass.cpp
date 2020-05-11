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
#include "pass.h"

#include "utility.h"

#include <stdexcept>
#include <string>

using std::make_unique;
using std::runtime_error;
using std::string;
using std::unique_ptr;

namespace yas6502
{
    /**
     * Constructor
     */
    Pass::Pass(SymbolTable &symtab, const OpcodeMap &opcodes)
        : symtab_(symtab)
        , opcodes_(opcodes)
        , loc_(0)
    {
    }

    /**
     * Destructor
     */
    Pass::~Pass()
    {
    }

    /**
     * Return the current location counter.
     */
    int Pass::loc() const
    {
        return loc_;
    }

    /**
     * Sets the current location counter.
     */
    void Pass::setLoc(int loc)
    {
        if (loc > 0xFFFF) {
            throw runtime_error{ "Location counter cannot exceed $FFFF." };
        } else if (loc < 0) {
            throw runtime_error{ "Location counter cannot be negative." };
        }

        loc_ = loc;
    }

    /**
     * Look up an opcode in the opcode map. Case insensitive.
     * Returns nullptr if there is no such opcode.
     */
    unique_ptr<Opcode> Pass::findOpcode(const string &op)
    {
        auto it = opcodes_.find(toUpper(op));
        if (it == opcodes_.end()) {
            return nullptr;
        }

        return make_unique<Opcode>( it->second ); 
    }

    /**
     * Return the symbol table.
     */
    SymbolTable &Pass::symtab()
    {
        return symtab_;
    }
}
