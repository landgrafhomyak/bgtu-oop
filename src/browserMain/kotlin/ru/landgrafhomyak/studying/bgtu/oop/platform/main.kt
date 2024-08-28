package ru.landgrafhomyak.studying.bgtu.oop.platform

import kotlinx.browser.document
import org.w3c.dom.HTMLElement
import org.w3c.dom.HTMLInputElement
import org.w3c.dom.svg.SVGSVGElement
import ru.landgrafhomyak.studying.bgtu.oop.Application
import ru.landgrafhomyak.studying.bgtu.oop.parser.FunExpressionParser

public fun main() {
    val canvas = CanvasProviderImpl(document.getElementById("canvas")!! as SVGSVGElement)
    val app = Application(
        canvas = canvas,
        parser = FunExpressionParser
    )

    document.getElementById("zoom-x+")!!.addEventListener("click", { app.callbacks.zoomX(1) })
    document.getElementById("zoom-x-")!!.addEventListener("click", { app.callbacks.zoomX(-1) })
    document.getElementById("zoom-y+")!!.addEventListener("click", { app.callbacks.zoomY(1) })
    document.getElementById("zoom-y-")!!.addEventListener("click", { app.callbacks.zoomY(-1) })
    document.getElementById("scroll-x+")!!.addEventListener("click", { app.callbacks.scrollX(1) })
    document.getElementById("scroll-x-")!!.addEventListener("click", { app.callbacks.scrollX(-1) })
    document.getElementById("scroll-y+")!!.addEventListener("click", { app.callbacks.scrollY(1) })
    document.getElementById("scroll-y-")!!.addEventListener("click", { app.callbacks.scrollY(-1) })
    val input = document.getElementById("expression")!! as HTMLInputElement
    input.addEventListener("change", { app.callbacks.inputExpression(input.value) })

}