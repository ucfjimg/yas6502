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
#include "utility.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

using std::cerr;
using std::endl;
using std::ofstream;
using std::string;

using ss = std::stringstream;

namespace
{
    void showErrors(yas6502::Assembler &asmb);
    void writeObjectFile(const std::string &fn, const yas6502::Image &image);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        cerr << "yas6502: filename" << endl;
        return 1;
    }
    yas6502::Assembler asmb{};

    try {
        string sourceFile{ argv[1] };
        asmb.assemble(sourceFile);
        
        if (asmb.errors() || asmb.warnings()) {
            showErrors(asmb);

            if (asmb.errors()) {
                return 1;
            }
        }

        writeObjectFile(yas6502::replaceOrAppendExtension(sourceFile, "o"), asmb.image());        
    } catch (yas6502::Error &ex) {
        cerr << ex.message() << endl;
    }
}

namespace 
{
    /**
     * Print errors to stderr
     */
    void showErrors(yas6502::Assembler &asmb)
    {
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
    }

    /**
     * Write a simple object file format.
     */
    void writeObjectFile(const std::string &fn, const yas6502::Image &image)
    {
        ofstream out{ fn };
        if (!out) {
            ss err{};
            err
                << "Could not open object file `"
                << fn
                << "' for write.";
            throw yas6502::Error{ err.str() };
        }
        
        int last = -1;
        int col = 0;
        
        out 
            << std::hex 
            << std::setfill('0')
            << std::uppercase;

        for (int addr = 0; addr < 0x10000; addr++) {
            if (image[addr] == -1) {
                continue;
            }

            if (addr != last + 1) {
                if (col != 0) {
                    out << endl;
                }
                out << '@' << std::setw(4) << addr << endl;
            } 

            out << std::setw(2) << (int)image[addr];
            col++;
            if (col < 16) {
                out << " ";
            } else {
                col = 0;
                out << endl;
            }

            last = addr;
        }
    }
}
