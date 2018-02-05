
#include <atomic>
#include "AbstractServer.h"

AbstractServer::~AbstractServer() {
    delete (serverThread);
    close(serverFd);
}

AbstractServer::AbstractServer(const string &host, int port, Container *container, atomic<bool> *isRunning)
        : port(port), host(host), container(container), isRunning(isRunning) {
    this->serverThread = new thread([&]() {
        startServer();
    });
}

void AbstractServer::startServer() {

    printf("Server works at %s:%d\n", this->host.c_str(), this->port);

    int socketNum = createSocket();
    if (listen(socketNum, QUEUE_SIZE) < 0) {
        perror("Listen error");
        exit(-1);
    }
    struct sockaddr_in remote{};

    socklen_t sockSize = sizeof(struct sockaddr);
    while (this->isRunning->load()) {
        int connection_descriptor = accept(socketNum, (struct sockaddr *) &remote, &sockSize);
        if (connection_descriptor < 0) {
            perror("Client accepting error");
            continue;
        }

        char remoteAddr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(remote.sin_addr), remoteAddr, INET_ADDRSTRLEN);
        //pass structure with client's data port
        printf("Client connected with %s. Descriptor assigned: %d\n", remoteAddr, connection_descriptor);
        thread([&]() {
            cout << "Initialization successfull. Descriptor " << GREEN_TEXT(connection_descriptor) << " from "
                 << GREEN_TEXT(remoteAddr) << ".\n";
            onConnection(connection_descriptor, remoteAddr);
        }).detach();
    }
    cout << "end?" << endl;
    exit(0);
}

void AbstractServer::onConnection(int clientSocket, const char *remoteAddr) {
    auto buffer = new char[BUFFER_SIZE];
    auto proxy = new ClientProxy(clientSocket, this->container);
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t value = read(clientSocket, buffer, BUFFER_SIZE);
        if (value > 0) {
            manageRequestCoroutine(clientSocket, remoteAddr, buffer, proxy);
        } else if (buffer[0] == 0) {
            cout << RED_TEXT("Client from " << remoteAddr
                                            << ", descriptor "
                                            << clientSocket
                                            << " has disconnected!\n");
            delete proxy;
            return;
        } else {
            printf("Undefined response.\n");
        }
    }
}

void AbstractServer::manageRequestCoroutine(int clientSocket, const char *remoteAddr, char *buffer, ClientProxy *proxy) {
    displayRequest(clientSocket, buffer);
    auto response = proxy->onNewMessage(buffer);
    auto serializedResponse = response.serialize();
    displayResponse(clientSocket, serializedResponse.c_str());
    const auto wrote = proxy->sendMessage(serializedResponse);
    if (wrote == -1) {
        cout << RED_TEXT("Error while sending client response: \n\t")
             << MAGENTA_TEXT(serializedResponse)
             << RED_TEXT("\n to " << remoteAddr);
    }
}


void AbstractServer::displayRequest(int socketDescriptor, const char *request) {
    cout << YELLOW_TEXT("Client " << socketDescriptor) << "\n";
    cout << "\t" << MAGENTA_TEXT("Request from " << socketDescriptor << ":\t") << GREEN_TEXT(request);
}


void AbstractServer::displayResponse(int socketDescriptor, const char *response) {
    cout << CYAN_TEXT("Client " << socketDescriptor) << "\n";
    cout << "\t" << MAGENTA_TEXT("Response to " << socketDescriptor << ":\t") << GREEN_TEXT(response) << endl;
}