package ru.landgrafhomyak.studying.bgtu.oop.ui

public interface ApplicationCallbacks {
    public fun scrollX(steps: Int)
    public fun scrollY(steps: Int)
    public fun zoomX(steps: Int)
    public fun zoomY(steps: Int)
    public fun inputExpression(raw: String)
    public fun setParam(name: String, value: Double)
    public fun removeParam(name: String)
}