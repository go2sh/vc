#include <Common/MemoryBuffer.h>
#include <Support/FileSystem.h>

using namespace vc;
using namespace vfs;

namespace vc {
namespace vfs {
FileSystem::~FileSystem() = default;

namespace detail {

class MemoryNode {
  std::shared_ptr<MemoryNode> Parrent;
  Status Stat;

public:
  MemoryNode(Status Stat) : Stat(Stat) {}
  virtual ~MemoryNode() = default;
};

class MemoryFile : public MemoryNode {
  std::unique_ptr<MemoryBuffer> Buffer;
};

class MemoryDirectory : public MemoryNode {
  std::vector<std::shared_ptr<MemoryNode>> Children;

public:
  MemoryDirectory(Status Stat) : MemoryNode(std::move(Stat)) {}
  virtual ~MemoryDirectory() = default;
};
} // namespace detail

namespace {
class MemoryFile : public File {
  virtual Status status() { return Status(); }
  virtual std::unique_ptr<MemoryBuffer> getBuffer() {
    return std::make_unique<MemoryBuffer>();
  }
};
} // namespace
MemoryFileSystem::MemoryFileSystem()
    : Root(new detail::MemoryDirectory(
          Status("", boost::filesystem::file_status(
                         boost::filesystem::file_type::directory_file)))) {}
MemoryFileSystem::~MemoryFileSystem() = default;

std::unique_ptr<File> MemoryFileSystem::getFile(const std::string &FileName) {
  return std::make_unique<MemoryFile>();
}

Status MemoryFileSystem::getStatus(const std::string &FileName) {
  return Status();
}

std::unique_ptr<MemoryBuffer>
MemoryFileSystem::getBuffer(const std::string &FileName) {
  return std::make_unique<MemoryBuffer>();
}

void MemoryFileSystem::addFile(const std::string &FileName, Status Stat,
                               std::unique_ptr<MemoryBuffer> Buffer) {}

} // namespace vfs
} // namespace vc