
#include "../utility/json.hpp"
#include "StreamerClient.h"

using json = nlohmann::json;
using namespace std;

StreamerClient::StreamerClient(int socketDescriptor):
        socketDescriptor(socketDescriptor) {}
