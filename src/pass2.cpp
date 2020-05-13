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
#include "pass2.h"

#include "ast.h"
#include "except.h"
#include "symtab.h"
#include "utility.h"

#include <cassert>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

using std::array;
using std::cerr;
using std::endl;
using std::string;
using std::unique_ptr;
using std::vector;

using ss = std::stringstream;

namespace yas6502
{
    /**
     * Constructor
     */
    Pass2::Pass2(SymbolTable &symtab, const opcodes::OpcodeMap &opcodes)
        : Pass(symtab, opcodes)
    {
    }

    /**
     * Execute pass2. This pass just computes location counter values,
     * defines symbols, and reports basic errors. Actual code generation
     * happens in pass 2.
     */
    void Pass2::pass2(vector<unique_ptr<ast::Node>> &ast)
    {
        loc_ = 0;
        std::fill(image_.begin(), image_.end(), -1);

        for (auto &node : ast) {
            try {
                node->pass2(*this);
                node->setNextLoc(loc_);
            } catch (Error &ex) {
                bool warning = ex.type() == ErrorType::Warning;
                pushMessage(Message{ warning, node->line(), ex.message() });
            }
        }
    }

    /**
     * Returns the assembled image. Bytes which have no data
     * are marked with -1.
     */
    const array<int, 65536> &Pass2::image() const
    {
        return image_;
    }

    /**
     * Add a byte to the assembled image at the current location
     * counter, and increment the location counter.
     */
    void Pass2::emit(unsigned byte)
    {
        if (loc_ < 0 || loc_ >= 0xffff) {
            ss err{};
            err
                << "Attempt to store data outside the addressing range of "
                << "$0000-$FFFF. Location counter is $"
                << std::hex << std::setw(8) << std::setfill('0') << loc_
                << ".";
            throw Error{ err.str() };
        }

        image_[loc_++] = byte & 0xFF;
    }

    /**
     * Evaluate the given expression and throw an exception
     * if there are any undefined symbols; else return the
     * integer value of the expression.
     */
    int Pass2::evalCheckDefined(ast::Expression &expr)
    {
        ast::ExprResult er = expr.eval(symtab_);
        if (!er.defined()) {
            // All symbols must be fully defined in pass 2.
            ss err{};
            err
                << "Symbols '"
                << concatSet(er.undefinedSymbols(), "', '")
                << "' are undefined in instruction operand.";
            throw Error{ err.str() };
        }
        return er.value();
    }

    /**
     * Check that the given value is in the range of a signed
     * or unsigned byte.
     */ 
    void Pass2::checkByte(int value)
    {
        if (value >= -128 && value <= 255) {
            return;
        }

        ss err{};
        err
            << "Operand value "
            << value
            << " should fit in one byte; truncated.";
            throw Error{ err.str(), ErrorType::Warning };
    }

    namespace ast
    {
        /**
         * Pass 2 for the base node class
         */
        void Node::pass2(Pass2 &pass2)
        {
        }

        /**
         * Pass 2 for setting the location counter 
         */
        void OrgNode::pass2(Pass2 &pass2)
        {
            Node::pass2(pass2);

            int value = pass2.evalCheckDefined(*locExpr_);
            
            // The expression was fully defined in pass 1, 
            // so sanity check that it hasn't changed.
            if (value != computedLoc_) {
                throw Error{ "ORG expression has a different value in pass 2." };
            }

            pass2.setLoc(computedLoc_);
        }

        /**
         * Pass 2 for setting a symbol value
         */
        void SetNode::pass2(Pass2 &pass2)
        {
            Node::pass2(pass2);

            // NB the symbol table will throw an error if the value
            // unexpectedly changed.
            pass2.symtab().setValue(symbol_, pass2.evalCheckDefined(*value_));
        }

        namespace
        {
            // helpers for InstructionNode

