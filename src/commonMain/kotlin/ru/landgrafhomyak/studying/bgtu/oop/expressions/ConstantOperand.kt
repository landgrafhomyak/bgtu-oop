package ru.landgrafhomyak.studying.bgtu.oop.expressions

public class ConstantOperand(private val value: Double) : Expression<ExpressionContext> {
    override fun calculate(ctx: ExpressionContext): Double {
        return this.value
    }
}