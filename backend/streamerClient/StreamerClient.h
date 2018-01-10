#include <string>

#ifndef MUSICSTREAMER_STREAMERCLIENT_H
#define MUSICSTREAMER_STREAMERCLIENT_H

using namespace std;
class StreamerClient {
private:
    int socketDescriptor;
    string name;
public:
    explicit StreamerClient(int socketDescriptor);
    ~StreamerClient();
    string getName() const;
    void setName(const string& name);
    ssize_t sendMessage(const string &mes);
};


#endif //MUSICSTREAMER_STREAMERCLIENT_H
