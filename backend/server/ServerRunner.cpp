#include <utility/Config.h>
#include "../logic/Container.h"
#include "CommunicationServer.h"
#include <csignal>
#include "ServerRunner.h"
#include "BroadCastServer.h"

ServerManager * serverRef;

int main(int argc, char *argv[]) {
    signal(SIGINT, cleanUp);
    // prevent dead sockets from throwing pipe errors on write
    signal(SIGPIPE, SIG_IGN);
    serverRef = new ServerManager(new Container());
    string command;

    string configPath = argc == 2 ? argv[1] : DEF_CONFIG_PATH;

    auto config = Config(configPath);
    auto host = config.get("server.host", DEF_ADDR);
    auto port = config.get("server.communication.port", DEF_COMMUN_PORT);
    auto broadCastPort = config.get("server.broadcast.port", DEF_BROAD_PORT);

    serverRef->communicationServer = new CommunicationServer(host, port, serverRef);
    serverRef->broadCaster = new BroadCastServer(host, broadCastPort, serverRef);

    do {
        cin >> command;
        parseCommand(command);
    } while (command != "stop");
    cleanUp(0);
    return 0;
}


void parseCommand(const string &command) {
    if (command.find("stop") != string::npos) {
        cout << GREEN_TEXT("Stopping server....\n");
        serverRef->isRunning = false;
    } else {
        cout << RED_TEXT("No such function ") << WHITE_TEXT(command) << "\n";
    }
}


void cleanUp(int) {
    cout<< GREEN_TEXT("cleaning up server") << endl;
    serverRef -> isRunning = false;
    delete serverRef->communicationServer;
    delete serverRef->streamer;
    delete serverRef->broadCaster;
    delete serverRef->uploader;
    delete serverRef->container;
    delete serverRef;
    serverRef = nullptr;
    exit(0);
}