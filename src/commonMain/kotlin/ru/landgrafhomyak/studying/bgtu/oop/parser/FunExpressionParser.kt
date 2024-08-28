package ru.landgrafhomyak.studying.bgtu.oop.parser

import ru.landgrafhomyak.studying.bgtu.oop.expressions.ConstantOperand
import ru.landgrafhomyak.studying.bgtu.oop.expressions.Expression
import ru.landgrafhomyak.studying.bgtu.oop.expressions.ExpressionContextWithParameters
import ru.landgrafhomyak.studying.bgtu.oop.expressions.ParameterOperand
import ru.landgrafhomyak.studying.bgtu.oop.expressions.operators.AddExpression
import ru.landgrafhomyak.studying.bgtu.oop.expressions.operators.DivExpression
import ru.landgrafhomyak.studying.bgtu.oop.expressions.operators.MulExpression
import ru.landgrafhomyak.studying.bgtu.oop.expressions.operators.NegExpression
import ru.landgrafhomyak.studying.bgtu.oop.expressions.operators.PowExpression
import ru.landgrafhomyak.studying.bgtu.oop.expressions.operators.SubExpression

public object FunExpressionParser : Parser<ExpressionContextWithParameters> {
    private class FunctionCallNode(
        val prev: FunctionCallNode?,
        val name: String
    ) {
        val args = ArrayList<Expression<ExpressionContextWithParameters>>()
    }

    private enum class State {
        EXPECTING_OPERAND,
        EXPECTING_OPEN,
        EXPECTING_CLOSE_OR_SEP,
    }

    public override fun parse(raw: String): Expression<ExpressionContextWithParameters>? {
        var pos = 0
        var stack: FunctionCallNode? = null
        var state = State.EXPECTING_OPERAND
        while (pos < raw.length) {
            if (this.spacesPattern.matchAt(raw, pos)?.let { m -> pos = m.range.last + 1 } != null)
                continue
            when (state) {
                State.EXPECTING_OPERAND -> {
                    var compiled: Expression<ExpressionContextWithParameters>? = null

                    null ?: this.namePattern.matchAt(raw, pos)?.let { m ->
                        compiled = ParameterOperand(m.value)
                        pos = m.range.last + 1
                        state = State.EXPECTING_CLOSE_OR_SEP
                    } ?: this.constantPattern.matchAt(raw, pos)?.let { m ->
                        compiled = ConstantOperand(m.value.toDoubleOrNull() ?: return null)
                        pos = m.range.last + 1
                        state = State.EXPECTING_CLOSE_OR_SEP
                    } ?: this.functionPattern.matchAt(raw, pos)?.let { m ->
                        stack = FunctionCallNode(stack, m.value)
                        pos = m.range.last + 1
                        state = State.EXPECTING_OPEN
                    } ?: return null

                    if (compiled != null) {
                        val top = stack
                        if (top != null) {
                            top.args.add(compiled!!)
                        } else {
                            if (pos < raw.length && !this.spacesPattern.matches(raw.substring(pos)))
                                return null
                            return compiled
                        }
                    }
                }

                State.EXPECTING_OPEN -> {
                    if (raw[pos] != '(')
                        return null
                    state = State.EXPECTING_OPERAND
                    pos++
                }

                State.EXPECTING_CLOSE_OR_SEP -> {
                    when (raw[pos]) {
                        ',' -> {
                            pos++
                            state = State.EXPECTING_OPERAND
                        }

                        ')' -> {
                            pos++
                            state = State.EXPECTING_CLOSE_OR_SEP

                            val top = stack ?: return null
                            stack = top.prev
                            val compiled: Expression<ExpressionContextWithParameters>
                            when (top.name) {
                                "+" -> {
                                    if (top.args.size != 2) return null
                                    compiled = AddExpression(top.args[0], top.args[1])
                                }

                                "-" -> {
                                    @Suppress("LiftReturnOrAssignment")
                                    when (top.args.size) {
                                        1 -> compiled = NegExpression(top.args[0])
                                        2 -> compiled = SubExpression(top.args[0], top.args[1])
                                        else -> return null
                                    }
                                }

                                "*" -> {
                                    if (top.args.size != 2) return null
                                    compiled = MulExpression(top.args[0], top.args[1])
                                }

                                "/" -> {
                                    if (top.args.size != 2) return null
                                    compiled = DivExpression(top.args[0], top.args[1])
                                }

                                "^" -> {
                                    if (top.args.size != 2) return null
                                    compiled = PowExpression(top.args[0], top.args[1])
                                }

                                else -> return null
                            }
                            if (top.prev != null) {
                                top.prev.args.add(compiled)
                            } else {
                                if (pos < raw.length && !this.spacesPattern.matches(raw.substring(pos)))
                                    return null
                                return compiled
                            }
                        }

                        else -> return null
                    }
                }
            }
        }
        return null
    }

    private val spacesPattern = Regex("""\s+""")
    private val namePattern = Regex("""(?=\D)\w+""")
    private val functionPattern = Regex("""[+\-*/^]""")
    private val constantPattern = Regex("""\d+(?:\.\d+)?""")

}