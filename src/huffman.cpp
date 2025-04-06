#include "huffman.h"
#include <queue>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>

struct HuffmanNode {
    char ch;
    int freq;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->freq > b->freq;
    }
};

std::unordered_map<char, int> buildFrequencyMap(const std::string& input) {
    std::unordered_map<char, int> freqMap;
    for (char c : input) freqMap[c]++;
    return freqMap;
}

HuffmanNode* buildHuffmanTree(const std::unordered_map<char, int>& freqMap) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, Compare> pq;
    for (auto& pair : freqMap) {
        pq.push(new HuffmanNode(pair.first, pair.second));
    }
    while (pq.size() > 1) {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();
        HuffmanNode* merged = new HuffmanNode('\0', left->freq + right->freq);
        merged->left = left;
        merged->right = right;
        pq.push(merged);
    }
    return pq.top();
}

void generateCodes(HuffmanNode* root, std::string code, std::unordered_map<char, std::string>& huffmanCodes) {
    if (!root) return;
    if (!root->left && !root->right) huffmanCodes[root->ch] = code;
    generateCodes(root->left, code + "0", huffmanCodes);
    generateCodes(root->right, code + "1", huffmanCodes);
}

std::string encode(const std::string& input, std::unordered_map<char, std::string>& huffmanCodes) {
    std::string encoded = "";
    for (char c : input) encoded += huffmanCodes[c];
    return encoded;
}

std::string huffman_encode(const std::string& input) {
    auto freqMap = buildFrequencyMap(input);
    HuffmanNode* root = buildHuffmanTree(freqMap);
    std::unordered_map<char, std::string> huffmanCodes;
    generateCodes(root, "", huffmanCodes);

    std::string encoded = encode(input, huffmanCodes);

    std::string header = "";
    for (auto& pair : huffmanCodes) {
        header += pair.first;
        header += ":";
        header += pair.second;
        header += ";";
    }

    return header + "|" + encoded;
}

std::string huffman_decode(const std::string& compressed) {
    size_t sep = compressed.find('|');
    if (sep == std::string::npos) return "";

    std::string header = compressed.substr(0, sep);
    std::string encoded = compressed.substr(sep + 1);

    std::unordered_map<std::string, char> reverseCodes;
    std::stringstream ss(header);
    std::string entry;
    while (std::getline(ss, entry, ';')) {
        if (entry.empty()) continue;
        char ch = entry[0];
        std::string code = entry.substr(2);
        reverseCodes[code] = ch;
    }

    std::string decoded = "", current = "";
    for (char bit : encoded) {
        current += bit;
        if (reverseCodes.count(current)) {
            decoded += reverseCodes[current];
            current = "";
        }
    }

    return decoded;
}