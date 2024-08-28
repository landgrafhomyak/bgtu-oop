package ru.landgrafhomyak.studying.bgtu.oop.expressions

public interface ExpressionContextWithParameters : ExpressionContext {
    public class ParameterNotFoundException(@Suppress("MemberVisibilityCanBePrivate") public val parameterName: String) : RuntimeException("Variable not found: $parameterName")

    @Throws(ParameterNotFoundException::class)
    public fun getParameter(name: String): Double
}