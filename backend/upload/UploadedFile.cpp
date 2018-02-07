#include "UploadedFile.h"

using namespace std;


UploadedFile::UploadedFile(const string& fileName) {
    this->fileName = fileName;
}


const string& UploadedFile::getFileName() {
    return fileName;
}
