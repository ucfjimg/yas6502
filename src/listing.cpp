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
#include <string>
#include <vector>

using std::string;
using std::vector;

using ss = std::stringstream;

namespace yas6502
{
    namespace ast 
    {
        /**
         * Return default string for node attributes 
         */
        string Node::attributes() const
        {
            return "";
        }

        /**
         * Convert this line to a string. This is the public 
         * interface that handles the fields every node has, and
         * toString() is overridden by each subclass to convert
         * their data.
         */
        vector<string> Node::str(const Image &image)
        {
            vector<string> lines{};

            ss line{};

            line
                << std::setw(5) << line_ << " "
                << std::setw(4) << std::hex << std::setfill('0') << std::uppercase << loc_
                << "  ";

            const int MAX_BYTES = 5;
            int bytes = std::min(MAX_BYTES, length());

            int i = 0;
            for (; i < bytes; i++) {
                line << std::setw(2) << std::setfill('0') << std::hex << (int)image[loc_ + i] << " "; 
            }

            for (; i < MAX_BYTES; i++) {
                line << "   ";
            }

            line 
                << std::setfill(' ')
                << std::setw(8) << attributes()
                << " ";

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

            lines.push_back(line.str());

            // If the statement emitted more bytes than we put in one line, add the rest
            // as more lines
            //
            int bytesLeft = length() - bytes;
            int addr = loc_ + bytes;

            while (bytesLeft) {
                int n = std::min(MAX_BYTES, bytesLeft);

                ss line{};

                line
                    << std::uppercase
                    << std::setw(5) << std::dec << std::setfill(' ') << line_ << " "
                    << std::setw(4) << std::hex << std::setfill('0') << addr
                    << "  "
                    << std::hex << std::setfill('0');

                for (int i = 0; i < n; i++) {
                    line 
                        << std::setw(2) << (int)image[addr++] << " "; 
                }

                bytesLeft -= n;
                lines.push_back(line.str());
            }

            return lines;
        }

        /**
         * Placeholder node with no operation
         */
        string NoopNode::toString()
        {
            return "";
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
         * Return the clock cycles description
         */
        string InstructionNode::attributes() const
        {
            ss text{};
            text 
                << clockCycles_
                << (hasExtraClockCycles_ ? '+' : ' ')
                << " "
                << (undocumented_ ? 'U' : ' ')
                << (unstable_ ? 'S' : ' ');

            return text.str();
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
         * Convert to string
         */
        string OrgNode::toString()
        {
            ss line{};

            line << "ORG " << locExpr_->str();

            return line.str(); 
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
         * Convert an instruction operand to a string
         */
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
         * Convert a symbol expression to a string. 
         */
        string SymbolExpression::str()
        {
            return symbol_;
        }

        /**
         * Convert a constant expression to a string. TODO this 
         * should be in the same form as the original token.
         */
        string ConstantExpression::str()
        {
            ss line{};

            line << '$' << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << value_;
            return line.str();
        }
    }
}
