#pragma once

#include <string>
#include <utility>
#include <LdH/studying/bgtu/oop/expression.hpp>

namespace LdH::Studying::BGTU::OOP::FunExpressionParser {
    class ParameterOperand final : public Interface::Expression {
    private:
        std::string n;

    public:
        explicit inline ParameterOperand(std::string &&name) : n{std::move(name)} {}

        inline double calculate(const LdH::Studying::BGTU::OOP::Interface::ExpressionContext *ctx) const final {
            return ctx->get_parameter(this->n);
        }

        inline ~ParameterOperand() final {
            operator delete(this);
        };
    };


    class ConstantOperand final : public Interface::Expression {
    private:
        double c;

    public:
        explicit inline ConstantOperand(double value) : c{value} {}

        inline double calculate(const LdH::Studying::BGTU::OOP::Interface::ExpressionContext *ctx) const final {
            return this->c;
        }

        inline ~ConstantOperand() final {
            operator delete(this);
        };
    };

    template<double (op_action)(double, double)>
    class BinaryExpression final : public Interface::Expression {
    private:
        Interface::Expression *const o1;
        Interface::Expression *const o2;

    public:
        explicit inline BinaryExpression(Interface::Expression *o1, Interface::Expression *o2) : o1{o1}, o2{o2} {}

        inline double calculate(const LdH::Studying::BGTU::OOP::Interface::ExpressionContext *ctx) const final {
            return op_action(this->o1->calculate(ctx), this->o2->calculate(ctx));
        }

        inline ~BinaryExpression() final {
            this->o1->~Expression();
            this->o2->~Expression();
            operator delete(this);
        };
    };

    class NegExpression final : public Interface::Expression {
    private:
        Interface::Expression *const o1;

    public:
        explicit inline NegExpression(Interface::Expression *o1) : o1{o1} {}

        inline double calculate(const LdH::Studying::BGTU::OOP::Interface::ExpressionContext *ctx) const final {
            return -(this->o1->calculate(ctx));
        }

        inline ~NegExpression() final {
            this->o1->~Expression();
            operator delete(this);
        };
    };
}
