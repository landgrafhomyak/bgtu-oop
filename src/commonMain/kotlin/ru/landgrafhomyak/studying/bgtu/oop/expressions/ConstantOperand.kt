package ru.landgrafhomyak.studying.bgtu.oop.expressions

class ConstantOperand(private val parameterName: String) : Expression<ExpressionContextWithParameters> {
    override fun calculate(ctx: ExpressionContextWithParameters): Double {
        return ctx.getParameter(this.parameterName)
    }
}