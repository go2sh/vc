#include "Common/MemoryBuffer.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace vc;

void MemoryBuffer::init(const char *Start, const char *End) {
  BufferStart = Start;
  BufferEnd = End;
};

std::unique_ptr<MemoryBuffer>
MemoryBuffer::getFile(const std::string &Filename) {
  std::ifstream inputFile(Filename.c_str(), std::ios::in | std::ios::binary);
  std::unique_ptr<MemoryBuffer> MemBuf = std::make_unique<MemoryBuffer>();
  std::size_t size;
  char *data;

  // Get input filesize
  inputFile.seekg(0, std::ios::end);
  size = inputFile.tellg();
  inputFile.seekg(0, std::ios::beg);

  // Create buffer and read actual data
  data = (char *)malloc(size);
  inputFile.read(data, size);
  data[size - 1] = 0;

  inputFile.close();

  MemBuf->init(data, data + size - 1);
  return std::move(MemBuf);
}

std::unique_ptr<MemoryBuffer> MemoryBuffer::getSTDIN() {
  std::vector<char> Buffer;
  std::unique_ptr<MemoryBuffer> MemBuf = std::make_unique<MemoryBuffer>();
  std::size_t Size;
  char *Mem;

  // FIXME: Slow vector implementation for reeding
  Buffer.reserve(4096);

  while (!std::cin.eof()) {
    std::cin.read(Buffer.data() + Buffer.size(), 4096);
    Buffer.resize(std::cin.gcount());
  }

  Size = Buffer.size() + 1;
  Mem = static_cast<char*>(std::malloc(Size));

  std::memcpy(Mem, Buffer.data(), Buffer.size());
  Mem[Buffer.size()] = '\0';
  MemBuf->init(Mem, Size);

  return MemBuf;
}