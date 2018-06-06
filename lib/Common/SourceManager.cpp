#include "Common/SourceManager.h"
#include "Common/MemoryBuffer.h"

using namespace vc;

Detail::ContentCache::~ContentCache() {
  if (Buffer) {
    delete Buffer;
  }
}

MemoryBuffer *Detail::ContentCache::getBuffer() {
  if (Buffer) {
    return Buffer;
  } else {
    auto Buf = MemoryBuffer::getFile(Path);
    replaceBuffer(Buf.release());
  }
  return Buffer;
}

SourceFile SourceManager::createSourceFile(std::unique_ptr<MemoryBuffer> Buffer) {
  Detail::ContentCache *Entry = new Detail::ContentCache();
  Entry->replaceBuffer(Buffer.release());
  FileCache.push_back(Entry);
  
  return SourceFile::fromRawEncoding(FileCache.size() - 1);
}

SourceFile SourceManager::createSourceFile(const std::string &Path) {
  Detail::ContentCache *Entry = new Detail::ContentCache(Path);
  FileCache.push_back(Entry);

  return SourceFile::fromRawEncoding(FileCache.size() - 1);
}

// Detail::ContentCache *SourceManager::getContentCache(SourceFile File) {}

std::pair<SourceFile, unsigned> SourceManager::getDecomposedLocation(SourceLocation Loc) {
    return std::pair<SourceFile, unsigned>(SourceFile::fromRawEncoding(1), Loc.getRawEnconding() - 1);
}

MemoryBuffer *SourceManager::getBuffer(SourceFile File) {
  Detail::ContentCache *Cache = FileCache[File.ID];
  return Cache->getBuffer();
}
uint32_t SourceManager::getColumnNumber(SourceFile File, uint32_t Offset) {
  if (FileCache.size() < File.ID)
    return 0;
  Detail::ContentCache *Content = FileCache[File.ID];
  if (Content->getBuffer()->getSize() < Offset)
    return 0;
  const char *Buf = Content->getBuffer()->getStart();
  const char *Pos = Buf;
  const char *BufEnd = Buf + Offset;
  while (Buf <= BufEnd) {
    if (*Buf == '\n')
      Pos = Buf;
    Buf++;
  }
  return static_cast<uint32_t>(BufEnd - Pos);
}

uint32_t SourceManager::getLineNumber(SourceFile File, uint32_t Offset) {
  if (FileCache.size() < File.ID)
    return 0;
  Detail::ContentCache *Content = FileCache[File.ID];
  if (Content->getBuffer()->getSize() < Offset)
    return 0;
  const char *Buf = Content->getBuffer()->getStart();
  const char *BufEnd = Buf + Offset;
  uint32_t Line = 1;
  while (Buf <= BufEnd) {
    if (*Buf == '\n')
      Line++;
    Buf++;
  }
  return Line;
}

std::string SourceManager::getPath(SourceFile File) {
  return FileCache[File.ID]->getPath();
}