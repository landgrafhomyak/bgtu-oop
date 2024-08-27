package ru.landgrafhomyak.studying.bgtu.oop.expressions.operators

import kotlin.math.pow
import ru.landgrafhomyak.studying.bgtu.oop.expressions.Expression
import ru.landgrafhomyak.studying.bgtu.oop.expressions.ExpressionContext

class PowExpression<C : ExpressionContext>(
    private val operand: Expression<C>,
    private val power: Expression<C>,
) : Expression<C> {
    override fun calculate(ctx: C): Double {
        return this.operand.calculate(ctx).pow(this.power.calculate(ctx))
    }
}