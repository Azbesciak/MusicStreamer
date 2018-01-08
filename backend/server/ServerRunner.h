#include "../utility/TerminalUtils.h"
#include "../streamerClient/StreamerClient.h"
#include "Container.h"
//system includes
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <dirent.h>
#include <iostream>

#define BUFFER_SIZE 1000
#define QUEUE_SIZE 5

#define DEFAULT_PORT 10001
#define DEFAULT_ADDR "127.0.0.1"

using namespace std;

struct server_opts
{
    char * addr;
    int port;
};

//struktura zawierająca dane, które zostaną przekazane do wątku
struct thread_data_t
{
    int socketDescriptor;
    struct sockaddr_in *remote;
    Container* container;
};

void *startServer(void *server_opts);
void *connection(void *t_data);
void handleConnection(Container *connection_socket_descriptor, int remote, sockaddr_in *pIn);
void parseCommand(string command);
int createServerThread(char * addr, int port);
void cleanRoutine(void *arg);
void displayRequest(int socketDescriptor, const char * request);
void displayResponse(int socketDescriptor, const char *request);
void manageRequestCoroutine(const thread_data_t *th_data, const char *remoteAddr, char *buffer,
                            StreamerClient *client);
int createServerThread(pthread_t &serverThread, const server_opts *serverOpts);
void onConnection(const thread_data_t *th_data, const char *remoteAddr);
