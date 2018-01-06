#include <iostream>
// https://github.com/nlohmann/json
#include "json.hpp"


using json = nlohmann::json;
using namespace std;

int main() {

    json j;
    j["ala"] = "ma kota";
    cout<< j.dump(2) <<endl;
}