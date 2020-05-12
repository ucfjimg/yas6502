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

#include "except.h"
#include "opcodes.h"
#include "pass1.h"
#include "pass2.h"
#include "symtab.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>

using std::cout;
using std::endl;
using std::make_unique;
using std::string;
using std::unique_ptr;
using std::vector;

using ss = std::stringstream;

extern FILE *yyin;
extern int yy_flex_debug;

namespace yas6502
{
    /**
     * Constructor
     */
    Assembler::Assembler()
        : trace_(false)
    {
        opcodes_ = makeOpcodeMap();
    }

    /**
     * Access to the location -- used by scanner.
     */
    yy::location &Assembler::loc()
    {
        return location_;
    }

    /**
     * const version of location
     */
    const yy::location &Assembler::loc() const
    {
        return location_;
    }

    /**
     * Return a pointer to an Opcode struct for the given mnemonic, or
     * nullptr if there is no instruction with that name.
     */
    const Opcode *Assembler::opcode(const string &op) const
    {
        auto it = opcodes_.find(op);
        if (it != opcodes_.end()) {
            return &it->second;
        }
        return nullptr;    
    }

    /**
     * Run the parser
     */
    void Assembler::parse()
    {
        location_.initialize(&file_);

        yy_flex_debug = trace_;

        yyin = fopen(file_.c_str(), "r");
        if (yyin == nullptr) {
            ss err{};
            err
                << "Could not open `" << file_ << "': " << strerror(errno) << std::endl;
            throw Error{ err.str() };
        }

        yy::parser parse(*this);
        parse.set_debug_level(trace_);
        parse();

        fclose(yyin);
        yyin = nullptr;
    }

    /**
     * Parse and assemble the given file.
     */
    void Assembler::assemble(const string &filename)
    {
        file_ = filename;
        program_.clear();

        parse();

        symtab_.clear();
        pass1_ = make_unique<Pass1>( symtab_, opcodes_ );
        pass2_ = make_unique<Pass2>( symtab_, opcodes_ );

        pass1_->pass1(program_);
        if (pass1_->errors() == 0) {
            pass2_->pass2(program_);
        }
    }
    
    /**
     * Return the number of errors.
     */
    int Assembler::errors() const
    {
        if (pass1_ == nullptr || pass2_ == nullptr) {
            return 0;
        }

        return pass1_->errors() + pass2_->errors();
    }

    /**
     * Return the number of warnings
     */
    int Assembler::warnings() const
    {
        if (pass1_ == nullptr || pass2_ == nullptr) {
            return 0;
        }

        return pass1_->warnings() + pass2_->warnings();
    }

    /**
     * Return all of the warnings and errors, sorted by line.
     */
    vector<Message> Assembler::messages() const
    {
        vector<Message> ret{};

        if (pass1_ != nullptr && pass2_ != nullptr) {
            std::copy(pass1_->messages().begin(), pass1_->messages().end(), std::back_inserter(ret));
            std::copy(pass2_->messages().begin(), pass2_->messages().end(), std::back_inserter(ret));
        }

        std::sort(
            ret.begin(),
            ret.end(),
            [&](const Message &left, const Message &right) {
                return left.line() < right.line();
            }
        );

        return ret;
    }

    /**
     * Called by the parser to set the program when parsing is done.
     */
    void Assembler::setProgram(vector<unique_ptr<ast::Node>> &&program)
    {
        program_ = std::move(program);
    }
}
