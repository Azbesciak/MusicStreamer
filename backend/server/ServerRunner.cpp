#include "ServerRunner.h"


atomic<bool> runserver(true);

ServerManager * serverRef;

int main(int argc, char *argv[]) {
    signal(SIGINT, cleanUp);
    // prevent dead sockets from throwing pipe errors on write
    signal(SIGPIPE, SIG_IGN);
    auto container = new Container();
    serverRef = new ServerManager(container);
    string command;

    string configPath = argc == 2 ? argv[1] : DEFAULT_CONFIG_PATH;

    auto config = Config(configPath);
    auto host = config.get("server.host", DEFAULT_ADDR);
    auto port = config.get("server.port", DEFAULT_PORT);

    auto reqResServ = new RequestResponseServer(host, port, serverRef->container, &runserver);

    do {
        cin >> command;
        parseCommand(command);
    } while (command != "stop");
    delete reqResServ;
    return 0;
}


void parseCommand(const string &command) {
    if (command.find("stop") != string::npos) {
        cout << GREEN_TEXT("Stopping server....\n");
        runserver = false;
    } else {
        cout << RED_TEXT("No such function ") << WHITE_TEXT(command) << "\n";
    }
}



void cleanUp(int) {
    cout<< GREEN_TEXT("cleaning up server") << endl;
    delete serverRef;
    serverRef = nullptr;
    runserver = false;
    exit(0);
}