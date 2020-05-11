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

#include "ast.h"

#include <iomanip>
#include <sstream>

using std::string;
using std::unique_ptr;
using std::vector;

using ss = std::stringstream;

namespace yas6502
{
    namespace ast
    {
        /**
         * Destructor
         */
        Node::~Node()
        {
        }

        /**
         * Set the label for this line.
         */
        void Node::setLabel(const std::string &label)
        {
            label_ = label;
        }

        /**
         * Set the comment for this line.
         */
        void Node::setComment(const std::string &comment)
        {
            comment_ = comment;
        }

        /**
         * Convert this line to a string. This is the public 
         * interface that handles the fields every node has, and
         * toString() is overridden by each subclass to convert
         * their data.
         */
        string Node::str()
        {
           ss line{};

           if (!label_.empty()) {
               line << std::setw(9) << std::left << (label_ + ":");
           } else {
               line << std::setw(9) << " ";
           }
           
           line 
               << "  "
               << std::setw(20)
               << std::left
               << toString()
               << comment_;
           
            return line.str();
        }

        /**
         * Placeholder node with no operation
         */
        string NoopNode::toString()
        {
            return "";
        }

        /**
         * Construct an initialized data node
         */
        DataNode::DataNode(DataSize size, vector<ExpressionPtr> &&data)
            : size_(size)
            , data_(std::move(data))
        {
        }

        /**
         * Convert to string
         */
        string DataNode::toString()
        {
            ss line{};

            line
                << (size_ == DataSize::Byte ? "BYTE " : "WORD ");

            for (unsigned i = 0; i < data_.size(); i++) {
                line << data_[i]->str();
                if (i < data_.size() - 1) {
                    line << ", ";
                }
            }

            return line.str();
        }

        /**
         * Construct an instruction node
         */
        InstructionNode::InstructionNode(std::string &opcode, AddressPtr address)
            : opcode_(opcode)
            , address_(std::move(address))
        {
        }

        /**
         * Convert to string
         */
        string InstructionNode::toString()
        {
            ss line{};

            line << opcode_ << " " << address_->str();

            return line.str(); 
        }

        /**
         * Construct an ORG node, which sets the location counter.
         */
        OrgNode::OrgNode(ExpressionPtr loc)
            : loc_(std::move(loc))
        {
        }

        /**
         * Convert to string
         */
        string OrgNode::toString()
        {
            ss line{};

            line << "ORG " << loc_->str();

            return line.str(); 
        }

        /**
         * Construct a symbol assignment node
         */
        SetNode::SetNode(const std::string &symbol, ExpressionPtr value)
            : symbol_(symbol)
            , value_(std::move(value))
        {
        }

        /**
         * Convert to string
         */
        string SetNode::toString()
        {
            ss line{};

            line << "SET " << symbol_ << " = " << value_->str();

            return line.str(); 
        }
        
        /**
         * Base class destructor
         */
        Expression::~Expression()
        {
        }

        /**
         * Construct an address
         */
        Address::Address(AddrMode mode, ExpressionPtr address)
            : mode_(mode)
            , address_(std::move(address))
        {
        }

        string Address::str()
        {
            ss line{};

            switch (mode_) {
            case AddrMode::Implied:
                break;

            case AddrMode::Immediate:
                line << '#' << address_->str();
                break;

            case AddrMode::Accumulator:
                line << "A";
                break;

            case AddrMode::Address:
                line << address_->str();
                break;

            case AddrMode::AddressX:
                line << address_->str() << ",X";
                break;

            case AddrMode::AddressY:
                line << address_->str() << ",Y";
                break;

            case AddrMode::Indirect:
                line << '(' << address_->str() << ')';
                break;

            case AddrMode::IndirectX:
                line << '(' << address_->str() << "),X";
                break;

            case AddrMode::IndirectY:
                line << '(' << address_->str() << "),Y";
                break;
            }

            return line.str();
        }

        namespace
        {
            string operatorToStr(Operator op)
            {
                switch (op) {
                case Operator::Add: return "+";
                case Operator::Sub: return "-";
                case Operator::Mul: return "*";
                case Operator::Div: return "/";
                case Operator::Neg: return "-";
                }

                return "?";
            }
        }

        /**
         * Construct a unary operator
         */
        UnaryOp::UnaryOp(Operator op, ExpressionPtr operand)
            : op_(op)
            , operand_(std::move(operand))
        {
        }

        /**
         * Convert a unary operator to a string. 
         */
        string UnaryOp::str()
        {
            ss line{};

            line
                << operatorToStr(op_)
                << operand_->str();

            return line.str();
        }

        /**
         * Construct a binary operator
         */
        BinaryOp::BinaryOp(Operator op, ExpressionPtr left, ExpressionPtr right)
            : op_(op)
            , left_(std::move(left))
            , right_(std::move(right))
        {
        }

        /**
         * Convert a binary operator to a string. 
         */
        string BinaryOp::str()
        {
            ss line{};

            line
                << left_->str()
                << operatorToStr(op_)
                << right_->str();

            return line.str();
        }

        /**
         * Construct a symbol expression
         */
        SymbolExpression::SymbolExpression(const std::string &symbol)
            : symbol_(symbol)
        {
        }

        /**
         * Convert a symbol expression to a string. 
         */
        string SymbolExpression::str()
        {
            return symbol_;
        }

        /**
         * Construct a constant expression
         */
        ConstantExpression::ConstantExpression(int value)
            : value_(value)
        {
        }

        /**
         * Convert a constant expression to a string. TODO this 
         * should be in the same form as the original token.
         */
        string ConstantExpression::str()
        {
            ss line{};

            line << '$' << std::setfill('0') << std::setw(4) << value_;
            return line.str();
        }
    }
}
