#include "Common/SourceManager.h"
#include "Common/MemoryBuffer.h"
#include "SourceManager.h"

using namespace vc;
using namespace vc::SourceManager;

uint32_t SourceManager::getLineNumber(FileLocation Location) {
    if (FileCache.size() < FileLocation.getFileID())
        return 0;
    ContentCache *Content = FileCache[FileLocation.getFileID()];
    if (Content->getBuffer()->getBufferSize() < Location.getOffset())
        return 0;
    const char *Buf = Content->getBuffer()->getBufferStart();
    char *BufEnd = Buf + (char *)Location.getOffset();
    uint32_t Line = 1;
    while (Buf <= BufEnd) {
        if (*Buf == '\n') Line++;
        Buf++;
    }
    return Line;
}