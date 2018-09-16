#include <Common/SourceManager.h>
#include <Format/Format.h>

#include <boost/utility.hpp>
#include <boost/filesystem.hpp>

int main(int argc, char **argv) {

  vc::SourceManager ScrMgr;
  vc::format::FormatStyle Style;

  vc::SourceFile SF = ScrMgr.createSourceFile(vc::MemoryBuffer::getFile("/home/chris/test.vhd"));
  vc::format::reformat(ScrMgr, SF, Style);
}
