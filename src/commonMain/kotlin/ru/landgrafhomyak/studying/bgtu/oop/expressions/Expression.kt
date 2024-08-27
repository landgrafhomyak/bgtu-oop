package ru.landgrafhomyak.studying.bgtu.oop.expressions

interface Expression<in C : ExpressionContext> {
    fun calculate(ctx: C): Double
}