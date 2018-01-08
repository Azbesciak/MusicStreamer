#include "ServerRunner.h"

bool runserver = true;

void manageRequestCoroutine(const thread_data_t *th_data, const char *remoteAddr, const char *buffer,
                            StreamerClient *client);

int main(int argc, char *argv[]) {
    string command;
    char *serverAddr = argc == 3 ? argv[1] : (char *) DEFAULT_ADDR;
    int port = argc == 2 ? atoi(argv[1]) : (argc == 3 ? atoi(argv[2]) : DEFAULT_PORT);

    int serverThread = createServerThread(serverAddr, port);

    do {
        cin >> command;
        parseCommand(command);
    } while (command != "stop");
    pthread_cancel(static_cast<pthread_t>(serverThread));
    return 0;
}

int createServerThread(char *addr, int port) {
    //uchwyt na wątek
    pthread_t serverThread;
    server_opts *serverOpts;
    serverOpts = new server_opts;
    serverOpts->addr = addr;
    serverOpts->port = port;
    return createServerThread(serverThread, serverOpts);
}

int createServerThread(pthread_t &serverThread, const server_opts *serverOpts) {
    int create_result = pthread_create(&serverThread, nullptr, startServer, (void *) serverOpts);
    if (create_result != 0) {
        printf("Error while creating server thread, code: %d\n", create_result);
    } else {
        cout << "Server is running... " << "\n";
    }
    return create_result;
}

void cleanRoutine(void *arg) {
    cout << "Cleaning routine\n";
}

//strtok - rozbija string z delimiterem
void *startServer(void *serverOpts) {
    auto *options = (server_opts *) serverOpts;
    printf("Server works at %s:%d\n", options->addr, options->port);

    struct sockaddr_in sockAddr{};
    struct sockaddr_in remote{};

    int socketNum = socket(AF_INET, SOCK_STREAM, 0);
    if (socketNum < 0) {
        printf("Socket error\n");
        exit(-1);
    }
    //socket tylko uzywamy do czasu accept

    sockAddr.sin_family = AF_INET;
    inet_pton(AF_INET, options->addr, &sockAddr.sin_addr);
    sockAddr.sin_port = htons(static_cast<uint16_t>(options->port));
    sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    //bindowanie do socketu
    int time = 1;
    setsockopt(socketNum, SOL_SOCKET, SO_REUSEADDR, (char *) &time, sizeof(time));
    if (bind(socketNum, (struct sockaddr *) &sockAddr, sizeof(sockAddr)) < 0) {
        perror("Binding error");
        exit(-1);
    }


    if (listen(socketNum, QUEUE_SIZE) < 0) {
        perror("Listen error");
        exit(-1);
    }
    auto container = new Container();
    pthread_cleanup_push(cleanRoutine, (void *) 1);
        socklen_t sockSize = sizeof(struct sockaddr);
        while (runserver) {
            int connection_descriptor = accept(socketNum, (struct sockaddr *) &remote, &sockSize);
            if (connection_descriptor < 0) {
                perror("Client accepting error, shutdown server...");
                runserver = false;
                continue;
            }

            char remoteAddr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(remote.sin_addr), remoteAddr, INET_ADDRSTRLEN);
            //pass structure with client's data port
            printf("Client connected with %s. Descriptor assigned: %d\n", remoteAddr, connection_descriptor);
            handleConnection(container, connection_descriptor, &remote);
        }
    pthread_cleanup_pop(true);
    close(socketNum);
    delete container;
    exit(0);
}

//funkcja obsługująca połączenie z nowym klientem
void handleConnection(Container *container, int connection_socket_descriptor, sockaddr_in* remote) {
    pthread_t clientThread;

    //dane, które zostaną przekazane do wątku
    //t_data jest usuwany po odlaczeniu sie klienta
    struct thread_data_t *t_data;
    t_data = new thread_data_t;
    t_data->socketDescriptor = connection_socket_descriptor;
    t_data->remote = remote;
    t_data->container = container;

    //tworzy watek dla nowego klienta
    int create_result = pthread_create(&clientThread, nullptr, connection, (void *) t_data);
    if (create_result != 0) {
        printf("Error while creating thread, code: %d\n", create_result);
        exit(-1);
    }
}

//funkcja opisującą zachowanie wątku - musi przyjmować argument typu (void *) i zwracać (void *)
void *connection(void *t_data) {
    pthread_detach(pthread_self());
    auto *th_data = (struct thread_data_t *) t_data;

    char remoteAddr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(th_data->remote->sin_addr), remoteAddr, INET_ADDRSTRLEN);

    cout << "Initialization successfull. Descriptor " << GREEN_TEXT(th_data->socketDescriptor) << " from "
         << GREEN_TEXT(remoteAddr) << ".\n";

    //main client's loop
    onConnection(th_data, remoteAddr);
    delete (struct thread_data_t *) t_data;
    pthread_exit(nullptr);
}

void onConnection(const thread_data_t *th_data, const char *remoteAddr) {
    auto *buffer = new char[BUFFER_SIZE];
    auto *client = new StreamerClient(th_data->socketDescriptor, th_data->container);
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t value = read(th_data->socketDescriptor, buffer, BUFFER_SIZE);
        if (value > 0) {
            manageRequestCoroutine(th_data, remoteAddr, buffer, client);
        } else if (buffer[0] == 0) {
            cout << RED_TEXT("Client from " << remoteAddr
                                            << ", descriptor "
                                            << th_data->socketDescriptor
                                            << " has disconnected!\n");
            delete client;
            return;
        } else {
            printf("Undefined response.\n");
        }
    }
}

void manageRequestCoroutine(const thread_data_t *th_data, const char *remoteAddr, char *buffer,
                            StreamerClient *client) {
    displayRequest(th_data->socketDescriptor, buffer);
    auto response = client->onNewMessage(buffer);
    auto serializedResponse = response.serialize();
    displayResponse(th_data->socketDescriptor, serializedResponse.c_str());
    const auto wrote = write(th_data->socketDescriptor, serializedResponse.c_str(), serializedResponse.size());
    if (wrote  == -1) {
                cout << RED_TEXT("Error while sending client response: \n\t")
                     << MAGENTA_TEXT(serializedResponse)
                     << RED_TEXT("\n to " << remoteAddr);
            }
}


void parseCommand(string command) {
    if (command.find("restart") != string::npos) {
        cout << GREEN_TEXT("Server restart.\n");
        runserver = false;
        sleep(1);
        runserver = true;
    } else if (command.find("stop") != string::npos) {
        cout << GREEN_TEXT("Stopping server....\n");
        runserver = false;
    } else {
        cout << RED_TEXT("No such function ") << WHITE_TEXT(command) << "\n";
    }
}


void displayRequest(int socketDescriptor, const char *request) {
    cout << YELLOW_TEXT("Client " << socketDescriptor) << "\n";
    cout << "\t" << MAGENTA_TEXT("Request from " << socketDescriptor << ":\t") << GREEN_TEXT(request);
}


void displayResponse(int socketDescriptor, const char *response) {
    cout << CYAN_TEXT("Client " << socketDescriptor) << "\n";
    cout << "\t" << MAGENTA_TEXT("Response to " << socketDescriptor << ":\t") << GREEN_TEXT(response) << endl;
}