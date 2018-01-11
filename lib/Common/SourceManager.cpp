#include "Common/SourceManager.h"
#include "Common/MemoryBuffer.h"
#include "SourceManager.h"


uint32_t SourceManager::getLineNumber(FileLocation Location) {
    if (FileCache.size() < FileLocation.getID())
        return 0;
    ContentCache *Content = FileCache[FileLocation.getID()]
    if (Content->getBuffer()->getLength() < Location.getOffset())
        return 0;
    char *Buf = Content->getBuffer()->getBufferBegin();
    char *BufEnd = Buf + (char *)Location.getOffset();
    uint32_t Line = 1;
    while (Buf <= BufEnd) {
        if (*Buf == '\n') Line++;
        Buf++;
    }
    return Line;
}