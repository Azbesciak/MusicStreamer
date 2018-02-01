#include "FileUploadException.h"


FileUploadException::FileUploadException(int statusCode, std::string message) throw() {

    this->statusCode = statusCode;
    this->message = message;
}


const char* FileUploadException::what() const throw() {
    return message.c_str();
}

int FileUploadException::getStatusCode() {
    return statusCode;
}
