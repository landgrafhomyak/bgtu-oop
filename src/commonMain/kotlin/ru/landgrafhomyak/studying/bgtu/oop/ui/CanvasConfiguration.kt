package ru.landgrafhomyak.studying.bgtu.oop.ui

public interface CanvasConfiguration {
    public fun setXRange(start: Double, end: Double)
    public fun setYRange(start: Double, end: Double)

    public fun interface LineDrawer {
        public fun draw(plt: Plot)
    }

    public fun addLine(generator: LineDrawer)

}
