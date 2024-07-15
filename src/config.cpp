#include "config.h"

void print(std::string msg) {
    #if DEBUG_PRINT == 1 
        cout<< msg << endl;
    #endif
}

