package ru.landgrafhomyak.studying.bgtu.oop.platform

import kotlinx.browser.document
import kotlinx.dom.appendText
import org.w3c.dom.HTMLElement
import org.w3c.dom.HTMLInputElement
import ru.landgrafhomyak.studying.bgtu.oop.ui.ApplicationCallbacks

public class ParamsController(
    private val dst: ApplicationCallbacks,
    private val paramsContainer: HTMLElement
) {
    private val paramNames = HashSet<String>()

    public fun addParamCallback() {
        val tr = document.createElement("tr") as HTMLElement
        this.paramsContainer.appendChild(tr)

        val removeBtn = document.createElement("td")
        tr.appendChild(removeBtn)
        removeBtn.classList.add("button")
        removeBtn.appendText("x")

        val nameContainer = document.createElement("td")
        tr.appendChild(nameContainer)

        val nameInput = document.createElement("input") as HTMLInputElement
        nameContainer.appendChild(nameInput)


        val valueContainer = document.createElement("td")
        tr.appendChild(valueContainer)

        val valueInput = document.createElement("input") as HTMLInputElement
        valueContainer.appendChild(valueInput)

        val callbacks = this.ParamCallbacks(tr, nameInput, valueInput)

        removeBtn.addEventListener("click", { callbacks.remove() })
        nameInput.addEventListener("change", { callbacks.nameChanged() })
        valueInput.addEventListener("change", { callbacks.valueChanged() })

    }

    private inner class ParamCallbacks(
        private val root: HTMLElement,
        private val nameInput: HTMLInputElement,
        private val valueInput: HTMLInputElement,
    ) {
        var name: String = ""
        var value: Double? = null
        var isNameCollision = true

        fun remove() {
            this@ParamsController.dst.removeParam(this.name)
            this.root.remove()
            if (!this.isNameCollision)
                this@ParamsController.paramNames.remove(this.name)
        }

        fun nameChanged() {
            val newName = this.nameInput.value
            this.name = newName
            if (newName !in this@ParamsController.paramNames) {
                this.isNameCollision = false
                this.nameInput.classList.remove("error")
                this@ParamsController.paramNames.add(newName)
                val v = this.value
                if (v != null) {
                    this@ParamsController.dst.setParam(this.name, v)
                }
            } else {
                this.isNameCollision = true
                this.nameInput.classList.add("error")
            }
        }

        fun valueChanged() {
            val newValue = this.valueInput.value.toDoubleOrNull()
            this.value = newValue
            if (newValue == null) {
                this.nameInput.classList.add("error")
                if (!this.isNameCollision)
                this@ParamsController.dst.removeParam(this.name)
            } else {
                this.nameInput.classList.remove("error")
                if (!this.isNameCollision)
                    this@ParamsController.dst.setParam(this.name, newValue)
            }
        }
    }
}