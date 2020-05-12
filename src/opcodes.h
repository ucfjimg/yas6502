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
#ifndef OPCODES_H_
#define OPCODES_H_

#include <map>
#include <string>

namespace yas6502
{
    namespace opcodes
    {
        class Encoding
        {
        public:
            Encoding();
            Encoding(unsigned opcode);
            Encoding &setUndocumented();
            Encoding &setUnstable();
            Encoding &setClocks(int clocks);
            Encoding &setExtraClocks();

            bool undocumented() const;
            bool unstable() const;
            int clocks() const;
            bool extraClocks() const;

        private:
            unsigned opcode_;
            unsigned clocks_;
            bool undocumented_;
            bool unstable_;
            bool extraClocks_;
        };

        enum class AddrMode
        {
            Accumulator,
            Immediate,
            Implied,
            ZeroPageX,
            ZeroPageY,
            Absolute,
            AbsoluteX,
            AbsuloteY,
            Indirect,
            IndirectX,
            IndirectY,
            Relative,
        };

        class Instruction
        {
        public:
            Instruction &addEncoding(AddrMode mode, Encoding &&encoding);

        private:
            std::map<AddrMode, Encoding> encodings_;
        };


        struct Opcode
        {
            bool undocumented;
            bool unstable;

            int accumulator;
            int immediate;
            int implied;
            int zeroPage;
            int zeroPageX;
            int zeroPageY;
            int absolute;
            int absoluteX;
            int absoluteY;
            int indirect;
            int indirectX;
            int indirectY;
            int relative;

            Opcode();
        };

        using OpcodeMap = std::map<std::string, Opcode>;

        extern OpcodeMap makeOpcodeMap();
    }
}

#endif


