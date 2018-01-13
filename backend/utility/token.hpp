#ifndef MUSICSTREAMER_TOKEN_HPP
#define MUSICSTREAMER_TOKEN_HPP

#include <string>
#include <ctime>
#include <random>

class TokenGenerator {

public:

    static std::string alfanumeric(int size) {

        static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        static int alphanums = sizeof(alphanum) / sizeof(alphanum[0]);

        std::string token = "";

        std::mt19937 mt((unsigned long) time(0));
        std::uniform_int_distribution<> dist(0, alphanums - 1);

        for (int i = 0; i < size; ++i)
            token += alphanum[dist(mt)];

        return token;
    }
};

#endif //MUSICSTREAMER_TOKEN_HPP
