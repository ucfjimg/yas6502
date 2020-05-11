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
#include "symtab.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>

using std::map;
using std::runtime_error;
using std::string;

using ss = std::stringstream;

namespace yas6502
{
    /**
     * Symbol constructor
     */
    Symbol::Symbol()
        : defined(false)
        , value(1)
    {
    }

    /**
     * Look up a symbol. Always returns a value, which may not yet be defined.
     */
    Symbol SymbolTable::lookup(const string& name)
    {
        string uname = name;
        std::transform(uname.begin(), uname.end(), uname.begin(), toupper);

        return symbols_[name];
    }
    
    /**
     * Set the value of a symbol.
     */
    void SymbolTable::setValue(const std::string &name, int value)
    {
        string uname = name;
        std::transform(uname.begin(), uname.end(), uname.begin(), toupper);

        Symbol oldValue = symbols_[name];
        if (oldValue.defined && oldValue.value != value) {
            ss err{};

            err
                << "Cannot redefine symbol `" 
                << name
                << "'.";

            throw runtime_error{ err.str() };
        }

        symbols_[name].value = value;
    }
}
