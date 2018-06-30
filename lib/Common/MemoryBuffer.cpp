#include "Common/MemoryBuffer.h"

#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

using namespace vc;

MemoryBuffer::~MemoryBuffer() {}

void MemoryBuffer::init(const char *Start, const char *End) {
  BufferStart = Start;
  BufferEnd = End;
};

std::unique_ptr<DefaultMemoryBuffer>
DefaultMemoryBuffer::createMemoryBuffer(std::size_t Size,
                                        const std::string &Identifier) {
  std::size_t MemSize =
      sizeof(DefaultMemoryBuffer) + Identifier.size() + 1 + Size + 1;
  char *Mem = static_cast<char *>(operator new(MemSize));
  char *IdentBuf = Mem + sizeof(DefaultMemoryBuffer);
  char *MemBuf = IdentBuf + Identifier.size() + 1;

  // Copy identifier
  std::memcpy(IdentBuf, Identifier.data(), Identifier.size() + 1);

  // Null-terminate buffer
  MemBuf[Size] = '\0';

  // Create object
  DefaultMemoryBuffer *Obj = new (Mem) DefaultMemoryBuffer();
  Obj->init(MemBuf, MemBuf + Size);

  return std::unique_ptr<DefaultMemoryBuffer>(Obj);
}

std::unique_ptr<MemoryBuffer>
MemoryBuffer::getFile(const std::string &Filename) {
  std::ifstream InputFile(Filename.c_str(), std::ios::in | std::ios::binary);
  std::size_t Size;

  // Get input filesize
  InputFile.seekg(0, std::ios::end);
  Size = InputFile.tellg();
  InputFile.seekg(0, std::ios::beg);

  // Create buffer and read actual data
  auto MemBuf = DefaultMemoryBuffer::createMemoryBuffer(Size, Filename);
  InputFile.read(MemBuf->getStart(), Size);
  InputFile.close();

  return std::move(MemBuf);
}

std::unique_ptr<MemoryBuffer> MemoryBuffer::getSTDIN() {
  std::vector<char> Buffer;

  //FIXME: Slow std implementation
  std::noskipws(std::cin);
  std::copy(std::istream_iterator<char>(std::cin), std::istream_iterator<char>(),
            std::back_inserter(Buffer));

  auto MemBuf =
      DefaultMemoryBuffer::createMemoryBuffer(Buffer.size(), "<STDIN>");
  std::copy(Buffer.begin(), Buffer.end(), MemBuf->getStart());

  return std::move(MemBuf);
}