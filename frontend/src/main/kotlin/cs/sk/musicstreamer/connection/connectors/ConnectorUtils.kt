package cs.sk.musicstreamer.connection.connectors

import cs.sk.musicstreamer.connection.ErrorResponse
import cs.sk.musicstreamer.connection.JsonResponse


class ConnectionListener(
        val onConnection: () -> Unit = {},
        val onDisconnect: () -> Unit = {},
        val onError: (Throwable) -> Unit = {}
)

class ConnectionError(reason: String) : Error(reason)

class ResponseListener(
        val onResponse: (JsonResponse) -> Unit,
        val onError: (ErrorResponse) -> Unit = {}
)

