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

namespace yas6502
{
    Opcode::Opcode()
        : accumulator(-1)
        , immediate(-1)
        , implied(-1)
        , zeroPage(-1)
        , zeroPageX(-1)
        , zeroPageY(-1)
        , absolute(-1)
        , absoluteX(-1)
        , absoluteY(-1)
        , indirect(-1)
        , indirectX(-1)
        , indirectY(-1)
        , relative(-1)
    {
    }

    OpcodeMap makeOpcodeMap()
    {
        OpcodeMap opcodes{};

        Opcode ADC{};
        ADC.immediate = 0x69;
        ADC.zeroPage = 0x65;
        ADC.zeroPageX = 0x75;
        ADC.absolute = 0x6D;
        ADC.absoluteX = 0x7D;
        ADC.absoluteY = 0x79;
        ADC.indirectX = 0x61;
        ADC.indirectY = 0x71;
        opcodes["ADC"] = ADC;

        Opcode AND{};
        AND.immediate = 0x29;
        AND.zeroPage = 0x25;
        AND.zeroPageX = 0x35;
        AND.absolute = 0x2D;
        AND.absoluteX = 0x3D;
        AND.absoluteY = 0x39;
        AND.indirectX = 0x21;
        AND.indirectY = 0x31;
        opcodes["AND"] = AND;

        Opcode ASL{};
        ASL.accumulator = 0x0A;
        ASL.zeroPage = 0x06;
        ASL.zeroPageX = 0x16;
        ASL.absolute = 0x0E;
        ASL.absoluteX = 0x1E;
        opcodes["ASL"] = ASL;

        Opcode BCC{};
        BCC.relative = 0x90;
        opcodes["BCC"] = BCC;

        Opcode BCS{};
        BCS.relative = 0xB0;
        opcodes["BCS"] = BCS;

        Opcode BEQ{};
        BEQ.relative = 0xF0;
        opcodes["BEQ"] = BEQ;

        Opcode BIT{};
        BIT.zeroPage = 0x24;
        BIT.absolute = 0x2C;
        opcodes["BIT"] = BIT;

        Opcode BMI{};
        BMI.relative = 0x30;
        opcodes["BMI"] = BMI;

        Opcode BNE{};
        BNE.relative = 0xD0;
        opcodes["BNE"] = BNE;

        Opcode BPL{};
        BPL.relative = 0x10;
        opcodes["BPL"] = BPL;

        Opcode BRK{};
        BRK.implied = 0x00;
        opcodes["BRK"] = BRK;

        Opcode BVC{};
        BVC.relative = 0x50;
        opcodes["BVC"] = BVC;

        Opcode BVS{};
        BVS.relative = 0x70;
        opcodes["BVS"] = BVS;

        Opcode CLC{};
        CLC.implied = 0x18;
        opcodes["CLC"] = CLC;

        Opcode CLD{};
        CLD.implied = 0xD8;
        opcodes["CLD"] = CLD;

        Opcode CLI{};
        CLI.implied = 0x58;
        opcodes["CLI"] = CLI;

        Opcode CLV{};
        CLV.implied = 0xB8;
        opcodes["CLV"] = CLV;

        Opcode CMP{};
        CMP.immediate = 0xC9;
        CMP.zeroPage = 0xC5;
        CMP.zeroPageX = 0xD5;
        CMP.absolute = 0xCD;
        CMP.absoluteX = 0xDD;
        CMP.absoluteY = 0xD9;
        CMP.indirectX = 0xC1;
        CMP.indirectY = 0xD1;
        opcodes["CMP"] = CMP;

        Opcode CPX{};
        CPX.immediate = 0xE0;
        CPX.zeroPage = 0xE4;
        CPX.absolute = 0xEC;
        opcodes["CPX"] = CPX;

        Opcode CPY{};
        CPY.immediate = 0xC0;
        CPY.zeroPage = 0xC4;
        CPY.absolute = 0xCC;
        opcodes["CPY"] = CPY;

        Opcode DEC{};
        DEC.zeroPage = 0xC6;
        DEC.zeroPageX = 0xD6;
        DEC.absolute = 0xCE;
        DEC.absoluteX = 0xDE;
        opcodes["DEC"] = DEC;

        Opcode DEX{};
        DEX.implied = 0xCA;
        opcodes["DEX"] = DEX;

        Opcode DEY{};
        DEY.implied = 0x88;
        opcodes["DEY"] = DEY;

        Opcode EOR{};
        EOR.immediate = 0x49;
        EOR.zeroPage = 0x45;
        EOR.zeroPageX = 0x55;
        EOR.absolute = 0x4D;
        EOR.absoluteX = 0x5D;
        EOR.absoluteY = 0x59;
        EOR.indirectX = 0x41;
        EOR.indirectY = 0x51;
        opcodes["EOR"] = EOR;

        Opcode INC{};
        INC.zeroPage = 0xE6;
        INC.zeroPageX = 0xF6;
        INC.absolute = 0xEE;
        INC.absoluteX = 0xFE;
        opcodes["INC"] = INC;

        Opcode INX{};
        INX.implied = 0xE8;
        opcodes["INX"] = INX;

        Opcode INY{};
        INY.implied = 0xC8;
        opcodes["INY"] = INY;

        Opcode JMP{};
        JMP.absolute = 0x4C;
        JMP.indirect = 0x6C;
        opcodes["JMP"] = JMP;

        Opcode JSR{};
        JSR.absolute = 0x20;
        opcodes["JSR"] = JSR;

        Opcode LDA{};
        LDA.immediate = 0xA9;
        LDA.zeroPage = 0xA5;
        LDA.zeroPageX = 0xB5;
        LDA.absolute = 0xAD;
        LDA.absoluteX = 0xBD;
        LDA.absoluteY = 0xB9;
        LDA.indirectX = 0xA1;
        LDA.indirectY = 0xB1;
        opcodes["LDA"] = LDA;

        Opcode LDX{};
        LDX.immediate = 0xA2;
        LDX.zeroPage = 0xA6;
        LDX.zeroPageY = 0xB6;
        LDX.absolute = 0xAE;
        LDX.absoluteY = 0xBE;
        opcodes["LDX"] = LDX;

        Opcode LDY{};
        LDY.immediate = 0xA0;
        LDY.zeroPage = 0xA4;
        LDY.zeroPageX = 0xB4;
        LDY.absolute = 0xAC;
        LDY.absoluteX = 0xBC;
        opcodes["LDY"] = LDY;

        Opcode LSR{};
        LSR.accumulator = 0x4A;
        LSR.zeroPage = 0x46;
        LSR.zeroPageX = 0x56;
        LSR.absolute = 0x4E;
        LSR.absoluteX = 0x5E;
        opcodes["LSR"] = LSR;

        Opcode NOP{};
        NOP.implied = 0xEA;
        opcodes["NOP"] = NOP;

        Opcode ORA{};
        ORA.immediate = 0x09;
        ORA.zeroPage = 0x05;
        ORA.zeroPageX = 0x15;
        ORA.absolute = 0x0D;
        ORA.absoluteX = 0x1D;
        ORA.absoluteY = 0x19;
        ORA.indirectX = 0x01;
        ORA.indirectY = 0x11;
        opcodes["ORA"] = ORA;

        Opcode PHA{};
        PHA.implied = 0x48;
        opcodes["PHA"] = PHA;

        Opcode PHP{};
        PHP.implied = 0x08;
        opcodes["PHP"] = PHP;

        Opcode PLA{};
        PLA.implied = 0x68;
        opcodes["PLA"] = PLA;

        Opcode PLP{};
        PLP.implied = 0x28;
        opcodes["PLP"] = PLP;

        Opcode ROL{};
        ROL.accumulator = 0x2A;
        ROL.zeroPage = 0x26;
        ROL.zeroPageX = 0x36;
        ROL.absolute = 0x2E;
        ROL.absoluteX = 0x3E;
        opcodes["ROL"] = ROL;

        Opcode ROR{};
        ROR.accumulator = 0x6A;
        ROR.zeroPage = 0x66;
        ROR.zeroPageX = 0x76;
        ROR.absolute = 0x6E;
        ROR.absoluteX = 0x7E;
        opcodes["ROR"] = ROR;

        Opcode RTI{};
        RTI.implied = 0x40;
        opcodes["RTI"] = RTI;

        Opcode RTS{};
        RTS.implied = 0x60;
        opcodes["RTS"] = RTS;

        Opcode SBC{};
        SBC.immediate = 0xE9;
        SBC.zeroPage = 0xE5;
        SBC.zeroPageX = 0xF5;
        SBC.absolute = 0xED;
        SBC.absoluteX = 0xFD;
        SBC.absoluteY = 0xF9;
        SBC.indirectX = 0xE1;
        SBC.indirectY = 0xF1;
        opcodes["SBC"] = SBC;

        Opcode SEC{};
        SEC.implied = 0x38;
        opcodes["SEC"] = SEC;

        Opcode SED{};
        SED.implied = 0xF8;
        opcodes["SED"] = SED;

        Opcode SEI{};
        SEI.implied = 0x78;
        opcodes["SEI"] = SEI;

        Opcode STA{};
        STA.zeroPage = 0x85;
        STA.zeroPageX = 0x95;
        STA.absolute = 0x8D;
        STA.absoluteX = 0x9D;
        STA.absoluteY = 0x99;
        STA.indirectX = 0x81;
        STA.indirectY = 0x91;
        opcodes["STA"] = STA;

        Opcode STX{};
        STX.zeroPage = 0x86;
        STX.zeroPageY = 0x96;
        STX.absolute = 0x8E;
        opcodes["STX"] = STX;

        Opcode STY{};
        STY.zeroPage = 0x84;
        STY.zeroPageX = 0x94;
        STY.absolute = 0x8C;
        opcodes["STY"] = STY;

        Opcode TAX{};
        TAX.implied = 0xAA;
        opcodes["TAX"] = TAX;

        Opcode TAY{};
        TAY.implied = 0xA8;
        opcodes["TAY"] = TAY;

        Opcode TSX{};
        TSX.implied = 0xBA;
        opcodes["TSX"] = TSX;

        Opcode TXA{};
        TXA.implied = 0x8A;
        opcodes["TXA"] = TXA;

        Opcode TXS{};
        TXS.implied = 0x9A;
        opcodes["TXS"] = TXS;

        Opcode TYA{};
        TYA.implied = 0x98;
        opcodes["TYA"] = TYA;

        for (const auto &p : opcodes) {
            const Opcode &opcode = p.second;
            if (opcode.zeroPage != -1) {
                assert(opcode.absolute != -1);
            }
        }

        return opcodes;
    }
}
