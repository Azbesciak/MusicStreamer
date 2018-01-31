package cs.sk.musicstreamer.authorization

import org.springframework.stereotype.Service

@Service
class AuthService(private val authModal: AuthModal) {
    private lateinit var userName: String

    suspend fun requestAuthorization() {
        if (!this::userName.isInitialized)
            authModal.showLoginModal()
    }
}
