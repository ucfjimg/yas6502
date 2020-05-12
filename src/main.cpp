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

#include "ast.h"
#include "except.h"
#include "utility.h"

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <unistd.h>

using std::cerr;
using std::endl;
using std::ofstream;
using std::string;
using std::unique_ptr;
using std::vector;

using ss = std::stringstream;

using yas6502::Assembler;
using yas6502::Image;
using yas6502::Message;

namespace ast = yas6502::ast;

namespace
{
    void usage();
    void showErrors(Assembler &asmb);
    void writeObjectFile(const string &fn, const Image &image);
    void writeListingFile(const string &fn, const vector<unique_ptr<ast::Node>> &program, const Image &image);
}

int main(int argc, char *argv[])
{
    bool listing = false;
    string listingFile = "";
    string objectFile = "";
    int ch;

    while ((ch = getopt(argc, argv, "Ll:o:")) != -1) {
        switch (ch) {
        case 'L':
            listing = true;
            break;

        case 'l':
            listing = true;
            listingFile = string{ optarg };
            break;

        case 'o':
            objectFile = string{ optarg };
            break;

        default:
            usage();
        }      
    }

    if (optind >= argc) {
        usage();
    }

    string sourceFile{ argv[optind] };
    if (listing && listingFile.empty()) {
        listingFile = yas6502::replaceOrAppendExtension(sourceFile, "lst");
    }

    if (objectFile.empty()) {
        objectFile = yas6502::replaceOrAppendExtension(sourceFile, "o");
    }

    Assembler asmb{};

    try {
        asmb.assemble(sourceFile);
        
        if (asmb.errors() || asmb.warnings()) {
            showErrors(asmb);

            if (asmb.errors()) {
                return 1;
            }
        }

        writeObjectFile(objectFile, asmb.image());        
        if (listing) {
            writeListingFile(listingFile, asmb.program(), asmb.image());
        }
    } catch (yas6502::Error &ex) {
        cerr << ex.message() << endl;
    }
}

namespace 
{
    /**
     * Print usage and exit
     */
    void usage()
    {
        cerr
            << "yas6502: [-L] [-l listing-file] [-o object-file] source-file"
            << endl;
        exit(1);
    }

    /**
     * Print errors to stderr
     */
    void showErrors(Assembler &asmb)
    {
        for (Message message : asmb.messages()) {
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
    void writeObjectFile(const string &fn, const Image &image)
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

    void writeListingFile(const string &fn, const vector<unique_ptr<ast::Node>> &program, const Image &image)
    {
        ofstream out{ fn };
        if (!out) {
            ss err{};
            err
                << "Could not open listing file `"
                << fn
                << "' for write.";
            throw yas6502::Error{ err.str() };
        }
        
        int last = 0;
        for (const auto &stmt : program) {
            // The assembler doesn't save blank lines with an empty AST node,
            // so put them back in for proper listing format.
            //
            for (; last < stmt->line() - 1; last++) {
                out << std::setw(5) << last << endl;
            }
            for (string line : stmt->str(image)) {
                out << line << endl;
            }
            last = stmt->line();
        }
    }
}
