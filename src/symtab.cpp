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

#include "except.h"
#include "utility.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>

using std::map;
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
     * Clear all symbols
     */
    void SymbolTable::clear()
    {
        symbols_.clear();
    }

    /**
     * Look up a symbol. Always returns a value, which may not yet be defined.
     */
    Symbol SymbolTable::lookup(const string& name) const
    {
        auto it = symbols_.find(toUpper(name));
        if (it == symbols_.end()) {
            return Symbol{};
        }
        return it->second;
    }
    
    /**
     * Set the value of a symbol.
     */
    void SymbolTable::setValue(const std::string &name, int value)
    {
        string uname = toUpper(name);

        Symbol oldValue = symbols_[uname];
        if (oldValue.defined && oldValue.value != value) {
            ss err{};

            err
                << "Cannot redefine symbol `" 
                << uname
                << "'.";

            throw Error{ err.str() };
        }

        Symbol &sym = symbols_[uname];
        sym.defined = true;
        sym.value = value;
    }

    /**
     * Return an iterator to the start of the symbol table
     */ 
    SymbolTable::SymbolMapIter SymbolTable::begin() const
    {
        return symbols_.begin();
    }

    /**
     * Return an iterator to just past the end of the symbol
     * table
     */
    SymbolTable::SymbolMapIter SymbolTable::end() const
    {
        return symbols_.end();
    }
}
