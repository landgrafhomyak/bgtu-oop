package ru.landgrafhomyak.studying.bgtu.oop.ui

public interface CanvasProvider {
    public fun interface Initializer {
        public fun configure(builder: CanvasConfiguration)
    }

    public fun reconfigureCanvas(action: Initializer)
}