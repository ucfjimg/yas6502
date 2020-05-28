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

#include "except.h"
#include "utility.h"

#include <sstream>
#include <stdexcept>
#include <string>

using std::make_unique;
using std::string;
using std::unique_ptr;
using std::vector;

using ss = std::stringstream;

namespace yas6502
{
    /**
     * Constructor
     */
    Message::Message()
        : warning_(false)
        , line_(0)
    {
    }

    /**
     * Constructor
     */
    Message::Message(bool warning, int line, const std::string &message)
        : warning_(warning)
        , line_(line)
        , message_(message)
    {
    }

    /**
     * Return true if the message is a warning
     */
    bool Message::warning() const
    {
        return warning_;
    }

    /**
     * Return the line number the message applies to
     */
    int Message::line() const
    {
        return line_;
    }

    /**
     * Return the text of the message
     */
    string Message::message() const
    {
        return message_;
    }

    /**
     * Constructor
     */
    Pass::Pass(SymbolTable &symtab, const opcodes::OpcodeMap &opcodes)
        : symtab_(symtab)
        , opcodes_(opcodes)
        , loc_(0)
        , errors_(0)
        , warnings_(0)
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
        // NB we have to allow the location counter to go one past the
        // end of memory, as it's the position of the *next* byte to
        // emit.
        if (loc > 0xFFFF + 1) {
            throw Error{ "Location counter cannot exceed $FFFF." };
        } else if (loc < 0) {
            throw Error{ "Location counter cannot be negative." };
        }

        loc_ = loc;
    }

    /**
     * Push a warning or error message ont the error list.
     */
    void Pass::pushMessage(const Message &msg)
    {
        if (msg.warning()) {
            warnings_++;
        } else {
            errors_++;
        }

        messages_.push_back(msg);
    }

    /**
     * Look up an opcode in the opcode map. Case insensitive.
     * Returns nullptr if there is no such opcode.
     */
    const opcodes::Instruction &Pass::findInstruction(const string &op)
    {
        auto it = opcodes_.find(toUpper(op));
        if (it == opcodes_.end()) {
            ss err{};
            err
                << "Unknown opcode `" << op << "'.";
            throw Error{ err.str() };
        }

        return it->second;
    }

    /**
     * Return the symbol table.
     */
    SymbolTable &Pass::symtab()
    {
        return symtab_;
    }

    /** 
     * Return the warning count
     */
    int Pass::warnings() const
    {
        return warnings_;
    }

    /**
     * Return the error count
     */
    int Pass::errors() const
    {
        return errors_;
    }

    /**
     * Return the assembly messages from this pass
     */
    const vector<Message> &Pass::messages() const
    {
        return messages_;
    }
}
