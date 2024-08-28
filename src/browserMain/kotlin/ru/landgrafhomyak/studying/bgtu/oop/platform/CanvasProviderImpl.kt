package ru.landgrafhomyak.studying.bgtu.oop.platform

import kotlinx.browser.document
import kotlinx.dom.clear
import org.w3c.dom.svg.SVGSVGElement
import ru.landgrafhomyak.studying.bgtu.oop.ui.CanvasConfiguration
import ru.landgrafhomyak.studying.bgtu.oop.ui.CanvasProvider
import ru.landgrafhomyak.studying.bgtu.oop.ui.Plot

public class CanvasProviderImpl(
    private val canvas: SVGSVGElement
) : CanvasProvider {
    private var vbX: Double = 0.0
    private var vbY: Double = 0.0
    private var vbW: Double = 10.0
    private var vbH: Double = 10.0

    override fun reconfigureCanvas(action: CanvasProvider.Initializer) {
        this.canvas.clear()
        action.configure(this.Initializer())
        this.canvas.setAttribute("viewBox", "${this.vbX} ${this.vbY} ${this.vbW} ${this.vbH}")
    }

    private inner class Initializer : CanvasConfiguration {
        override fun setXRange(start: Double, end: Double) {
            this@CanvasProviderImpl.vbX = start
            this@CanvasProviderImpl.vbW = end - start
        }

        override fun setYRange(start: Double, end: Double) {
            this@CanvasProviderImpl.vbY = start
            this@CanvasProviderImpl.vbH = end - start
        }

        override fun addLine(generator: CanvasConfiguration.LineDrawer) {
            val points = arrayListOf<Pair<Double, Double>>()
            generator.draw(LinePlot(points))
            if (points.isNotEmpty()) {
                val path = document.createElementNS("http://www.w3.org/2000/svg", "path")
                val d = buildString {
                    append("M ${points[0].first} ${points[0].second} ")
                    for (e in points.drop(1))
                        append("L ${e.first} ${e.second} ")
                }
                path.setAttribute("d", d)
                this@CanvasProviderImpl.canvas.append(path)
            }
        }
    }

    private class LinePlot(private val dst: MutableList<Pair<Double, Double>>) : Plot {
        override fun drawTo(x: Double, y: Double) {
            this.dst.add(Pair(x, y))
        }
    }
}