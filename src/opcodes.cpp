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
#include "opcodes.h"

#include <cassert>

using std::string;

namespace yas6502
{
    namespace opcodes
    {
        /** 
         * Default construct an instruction encoding
         */ 
        Encoding::Encoding()
            : exists_(false)
            , opcode_(0)
            , clocks_(0)
            , undocumented_(false)
            , unstable_(false)
            , extraClocks_(false)
        {
        }

        /** 
         * Construct an instruction encoding
         */ 
        Encoding::Encoding(unsigned opcode)
            : exists_(true)
            , opcode_(opcode)
            , clocks_(0)
            , undocumented_(false)
            , unstable_(false)
            , extraClocks_(false)
        {
        }


        /**
         * Mark an encoding as undocumented
         */
        Encoding &Encoding::setUndocumented()
        {
            undocumented_ = true;
            return *this;
        }

        /**
         * Mark an encoding as unstablke
         */
        Encoding &Encoding::setUnstable()
        {
            unstable_ = true;
            return *this;
        }

        /**
         * Set the base number of clocks (the instruction may take more due
         * to branching or page crossings, if so, extra clocks will be set) 
         */
        Encoding &Encoding::setClocks(int clocks)
        {
            clocks_ = clocks;
            return *this;
        }

        /**
         * Mark an encoding as undocumented
         */
        Encoding &Encoding::setExtraClocks()
        {
            extraClocks_ = true;
            return *this;
        }

        /**
         * True if this encoding exists. False if this is a marker
         * for a non-existent encoding in the opcode map.
         */
        bool Encoding::exists() const
        {
            return exists_;
        }

        /**
         * Return the one-byte opcode for this encoding
         */
        unsigned Encoding::opcode() const
        {
            return opcode_;
        }

        /**
         * Default constructor
         */
        Instruction::Instruction()
        {
        }

        /**
         * Constructor
         */
        Instruction::Instruction(const string& mnemonic)
            : mnemonic_(mnemonic)
        {
        }

        /**
         * Add the encoding for an addressing mode to an instruction.
         */
        Instruction &Instruction::addEncoding(AddrMode mode, const Encoding &encoding)
        {
            encodings_[mode] = encoding;
            return *this;
        }

        /**
         * Return the instruction's mnemonic
         */ 
        string Instruction::mnemonic() const
        {
            return mnemonic_;
        }

        /** 
         * Test if the instruction has an encoding
         */
        bool Instruction::hasEncoding(AddrMode mode) const
        {
            return encodings_.find(mode) != encodings_.end();
        }

        /**
         * Return a mode encoding for the instruction
         */
        const Encoding &Instruction::encoding(AddrMode mode) const 
        {
            auto it = encodings_.find(mode);
            if (it != encodings_.end()) {
                return it->second;
            }

            return nullEncoding_;
        }
        
