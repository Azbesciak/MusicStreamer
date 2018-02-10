package cs.sk.musicstreamer.authorization

import javafx.scene.layout.StackPane
import org.springframework.stereotype.Service
import kotlin.coroutines.experimental.suspendCoroutine

@Service
class AuthService(private val authModal: AuthModal) {
    private var userName: String? = null

    @Synchronized
    suspend fun requestAuthorization(container: StackPane): String {
       return if (userName == null)
            suspendCoroutine {cont ->
                authModal.showLoginModal(container, {
                    userName = it
                    cont.resume(it)
                })
            }
        else userName!!
    }

    @Synchronized
    fun getUserName() = userName

    @Synchronized
    fun cleanUser() {
        userName = null
    }


}
