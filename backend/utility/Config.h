#include <iostream>
#include <map>



#ifndef MUSICSTREAMER_CONFIGPARSER_H
#define MUSICSTREAMER_CONFIGPARSER_H

using namespace std;

class Config {
map<string, string> properties;
public:
    explicit Config(const string &path);
    string get(const string &property, const string &defValue);
    int get(const string &property, int defValue);
    string get(const string &property);


};


#endif //MUSICSTREAMER_CONFIGPARSER_H
