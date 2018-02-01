#include "Common/MemoryBuffer.h"
#include <fstream>
#include <iostream>
#include <string>
#include <memory>


using namespace vc;

void MemoryBuffer::init(const char *Start, const char *End){
  BufferStart = Start;
  BufferEnd = End;
};


std::unique_ptr<MemoryBuffer> MemoryBuffer::getFile(const std::string &Filename) {
    std::ifstream inputFile(Filename.c_str(), std::ios::in | std::ios::binary);
    std::unique_ptr<MemoryBuffer> mem = std::make_unique<MemoryBuffer>();
    std::size_t size;
    char *data;

    // Get input filesize
    inputFile.seekg(0, std::ios::end);
    size = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);

    // Create buffer and read actual data
    data = (char*) malloc(size);
    inputFile.read(data, size);
    data[size-1] = 0;

    inputFile.close();

    mem->init(data, data+size - 1);
    return std::move(mem);
}