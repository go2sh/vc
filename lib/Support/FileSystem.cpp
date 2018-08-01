#include <Common/MemoryBuffer.h>
#include <Support/FileSystem.h>

#include <map>

using namespace vc;
using namespace vfs;

namespace vc {
namespace vfs {
FileSystem::~FileSystem() = default;

namespace detail {

class MemoryNode {
  Status Stat;

public:
  MemoryNode(Status Stat) : Stat(Stat) {}
  virtual ~MemoryNode() = default;

  Status &getStatus() { return Stat; }
};

class MemoryFile : public MemoryNode {
  std::unique_ptr<MemoryBuffer> Buffer;

public:
  MemoryFile(Status Stat, std::unique_ptr<MemoryBuffer> Buffer)
      : MemoryNode(std::move(Stat)), Buffer(std::move(Buffer)) {}

  MemoryBuffer *getBuffer() const { return Buffer.get(); }
};

class MemoryDirectory : public MemoryNode {
  std::map<std::string, std::unique_ptr<MemoryNode>> Children;

public:
  MemoryDirectory(Status Stat) : MemoryNode(std::move(Stat)) {}

  std::optional<MemoryNode *> getChild(const std::string &Name) {
    auto Result = Children.find(Name);
    if (Result != Children.end()) {
      return Result->second.get();
    }
    return std::optional<MemoryNode *>();
  }
  MemoryNode *addChild(std::string Name, std::unique_ptr<MemoryNode> Child) {
    auto Entry = Children.insert(
        std::make_pair<std::string, std::unique_ptr<MemoryNode>>(
            std::move(Name), std::move(Child)));
    return Entry.first->second.get();
  }
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

std::unique_ptr<MemoryBuffer>
MemoryFileSystem::getBuffer(const std::string &FileName) {
  auto Node = lookupNode(FileName);
  auto File = dynamic_cast<detail::MemoryFile *>(*Node);
  if (Node && File) {
    MemoryBuffer *Buf = File->getBuffer();
    return MemoryBuffer::getMemoryBuffer(Buf->getBuffer(),
                                         Buf->getIdentifier());
  }
  return std::make_unique<MemoryBuffer>();
}

Status MemoryFileSystem::status(const std::string &FileName) {
  auto Node = lookupNode(FileName);
  if (Node)
    return (*Node)->getStatus();
  return Status(std::string(),
                boost::filesystem::file_status(
                    boost::filesystem::file_type::file_not_found),
                -1);
}

void MemoryFileSystem::addFile(const std::string &FileName,
                               const boost::filesystem::file_status &FileStatus,
                               const boost::uintmax_t FileSize,
                               std::unique_ptr<MemoryBuffer> Buffer) {
  boost::filesystem::path FilePath(FileName);
  boost::filesystem::path CurrentPath;
  detail::MemoryDirectory *CurrentDir = Root.get();

  for (auto PathIt = FilePath.begin(); PathIt != FilePath.end(); PathIt++) {
    auto Node = CurrentDir->getChild(PathIt->string());
    CurrentPath /= *PathIt;
    if (!Node) {
      if (std::next(PathIt, 1) == FilePath.end()) {
        Status Stat(FileName, FileStatus, FileSize);
        if (FileStatus.type() == boost::filesystem::file_type::directory_file) {
          CurrentDir->addChild(
              PathIt->string(),
              std::make_unique<detail::MemoryDirectory>(std::move(Stat)));
        } else {
          CurrentDir->addChild(PathIt->string(),
                               std::make_unique<detail::MemoryFile>(
                                   std::move(Stat), std::move(Buffer)));
        }
        return;
      }

      Status DirStat(CurrentPath.string(),
                     boost::filesystem::file_status(
                         boost::filesystem::file_type::directory_file));
      CurrentDir = static_cast<detail::MemoryDirectory *>(CurrentDir->addChild(
          PathIt->string(),
          std::make_unique<detail::MemoryDirectory>(std::move(DirStat))));
      continue;
    } else {
      if (auto *NextDir = dynamic_cast<detail::MemoryDirectory *>(*Node)) {
        if (std::next(PathIt, 1) == FilePath.end()) {
          return;
        }
        CurrentDir = NextDir;
      } else {
        return;
      }
    }
  }
}

std::optional<detail::MemoryNode *>
MemoryFileSystem::lookupNode(const std::string &FileName) {
  detail::MemoryDirectory *CurrentDir = Root.get();
  boost::filesystem::path Path(FileName);

  for (auto PathIt = Path.begin(); PathIt != Path.end(); PathIt++) {
    auto Result = CurrentDir->getChild(PathIt->string());
    if (!Result) {
      break;
    }
    if (std::next(PathIt, 1) == Path.end()) {
      return Result;
    }
    if (auto NextDir = dynamic_cast<detail::MemoryDirectory *>(*Result)) {
      CurrentDir = NextDir;
    } else {
      break;
    }
  }
  return std::optional<detail::MemoryNode *>();
}
} // namespace vfs
} // namespace vc