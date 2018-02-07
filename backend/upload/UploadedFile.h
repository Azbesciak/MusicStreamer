#ifndef MUSICSTREAMER_UPLOADEDFILE_H
#define MUSICSTREAMER_UPLOADEDFILE_H

#include <string>

class UploadedFile {

private:

    std::string fileName;

public:

    UploadedFile(const std::string& fileName);

    const std::string& getFileName();
};


#endif //MUSICSTREAMER_UPLOADEDFILE_H
