#include <cmath>
#include <utility>
#include <string>
#include <vector>
#include <stack>
#include <LdH/studying/bgtu/oop/expression.hpp>
#include <LdH/studying/bgtu/oop/parser.hpp>
#include <LdH/studying/bgtu/oop/fun_expression_parser.hpp>
#include "expressions.hpp"

namespace LdH::Studying::BGTU::OOP {
    namespace FunExpressionParser {
        class FunExpressionParser final : public Interface::ExpressionParser {
        private:
            enum class operator_ {
                ADD,
                SUB,
                MUL,
                POW,
                DIV,
            };

            struct function_call_stack_node {
            public:
                operator_ op;
                std::vector<Interface::Expression *> args;

                explicit function_call_stack_node(operator_ op) : op{op}, args{} {}
            };

            enum class state {
                EXPECTING_OPERAND,
                EXPECTING_OPEN,
                EXPECTING_CLOSE_OR_SEP
            };

            static bool is_space(char c) noexcept {
                switch (c) {
                    case ' ':
                    case '\t':
                    case '\n':
                    case '\v':
                        return true;
                    default:
                        return false;
                }
            }

            using string_iterator = decltype(std::declval<std::string>().cbegin());

            static bool parse_name(std::string *dst, string_iterator &it, string_iterator const &end) {
                char c = *it;
                string_iterator start = it;
                if (!('a' <= c && c <= 'z' || 'A' <= c && c <= 'Z' || c == '_'))
                    return false;

                for (; it != end; it++) {
                    c = *it;
                    if ('a' <= c && c <= 'z' || 'A' <= c && c <= 'Z' || '0' <= c && c <= '9' || c == '_')
                        continue;

                    break;
                }
                *dst = std::string(start, it);
                return true;
            }

            static bool parse_num(double *dst, string_iterator &it, string_iterator const &end) {
                char c = *it;
                string_iterator start = it;
                if (!('0' <= c && c <= '9'))
                    return false;

                for (; it != end; it++) {
                    c = *it;
                    if ('0' <= c && c <= '9' || c == '.')
                        continue;

                    break;
                }
                *dst = std::stod(std::string(start, it));
                return true;
            }

            static bool parse_operator(operator_ *dst, string_iterator &it, string_iterator const &end) {
                switch (*it) {
                    case '+':
                        it++;
                        *dst = operator_::ADD;
                        return true;
                    case '-':
                        it++;
                        *dst = operator_::SUB;
                        return true;
                    case '*':
                        it++;
                        *dst = operator_::MUL;
                        return true;
                    case '/':
                        it++;
                        *dst = operator_::DIV;
                        return true;
                    case '^':
                        it++;
                        *dst = operator_::POW;
                        return true;
                    default:
                        return false;
                }
            }

        public:
            [[nodiscard]] Interface::Expression *parse(const std::string &raw) const override {
                std::stack<function_call_stack_node> stack;
                state state_ = state::EXPECTING_OPERAND;

                for (auto c = raw.begin(), end = raw.end(); c != end;) {
                    if (is_space(*c)) {
                        c++;
                        continue;
                    }

                    switch (state_) {
                        case state::EXPECTING_OPERAND: {
                            Interface::Expression *compiled = nullptr;
                            std::string name;
                            double constant;
                            operator_ op;

                            if (parse_name(&name, c, end)) {
                                compiled = new ParameterOperand(std::move(name));
                                state_ = state::EXPECTING_CLOSE_OR_SEP;
                            } else if (parse_num(&constant, c, end)) {
                                compiled = new ConstantOperand(constant);
                                state_ = state::EXPECTING_CLOSE_OR_SEP;
                            } else if (parse_operator(&op, c, end)) {
                                stack.emplace(op);
                                state_ = state::EXPECTING_OPEN;
                            } else {
                                throw BadExpressionException(raw, "Unknown symbol at operand");
                            }

                            if (compiled != nullptr) {
                                if (stack.empty()) {
                                    for (; c != end; c++) {
                                        if (is_space(*c))
                                            continue;
                                        throw BadExpressionException(raw, "Unexpected tokens after expressions");
                                    }
                                    return compiled;
                                } else {
                                    stack.top().args.push_back(compiled);
                                }
                            }
                            break;
                        }
                        case state::EXPECTING_OPEN: {
                            if (*c != '(')
                                throw BadExpressionException(raw, "Missed ( after function name");
                            state_ = state::EXPECTING_OPERAND;
                            c++;
                            break;
                        }
                        case state::EXPECTING_CLOSE_OR_SEP: {
                            switch (*c) {
                                case ',':
                                    state_ = state::EXPECTING_OPERAND;
                                    c++;
                                    break;
                                case ')': {
                                    c++;
                                    state_ = state::EXPECTING_CLOSE_OR_SEP;
                                    if (stack.empty())
                                        throw BadExpressionException(raw, "No matching ( for )");


                                    function_call_stack_node node = std::move(stack.top());
                                    stack.pop();
                                    Interface::Expression *compiled;
                                    switch (node.op) {
                                        case operator_::ADD:
                                            if (node.args.size() != 2)
                                                throw BadExpressionException(raw, "+() has only 2 arguments");
                                            compiled = new BinaryExpression<[](double a, double b) -> double { return a + b; }>{node.args[0], node.args[1]};
                                            break;
                                        case operator_::SUB:
                                            switch (node.args.size()) {
                                                case 2:
                                                    compiled = new BinaryExpression<[](double a, double b) -> double { return a - b; }>{node.args[0], node.args[1]};
                                                    break;
                                                case 1:
                                                    compiled = new NegExpression(node.args[0]);
                                                    break;
                                                default:
                                                    throw BadExpressionException(raw, "-() has only 1 or 2 arguments");
                                            }
                                            break;
                                        case operator_::MUL:
                                            if (node.args.size() != 2)
                                                throw BadExpressionException(raw, "*() has only 2 arguments");
                                            compiled = new BinaryExpression<[](double a, double b) -> double { return a * b; }>{node.args[0], node.args[1]};
                                            break;
                                        case operator_::POW:
                                            if (node.args.size() != 2)
                                                throw BadExpressionException(raw, "^() has only 2 arguments");
                                            compiled = new BinaryExpression<[](double a, double b) -> double { return std::pow(a, b); }>{node.args[0], node.args[1]};
                                            break;
                                        case operator_::DIV:
                                            if (node.args.size() != 2)
                                                throw BadExpressionException(raw, "/() has only 2 arguments");
                                            compiled = new BinaryExpression<[](double a, double b) -> double { return a / b; }>{node.args[0], node.args[1]};
                                            break;
                                    }
                                    if (stack.empty()) {
                                        for (; c != end; c++) {
                                            if (is_space(*c))
                                                continue;
                                            throw BadExpressionException(raw, "Unexpected tokens after expressions");
                                        }
                                        return compiled;
                                    } else {
                                        stack.top().args.push_back(compiled);
                                    }

                                    break;
                                }
                                default:
                                    throw BadExpressionException(raw, "Expected , or )");
                            }
                            break;
                        }
                    }
                }
                throw BadExpressionException(raw, "Expression not finished");
            }
        };


        static const FunExpressionParser _fun_expression_parser{};
    }


    [[gnu::dllexport]] Interface::ExpressionParser const *const fun_expression_parser = &FunExpressionParser::_fun_expression_parser;
}
