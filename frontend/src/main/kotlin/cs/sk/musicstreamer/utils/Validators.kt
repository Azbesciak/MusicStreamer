package cs.sk.musicstreamer.utils

import com.jfoenix.validation.base.ValidatorBase
import javafx.scene.control.TextInputControl
import kotlinx.coroutines.experimental.delay
import kotlinx.coroutines.experimental.launch
import java.util.concurrent.atomic.AtomicBoolean

class InvalidFieldValueValidator(
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
    private val wasBlocked = AtomicBoolean(false)
    fun setInvalid() {
        if (!wasBlocked.getAndSet(true)) {
            hasErrors.set(true)
            validate()
            launch {
                delay(5000)
                wasBlocked.set(false)
                hasErrors.set(false)
                validate()
            }
        }
    }

    override fun eval() {}

}