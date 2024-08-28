package ru.landgrafhomyak.studying.bgtu.oop.expressions.operators

import ru.landgrafhomyak.studying.bgtu.oop.expressions.Expression
import ru.landgrafhomyak.studying.bgtu.oop.expressions.ExpressionContext

public class AddExpression<C : ExpressionContext>(
    private val leftOperand: Expression<C>,
    private val rightOperand: Expression<C>,
) : Expression<C> {
    override fun calculate(ctx: C): Double {
        return this.leftOperand.calculate(ctx) + this.rightOperand.calculate(ctx)
    }
}