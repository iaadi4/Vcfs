#include <iostream>
#include "vcs.h"

int main(int argc, char *argv[]) {
    if(argc != 2) {
        std::cerr << "Usage: " << argv[0] << "<command>" << std::endl;
        return 1;
    }
    std::string command = argv[1];
    if(command == "init") {
        init_vcfs();
    } else {
        std::cerr << "Unknown command: " << command << std::endl;
        return 1;
    }
    return 0;
}