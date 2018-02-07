
#include "RequestReader.h"
#include "unistd.h"
using namespace std;


RequestReader::RequestReader(int socketFd)
        :socketFd(socketFd), buffer(new char[BUFFER_SIZE]) {
}
ssize_t RequestReader::readFromSocket() {
    memset(buffer, 0, BUFFER_SIZE);
    return read(socketFd, buffer, BUFFER_SIZE);
}

#define readReq \
    auto r = readFromSocket(); \
    if (r <= 0) { \
        requests.push_back(nullptr);\
        return requests;\
    }\
    originalMes += string(buffer);

std::vector<Request*> RequestReader::readRequest() {
    vector<Request*> requests;
    string originalMes;
//    originalMes.tr
    readReq;
    int startFlag = 0;
    int startIndex = 0;
    for (int i = 0; i < originalMes.length(); i++) {
        if (originalMes[i] == '{') {
            if (startFlag == 0) {
                startIndex = i;
            }
            startFlag++;
        } else if (originalMes[i] == '}') {
            startFlag--;
            if (startIndex < i && startFlag == 0) {
                auto requestContent = originalMes.substr(startIndex, i - startIndex + 1);
                requests.push_back(new Request(requestContent));
                originalMes = originalMes.substr(i + 1);
                i = -1; // for will increment to 0
            } else if (startFlag < 0){
                requests.push_back(new Request(originalMes, false));
                return requests;
            }
        }
        if (i >= originalMes.length() - 1 && startFlag > 0) {
            readReq;
        }
    }
    return requests;
}
