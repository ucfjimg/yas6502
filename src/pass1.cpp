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
#include "pass1.h"

#include "except.h"

#include "ast.h"
#include "symtab.h"
#include "utility.h"

#include <cassert>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

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
    Pass1::Pass1(SymbolTable &symtab, const opcodes::OpcodeMap &opcodes)
        : Pass(symtab, opcodes)
    {
    }

    /**
     * Execute pass1. This pass just computes location counter values,
     * defines symbols, and reports basic errors. Actual code generation
     * happens in pass 2.
     */
    void Pass1::pass1(vector<unique_ptr<ast::Node>> &ast)
    {
        for (auto &node : ast) {
            try {
                node->setLoc(loc_);
                node->pass1(*this);
            } catch (Error &ex) {
                bool warning = ex.type() == ErrorType::Warning;
                pushMessage(Message{ warning, node->line(), ex.message() });
            }
        }
    }

    namespace ast
    {
        /**
         * Base class just needs to set the label's value, if there
         * is one.
         */
        void Node::pass1(Pass1 &pass1)
        {
            if (label_.empty()) {
                return;
            }

            pass1.symtab().setValue(label_, pass1.loc());
        }

        /**
         * ORG node sets the location counter.
         */
        void OrgNode::pass1(Pass1 &pass1)
        {
            Node::pass1(pass1);

            ExprResult er = locExpr_->eval(pass1);
            if (!er.defined()) {
               ss err{};
               err
                   << "ORG expression must be fully defined in pass1, but contains undefined symbols '"
                   << concatSet(er.undefinedSymbols(), "', '")
                   << "'.";
                pass1.pushMessage(Message{ false, line(), err.str() });
            }

            computedLoc_ = er.value();
            pass1.setLoc(computedLoc_);
        }

        /**
         * SET node sets a symbol value.
         */
        void SetNode::pass1(Pass1 &pass1)
        {
            Node::pass1(pass1);

            // It's ok for a symbol to not be fully defined in pass 1.
            // 
            ExprResult er = value_->eval(pass1);
            if (!er.defined()) {
                return;
            }
            pass1.symtab().setValue(symbol_, er.value());
        }

        /**
         * Instruction node assembles an instruction.
         */
        void InstructionNode::pass1(Pass1 &pass1)
        {
            Node::pass1(pass1);

            if (address_->addressExpr() != nullptr && address_->addressExpr()->parenthesized()) {
                pass1.pushMessage(Message{ 
                    true, 
                    line(), 
                    "Top level expression is parenthesized, did you mean to use brackets for indirect addressing?"
                });
            }

            // for an instruction, we need to update the location
            // counter, but we also need to remember if we decided
            // to use a zero page or absolute address. we can't
            // choose zero page if the address expression is not
            // defined, as we can't guarantee it'll fit in 8 bits
            // in pass 2. this means we could have a sub-optimal
            // encoding due to a forward symbol reference but that
            // case will be rare.
            //
            
            int size = 0;
            ExprResult er{ 1 };
            
            const auto &instr = pass1.findInstruction(opcode_);

            switch (address_->mode()) {
            case AddrMode::Implied:
            case AddrMode::Accumulator:
                size = 1;
                break;

            case AddrMode::Immediate:
                size = 2;
                break;

            case AddrMode::Address: 
                if (instr.hasEncoding(opcodes::AddrMode::Relative)) {
                    // relative branch is always a 2-byte instruction
                    size = 2;
                    break;
                } 

                size = 3;

                // See if we can fit in zero page. Must have an encoding,
                // and must have a fully defined operand that fits in
                // one byte.
                if (instr.hasEncoding(opcodes::AddrMode::ZeroPage)) {
                    er = address_->addressExpr()->eval(pass1);
                    if (er.defined() && er.value() >= 0 && er.value() <= 0xFF) {
                        size = 2;
                    }
                }
                break;
                
            case AddrMode::AddressX:
            case AddrMode::AddressY: 
                // See if we can fit in zero page. Must have an encoding,
                // and must have a fully defined operand that fits in
                // one byte.
                {
                    size = 3;

                    bool hasZeroPage =
                        (address_->mode() == AddrMode::AddressX && instr.hasEncoding(opcodes::AddrMode::ZeroPageX)) ||
                        (address_->mode() == AddrMode::AddressY && instr.hasEncoding(opcodes::AddrMode::ZeroPageY));

                    if (hasZeroPage) {
                        er = address_->addressExpr()->eval(pass1);
                        if (er.defined() && er.value() >= 0 && er.value() <= 0xFF) {
                            size = 2;
                        }
                    }
                }
                break;

            case AddrMode::Indirect:
                // Indirect is always a word operand
                size = 3;
                break;

            case AddrMode::IndirectX:
            case AddrMode::IndirectY:
                // Indirect X and Y are implicitly zero page instructions
                size = 2;
                break;
            }

            if (size == 3) {
                operandSize_ = DataSize::Word;
            } else {
                operandSize_ = DataSize::Byte;
            }

            pass1.setLoc(pass1.loc() + size);
        }

        /**
         * Data nodes stores bytes or words.
         */
        void DataNode::pass1(Pass1 &pass1)
        {
            Node::pass1(pass1);

            // in pass 1, all we need to do is update the location
            // counter.
            //
            int size = (size_ == DataSize::Byte) ? 1 : 2;

            int elements = 0;
            for (const auto &de : data_) {
                int count = 1;
                if (de->count != nullptr) {
                    ExprResult er = de->count->eval(pass1);
                    if (!er.defined()) {
                        ss err{};
                        err 
                            << "REP count expression must be fully defined in pass 1, but contains undefined symbols '"
                            << concatSet(er.undefinedSymbols(), "', '")
                            << "'.";
                        pass1.pushMessage(Message{ false, line(), err.str() });
                        continue;
                    }
                    if (er.value() < 1) {
                        pass1.pushMessage(Message{ false, line(), "REP count expression must be positive." });
                        continue;
                    }
                    count = er.value();
                }
                elements += count; 
            }

            size *= elements;
            pass1.setLoc(pass1.loc() + size);
        }
            
        /**
         * Reserve space.
         */
        void SpaceNode::pass1(Pass1 &pass1)
        {
            int size = (size_ == DataSize::Byte) ? 1 : 2;
            ExprResult er = count_->eval(pass1);
            if (!er.defined()) {
               ss err{};
               err
                   << "SPACE expression must be fully defined in pass 1, but contains undefined symbols '"
                   << concatSet(er.undefinedSymbols(), "', '")
                   << "'.";
                pass1.pushMessage(Message{ false, line(), err.str() });
            }

            pass1.setLoc(pass1.loc() + size * er.value());
        }
    }
}

