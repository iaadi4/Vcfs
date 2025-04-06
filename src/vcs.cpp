#include <iostream>
#include "vcs.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

void init_vcfs() {
    if(fs::exists(".vcfs")) {
        std::cerr << "Error: vcfs already initialized in this directory" << std::endl;
        return;
    }
    fs::create_directory(".vcfs");
    fs::create_directory(".vcfs/commits");
    fs::create_directory(".vcfs/blobs");
    std::ofstream index(".vcfs/index");
    if(!index) {
        std::cerr << "Error: could not create index file" << std::endl;
        return;
    }
    index.close();
    std::cout << "Vcfs initialized successfully" << std::endl;
    return;
}