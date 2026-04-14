#include <iostream>
#include <string>
using namespace std;

/*
Afficher les événements (debug + analyse)
*/

class Logger {
public:
    static void log(std::string msg) {
        std::cout << msg << std::endl;
    }
};