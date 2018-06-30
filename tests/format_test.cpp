#include <Common/SourceManager.h>
#include <Format/Format.h>

int main(int argc, char **argv) {
  vc::SourceManager ScrMgr;
  vc::format::FormatStyle Style;

  vc::SourceFile SF = ScrMgr.createSourceFile(vc::MemoryBuffer::getSTDIN());
  vc::format::reformat(ScrMgr, SF, Style);
}
