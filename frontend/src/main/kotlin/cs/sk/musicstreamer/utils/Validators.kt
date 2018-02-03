package cs.sk.musicstreamer.utils

import com.jfoenix.validation.base.ValidatorBase
import javafx.scene.control.TextInputControl

class InvalidFieldValueValidator (
        private val errorMessage: String = "That value can't be used",
        private val invalidValues: MutableSet<String> = mutableSetOf()
) : ValidatorBase() {
    infix fun addToBlackList(userName: String) = invalidValues.add(userName)

    override fun eval() {
        val text = (srcControl.value as TextInputControl).textProperty().get()
        hasErrors.set(invalidValues.contains(text))
    }

    override fun getMessage() = errorMessage
}

class VetoValidator : ValidatorBase() {
    fun setInvalid() = hasErrors.set(true)
    override fun eval() {}

}