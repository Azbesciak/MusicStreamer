
#include "../utility/json.hpp"
#include "StreamerClient.h"

using json = nlohmann::json;
using namespace std;

StreamerClient::StreamerClient(int socketDescriptor, Container* container):
        socketDescriptor(socketDescriptor),
        container(container) {}

void StreamerClient::onNewMessage(char *message) {
    try {
        auto request = json::parse(message);
        cout<<request.dump(1)<<endl;

    } catch (json::exception &e) {
        cout << e.what() << endl;
    }
}
