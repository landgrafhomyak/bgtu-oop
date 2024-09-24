#pragma once

#include <exception>
#include <string>

namespace LdH::Studying::BGTU::OOP::Interface {
    class ExpressionContext {
    public:
        [[nodiscard]] virtual double get_parameter(std::string const &name) const = 0;

        class ParameterNotFoundException : public std::exception {
        public:
            std::string const &parameter_name;

            inline explicit ParameterNotFoundException(std::string const &pn) : parameter_name{pn} {}


            [[nodiscard]] inline char const *what() const override {
                return "ParameterNotFoundException";
            }
        };
    };

    class Expression {
    protected:
        inline Expression() = default;

    public:
        [[nodiscard]] virtual double calculate(ExpressionContext const *ctx) const = 0;

        virtual ~Expression() = 0;
    };
}