package ru.landgrafhomyak.studying.bgtu.oop.expressions

public interface Expression<in C : ExpressionContext> {
    public fun calculate(ctx: C): Double
}