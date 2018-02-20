

#include <thread>
#include <queue>
#include "../utility/synch.h"
#include "Message.h"

using namespace std;

#ifndef MUSICSTREAMER_MESSAGESENDER_H
#define MUSICSTREAMER_MESSAGESENDER_H

class StreamerClient;
class ClientResponse;

class MessageSender {
    static MessageSender * instance;
    thread* broadcastThread;
    queue<Message> messages;
    recursive_mutex mesMut;
    void sendMessage(Message &message);
    MessageSender();
    ~MessageSender();
public:
    static void sendMessage(unordered_set<StreamerClient*> &clients, ClientResponse * response);
    static void send(Message &message);
    static void init();
    static void destroy();
};

#endif //MUSICSTREAMER_MESSAGESENDER_H
