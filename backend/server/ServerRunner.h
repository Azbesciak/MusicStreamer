#ifndef MUSICSTREAMER_SERVERRUNNER_H
#define MUSICSTREAMER_SERVERRUNNER_H

#include <iostream>

#define DEFAULT_PORT 10001
#define DEFAULT_ADDR "127.0.0.1"
#define DEFAULT_CONFIG_PATH "../server.properties"

using namespace std;

void parseCommand(const string &command);
void cleanUp(int);


#endif // MUSICSTREAMER_SERVERRUNNER_H