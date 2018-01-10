
#include <unistd.h>
#include "MessageSender.h"

MessageSender::MessageSender() {
    broadcastThread = new thread([&]() {
        while(true) {
            if (!messages.empty()) {
                Message* message;
                synchronized(mesMut) {
                    message = &messages.front();
                    messages.pop();
                }
                const string &content = message->getContent();
                for (auto && rec: message->getReceivers()) {
                    rec->sendMessage(content);
                }
            }
            sleep(1);
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


