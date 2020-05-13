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
#include "symtab.h"
#include "utility.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>

#include <unistd.h>

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::map;
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
    struct Symbol {
        string name;
        int value;
    };

    void usage();
    vector<char> readInputBuffer(const std::string &filename);
    void showErrors(Assembler &asmb);
    void writeObjectFile(const string &fn, const Image &image);
    void writeListingFile(const string &fn, const Assembler &asmb);
    void writeProgramLines(ofstream &out, const Assembler &asmb);
    void writeErrors(ofstream &out, const Assembler &asmb);
    void writeSymbolTable(ofstream &out, const Assembler &asmb);
    void writeSymbols(ofstream &out, const std::vector<Symbol>& symbols, int maxLen, int perLine);
}

int main(int argc, char *argv[])
{
    bool listing = false;
    string listingFile = "";
    string objectFile = "";
    int ch;

    while ((ch = getopt(argc, argv, "Ll:o:v")) != -1) {
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

        case 'v':
            cout 
                << "yas6502 version " 
                << YAS6502_VMAJOR 
                << "."
                << std::setfill('0') 
                << std::setw(2) << YAS6502_VMINOR
                << endl;
            return 0;

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
        vector<char> source = readInputBuffer(sourceFile);

        asmb.assemble(sourceFile, source);
        
        if (asmb.errors() || asmb.warnings()) {
            showErrors(asmb);
        }

        unlink(objectFile.c_str());
        if (asmb.errors() == 0) {
            writeObjectFile(objectFile, asmb.image());        
        }
        
        if (listing) {
            writeListingFile(listingFile, asmb);
        }

        if (asmb.errors()) {
            return 1;
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
    
    vector<char> readInputBuffer(const std::string &filename)
    {
        ifstream inp{ filename, std::ios::in | std::ios::binary };
        if (!inp) {
            ss err{};
            err
                << "Could not open source file `"
                << filename
                << "' for read.";
            throw yas6502::Error{ err.str() };
        }

        inp.seekg(0, inp.end);
        auto size = inp.tellg();
        inp.seekg(0);

        vector<char> source{};
        source.resize(size);
        inp.read(source.data(), size);
        if (!inp) {
            ss err{};
            err
                << "Failed to read entire input file `"
                << filename
                << "'.";
            throw yas6502::Error{ err.str() };
        }
        
        return source;
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

    void writeListingFile(const string &fn, const Assembler &asmb)
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

        writeProgramLines(out, asmb);
        writeErrors(out, asmb);
        writeSymbolTable(out, asmb);
    }

    /**
     * Write out annotated program lines
     */
    void writeProgramLines(ofstream &out, const Assembler &asmb)
    {
        const vector<unique_ptr<ast::Node>> &program{ asmb.program() };
        const Image &image{ asmb.image() };

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

    /**
     * Write out warnings and errors, if there are any
     */
    void writeErrors(ofstream &out, const Assembler &asmb)
    {
        if (!asmb.messages().empty()) {
            out << endl << "Errors and Warnings" << endl;

            for (const auto &msg : asmb.messages()) {
                out
                    << std::setw(5) << msg.line() << "  "
                    << (msg.warning() ? "Warning" : "Error  ")
                    << "  "
                    << msg.message()
                    << endl;
            }
        }
    }

    /**
     * Write out the symbol table
     */
    void writeSymbolTable(ofstream &out, const Assembler &asmb)
    {

        vector<Symbol> symbols{};

        string::size_type maxLen = 0;
        for (const auto &ent : asmb.symtab()) {
            maxLen = std::max(maxLen, ent.first.length());
            symbols.push_back(Symbol{ ent.first, ent.second.value });
        }

        const int COLUMNS = 132; 
        int perLine = COLUMNS / (maxLen + 8);
        perLine = std::max(1, perLine);

        out << endl << "Symbol table by name" << endl << endl;
        writeSymbols(out, symbols, maxLen, perLine);

        // now sort by value
        std::sort(symbols.begin(), symbols.end(), [](const Symbol &left, const Symbol &right) {
            return left.value < right.value;
        });

        out << endl << endl << "Symbol table by value" << endl << endl;
        writeSymbols(out, symbols, maxLen, perLine);
    }

    void writeSymbols(ofstream &out, const std::vector<Symbol>& symbols, int maxLen, int perLine)
    {
        int col = 0;

        out << std::uppercase;
        for (const Symbol &sym : symbols) {
            out 
                << std::setfill(' ') << std::setw(maxLen) << sym.name
                << " $"
                << std::setfill('0') << std::hex << std::setw(4) << sym.value;

            ++col;
            if (col == perLine) {
                out << endl;
                col = 0;
            } else {
                out << "  ";
            }
        }

        if (col) {
            out << endl;
        }
    }
}
