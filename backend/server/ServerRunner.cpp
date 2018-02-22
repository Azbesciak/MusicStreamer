#include <utility/Config.h>
#include "../logic/Container.h"
#include "CommunicationServer.h"
#include <csignal>
#include <upload/UploadHandler.h>
#include "ServerRunner.h"
#include "BroadCastServer.h"

ServerManager * serverRef;

int main(int argc, char *argv[]) {
    signal(SIGINT, cleanUp);
    // prevent dead sockets from throwing pipe errors on write
    signal(SIGPIPE, SIG_IGN);
    signal(SIGSEGV, cleanUp);
    signal(SIGABRT, cleanUp);
    serverRef = new ServerManager();
    Container::initialize();
    string command;

    string configPath = argc == 2 ? argv[1] : DEF_CONFIG_PATH;

    auto config = Config(configPath);
    auto host = config.get("server.host", DEF_ADDR);
    auto port = config.get("server.communication.port", DEF_COMMUN_PORT);
    auto broadCastPort = config.get("server.broadcast.port", DEF_BROAD_PORT);
    auto uploadPort = config.get("server.upload.port", DEF_UPLOAD_PORT);
    auto uploadDir = config.get("server.upload.dir", DEF_UPLOAD_DIR);
    auto minStreamerPort = config.get("server.streamer.port.min", DEF_MIN_STREAMER_PORT);
    auto maxStreamerPort = config.get("server.streamer.port.max", DEF_MAX_STREAMER_PORT);
    MessageSender::init();
    MusicStreamer::setPortsRange(minStreamerPort, maxStreamerPort);
    MusicStreamer::setHost(host);

    serverRef->communicationServer = new CommunicationServer(host, port, serverRef);
    serverRef->broadCaster = new BroadCastServer(host, broadCastPort, serverRef);
    UploadHandler::initialize(host, uploadPort, serverRef, uploadDir);

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
    UploadHandler::destroy();
    delete serverRef->communicationServer;
    delete serverRef->streamer;
    delete serverRef->broadCaster;
    delete serverRef->uploader;
    delete serverRef;
    Container::destroy();
    MessageSender::destroy();
    serverRef = nullptr;
    exit(0);
}