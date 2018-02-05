
#include <unistd.h>
#include "MessageSender.h"

MessageSender::MessageSender() {
    broadcastThread = new thread([&]() {
        while(true) {
            while (!messages.empty()) {
                Message* message = &messages.front();
                const string &content = message->getContent();
                for (auto && rec: message->getReceivers()) {
                    rec->sendOnBroadCast(content);
                }
                messages.pop();
            }
            sleep(1);
        }
    });
    broadcastThread->detach();
}

MessageSender::~MessageSender() {
 delete broadcastThread;
}

void MessageSender::sendMessage(Message &message) {
    synchronized(mesMut) {
        messages.push(message);
    }
}


