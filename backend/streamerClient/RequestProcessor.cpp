
#include <unistd.h>
#include "RequestProcessor.h"


ClientResponse RequestProcessor::onNewRequest(Request *request) {
    ClientResponse resp;
    auto resp2 = &resp;
    if (isCorrect(request, resp2)) {
        string method = request->getMethod();
        try {
            resp = onNewRequest(request, method, resp2);
            resp.fillOkResultIfNotSet();

        } catch (json::exception &e) {
            resp.setError(400, "Input is not correct");
        }
    }
    resp.addIdFromRequestIfPresent(request);
    return resp;
}

ssize_t RequestProcessor::respond(int socket, const string &message) {
    return write(socket, message.c_str(), message.length());
}

bool RequestProcessor::isCorrect(Request *request, ClientResponse *&response) {
    if (request == nullptr || errno == EWOULDBLOCK) {
        response->setError(408, "Read timeout");
        return false;
    } else if (!request->isCorrect()){
        response->setError(400, "Malformed input data");
        return false;
    }
    return true;
}
