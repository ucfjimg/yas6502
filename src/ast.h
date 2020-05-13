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

#include <array>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace yas6502
{
    class Pass;
    class Pass1;
    class Pass2;

    using Image = std::array<int, 65536>;

    namespace ast
    {
        class Expression;
        using ExpressionPtr = std::unique_ptr<Expression>;

        class Address;
        using AddressPtr = std::unique_ptr<Address>;

        class Node
        {
        public:
            Node();
            virtual ~Node();

            void setLine(int line);
            void setLoc(int loc);
            void setNextLoc(int loc);
            void setLabel(const std::string &label);
            void setComment(const std::string &comment);

            int line() const;
            int loc() const;
            virtual int length() const;
            virtual std::string attributes() const;

            std::vector<std::string> str(const Image &image);

            virtual void pass1(Pass1 &pass1);
            virtual void pass2(Pass2 &pass2);

        protected:
            virtual std::string toString() = 0;

            int line_;
            int loc_;
            int nextLoc_;  // the location of the following instruction
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

            virtual void pass1(Pass1 &pass1) override;
            virtual void pass2(Pass2 &pass2) override;
            virtual std::string toString() override;

        private:
            DataSize size_;
            std::vector<ExpressionPtr> data_;
        };

        class InstructionNode : public Node
        {
        public:
            InstructionNode(std::string &opcode, AddressPtr address);

            virtual void pass1(Pass1 &pass1) override;
            virtual void pass2(Pass2 &pass2) override;
            virtual std::string toString() override;

            virtual std::string attributes() const override;

        private:
            std::string opcode_;
            AddressPtr address_;
            int clockCycles_;
            bool hasExtraClockCycles_;
            bool undocumented_;
            bool unstable_;

            // Computed in pass 1
            //
            DataSize operandSize_;
        };

        class OrgNode : public Node
        {
        public:
            OrgNode(ExpressionPtr locExpr);

            virtual void pass1(Pass1 &pass1) override;
            virtual void pass2(Pass2 &pass2) override;
            virtual int length() const;
            virtual std::string toString() override;

        private:
            ExpressionPtr locExpr_;
            int computedLoc_;
        };

        class SetNode : public Node
        {
        public:
            SetNode(const std::string &symbol, ExpressionPtr value);

            virtual void pass1(Pass1 &pass1) override;
            virtual void pass2(Pass2 &pass2) override;
            virtual int length() const;
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

        class ExprResult
        {
        public:
            ExprResult(int value);
            ExprResult(std::set<std::string> &&undefined);

            bool defined() const;
            int value() const;
            const std::set<std::string> &undefinedSymbols() const;

        private:
            int value_;
            std::set<std::string> undefinedSymbols_;
        };

        class Expression
        {
        public:
            Expression();
            virtual ~Expression();

            bool parenthesized() const;
            void setParenthesized();

            virtual std::string str() = 0;
            virtual ExprResult eval(Pass &pass) = 0;

        private:
            bool parenthesized_;
        };

        class Address
        {
        public:
            Address(AddrMode mode, ExpressionPtr address);

            std::string str();

            AddrMode mode() const;
            Expression *addressExpr() const;

        private:
            AddrMode mode_;
            ExpressionPtr address_;
        };

        
        class UnaryOp : public Expression
        {
        public:
            UnaryOp(Operator op, ExpressionPtr operand);

            virtual std::string str() override;
            virtual ExprResult eval(Pass &pass) override;

        private:
            Operator op_;
            ExpressionPtr operand_;
        };

        class BinaryOp : public Expression
        {
        public:
            BinaryOp(Operator op, ExpressionPtr left, ExpressionPtr right);

            virtual std::string str() override;
            virtual ExprResult eval(Pass &pass) override;

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
            virtual ExprResult eval(Pass &pass) override;

        private:
            const std::string symbol_;
        };

        class ConstantExpression : public Expression
        {
        public:
            ConstantExpression(int value);

            virtual std::string str() override;
            virtual ExprResult eval(Pass &pass) override;

        private:
            int value_;
        };

        class LocationExpression : public Expression
        {
        public:
            virtual std::string str() override;
            virtual ExprResult eval(Pass &pass) override;
        };
    }
}

extern std::ostream &operator <<(std::ostream &, const yas6502::ast::Expression &);

#endif

