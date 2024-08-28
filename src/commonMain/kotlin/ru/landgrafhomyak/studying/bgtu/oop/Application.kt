@file:Suppress("RedundantVisibilityModifier")

package ru.landgrafhomyak.studying.bgtu.oop

import kotlin.jvm.JvmField
import kotlin.math.pow
import ru.landgrafhomyak.studying.bgtu.oop.expressions.Expression
import ru.landgrafhomyak.studying.bgtu.oop.expressions.ExpressionContextWithParameters
import ru.landgrafhomyak.studying.bgtu.oop.parser.Parser
import ru.landgrafhomyak.studying.bgtu.oop.ui.ApplicationCallbacks
import ru.landgrafhomyak.studying.bgtu.oop.ui.CanvasConfiguration
import ru.landgrafhomyak.studying.bgtu.oop.ui.CanvasProvider
import ru.landgrafhomyak.studying.bgtu.oop.ui.Plot

public class Application(
    private val canvas: CanvasProvider,
    private val parser: Parser<ExpressionContextWithParameters>,
    private val drawingStep: Double = 100.0,
    initialViewArea: Inputs.ViewArea = Inputs.ViewArea.DEFAULT,
    private val scrollAndZoomSettings: Inputs.ScrollAndZoomSettings = Inputs.ScrollAndZoomSettings.DEFAULT,
) {
    private val state = when (initialViewArea) {
        is Inputs.ViewArea.Rect -> this.State(initialViewArea)
        is Inputs.ViewArea.CenterAndRadius -> this.State(initialViewArea)
    }
    public val callbacks: ApplicationCallbacks = this.CallbacksImpl()
    private val expressionContext = this.ExpressionContextImpl()
    private val lineDrawer = this.LineDrawerImpl()
    private val configureNoPlot = this.ConfigureCanvasNoPlot()
    private val configureDraw = this.ConfigureCanvasDraw()


    init {
        this.canvas.reconfigureCanvas(this.configureNoPlot)
    }


    private fun redraw() {
        if (this.state.expression == null)
            this.canvas.reconfigureCanvas(this.configureNoPlot)
        else
            this.canvas.reconfigureCanvas(this.configureDraw)
    }

    private inner class ConfigureCanvasNoPlot : CanvasProvider.Initializer {
        @Suppress("DuplicatedCode")
        override fun configure(builder: CanvasConfiguration) {
            builder.setXRange(this@Application.state.xCenter - this@Application.state.xRadius, this@Application.state.xCenter + this@Application.state.xRadius)
            builder.setYRange(this@Application.state.yCenter - this@Application.state.yRadius, this@Application.state.yCenter + this@Application.state.yRadius)
        }
    }

    private inner class ConfigureCanvasDraw : CanvasProvider.Initializer {
        @Suppress("DuplicatedCode")
        override fun configure(builder: CanvasConfiguration) {
            builder.setXRange(this@Application.state.xCenter - this@Application.state.xRadius, this@Application.state.xCenter + this@Application.state.xRadius)
            builder.setYRange(this@Application.state.yCenter - this@Application.state.yRadius, this@Application.state.yCenter + this@Application.state.yRadius)
            builder.addLine(this@Application.lineDrawer)
        }
    }

    private inner class LineDrawerImpl : CanvasConfiguration.LineDrawer {
        override fun draw(plt: Plot) {
            val step = this@Application.state.xRadius * 2 / this@Application.drawingStep
            var x = this@Application.state.xCenter - this@Application.state.xRadius
            val xEnd = this@Application.state.xCenter + this@Application.state.xRadius

            while (x <= xEnd) {
                this@Application.state.params["x"] = x
                val y: Double
                try {
                    y = this@Application.state.expression!!.calculate(this@Application.expressionContext)
                } catch (_: ExpressionContextWithParameters.ParameterNotFoundException) {
                    return
                }
                plt.drawTo(x, y)
                x += step
            }
        }
    }


    private inner class State(initialValues: Inputs.ViewArea.CenterAndRadius) {
        constructor(initialValues: Inputs.ViewArea.Rect) : this(initialValues.toCenterAndRadius())

        public var xCenter: Double = initialValues.xCenter
            get() = when (field) {
                Double.POSITIVE_INFINITY -> Double.MAX_VALUE
                Double.NEGATIVE_INFINITY -> -Double.MAX_VALUE
                Double.NaN -> 0.0
                else -> field
            }

        public var xRadius: Double = initialValues.xRadius
            get() = when (field) {
                0.0 -> Double.MIN_VALUE
                -0.0 -> -Double.MIN_VALUE
                Double.POSITIVE_INFINITY -> Double.MAX_VALUE
                Double.NEGATIVE_INFINITY -> -Double.MAX_VALUE
                Double.NaN -> 10.0
                else -> field
            }

        public var yCenter: Double = initialValues.yCenter
            get() = when (field) {
                Double.POSITIVE_INFINITY -> Double.MAX_VALUE
                Double.NEGATIVE_INFINITY -> -Double.MAX_VALUE
                Double.NaN -> 0.0
                else -> field
            }

        public var yRadius: Double = initialValues.yRadius
            get() = when (field) {
                0.0 -> Double.MIN_VALUE
                -0.0 -> -Double.MIN_VALUE
                Double.POSITIVE_INFINITY -> Double.MAX_VALUE
                Double.NEGATIVE_INFINITY -> -Double.MAX_VALUE
                Double.NaN -> 10.0
                else -> field
            }

        public var params = HashMap<String, Double>()

        public var expression: Expression<ExpressionContextWithParameters>? = null
    }

    private inner class CallbacksImpl : ApplicationCallbacks {

        override fun scrollX(steps: Int) {
            this@Application.state.xCenter += this@Application.state.xRadius * steps * this@Application.scrollAndZoomSettings.xScrollStep
            this@Application.redraw()
        }

        override fun scrollY(steps: Int) {
            this@Application.state.yCenter += this@Application.state.xRadius * steps * this@Application.scrollAndZoomSettings.yScrollStep
            this@Application.redraw()
        }

        override fun zoomX(steps: Int) {
            this@Application.state.xRadius /= this@Application.scrollAndZoomSettings.xZoomStep.pow(steps)
            this@Application.redraw()
        }

        override fun zoomY(steps: Int) {
            this@Application.state.yRadius /= this@Application.scrollAndZoomSettings.yZoomStep.pow(steps)
            this@Application.redraw()
        }


        override fun inputExpression(raw: String) {
            if (raw.isBlank())
                this@Application.state.expression = null
            else
                this@Application.state.expression = this@Application.parser.parse(raw)

            this@Application.redraw()
        }

        override fun setParam(name: String, value: Double) {
            this@Application.state.params[name] = value
            this@Application.redraw()
        }

        override fun removeParam(name: String) {
            this@Application.state.params.remove(name)
            this@Application.redraw()
        }
    }

    private inner class ExpressionContextImpl : ExpressionContextWithParameters {
        override fun getParameter(name: String): Double =
            this@Application.state.params[name] ?: throw ExpressionContextWithParameters.ParameterNotFoundException(name)
    }

    public object Inputs {
        public sealed class ViewArea {
            public class CenterAndRadius(
                public val xCenter: Double,
                public val xRadius: Double,
                public val yCenter: Double,
                public val yRadius: Double
            ) : ViewArea() {
                public fun toRect(): Rect = Rect(
                    xStart = this.xCenter - this.xRadius,
                    xEnd = this.xCenter + this.xRadius,
                    yStart = this.yCenter - this.yRadius,
                    yEnd = this.yCenter + this.yRadius,
                )
            }

            public class Rect(
                public val xStart: Double,
                public val xEnd: Double,
                public val yStart: Double,
                public val yEnd: Double
            ) : ViewArea() {
                init {
                    require(this.xStart < this.xEnd) { "xStart must be less than xEnd" }
                    require(this.yStart < this.yEnd) { "yStart must be less than yEnd" }
                }

                public fun toCenterAndRadius(): CenterAndRadius = CenterAndRadius(
                    xCenter = (this.xEnd - this.xStart) / 2 + this.xStart,
                    xRadius = (this.xEnd - this.xStart) / 2,
                    yCenter = (this.yEnd - this.yStart) / 2 + this.yStart,
                    yRadius = (this.yEnd - this.yStart) / 2
                )
            }

            public companion object {
                @JvmField
                public val DEFAULT: ViewArea = CenterAndRadius(
                    xCenter = 0.0,
                    xRadius = 10.0,
                    yCenter = 0.0,
                    yRadius = 10.0,
                )
            }
        }

        public class ScrollAndZoomSettings(
            public val xScrollStep: Double,
            public val yScrollStep: Double,
            public val xZoomStep: Double,
            public val yZoomStep: Double,
        ) {
            public companion object {
                @JvmField
                public val DEFAULT: ScrollAndZoomSettings = ScrollAndZoomSettings(
                    xScrollStep = 0.1,
                    yScrollStep = 0.1,
                    xZoomStep = 1.3,
                    yZoomStep = 1.3
                )
            }
        }
    }
}