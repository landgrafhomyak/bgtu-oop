#pragma once

#include <exception>
#include <string>
#include "expression.hpp"

namespace LdH::Studying::BGTU::OOP::Interface {
    class ExpressionParser {
    public:
        [[nodiscard]] virtual Expression *parse(std::string const &raw) const = 0;

        class BadExpressionException : public std::exception {
        public:
            std::string const &raw;
            std::string const &reason;

            inline explicit BadExpressionException(std::string const &raw, std::string const &reason) : raw{raw}, reason{reason} {}

            [[nodiscard]] inline char const *what() const override {
                return "BadExpressionException";
            }
        };
    };
}