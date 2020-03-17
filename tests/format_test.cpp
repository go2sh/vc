#include <Common/SourceManager.h>
#include <Format/Format.h>

int main(int argc, char **argv) {
  vc::SourceManager ScrMgr;
  vc::SourceFile SF;
  vc::format::FormatStyle Style;

  try {
    SF = ScrMgr.createSourceFile(vc::MemoryBuffer::getFile("test.vhd"));
  } catch (std::exception e) {
    std::cerr << "Fatal error: " << e.what() << std::endl;
  }
  vc::format::reformat(ScrMgr, SF, Style);
}
