#include <iostream>
#include "vcs.h"
#include <filesystem>
#include <fstream>
#include "huffman.h"
#include "sha256.h"
#include <vector>
#include <ctime>
#include <map>

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

void add_all() {
    if(!fs::exists(".vcfs")) {
        std::cerr << "Error: vcfs not initialized in this directory" << std::endl;
        return;
    }
    for (const auto& entry : fs::directory_iterator(".")) {
        if (entry.is_regular_file() && entry.path().filename() != ".vcfs") {
            add_file(entry.path().string());
        }
    }
}

void commit_changes(const std::string& message) {
    std::string index_path = ".vcfs/index";
    std::ifstream index(index_path);
    if (!index) {
        std::cerr << "Error: could not open index file" << std::endl;
        return;
    }

    std::vector<std::string> entries;
    std::string line;
    while (std::getline(index, line)) {
        entries.push_back(line);
    }
    index.close();

    if (entries.empty()) {
        std::cout << "No changes to commit." << std::endl;
        return;
    }

    std::time_t now = std::time(nullptr);
    std::string timestamp = std::ctime(&now);
    timestamp.pop_back();

    std::string raw = message + timestamp;
    std::string commit_hash = sha256(raw);

    std::string commit_path = ".vcfs/commits/" + commit_hash;
    std::ofstream commit_file(commit_path);
    if (!commit_file) {
        std::cerr << "Error: could not create commit file" << std::endl;
        return;
    }

    commit_file << "message: " << message << std::endl;
    commit_file << "timestamp: " << timestamp << std::endl;
    for (const std::string& entry : entries) {
        commit_file << "file: " << entry << std::endl;
    }
    commit_file.close();

    std::ofstream clear_index(index_path, std::ios::trunc);
    clear_index.close();

    std::cout << "Committed successfully with hash: " << commit_hash << std::endl;
    return;
}

void show_log() {
    std::string commits_dir = ".vcfs/commits";

    if (!fs::exists(commits_dir)) {
        std::cerr << "Error: No commits directory found. Is VCFS initialized?\n";
        return;
    }

    std::map<std::string, std::string> logs;

    for (const auto& entry : fs::directory_iterator(commits_dir)) {
        std::ifstream commit_file(entry.path());
        if (!commit_file) continue;

        std::string line, message, timestamp;
        while (std::getline(commit_file, line)) {
            if (line.find("message: ") == 0) {
                message = line.substr(9);
            } else if (line.find("timestamp: ") == 0) {
                timestamp = line.substr(11);
            }
        }

        std::string commit_hash = entry.path().filename().string();
        std::string log_entry = "Commit: " + commit_hash + "\nMessage: " + message + "\nTimestamp: " + timestamp + "\n";
        logs[timestamp] = log_entry;
    }

    if (logs.empty()) {
        std::cout << "No commits found.\n";
        return;
    }

    std::cout << "=== Commit History ===\n";
    for (auto it = logs.rbegin(); it != logs.rend(); ++it) {
        std::cout << it->second << "\n";
    }
}
