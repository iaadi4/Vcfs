#ifndef HUFFMAN_H
#define HUFFMAN_H
#include <string>

std::string huffman_encode(const std::string &input);
std::string huffman_decode(const std::string &compressed);

#endif