            /** 
             * Convert addressing mode to string
             */
            string modeName(opcodes::AddrMode mode) {
                switch (mode) {
                case opcodes::AddrMode::Accumulator: return "accumulator";
                case opcodes::AddrMode::Immediate:   return "immediate";
                case opcodes::AddrMode::Implied:     return "implied";
                case opcodes::AddrMode::ZeroPage:    return "zero page";
                case opcodes::AddrMode::ZeroPageX:   return "zero page,x";
                case opcodes::AddrMode::ZeroPageY:   return "zero page,y";
                case opcodes::AddrMode::Absolute:    return "absolute";
                case opcodes::AddrMode::AbsoluteX:   return "absolute,x";
                case opcodes::AddrMode::AbsoluteY:   return "absolute,x";
                case opcodes::AddrMode::Indirect:    return "indirect";
                case opcodes::AddrMode::IndirectX:   return "indirect,x";
                case opcodes::AddrMode::IndirectY:   return "indirect,y";
                case opcodes::AddrMode::Relative:    return "relative";
                }
                return "";
            }
            
            /**
             * Get an opcode for an instruction, and throw
             * an exception if it doesn't exist.
             */
            const opcodes::Encoding &ensureEncoding(const opcodes::Instruction &instr, opcodes::AddrMode mode)
            {
                const opcodes::Encoding &enc = instr.encoding(mode);
                if (!enc.exists()) {
                    ss err{};
                    err 
                        << "Instruction `"
                        << instr.mnemonic()
                        << "' has no "
                        << modeName(mode)
                        << " mode.";
                    throw Error{ err.str() };
                }
                return enc;
            }
        }

