#ifndef MUSICSTREAMER_SERVERRUNNER_H
#define MUSICSTREAMER_SERVERRUNNER_H

#include <iostream>

#define DEF_COMMUN_PORT 10001
#define DEF_BROAD_PORT 10002
#define DEF_UPLOAD_PORT 21212
#define DEF_MIN_STREAMER_PORT 11111
#define DEF_MAX_STREAMER_PORT 33333
#define DEF_ADDR "127.0.0.1"
#define DEF_CONFIG_PATH "server.properties"
#define DEF_UPLOAD_DIR "uploaded-files/"

using namespace std;

void parseCommand(const string &command);
void cleanUp(int);


#endif // MUSICSTREAMER_SERVERRUNNER_H