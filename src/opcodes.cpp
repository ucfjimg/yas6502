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
                    .setClocks(2)
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x65 }
                    .setClocks(3)
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0x75 }
                    .setClocks(4)
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x6D }
                    .setClocks(4)
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0x7D }
                    .setClocks(4)
                    .setExtraClocks()
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0x79 }
                    .setClocks(4)
                    .setExtraClocks()
            )
            .addEncoding(AddrMode::IndirectX,
                Encoding{ 0x61 }
                    .setClocks(6)
            )
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0x71 }
                    .setClocks(5)
                    .setExtraClocks()
            );
            opcodes["ADC"] = ADC;

            Instruction AND("AND");
            AND
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0x29 }
                    .setClocks(2)
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x25 }
                    .setClocks(3)
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0x35 }
                    .setClocks(4)
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x2D }
                    .setClocks(4)
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0x3D }
                    .setClocks(4)
                    .setExtraClocks()
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0x39 }
                    .setClocks(4)
                    .setExtraClocks()
            )
            .addEncoding(AddrMode::IndirectX,
                Encoding{ 0x21 }
                    .setClocks(6)
            )
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0x31 }
                    .setClocks(5)
                    .setExtraClocks()
            );
            opcodes["AND"] = AND;

            Instruction ASL("ASL");
            ASL
            .addEncoding(AddrMode::Accumulator,
                Encoding{ 0x0A }
                    .setClocks(2)
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x06 }
                    .setClocks(5)
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0x16 }
                    .setClocks(6)
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x0E }
                    .setClocks(6)
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0x1E }
                    .setClocks(7)
            );
            opcodes["ASL"] = ASL;

            Instruction BCC("BCC");
            BCC
            .addEncoding(AddrMode::Relative,
                Encoding{ 0x90 }
                    .setClocks(2)
                    .setExtraClocks()
            );
            opcodes["BCC"] = BCC;

            Instruction BCS("BCS");
            BCS
            .addEncoding(AddrMode::Relative,
                Encoding{ 0xB0 }
                    .setClocks(2)
                    .setExtraClocks()
            );
            opcodes["BCS"] = BCS;

            Instruction BEQ("BEQ");
            BEQ
            .addEncoding(AddrMode::Relative,
                Encoding{ 0xF0 }
                    .setClocks(2)
                    .setExtraClocks()
            );
            opcodes["BEQ"] = BEQ;

            Instruction BIT("BIT");
            BIT
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x24 }
                    .setClocks(3)
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x2C }
                    .setClocks(4)
            );
            opcodes["BIT"] = BIT;

            Instruction BMI("BMI");
            BMI
            .addEncoding(AddrMode::Relative,
                Encoding{ 0x30 }
                    .setClocks(2)
                    .setExtraClocks()
            );
            opcodes["BMI"] = BMI;

            Instruction BNE("BNE");
            BNE
            .addEncoding(AddrMode::Relative,
                Encoding{ 0xD0 }
                    .setClocks(2)
                    .setExtraClocks()
            );
            opcodes["BNE"] = BNE;

            Instruction BPL("BPL");
            BPL
            .addEncoding(AddrMode::Relative,
                Encoding{ 0x10 }
                    .setClocks(2)
                    .setExtraClocks()
            );
            opcodes["BPL"] = BPL;

            Instruction BRK("BRK");
            BRK
            .addEncoding(AddrMode::Implied,
                Encoding{ 0x00 }
                    .setClocks(7)
            );
            opcodes["BRK"] = BRK;

            Instruction BVC("BVC");
            BVC
            .addEncoding(AddrMode::Relative,
                Encoding{ 0x50 }
                    .setClocks(2)
                    .setExtraClocks()
            );
            opcodes["BVC"] = BVC;

            Instruction BVS("BVS");
            BVS
            .addEncoding(AddrMode::Relative,
                Encoding{ 0x70 }
                    .setClocks(2)
                    .setExtraClocks()
            );
            opcodes["BVS"] = BVS;

            Instruction CLC("CLC");
            CLC
            .addEncoding(AddrMode::Implied,
                Encoding{ 0x18 }
                    .setClocks(2)
            );
            opcodes["CLC"] = CLC;

            Instruction CLD("CLD");
            CLD
            .addEncoding(AddrMode::Implied,
                Encoding{ 0xD8 }
                    .setClocks(2)
            );
            opcodes["CLD"] = CLD;

            Instruction CLI("CLI");
            CLI
            .addEncoding(AddrMode::Implied,
                Encoding{ 0x58 }
                    .setClocks(2)
            );
            opcodes["CLI"] = CLI;

            Instruction CLV("CLV");
            CLV
            .addEncoding(AddrMode::Implied,
                Encoding{ 0xB8 }
                    .setClocks(2)
            );
            opcodes["CLV"] = CLV;

            Instruction CMP("CMP");
            CMP
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0xC9 }
                    .setClocks(2)
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0xC5 }
                    .setClocks(3)
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0xD5 }
                    .setClocks(4)
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0xCD }
                    .setClocks(4)
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0xDD }
                    .setClocks(4)
                    .setExtraClocks()
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0xD9 }
                    .setClocks(4)
                    .setExtraClocks()
            )
            .addEncoding(AddrMode::IndirectX,
              Encoding{ 0xC1 }
                    .setClocks(6)
            )
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0xD1 }
                    .setClocks(5)
                    .setExtraClocks()
            );
            opcodes["CMP"] = CMP;

            Instruction CPX("CPX");
            CPX
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0xE0 }
                    .setClocks(2)
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0xE4 }
                    .setClocks(3)
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0xEC }
                    .setClocks(4)
            );
            opcodes["CPX"] = CPX;

            Instruction CPY("CPY");
            CPY
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0xC0 }
                    .setClocks(2)
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0xC4 }
                    .setClocks(3)
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0xCC }
                    .setClocks(4)
            );
            opcodes["CPY"] = CPY;

            Instruction DEC("DEC");
            DEC
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0xC6 }
                    .setClocks(5)
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0xD6 }
                    .setClocks(6)
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0xCE }
                    .setClocks(6)
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0xDE }
                    .setClocks(7)
            );
            opcodes["DEC"] = DEC;

            Instruction DEX("DEX");
            DEX
            .addEncoding(AddrMode::Implied,
                Encoding{ 0xCA }
                    .setClocks(2)
            );
            opcodes["DEX"] = DEX;

            Instruction DEY("DEY");
            DEY
            .addEncoding(AddrMode::Implied,
                Encoding{ 0x88 }
                    .setClocks(2)
            );
            opcodes["DEY"] = DEY;

            Instruction EOR("EOR");
            EOR
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0x49 }
                    .setClocks(2)
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x45 }
                    .setClocks(3)
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0x55 }
                    .setClocks(4)
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x4D }
                    .setClocks(4)
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0x5D }
                    .setClocks(4)
                    .setExtraClocks()
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0x59 }
                    .setClocks(4)
                    .setExtraClocks()
            )
            .addEncoding(AddrMode::IndirectX,
                Encoding{ 0x41 }
                    .setClocks(6)
            )
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0x51 }
                    .setClocks(5)
                    .setExtraClocks()
            );
            opcodes["EOR"] = EOR;

            Instruction INC("INC");
            INC
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0xE6 }
                    .setClocks(5)
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0xF6 }
                    .setClocks(6)
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0xEE }
                    .setClocks(6)
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0xFE }
                    .setClocks(7)
            );
            opcodes["INC"] = INC;

            Instruction INX("INX");
            INX
            .addEncoding(AddrMode::Implied,
                Encoding{ 0xE8 }
                    .setClocks(2)
            );
            opcodes["INX"] = INX;

            Instruction INY("INY");
            INY
            .addEncoding(AddrMode::Implied,
                Encoding{ 0xC8 }
                    .setClocks(2)
            );
            opcodes["INY"] = INY;

            Instruction JMP("JMP");
            JMP
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x4C }
                    .setClocks(3)
            )
            .addEncoding(AddrMode::Indirect,
                Encoding{ 0x6C }
                    .setClocks(5)
            );
            opcodes["JMP"] = JMP;

            Instruction JSR("JSR");
            JSR
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x20 }
                    .setClocks(6)
            );
            opcodes["JSR"] = JSR;

            Instruction LDA("LDA");
            LDA
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0xA9 }
                    .setClocks(2)
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0xA5 }
                    .setClocks(3)
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0xB5 }
                    .setClocks(4)
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0xAD }
                    .setClocks(4)
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0xBD }
                    .setClocks(4)
                    .setExtraClocks()
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0xB9 }
                    .setClocks(4)
                    .setExtraClocks()
            )
            .addEncoding(AddrMode::IndirectX,
                Encoding{ 0xA1 }
                    .setClocks(6)
            )
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0xB1 }
                    .setClocks(5)
                    .setExtraClocks()
            );
            opcodes["LDA"] = LDA;

            Instruction LDX("LDX");
            LDX
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0xA2 }
                    .setClocks(2)
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0xA6 }
                    .setClocks(3)
            )
            .addEncoding(AddrMode::ZeroPageY,
                Encoding{ 0xB6 }
                    .setClocks(4)
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0xAE }
                    .setClocks(4)
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0xBE }
                    .setClocks(4)
                    .setExtraClocks()
            );
            opcodes["LDX"] = LDX;

            Instruction LDY("LDY");
            LDY
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0xA0 }
                    .setClocks(2)
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0xA4 }
                    .setClocks(3)
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0xB4 }
                    .setClocks(4)
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0xAC }
                    .setClocks(4)
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0xBC }
                    .setClocks(4)
                    .setExtraClocks()
            );
            opcodes["LDY"] = LDY;

            Instruction LSR("LSR");
            LSR
            .addEncoding(AddrMode::Accumulator,
                Encoding{ 0x4A }
                    .setClocks(2)
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x46 }
                    .setClocks(5)
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0x56 }
                    .setClocks(6)
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x4E }
                    .setClocks(6)
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0x5E }
                    .setClocks(7)
            );
            opcodes["LSR"] = LSR;

            Instruction NOP("NOP");
            NOP
            .addEncoding(AddrMode::Implied,
                Encoding{ 0xEA }
                    .setClocks(2)
            )
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0x80 }
                    .setClocks(2)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x04 }
                    .setClocks(3)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0x14 }
                    .setClocks(4)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x0C }
                    .setClocks(4)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0x1C }
                    .setClocks(4)
                    .setExtraClocks()
                    .setUndocumented()
            );
            opcodes["NOP"] = NOP;

            Instruction ORA("ORA");
            ORA
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0x09 }
                    .setClocks(2)
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x05 }
                    .setClocks(3)
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0x15 }
                    .setClocks(4)
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x0D }
                    .setClocks(4)
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0x1D }
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0x19 }
                    .setClocks(4)
                    .setExtraClocks()
            )
            .addEncoding(AddrMode::IndirectX,
                Encoding{ 0x01 }
                    .setClocks(6)
            )
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0x11 }
                    .setClocks(5)
                    .setExtraClocks()
            );
            opcodes["ORA"] = ORA;

            Instruction PHA("PHA");
            PHA
            .addEncoding(AddrMode::Implied,
                Encoding{ 0x48 }
                    .setClocks(3)
            );
            opcodes["PHA"] = PHA;

            Instruction PHP("PHP");
            PHP
            .addEncoding(AddrMode::Implied,
                Encoding{ 0x08 }
                    .setClocks(3)
            );
            opcodes["PHP"] = PHP;

            Instruction PLA("PLA");
            PLA
            .addEncoding(AddrMode::Implied,
                Encoding{ 0x68 }
                    .setClocks(4)
            );
            opcodes["PLA"] = PLA;

            Instruction PLP("PLP");
            PLP
            .addEncoding(AddrMode::Implied,
                Encoding{ 0x28 }
                    .setClocks(4)
            );
            opcodes["PLP"] = PLP;

            Instruction ROL("ROL");
            ROL
            .addEncoding(AddrMode::Accumulator,
                Encoding{ 0x2A }
                    .setClocks(2)
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x26 }
                    .setClocks(5)
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0x36 }
                    .setClocks(6)
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x2E }
                    .setClocks(6)
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0x3E }
                    .setClocks(7)
            );
            opcodes["ROL"] = ROL;

            Instruction ROR("ROR");
            ROR
            .addEncoding(AddrMode::Accumulator,
                Encoding{ 0x6A }
                    .setClocks(2)
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x66 }
                    .setClocks(5)
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0x76 }
                    .setClocks(6)
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x6E }
                    .setClocks(6)
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0x7E }
                    .setClocks(7)
            );
            opcodes["ROR"] = ROR;

            Instruction RTI("RTI");
            RTI
            .addEncoding(AddrMode::Implied,
                Encoding{ 0x40 }
                    .setClocks(6)
            );
            opcodes["RTI"] = RTI;

            Instruction RTS("RTS");
            RTS
            .addEncoding(AddrMode::Implied,
                Encoding{ 0x60 }
                    .setClocks(6)
            );
            opcodes["RTS"] = RTS;

            Instruction SBC("SBC");
            SBC
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0xE9 }
                    .setClocks(2)
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0xE5 }
                    .setClocks(3)
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0xF5 }
                    .setClocks(4)
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0xED }
                    .setClocks(4)
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0xFD }
                    .setClocks(4)
                    .setExtraClocks()
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0xF9 }
                    .setClocks(4)
                    .setExtraClocks()
            )
            .addEncoding(AddrMode::IndirectX,
                Encoding{ 0xE1 }
                    .setClocks(6)
            )
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0xF1 }
                    .setClocks(5)
                    .setExtraClocks()
            );
            opcodes["SBC"] = SBC;

            Instruction SEC("SEC");
            SEC
            .addEncoding(AddrMode::Implied,
                Encoding{ 0x38 }
                    .setClocks(2)
            );
            opcodes["SEC"] = SEC;

            Instruction SED("SED");
            SED
            .addEncoding(AddrMode::Implied,
                Encoding{ 0xF8 }
                    .setClocks(2)
            );
            opcodes["SED"] = SED;

            Instruction SEI("SEI");
            SEI
            .addEncoding(AddrMode::Implied,
                Encoding{ 0x78 }
                    .setClocks(2)
            );
            opcodes["SEI"] = SEI;

            Instruction STA("STA");
            STA
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x85 }
                    .setClocks(3)
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0x95 }
                    .setClocks(4)
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x8D }
                    .setClocks(4)
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0x9D }
                    .setClocks(5)
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0x99 }
                    .setClocks(5)
            )
            .addEncoding(AddrMode::IndirectX,
                Encoding{ 0x81 }
                    .setClocks(6)
            )
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0x91 }
                    .setClocks(6)
            );
            opcodes["STA"] = STA;

            Instruction STX("STX");
            STX
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x86 }
                    .setClocks(3)
            )
            .addEncoding(AddrMode::ZeroPageY,
                Encoding{ 0x96 }
                    .setClocks(4)
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x8E }
                    .setClocks(4)
            );
            opcodes["STX"] = STX;

            Instruction STY("STY");
            STY
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x84 }
                    .setClocks(3)
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0x94 }
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x8C }
                    .setClocks(4)
            );
            opcodes["STY"] = STY;

            Instruction TAX("TAX");
            TAX
            .addEncoding(AddrMode::Implied,
                Encoding{ 0xAA }
                    .setClocks(2)
            );
            opcodes["TAX"] = TAX;

            Instruction TAY("TAY");
            TAY
            .addEncoding(AddrMode::Implied,
                Encoding{ 0xA8 }
                    .setClocks(2)
            );
            opcodes["TAY"] = TAY;

            Instruction TSX("TSX");
            TSX
            .addEncoding(AddrMode::Implied,
                Encoding{ 0xBA }
                    .setClocks(2)
            );
            opcodes["TSX"] = TSX;

            Instruction TXA("TXA");
            TXA
            .addEncoding(AddrMode::Implied,
                Encoding{ 0x8A }
                    .setClocks(2)
            );
            opcodes["TXA"] = TXA;

            Instruction TXS("TXS");
            TXS
            .addEncoding(AddrMode::Implied,
                Encoding{ 0x9A }
                    .setClocks(2)
            );
            opcodes["TXS"] = TXS;

            Instruction TYA("TYA");
            TYA
            .addEncoding(AddrMode::Implied,
                Encoding{ 0x98 }
                    .setClocks(2)
            );
            opcodes["TYA"] = TYA;

            // Undocumented opcodes. 
            //
            Instruction SLO("SLO");
            SLO
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x07 }
                    .setClocks(5)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0x17 }
                    .setClocks(6)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::IndirectX,
                Encoding{ 0x03 }
                    .setClocks(8)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0x13 }
                    .setClocks(8)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x0F }
                    .setClocks(6)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0x1F }
                    .setClocks(7)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0x1B }
                    .setClocks(7)
                    .setUndocumented()
            );
            opcodes["SLO"] = SLO;

            Instruction RLA("RLA");
            RLA
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x27 }
                    .setClocks(5)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0x37 }
                    .setClocks(6)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::IndirectX,
                Encoding{ 0x23 }
                    .setClocks(8)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0x33 }
                    .setClocks(8)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x2F }
                    .setClocks(6)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0x3F }
                    .setClocks(7)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0x3B }
                    .setClocks(7)
                    .setUndocumented()
            );
            opcodes["RLA"] = RLA;

            Instruction SRE("SRE");
            SRE
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x47 }
                    .setClocks(5)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0x57 }
                    .setClocks(6)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::IndirectX,
                Encoding{ 0x43 }
                    .setClocks(8)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0x53 }
                    .setClocks(8)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x4F }
                    .setClocks(6)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0x5F }
                    .setClocks(7)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0x5B }
                    .setClocks(7)
                    .setUndocumented()
            );
            opcodes["SRE"] = SRE;

            Instruction RRA("RRA");
            RRA
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x67 }
                    .setClocks(5)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0x77 }
                    .setClocks(6)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::IndirectX,
                Encoding{ 0x63 }
                    .setClocks(8)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0x73 }
                    .setClocks(8)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x6F }
                    .setClocks(6)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0x7F }
                    .setClocks(7)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0x7B }
                    .setClocks(7)
                    .setUndocumented()
            );
            opcodes["RRA"] = RRA;

            Instruction SAX("SAX");
            SAX
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0x87 }
                    .setClocks(3)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::ZeroPageY,
                Encoding{ 0x97 }
                    .setClocks(4)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::IndirectX,
                Encoding{ 0x83 }
                    .setClocks(6)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0x8F }
                    .setClocks(4)
                    .setUndocumented()
            );
            opcodes["SAX"] = SAX;

            Instruction LAX("LAX");
            LAX
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0xAB }
                    .setClocks(2)
                    .setUndocumented()
                    .setUnstable()
            )
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0xA7 }
                    .setClocks(3)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::ZeroPageY,
                Encoding{ 0xB7 }
                    .setClocks(4)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::IndirectX,
                Encoding{ 0xA3 }
                    .setClocks(6)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0xB3 }
                    .setClocks(5)
                    .setExtraClocks()
                    .setUndocumented()
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0xAF }
                    .setClocks(4)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0xBF }
                    .setClocks(4)
                    .setExtraClocks()
                    .setUndocumented()
            );
            opcodes["LAX"] = LAX;

            Instruction DCP("DCP");
            DCP
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0xC7 }
                    .setClocks(5)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0xD7 }
                    .setClocks(6)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::IndirectX,
                Encoding{ 0xC3 }
                    .setClocks(8)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0xD3 }
                    .setClocks(8)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0xCF }
                    .setClocks(6)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0xDF }
                    .setClocks(7)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0xDB }
                    .setClocks(7)
                    .setUndocumented()
            );
            opcodes["DCP"] = DCP;

            Instruction ISC("ISC");
            ISC
            .addEncoding(AddrMode::ZeroPage,
                Encoding{ 0xE7 }
                    .setClocks(5)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::ZeroPageX,
                Encoding{ 0xF7 }
                    .setClocks(6)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::IndirectX,
                Encoding{ 0xE3 }
                    .setClocks(8)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0xF3 }
                    .setClocks(8)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::Absolute,
                Encoding{ 0xEF }
                    .setClocks(6)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0xFF }
                    .setClocks(7)
                    .setUndocumented()
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0xFB }
                    .setClocks(7)
                    .setUndocumented()
            );
            opcodes["ISC"] = ISC;

            Instruction ANC("ANC");
            ANC
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0x0B }
                    .setClocks(2)
                    .setUndocumented()
            );
            opcodes["ANC"] = ANC;

            Instruction ALR("ALR");
            ALR
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0x4B }
                    .setClocks(2)
                    .setUndocumented()
            );
            opcodes["ALR"] = ALR;

            Instruction ARR("ARR");
            ARR
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0x6B }
                    .setClocks(2)
                    .setUndocumented()
            );
            opcodes["ARR"] = ARR;

            Instruction XAA("XAA");
            XAA
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0x8B }
                    .setClocks(2)
                    .setUndocumented()
                    .setUnstable()
            );
            opcodes["XAA"] = XAA;

            Instruction AXS("AXS");
            AXS
            .addEncoding(AddrMode::Immediate,
                Encoding{ 0xCB }
                    .setClocks(2)
                    .setUndocumented()
            );
            opcodes["AXS"] = AXS;

            Instruction AHX("AHX");
            AHX 
            .addEncoding(AddrMode::IndirectY,
                Encoding{ 0x93 }
                    .setClocks(6)
                    .setUndocumented()
                    .setUnstable()
            )
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0x9F }
                    .setClocks(5)
                    .setUndocumented()
                    .setUnstable()
            );
            opcodes["AHX"] = AHX;

            Instruction SHX("SHX");
            SHX 
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0x9E }
                    .setClocks(5)
                    .setUndocumented()
                    .setUnstable()
            );
            opcodes["SHX"] = SHX;

            Instruction SHY("SHY");
            SHY 
            .addEncoding(AddrMode::AbsoluteX,
                Encoding{ 0x9C }
                    .setClocks(5)
                    .setUndocumented()
                    .setUnstable()
            );
            opcodes["SHY"] = SHY;

            Instruction TAS("TAS");
            TAS 
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0x9B }
                    .setClocks(5)
                    .setUndocumented()
                    .setUnstable()
            );
            opcodes["TAS"] = SHY;

            Instruction LAS("LAS");
            LAS 
            .addEncoding(AddrMode::AbsoluteY,
                Encoding{ 0xBB }
                    .setClocks(4)
                    .setExtraClocks()
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

