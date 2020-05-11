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
#ifndef AST_H_
#define AST_H_

#include <iostream>
#include <memory>
#include <vector>

namespace yas6502
{
    namespace ast
    {
        class Expression;
        using ExpressionPtr = std::unique_ptr<Expression>;

        class Address;
        using AddressPtr = std::unique_ptr<Address>;

        class Node
        {
        public:
            virtual ~Node();

            void setLabel(const std::string &label);
            void setComment(const std::string &comment);

            std::string str();

        protected:
            virtual std::string toString() = 0;

            std::string label_;
            std::string comment_;
        };

        using NodePtr = std::unique_ptr<Node>;

        class NoopNode : public Node
        {
        public:

        protected:
            virtual std::string toString() override;
        };

        enum class DataSize
        {
            Byte,
            Word,
        };

        class DataNode : public Node
        {
        public:
            DataNode(DataSize size, std::vector<ExpressionPtr> &&data);

            virtual std::string toString() override;

        private:
            DataSize size_;
            std::vector<ExpressionPtr> data_;
        };

        class InstructionNode : public Node
        {
        public:
            InstructionNode(std::string &opcode, AddressPtr address);

            virtual std::string toString() override;

        private:
            std::string opcode_;
            AddressPtr address_;
        };

        class OrgNode : public Node
        {
        public:
            OrgNode(ExpressionPtr loc);

            virtual std::string toString() override;

        private:
            ExpressionPtr loc_;
        };

        class SetNode : public Node
        {
        public:
            SetNode(const std::string &symbol, ExpressionPtr value);

            virtual std::string toString() override;

        private:
            std::string symbol_;
            ExpressionPtr value_;
        };

        enum class IndexReg
        {
            None,
            X,
            Y,
        };

        enum class AddrMode
        {
            Implied,
            Immediate,
            Accumulator,
            Address,
            AddressX,
            AddressY,
            Indirect,
            IndirectX,
            IndirectY,
        };

        inline AddrMode address(IndexReg idx)
        {
            switch (idx) {
            case IndexReg::None: return AddrMode::Address;
            case IndexReg::X: return AddrMode::AddressX;
            case IndexReg::Y: return AddrMode::AddressY;
            }
        }

        inline AddrMode indirect(IndexReg idx)
        {
            switch (idx) {
            case IndexReg::None: return AddrMode::Indirect;
            case IndexReg::X: return AddrMode::IndirectX;
            case IndexReg::Y: return AddrMode::IndirectY;
            }
        }

        enum class Operator
        {
            Add,
            Sub,
            Mul,
            Div,
            Neg,
        };

        class Expression
        {
        public:
            virtual ~Expression();

            virtual std::string str() = 0;
        };

        class Address
        {
        public:
            Address(AddrMode mode, ExpressionPtr address);

            std::string str();

        private:
            AddrMode mode_;
            ExpressionPtr address_;
        };

        
        class UnaryOp : public Expression
        {
        public:
            UnaryOp(Operator op, ExpressionPtr operand);

            virtual std::string str() override;

        private:
            Operator op_;
            ExpressionPtr operand_;
        };

        class BinaryOp : public Expression
        {
        public:
            BinaryOp(Operator op, ExpressionPtr left, ExpressionPtr right);

            virtual std::string str() override;

        private:
            Operator op_;
            ExpressionPtr left_;
            ExpressionPtr right_;
        };
        
        class SymbolExpression : public Expression
        {
        public:
            SymbolExpression(const std::string &symbol);

            virtual std::string str() override;

        private:
            const std::string symbol_;
        };

        class ConstantExpression : public Expression
        {
        public:
            ConstantExpression(int value);

            virtual std::string str() override;

        private:
            int value_;
        };
    }
}

extern std::ostream &operator <<(std::ostream &, const yas6502::ast::Expression &);

#endif

