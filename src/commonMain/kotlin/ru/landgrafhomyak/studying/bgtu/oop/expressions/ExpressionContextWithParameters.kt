package ru.landgrafhomyak.studying.bgtu.oop.expressions

interface ExpressionContextWithParameters : ExpressionContext {
    class ParameterNotFoundException(@Suppress("MemberVisibilityCanBePrivate") val parameterName: String) : RuntimeException("Variable not found: $parameterName")

    @Throws(ParameterNotFoundException::class)
    fun getParameter(name: String): Double
}