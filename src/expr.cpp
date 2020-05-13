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

#include "except.h"
#include "pass.h"
#include "symtab.h"

#include <algorithm>
#include <cassert>
#include <stdexcept>

using std::set;
using std::string;

using yas6502::ast::BinaryOp;
using yas6502::ast::ConstantExpression;
using yas6502::ast::SymbolExpression;
using yas6502::ast::LocationExpression;
using yas6502::ast::ExprResult;
using yas6502::ast::Operator;
using yas6502::ast::UnaryOp;

namespace yas6502
{
    /**
     * Construct a defined exppression result
     */
    ExprResult::ExprResult(int value)
        : value_(value)
    {
    }

    /**
     * Construct an undefined expression result
     */
    ExprResult::ExprResult(set<string> &&undefinedSymbols)
        : value_(1)
        , undefinedSymbols_(std::move(undefinedSymbols))
    {
    }
    
    /**
     * An expression result is defined if it has no undefined symbols.
     */
    bool ExprResult::defined() const
    {
        return undefinedSymbols_.empty();
    }

    /**
     * The value of evaluating an expression
     */
    int ExprResult::value() const
    {
        return value_;
    }

    /**
     * The undefined symbols encountered while evaluating an expression
     */
    const set<string> &ExprResult::undefinedSymbols() const
    {
        return undefinedSymbols_;
    }

    /**
     * Evaluate a unary operation
     */
    ExprResult UnaryOp::eval(Pass &pass)
    {
        ExprResult op = operand_->eval(pass);
        if (!op.defined()) {
            return op;
        }

        switch (op_) {
        case Operator::Neg:
            op = ExprResult{ -op.value() };
            break;
        }

        return op;
    }

    /**
     * Evaluate a binary operation
     */
    ExprResult BinaryOp::eval(Pass &pass)
    {
        ExprResult left = left_->eval(pass);
        ExprResult right = right_->eval(pass);
        if (!left.defined() || !right.defined()) {
            set<string> undefs{};

            for (string undef : left.undefinedSymbols()) {
                undefs.insert(undef);
            }

            for (string undef : right.undefinedSymbols()) {
                undefs.insert(undef);
            }
            
            return ExprResult{ std::move(undefs) };
        }

        switch (op_) {
        case Operator::Add:
            left = ExprResult{ left.value() + right.value() };
            break;

        case Operator::Sub:
            left = ExprResult{ left.value() - right.value() };
            break;

        case Operator::Mul:
            left = ExprResult{ left.value() * right.value() };
            break;

        case Operator::Div:
            if (right.value() == 0) {
                throw Error{ "Divide by zero." };
            }
            left = ExprResult{ left.value() / right.value() };
            break;
        }

        return left;
    }

    /**
     * Evaluate a symbol expression
     */
    ExprResult SymbolExpression::eval(Pass &pass)
    {
        Symbol sym = pass.symtab().lookup(symbol_);
        if (!sym.defined) {
            set<string> undefs{ symbol_ };
            return ExprResult{ std::move(undefs) };
        }
            
        return ExprResult{ sym.value };
    }

    /**
     * Evaluate a constant expression
     */
    ExprResult ConstantExpression::eval(Pass &pass)
    {
        return ExprResult{ value_ };
    }

    /** 
     * The current value of the location counter
     */
    ExprResult LocationExpression::eval(Pass &pass)
    {
        return ExprResult{ pass.loc() };
    }
}
