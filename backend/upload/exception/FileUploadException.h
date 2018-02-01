#ifndef MUSICSTREAMER_FILEBYTESUPLOADEXCEPTION_H
#define MUSICSTREAMER_FILEBYTESUPLOADEXCEPTION_H

#include <string>
#include <exception>


class FileUploadException : std::exception {

private:

    int statusCode;
    std::string message;

public:

    FileUploadException(int statusCode, std::string message) throw();

    virtual const char* what() const throw();
    int getStatusCode();
};


#endif //MUSICSTREAMER_FILEBYTESUPLOADEXCEPTION_H
