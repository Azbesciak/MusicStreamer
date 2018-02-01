package cs.sk.musicstreamer.authorization

import javafx.scene.layout.StackPane
import org.springframework.stereotype.Service

@Service
class AuthService(private val authModal: AuthModal) {
    private lateinit var userName: String

    suspend fun requestAuthorization(container: StackPane) {
        if (!this::userName.isInitialized)
            authModal.showLoginModal(container)
    }
}
