package ru.landgrafhomyak.studying.bgtu.oop.parser

import ru.landgrafhomyak.studying.bgtu.oop.expressions.Expression
import ru.landgrafhomyak.studying.bgtu.oop.expressions.ExpressionContext

public interface Parser<C : ExpressionContext> {
    public fun parse(raw: String): Expression<C>?
}