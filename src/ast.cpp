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

#include <algorithm>
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
         * Constructor
         */
        Node::Node()
            : line_(0)
            , loc_(0)
        {
        }

        /**
         * Destructor
         */
        Node::~Node()
        {
        }

        /**
         * Set the line number where the line was parsed.
         */
        void Node::setLine(int line)
        {
            line_ = line;
        }

        /**
         * Set the location counter at the start of this
         * line.
         */
        void Node::setLoc(int loc)
        {
            loc_ = loc;
        }

        /**
         * Set the location counter as of the node emitting its data.
         */
        void Node::setNextLoc(int loc)
        {
            nextLoc_ = loc;
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
         * Return the source file line containing this node.
         */
        int Node::line() const
        {
            return line_;
        }

        /**
         * Return the location counter associated with this node.
         */
        int Node::loc() const
        {
            return loc_;
        }

        /**
         * Return the number of bytes emitted into the assembly image
         * for this node.
         */
        int Node::length() const
        {
            return nextLoc_ - loc_;
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
         * Construct an instruction node
         */
        InstructionNode::InstructionNode(std::string &opcode, AddressPtr address)
            : opcode_(opcode)
            , address_(std::move(address))
            , operandSize_(DataSize::Byte)
        {
        }

        /**
         * Construct an ORG node, which sets the location counter.
         */
        OrgNode::OrgNode(ExpressionPtr locExpr)
            : locExpr_(std::move(locExpr))
            , computedLoc_(0)
        {
        }

        /**
         * Override length; the org node changes the location counter
         * but emits no data.
         */
        int OrgNode::length() const
        {
            return 0;
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
         * Override length; the set node emits no data.
         */
        int SetNode::length() const
        {
            return 0;
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

        /**
         * Get the addressing mode
         */
        AddrMode Address::mode() const
        {
            return mode_;
        }

        /**
         * Get the address expression. Can be nullptr for
         * some modes.
         */
        Expression *Address::addressExpr() const
        {
            return address_.get();
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
         * Construct a binary operator
         */
        BinaryOp::BinaryOp(Operator op, ExpressionPtr left, ExpressionPtr right)
            : op_(op)
            , left_(std::move(left))
            , right_(std::move(right))
        {
        }

        /**
         * Construct a symbol expression
         */
        SymbolExpression::SymbolExpression(const std::string &symbol)
            : symbol_(symbol)
        {
        }

        /**
         * Construct a constant expression
         */
        ConstantExpression::ConstantExpression(int value)
            : value_(value)
        {
        }
    }
}
