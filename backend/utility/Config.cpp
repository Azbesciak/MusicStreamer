#include "Config.h"
#include <fstream>
#include <sstream>


string Config::get(const string &property) {
    return properties[property];
}

string Config::get(const string &property, const string & defValue) {
    string val = get(property);
    return val.empty() ? defValue : val;
}

int Config::get(const string &property, int defValue) {
    auto val = get(property);
    return val.empty() ? defValue : atoi(val.c_str());
}


Config::Config(const string &path) {

    std::ifstream configFile(path);

    string line;
    while (getline(configFile, line)) {
        istringstream is_line(line);
        string key;
        if (getline(is_line, key, '=')) {
            std::string value;
            if (std::getline(is_line, value))
                properties[key] = value;
        }
    }
    configFile.close();
}
