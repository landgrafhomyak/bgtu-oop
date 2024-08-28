package ru.landgrafhomyak.studying.bgtu.oop.parser

import ru.landgrafhomyak.studying.bgtu.oop.expressions.ConstantOperand
import ru.landgrafhomyak.studying.bgtu.oop.expressions.ParameterOperand
import ru.landgrafhomyak.studying.bgtu.oop.expressions.Expression
import ru.landgrafhomyak.studying.bgtu.oop.expressions.ExpressionContextWithParameters
import ru.landgrafhomyak.studying.bgtu.oop.expressions.operators.AddExpression
import ru.landgrafhomyak.studying.bgtu.oop.expressions.operators.PowExpression
import ru.landgrafhomyak.studying.bgtu.oop.expressions.operators.SubExpression

public object FunExpressionParser : Parser<ExpressionContextWithParameters> {
    private class FunctionCallNode(
        val prev: FunctionCallNode?,
        val name: String
    ) {
        val args = ArrayList<Expression<ExpressionContextWithParameters>>()
    }

    public override fun parse(raw: String): Expression<ExpressionContextWithParameters>? {
        return AddExpression(SubExpression(PowExpression(ParameterOperand("x"), ConstantOperand(3.0)), PowExpression(ParameterOperand("x"), ConstantOperand(2.0))), ConstantOperand(3.0))
    }
}