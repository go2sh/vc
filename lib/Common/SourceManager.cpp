#include "Common/SourceManager.h"
#include "Common/MemoryBuffer.h"

using namespace vc;

MemoryBuffer *Detail::ContentCache::getBuffer() {
  if (Buffer) {
    return Buffer;
  } else {
      auto Buf = MemoryBuffer::getFile(Path);
      setBuffer(Buf.release());
  }
  return Buffer;
}

SourceFile SourceManager::createSourceFile(const std::string &Path) {
  Detail::ContentCache *Entry = new Detail::ContentCache(Path);
  FileCache.push_back(Entry);

  return SourceFile::fromRawEncoding(FileCache.size() - 1);
}

Detail::ContentCache* getContentCache(SourceFile File) const {
    
}
MemoryBuffer *SourceManager::getBuffer(SourceFile File) const {

}
uint32_t SourceManager::getLineNumber(SourceFile File, uint32_t Offset) {
  if (FileCache.size() < File[ID])
    return 0;
  Detail::ContentCache *Content = FileCache[File.ID];
  if (Content->getBuffer()->getBufferSize() < Offset)
    return 0;
  const char *Buf = Content->getBuffer()->getBufferStart();
  const char *BufEnd = Buf + Offset;
  uint32_t Line = 1;
  while (Buf <= BufEnd) {
    if (*Buf == '\n')
      Line++;
    Buf++;
  }
  return Line;
}