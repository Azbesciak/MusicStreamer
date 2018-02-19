#include "UploadedFile.h"

using namespace std;


UploadedFile::UploadedFile(const string& fileName, const string & path):
    fileName(fileName), filePath(path) {}


string UploadedFile::getFileName() {
    return fileName;
}

string UploadedFile::getFilePath() {
    return filePath;
}
