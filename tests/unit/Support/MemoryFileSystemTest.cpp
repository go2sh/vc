#include <Support/FileSystem.h>
#include <boost/filesystem.hpp>
#include <catch2/catch.hpp>

#include "../../common/matcher/MemoryBufferMatcher.h"

TEST_CASE("Basic MemoryFileSystem operations") {
  vc::vfs::MemoryFileSystem FS;
  std::unique_ptr<vc::MemoryBuffer> Buffer =
      vc::MemoryBuffer::getMemoryBuffer("Test Data.", "Test");
  std::unique_ptr<vc::MemoryBuffer> DataBuffer = vc::MemoryBuffer::getMemoryBuffer("Test Data.", "Test");
  std::size_t BufferSize = Buffer->getSize();

  SECTION("add file and get status") {
    FS.addFile("testfile.txt",
               boost::filesystem::file_status(
                   boost::filesystem::file_type::regular_file),
               BufferSize, std::move(Buffer));
    vc::vfs::Status Stat = FS.status("testfile.txt");
    CHECK(Stat.getType() == boost::filesystem::file_type::regular_file);
    CHECK(Stat.getSize() == 10);
    CHECK(Stat.getName() == "testfile.txt");
  }

  SECTION("add nested file and get status") {
    FS.addFile("a/b/testfile.txt",
               boost::filesystem::file_status(
                   boost::filesystem::file_type::regular_file),
               BufferSize, std::move(Buffer));
    vc::vfs::Status Stat = FS.status("a/b/testfile.txt");
    CHECK(Stat.getType() == boost::filesystem::file_type::regular_file);
    CHECK(Stat.getSize() == 10);
    CHECK(Stat.getName() == "a/b/testfile.txt");
  }

  SECTION("add file and get buffer") {
    FS.addFile("testfile.txt",
               boost::filesystem::file_status(
                   boost::filesystem::file_type::regular_file),
               BufferSize, std::move(Buffer));
    std::unique_ptr<vc::MemoryBuffer> FileBuffer = FS.getBuffer("testfile.txt");
    REQUIRE_THAT(FileBuffer.get(), Equals(DataBuffer.get()));
  }
}