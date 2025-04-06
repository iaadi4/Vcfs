#include <iostream>
#include "vcs.h"
#include <filesystem>
#include <fstream>
#include "huffman.h"
#include "sha256.h"

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

void add_file(const std::string &filename) {
    if(!fs::exists(".vcfs")) {
        std::cerr << "Error: vcfs not initialized in this directory" << std::endl;
        return;
    }
    if(!fs::exists(filename)) {
        std::cerr << "Error: file " << filename << " does not exist" << std::endl;
        return;
    }
    std::ifstream file(filename);
    if(!file) {
        std::cerr << "Error: could not open file " << filename << std::endl;
        return;
    }
    // Read the file content
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    std::string hash = sha256(content);
    std::string blob_path = ".vcfs/blobs/" + hash;
    if(fs::exists(blob_path)) {
        std::cout << "File " << filename << " is alredy added" << std::endl;
        return;
    }
    std::ofstream blob(blob_path, std::ios::binary);
    if(!blob) {
        std::cerr << "Error: could not create blob file" << std::endl;
        return;
    }
    blob << huffman_encode(content);
    blob.close();
    std::ofstream index(".vcfs/index", std::ios::app);
    if(!index) {
        std::cerr << "Error: could not open index file" << std::endl;
        return;
    }
    index << hash << " " << filename << std::endl;
    index.close();
    std::cout << "File " << filename << " added to vcfs" << std::endl;
    return;
}