        OpcodeMap makeOpcodeMap()
        {
            OpcodeMap opcodes{};

            Instruction ADC("ADC");
            ADC
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0x69 }
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x65 }
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0x75 }
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x6D }
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0x7D }
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0x79 }
            )
            .addEncoding(AddrMode::IndirectX,
                Encoding{ 0x61 }
            )
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0x71 }
            );
            opcodes["ADC"] = ADC;

            Instruction AND("AND");
            AND
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0x29 }
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x25 }
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0x35 }
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x2D }
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0x3D }
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0x39 }
            )
            .addEncoding(AddrMode::IndirectX,
                Encoding{ 0x21 }
            )
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0x31 }
            );
            opcodes["AND"] = AND;

            Instruction ASL("ASL");
            ASL
            .addEncoding(AddrMode::Accumulator,
                Encoding{ 0x0A }
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x06 }
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0x16 }
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x0E }
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0x1E }
            );
            opcodes["ASL"] = ASL;

            Instruction BCC("BCC");
            BCC
            .addEncoding(AddrMode::Relative,
                Encoding{ 0x90 }
            );
            opcodes["BCC"] = BCC;

            Instruction BCS("BCS");
            BCS
            .addEncoding(AddrMode::Relative,
                Encoding{ 0xB0 }
            );
            opcodes["BCS"] = BCS;

            Instruction BEQ("BEQ");
            BEQ
            .addEncoding(AddrMode::Relative,
                Encoding{ 0xF0 }
            );
            opcodes["BEQ"] = BEQ;

            Instruction BIT("BIT");
            BIT
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x24 }
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x2C }
            );
            opcodes["BIT"] = BIT;

            Instruction BMI("BMI");
            BMI
            .addEncoding(AddrMode::Relative,
                Encoding{ 0x30 }
            );
            opcodes["BMI"] = BMI;

            Instruction BNE("BNE");
            BNE
            .addEncoding(AddrMode::Relative,
                Encoding{ 0xD0 }
            );
            opcodes["BNE"] = BNE;

            Instruction BPL("BPL");
            BPL
            .addEncoding(AddrMode::Relative,
                Encoding{ 0x10 }
            );
            opcodes["BPL"] = BPL;

            Instruction BRK("BRK");
            BRK
            .addEncoding(AddrMode::Implied,
                Encoding{ 0x00 }
            );
            opcodes["BRK"] = BRK;

            Instruction BVC("BVC");
            BVC
            .addEncoding(AddrMode::Relative,
                Encoding{ 0x50 }
            );
            opcodes["BVC"] = BVC;

            Instruction BVS("BVS");
            BVS
            .addEncoding(AddrMode::Relative,
                Encoding{ 0x70 }
            );
            opcodes["BVS"] = BVS;

            Instruction CLC("CLC");
            CLC
            .addEncoding(AddrMode::Implied,
                Encoding{ 0x18 }
            );
            opcodes["CLC"] = CLC;

            Instruction CLD("CLD");
            CLD
            .addEncoding(AddrMode::Implied,
                Encoding{ 0xD8 }
            );
            opcodes["CLD"] = CLD;

            Instruction CLI("CLI");
            CLI
            .addEncoding(AddrMode::Implied,
                Encoding{ 0x58 }
            );
            opcodes["CLI"] = CLI;

            Instruction CLV("CLV");
            CLV
            .addEncoding(AddrMode::Implied,
                Encoding{ 0xB8 }
            );
            opcodes["CLV"] = CLV;

            Instruction CMP("CMP");
            CMP
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0xC9 }
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0xC5 }
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0xD5 }
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0xCD }
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0xDD }
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0xD9 }
            )
            .addEncoding(AddrMode::IndirectX,
                Encoding{ 0xC1 }
            )
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0xD1 }
            );
            opcodes["CMP"] = CMP;

            Instruction CPX("CPX");
            CPX
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0xE0 }
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0xE4 }
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0xEC }
            );
            opcodes["CPX"] = CPX;

            Instruction CPY("CPY");
            CPY
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0xC0 }
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0xC4 }
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0xCC }
            );
            opcodes["CPY"] = CPY;

            Instruction DEC("DEC");
            DEC
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0xC6 }
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0xD6 }
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0xCE }
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0xDE }
            );
            opcodes["DEC"] = DEC;

            Instruction DEX("DEX");
            DEX
            .addEncoding(AddrMode::Implied,
                Encoding{ 0xCA }
            );
            opcodes["DEX"] = DEX;

            Instruction DEY("DEY");
            DEY
            .addEncoding(AddrMode::Implied,
                Encoding{ 0x88 }
            );
            opcodes["DEY"] = DEY;

            Instruction EOR("EOR");
            EOR
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0x49 }
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x45 }
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0x55 }
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x4D }
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0x5D }
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0x59 }
            )
            .addEncoding(AddrMode::IndirectX,
                Encoding{ 0x41 }
            )
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0x51 }
            );
            opcodes["EOR"] = EOR;

            Instruction INC("INC");
            INC
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0xE6 }
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0xF6 }
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0xEE }
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0xFE }
            );
            opcodes["INC"] = INC;

            Instruction INX("INX");
            INX
            .addEncoding(AddrMode::Implied,
                Encoding{ 0xE8 }
            );
            opcodes["INX"] = INX;

            Instruction INY("INY");
            INY
            .addEncoding(AddrMode::Implied,
                Encoding{ 0xC8 }
            );
            opcodes["INY"] = INY;

            Instruction JMP("JMP");
            JMP
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x4C }
            )
            .addEncoding(AddrMode::Indirect,
                Encoding{ 0x6C }
            );
            opcodes["JMP"] = JMP;

            Instruction JSR("JSR");
            JSR
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x20 }
            );
            opcodes["JSR"] = JSR;

            Instruction LDA("LDA");
            LDA
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0xA9 }
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0xA5 }
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0xB5 }
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0xAD }
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0xBD }
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0xB9 }
            )
            .addEncoding(AddrMode::IndirectX,
                Encoding{ 0xA1 }
            )
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0xB1 }
            );
            opcodes["LDA"] = LDA;

            Instruction LDX("LDX");
            LDX
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0xA2 }
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0xA6 }
            )
            .addEncoding(AddrMode::ZeroPageY,
                Encoding{ 0xB6 }
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0xAE }
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0xBE }
            );
            opcodes["LDX"] = LDX;

            Instruction LDY("LDY");
            LDY
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0xA0 }
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0xA4 }
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0xB4 }
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0xAC }
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0xBC }
            );
            opcodes["LDY"] = LDY;

            Instruction LSR("LSR");
            LSR
            .addEncoding(AddrMode::Accumulator,
                Encoding{ 0x4A }
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x46 }
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0x56 }
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x4E }
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0x5E }
            );
            opcodes["LSR"] = LSR;

            Instruction NOP("NOP");
            NOP
            .addEncoding(AddrMode::Implied,
                Encoding{ 0xEA }
            )
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0x80 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x04 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0x14 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x0C }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0x1C }
                    .setUndocumented()
            );
            opcodes["NOP"] = NOP;

            Instruction ORA("ORA");
            ORA
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0x09 }
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x05 }
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0x15 }
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x0D }
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0x1D }
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0x19 }
            )
            .addEncoding(AddrMode::IndirectX,
                Encoding{ 0x01 }
            )
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0x11 }
            );
            opcodes["ORA"] = ORA;

            Instruction PHA("PHA");
            PHA
            .addEncoding(AddrMode::Implied,
                Encoding{ 0x48 }
            );
            opcodes["PHA"] = PHA;

            Instruction PHP("PHP");
            PHP
            .addEncoding(AddrMode::Implied,
                Encoding{ 0x08 }
            );
            opcodes["PHP"] = PHP;

            Instruction PLA("PLA");
            PLA
            .addEncoding(AddrMode::Implied,
                Encoding{ 0x68 }
            );
            opcodes["PLA"] = PLA;

            Instruction PLP("PLP");
            PLP
            .addEncoding(AddrMode::Implied,
                Encoding{ 0x28 }
            );
            opcodes["PLP"] = PLP;

            Instruction ROL("ROL");
            ROL
            .addEncoding(AddrMode::Accumulator,
                Encoding{ 0x2A }
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x26 }
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0x36 }
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x2E }
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0x3E }
            );
            opcodes["ROL"] = ROL;

            Instruction ROR("ROR");
            ROR
            .addEncoding(AddrMode::Accumulator,
                Encoding{ 0x6A }
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x66 }
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0x76 }
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x6E }
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0x7E }
            );
            opcodes["ROR"] = ROR;

            Instruction RTI("RTI");
            RTI
            .addEncoding(AddrMode::Implied,
                Encoding{ 0x40 }
            );
            opcodes["RTI"] = RTI;

            Instruction RTS("RTS");
            RTS
            .addEncoding(AddrMode::Implied,
                Encoding{ 0x60 }
            );
            opcodes["RTS"] = RTS;

            Instruction SBC("SBC");
            SBC
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0xE9 }
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0xE5 }
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0xF5 }
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0xED }
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0xFD }
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0xF9 }
            )
            .addEncoding(AddrMode::IndirectX,
                Encoding{ 0xE1 }
            )
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0xF1 }
            );
            opcodes["SBC"] = SBC;

            Instruction SEC("SEC");
            SEC
            .addEncoding(AddrMode::Implied,
                Encoding{ 0x38 }
            );
            opcodes["SEC"] = SEC;

            Instruction SED("SED");
            SED
            .addEncoding(AddrMode::Implied,
                Encoding{ 0xF8 }
            );
            opcodes["SED"] = SED;

            Instruction SEI("SEI");
            SEI
            .addEncoding(AddrMode::Implied,
                Encoding{ 0x78 }
            );
            opcodes["SEI"] = SEI;

            Instruction STA("STA");
            STA
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x85 }
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0x95 }
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x8D }
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0x9D }
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0x99 }
            )
            .addEncoding(AddrMode::IndirectX,
                Encoding{ 0x81 }
            )
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0x91 }
            );
            opcodes["STA"] = STA;

            Instruction STX("STX");
            STX
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x86 }
            )
            .addEncoding(AddrMode::ZeroPageY,
                Encoding{ 0x96 }
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x8E }
            );
            opcodes["STX"] = STX;

            Instruction STY("STY");
            STY
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x84 }
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0x94 }
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x8C }
            );
            opcodes["STY"] = STY;

            Instruction TAX("TAX");
            TAX
            .addEncoding(AddrMode::Implied,
                Encoding{ 0xAA }
            );
            opcodes["TAX"] = TAX;

            Instruction TAY("TAY");
            TAY
            .addEncoding(AddrMode::Implied,
                Encoding{ 0xA8 }
            );
            opcodes["TAY"] = TAY;

            Instruction TSX("TSX");
            TSX
            .addEncoding(AddrMode::Implied,
                Encoding{ 0xBA }
            );
            opcodes["TSX"] = TSX;

            Instruction TXA("TXA");
            TXA
            .addEncoding(AddrMode::Implied,
                Encoding{ 0x8A }
            );
            opcodes["TXA"] = TXA;

            Instruction TXS("TXS");
            TXS
            .addEncoding(AddrMode::Implied,
                Encoding{ 0x9A }
            );
            opcodes["TXS"] = TXS;

            Instruction TYA("TYA");
            TYA
            .addEncoding(AddrMode::Implied,
                Encoding{ 0x98 }
            );
            opcodes["TYA"] = TYA;

            // Undocumented opcodes. 
            //
            Instruction SLO("SLO");
            SLO
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x07 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0x17 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::IndirectX,
                Encoding{ 0x03 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0x13 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x0F }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0x1F }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0x1B }
                    .setUndocumented()
            );
            opcodes["SLO"] = SLO;

            Instruction RLA("RLA");
            RLA
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x27 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0x37 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::IndirectX,
                Encoding{ 0x23 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0x33 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x2F }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0x3F }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0x3B }
                    .setUndocumented()
            );
            opcodes["RLA"] = RLA;

            Instruction SRE("SRE");
            SRE
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x47 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0x57 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::IndirectX,
                Encoding{ 0x43 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0x53 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x4F }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0x5F }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0x5B }
                    .setUndocumented()
            );
            opcodes["SRE"] = SRE;

            Instruction RRA("RRA");
            RRA
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x67 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0x77 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::IndirectX,
                Encoding{ 0x63 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0x73 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x6F }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0x7F }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0x7B }
                    .setUndocumented()
            );
            opcodes["RRA"] = RRA;

            Instruction SAX("SAX");
            SAX
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x87 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::ZeroPageY,
                Encoding{ 0x97 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::IndirectX,
                Encoding{ 0x83 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x8F }
                    .setUndocumented()
            );
            opcodes["SAX"] = SAX;

            Instruction LAX("LAX");
            LAX
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0xAB }
                    .setUndocumented()
                    .setUnstable()
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0xA7 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::ZeroPageY,
                Encoding{ 0xB7 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::IndirectX,
                Encoding{ 0xA3 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0xB3 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0xAF }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0xBF }
                    .setUndocumented()
            );
            opcodes["LAX"] = LAX;

            Instruction DCP("DCP");
            DCP
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0xC7 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0xD7 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::IndirectX,
                Encoding{ 0xC3 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0xD3 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0xCF }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0xDF }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0xDB }
                    .setUndocumented()
            );
            opcodes["DCP"] = DCP;

            Instruction ISC("ISC");
            ISC
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0xE7 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0xF7 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::IndirectX,
                Encoding{ 0xE3 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0xF3 }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0xEF }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0xFF }
                    .setUndocumented()
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0xFB }
                    .setUndocumented()
            );
            opcodes["ISC"] = ISC;

            Instruction ANC("ANC");
            ANC
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0x0B }
                    .setUndocumented()
            );
            opcodes["ANC"] = ANC;

            Instruction ALR("ALR");
            ALR
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0x4B }
                    .setUndocumented()
            );
            opcodes["ALR"] = ALR;

            Instruction ARR("ARR");
            ARR
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0x6B }
                    .setUndocumented()
            );
            opcodes["ARR"] = ARR;

            Instruction XAA("XAA");
            XAA
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0x8B }
                    .setUndocumented()
                    .setUnstable()
            );
            opcodes["XAA"] = XAA;

            Instruction AXS("AXS");
            AXS
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0xCB }
                    .setUndocumented()
            );
            opcodes["AXS"] = AXS;

            Instruction AHX("AHX");
            AHX 
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0x93 }
                    .setUndocumented()
                    .setUnstable()
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0x9F }
                    .setUndocumented()
                    .setUnstable()
            );
            opcodes["AHX"] = AHX;

            Instruction SHX("SHX");
            SHX 
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0x9E }
                    .setUndocumented()
                    .setUnstable()
            );
            opcodes["SHX"] = SHX;

            Instruction SHY("SHY");
            SHY 
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0x9C }
                    .setUndocumented()
                    .setUnstable()
            );
            opcodes["SHY"] = SHY;

            Instruction TAS("TAS");
            TAS 
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0x9B }
                    .setUndocumented()
                    .setUnstable()
            );
            opcodes["TAS"] = SHY;

            Instruction LAS("LAS");
            LAS 
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0xBB }
                    .setUndocumented()
            );
            opcodes["LAS"] = LAS;

            for (const auto &p : opcodes) {
                const Instruction &instr = p.second;
                if (instr.hasEncoding(AddrMode::ZeroPage)) {
                    assert(instr.hasEncoding(AddrMode::Absolute));
                }
            }
            return opcodes;
        }
    }
}

