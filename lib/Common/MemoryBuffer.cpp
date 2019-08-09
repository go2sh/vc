#include "Common/MemoryBuffer.h"

#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>

using namespace vc;

template <class MB>
static std::unique_ptr<MB> getFileImpl(const std::string &Filename,
                                    uint64_t FileSize = -1) {
  boost::filesystem::file_status Status = boost::filesystem::status(Filename);

  if (Status.type() != boost::filesystem::file_type::regular_file &&
      Status.type() != boost::filesystem::file_type::block_file) {
    throw std::invalid_argument("Must be a valid file.");
  }

  if (FileSize == -1) {
    FileSize = boost::filesystem::file_size(Filename);
  }

  std::ifstream InputFile(Filename.c_str(), std::ios::in | std::ios::binary);

  // Create buffer and read actual data
  auto MemBuf = DefaultMemoryBuffer::createMemoryBuffer(FileSize, Filename);
  InputFile.read(MemBuf->getStart(), FileSize);
  InputFile.close();

  return std::move(MemBuf);
}

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
MemoryBuffer::getMemoryBuffer(StringRef Data, const std::string &Filename) {
  auto MemBuf = DefaultMemoryBuffer::createMemoryBuffer(0, Filename);
  MemBuf->init(Data.data(), Data.data() + Data.size());

  return std::move(MemBuf);
}

std::unique_ptr<MemoryBuffer>
MemoryBuffer::getFile(const std::string &Filename) {
  return getFileImpl<MemoryBuffer>(Filename);
}

std::unique_ptr<MemoryBuffer> MemoryBuffer::getSTDIN() {
  std::vector<char> Buffer;

  // FIXME: Slow std implementation
  std::noskipws(std::cin);
  std::copy(std::istream_iterator<char>(std::cin),
            std::istream_iterator<char>(), std::back_inserter(Buffer));

  auto MemBuf =
      DefaultMemoryBuffer::createMemoryBuffer(Buffer.size(), "<STDIN>");
  std::copy(Buffer.begin(), Buffer.end(), MemBuf->getStart());

  return std::move(MemBuf);
}