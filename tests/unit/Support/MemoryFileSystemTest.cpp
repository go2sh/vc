#include <Support/FileSystem.h>
#include <boost/filesystem.hpp>
#include <catch2/catch.hpp>


TEST_CASE("Basic MemoryFileSystem operations") {
    vc::vfs::MemoryFileSystem FS;
    std::unique_ptr<vc::MemoryBuffer> Buffer = vc::MemoryBuffer::getMemoryBuffer("Test Data.", "Test");

    SECTION("add file and get status" ) {
        FS.addFile("testfile.txt",vc::vfs::Status("testfile.txt",boost::filesystem::file_status(
                         boost::filesystem::file_type::directory_file),12), std::move(Buffer));
    }

}