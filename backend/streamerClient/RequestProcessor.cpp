
#include <unistd.h>
#include "RequestProcessor.h"


ClientResponse RequestProcessor::onNewRequest(Request *request) {
    ClientResponse resp;
    if (!request->isCorrect()) {
        resp.setError(400, "Malformed input data");
    } else {
        string method = request->getMethod();
        try {
            auto resp2 = &resp;
            auto response = onNewRequest(request, method, resp2);
            response.fillOkResultIfNotSet();
            return response;

        } catch (json::exception &e) {
            resp.setError(400, "Input is not correct");
        }
    }

    return resp;
}

ssize_t RequestProcessor::respond(int socket, const string &message) {
    return write(socket, message.c_str(), message.length());
}
