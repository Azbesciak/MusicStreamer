
#include <unistd.h>
#include "MessageSender.h"

MessageSender::MessageSender() {
    broadcastThread = new thread([&]() {
        while(true) {
            Message* message;
            synchronized(mesMut) {
                message = &messages.front();
            }
            const string &content = message->getContent();
            for (auto && rec: message->getReceivers()) {
                rec->sendMessage(content);
            }
        }
    });
}

MessageSender::~MessageSender() {
 delete broadcastThread;
}

void MessageSender::sendMessage(Message &message) {
    synchronized(mesMut) {
        messages.push(message);
    }
}


