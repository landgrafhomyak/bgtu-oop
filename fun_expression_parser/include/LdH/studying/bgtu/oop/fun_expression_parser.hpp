#pragma once

#include <LdH/studying/bgtu/oop/expression.hpp>
#include <LdH/studying/bgtu/oop/parser.hpp>

namespace LdH::Studying::BGTU::OOP {
    [[gnu::dllexport, ]]  extern Interface::ExpressionParser const *const fun_expression_parser;
}