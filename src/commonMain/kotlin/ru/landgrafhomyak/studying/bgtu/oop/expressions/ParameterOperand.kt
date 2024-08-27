package ru.landgrafhomyak.studying.bgtu.oop.expressions

class ParameterOperand(private val value: Double) : Expression<ExpressionContext> {
    override fun calculate(ctx: ExpressionContext): Double {
        return this.value
    }
}