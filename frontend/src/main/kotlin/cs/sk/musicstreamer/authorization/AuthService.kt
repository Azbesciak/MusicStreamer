package cs.sk.musicstreamer.authorization

import javafx.scene.layout.StackPane
import org.springframework.stereotype.Service
import kotlin.coroutines.experimental.suspendCoroutine

@Service
class AuthService(private val authModal: AuthModal) {
    private lateinit var userName: String

    suspend fun requestAuthorization(container: StackPane): String {
       return if (!this::userName.isInitialized)
            suspendCoroutine {cont ->
                authModal.showLoginModal(container, {
                    userName = it
                    cont.resume(it)
                })
            }
        else userName
    }
}
