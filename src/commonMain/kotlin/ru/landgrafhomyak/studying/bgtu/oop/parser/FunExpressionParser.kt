package ru.landgrafhomyak.studying.bgtu.oop.parser

import ru.landgrafhomyak.studying.bgtu.oop.expressions.ConstantOperand
import ru.landgrafhomyak.studying.bgtu.oop.expressions.ParameterOperand
import ru.landgrafhomyak.studying.bgtu.oop.expressions.Expression
import ru.landgrafhomyak.studying.bgtu.oop.expressions.ExpressionContextWithParameters

public object FunExpressionParser : Parser<ExpressionContextWithParameters> {
    private class FunctionCallNode(
        val prev: FunctionCallNode?,
        val name: String
    ) {
        val args = ArrayList<Expression<ExpressionContextWithParameters>>()
    }

    public override fun parse(raw: String): Expression<ExpressionContextWithParameters>? {
        return ParameterOperand("x")
    }
}