        /**
         * Pass 2 for an instruction
         */
        void InstructionNode::pass2(Pass2 &pass2)
        {
            Node::pass2(pass2);

            // We should have all the information we need to build instructions.
            // Note that we did not check everything to do with encodings in pass 1,
            // just enough to determine relative branches and if zero page was
            // an option, so we have to check available address mode encodings here.
            //
            const opcodes::Instruction &instr = pass2.findInstruction(opcode_);

            int value = 0;


            const opcodes::Encoding *enc = nullptr;

            // TODO I'm not a fan of this big switch nor the matching
            // one in pass 1.
            switch (address_->mode()) {
            case AddrMode::Implied:
                enc = &ensureEncoding(instr, opcodes::AddrMode::Implied);
                pass2.emit(enc->opcode());
                break;

            case AddrMode::Accumulator:
                enc = &ensureEncoding(instr, opcodes::AddrMode::Accumulator);
                pass2.emit(enc->opcode());
                break;

            case AddrMode::Immediate:
                value = pass2.evalCheckDefined(*address_->addressExpr());
                enc = &ensureEncoding(instr, opcodes::AddrMode::Immediate);
                pass2.emit(enc->opcode());
                pass2.emit(value);

                // NB do this checks after emit() because they throw
                // a warning so we want assembly to still succeed
                pass2.checkByte(value);
                break;

            case AddrMode::Address:
                {
                    value = pass2.evalCheckDefined(*address_->addressExpr());
                    if (instr.hasEncoding(opcodes::AddrMode::Relative)) {
                        enc = &instr.encoding(opcodes::AddrMode::Relative);

                        int delta = value - (pass2.loc() + 2);

                        pass2.emit(enc->opcode());
                        pass2.emit(delta);

                        if (delta < -128 || delta > 127) {
                            throw Error{ "Relative branch is out of range." };
                        }
                        break;
                    }

                    if (operandSize_ == DataSize::Byte) {
                        enc = &ensureEncoding(instr, opcodes::AddrMode::ZeroPage);
                        pass2.emit(enc->opcode());
                        pass2.emit(value);
                        break;
                    }
                    
                    enc = &ensureEncoding(instr, opcodes::AddrMode::Absolute);
                    pass2.emit(enc->opcode());
                    pass2.emit(value & 0xFF);
                    pass2.emit(value >> 8);
                    break;
                }

            case AddrMode::AddressX:
            case AddrMode::AddressY:
                {
                    value = pass2.evalCheckDefined(*address_->addressExpr());
                    bool isX = address_->mode() == AddrMode::AddressX;
                    unsigned op = -1;

                    // This whole bit of logic is unfortunately complex.
                    // There are a couple of opcodes which have zero page,[xy]
                    // modes that do NOT have a matching abs,[xy] mode.
                    //
                    if (operandSize_ == DataSize::Byte) {
                        if (isX) {
                            enc = &ensureEncoding(instr, opcodes::AddrMode::ZeroPageX);
                        } else {
                            enc = &ensureEncoding(instr, opcodes::AddrMode::ZeroPageY);
                        }
                    } else {
                        if (isX) {
                            if (!instr.hasEncoding(opcodes::AddrMode::AbsoluteX)) {
                                enc = &ensureEncoding(instr, opcodes::AddrMode::ZeroPageX);

                                if (value < -127 || value > 255) {
                                    ss err{};
                                    err
                                        << "There is no absolute,x mode for this instruction, "
                                        << "and the address is not in zero page.";
                                    throw Error{ err.str() };
                                }

                                operandSize_ = DataSize::Byte;
                            } else {
                                enc = &ensureEncoding(instr, opcodes::AddrMode::AbsoluteX);
                            }
                        } else {
                            if (!instr.hasEncoding(opcodes::AddrMode::AbsoluteY)) {
                                enc = &ensureEncoding(instr, opcodes::AddrMode::ZeroPageX);

                                if (value < -127 || value > 255) {
                                    ss err{};
                                    err
                                        << "There is no absolute,y mode for this instruction, "
                                        << "and the address is not in zero page.";
                                    throw Error{ err.str() };
                                }

                                operandSize_ = DataSize::Byte;
                            } else {
                                enc = &ensureEncoding(instr, opcodes::AddrMode::AbsoluteY);
                            }
                        }
                    }

                    pass2.emit(enc->opcode());
                    pass2.emit(value & 0xFF);
                    if (operandSize_ == DataSize::Word) {
                        pass2.emit(value >> 8);
                    }
                }
                break;

            case AddrMode::Indirect:
                value = pass2.evalCheckDefined(*address_->addressExpr());
                enc = &ensureEncoding(instr, opcodes::AddrMode::Indirect);
                pass2.emit(enc->opcode());
                pass2.emit(value & 0xFF);
                pass2.emit(value >> 8);
                break;

            case AddrMode::IndirectX:
            case AddrMode::IndirectY:
                int op = -1;
                if (address_->mode() == AddrMode::IndirectX) {
                    enc = &ensureEncoding(instr, opcodes::AddrMode::IndirectX);
                } else {
                    enc = &ensureEncoding(instr, opcodes::AddrMode::IndirectY);
                }

                value = pass2.evalCheckDefined(*address_->addressExpr());

                pass2.emit(enc->opcode());
                pass2.emit(value & 0xFF);

                if (value < 0 || value > 0xFF) {
                    throw Error{ "Address is not in zero page." };
                }
                break;
            }

            if (enc != nullptr) {
                clockCycles_ = enc->clocks();
                hasExtraClockCycles_ = enc->extraClocks();
                undocumented_ = enc->undocumented();
                unstable_ = enc->unstable();
            }
        }

        /**
         * Pass 2 for data declarations
         */
        void DataNode::pass2(Pass2 &pass2)
        {
            Node::pass2(pass2);

            for (const auto &expr : data_) {
                int value = pass2.evalCheckDefined(*expr);

                pass2.emit(value & 0xFF);
                if (size_ == DataSize::Byte) {
                    pass2.checkByte(value);
                } else {
                    pass2.emit(value >> 8);
                }
            } 
        }
    }
}
