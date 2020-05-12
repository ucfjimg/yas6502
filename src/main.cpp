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

#include <iomanip>
#include <iostream>

using std::cerr;
using std::endl;
using std::string;

int main(int argc, char *argv[])
{
    if (argc != 2) {
        cerr << "yas6502: filename" << endl;
        return 1;
    }
    yas6502::Assembler asmb{};

    try {
        asmb.assemble(string{ argv[1] });
        
        if (asmb.errors() || asmb.warnings()) {
            for (yas6502::Message message : asmb.messages()) {
                cerr
                    << std::setw(5) << message.line() << ": "
                    << (message.warning() ? "Warning" : "Error")
                    << ": "
                    << message.message()
                    << endl;
            }

            cerr
                << asmb.errors() << " error(s), "
                << asmb.warnings() << " warning(s)."
                << endl;

            if (asmb.errors()) {
                return 1;
            }
        }


    } catch (yas6502::Error &ex) {
        cerr << ex.message() << endl;
    }
}
