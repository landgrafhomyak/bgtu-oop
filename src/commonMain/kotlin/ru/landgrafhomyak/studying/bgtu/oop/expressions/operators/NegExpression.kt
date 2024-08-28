package ru.landgrafhomyak.studying.bgtu.oop.expressions.operators

import ru.landgrafhomyak.studying.bgtu.oop.expressions.Expression
import ru.landgrafhomyak.studying.bgtu.oop.expressions.ExpressionContext

public class NegExpression<C : ExpressionContext>(
    private val operand: Expression<C>,
) : Expression<C> {
    override fun calculate(ctx: C): Double {
        return -this.operand.calculate(ctx)
    }
}