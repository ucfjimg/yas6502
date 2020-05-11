#include "pass1.h"

#include "ast.h"
#include "opcodes.h"
#include "symtab.h"
#include "utility.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <stdexcept>

using std::cerr;
using std::endl;
using std::make_unique;
using std::runtime_error;
using std::string;
using std::unique_ptr;
using std::vector;

using ss = std::stringstream;

namespace yas6502
{
    /**
     * Constructor
     */
    Pass1::Pass1(SymbolTable &symtab, const OpcodeMap &opcodes)
        : symtab_(symtab)
        , opcodes_(opcodes)
        , loc_(0)
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
            } catch (runtime_error &ex) {
                // TODO carry along location information
                cerr << ex.what() << endl; 
            }
        }
    }

    /**
     * Return the current location counter.
     */
    int Pass1::loc() const
    {
        return loc_;
    }

    /**
     * Sets the current location counter.
     */
    void Pass1::setLoc(int loc)
    {
        if (loc > 0xFFFF) {
            throw runtime_error{ "Location counter cannot exceed $FFFF." };
        } else if (loc < 0) {
            throw runtime_error{ "Location counter cannot be negative." };
        }

        loc_ = loc;
    }

    /**
     * Look up an opcode in the opcode map. Case insensitive.
     * Returns nullptr if there is no such opcode.
     */
    unique_ptr<Opcode> Pass1::findOpcode(const string &op)
    {
        auto it = opcodes_.find(toUpper(op));
        if (it == opcodes_.end()) {
            return nullptr;
        }

        return make_unique<Opcode>( it->second ); 
    }

    /**
     * Return the symbol table.
     */
    SymbolTable &Pass1::symtab()
    {
        return symtab_;
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

            ExprResult er = loc_->eval(pass1.symtab());
            if (!er.defined()) {
               ss err{};
               err
                   << "ORG expression must be fully defined in pass1, but contains undefined symbols '"
                   << concatSet(er.undefinedSymbols(), "', '")
                   << "'.";
               throw runtime_error{ err.str() };
            }

            pass1.setLoc(er.value());
        }

        /**
         * SET node sets a symbol value.
         */
        void SetNode::pass1(Pass1 &pass1)
        {
            Node::pass1(pass1);

            // It's ok for a symbol to not be fully defined in pass 1.
            // 
            ExprResult er = value_->eval(pass1.symtab());
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
            
            auto opcode = pass1.findOpcode(opcode_);
            assert(opcode.get() != nullptr);

            switch (address_->mode()) {
            case AddrMode::Implied:
            case AddrMode::Accumulator:
                size = 1;
                break;

            case AddrMode::Immediate:
                size = 2;
                break;

            case AddrMode::Address: 
                if (opcode->relative != -1) {
                    // relative branch is always a 2-byte instruction
                    size = 2;
                    break;
                } 

                size = 3;

                // See if we can fit in zero page. Must have an encoding,
                // and must have a fully defined operand that fits in
                // one byte.
                if (opcode->zeroPage != -1) {
                    er = address_->addressExpr()->eval(pass1.symtab());
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
                        (address_->mode() == AddrMode::AddressX && opcode->zeroPageX != -1) ||
                        (address_->mode() == AddrMode::AddressY && opcode->zeroPageY != -1);

                    if (hasZeroPage) {
                        er = address_->addressExpr()->eval(pass1.symtab());
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
            size *= data_.size();
            pass1.setLoc(pass1.loc() + size);
        }
    }
}

