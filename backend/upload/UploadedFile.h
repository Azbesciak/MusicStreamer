#ifndef MUSICSTREAMER_UPLOADEDFILE_H
#define MUSICSTREAMER_UPLOADEDFILE_H

#include <string>

class UploadedFile {

private:

    std::string fileName;
    std::string filePath;

public:

    UploadedFile(const std::string& fileName, const std::string & path);

    std::string getFileName();

    std::string getFilePath();
};


#endif //MUSICSTREAMER_UPLOADEDFILE_H
