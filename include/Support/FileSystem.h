#include <Common/MemoryBuffer.h>

#include <filesystem>
#include <memory>
#include <optional>

namespace vc {
namespace vfs {

namespace fs = std::filesystem;

class Status {
  std::string FileName;
  fs::file_status FileStatus;
  std::uintmax_t FileSize;

public:
  Status() {}
  Status(const std::string &FileName, const fs::file_status &FileStatus,
         const std::uintmax_t FileSize = -1)
      : FileName(FileName), FileStatus(FileStatus), FileSize(FileSize) {}

  std::string getName() const { return FileName; }
  fs::file_type getType() const { return FileStatus.type(); }
  fs::perms getPermissions() const { return FileStatus.permissions(); }
  std::uintmax_t getSize() const { return FileSize; }

  bool isValid() { return fs::status_known(FileStatus); }
};

class File {
public:
  virtual Status status() = 0;
  virtual std::unique_ptr<MemoryBuffer> getBuffer() = 0;
};
class FileSystem {

public:
  virtual ~FileSystem();

  virtual std::unique_ptr<File> getFile(const std::string &FileName) = 0;
  virtual Status status(const std::string &FileName) = 0;
  virtual std::unique_ptr<MemoryBuffer>
  getBuffer(const std::string &FileName) = 0;
};

namespace detail {
class MemoryNode;
class MemoryDirectory;

} // namespace detail
class MemoryFileSystem : public FileSystem {
  std::unique_ptr<detail::MemoryDirectory> Root;

public:
  MemoryFileSystem();
  ~MemoryFileSystem() override;
  bool addFile(const std::string &FileName,
               std::unique_ptr<MemoryBuffer> Buffer,
               std::optional<fs::file_status> FileStatus = std::nullopt);

  bool updateFile(const std::string &FileName,
                  std::unique_ptr<MemoryBuffer> Buffer);

  std::unique_ptr<File> getFile(const std::string &FileName);
  Status status(const std::string &FileName) override;
  std::unique_ptr<MemoryBuffer> getBuffer(const std::string &FileName) override;

private:
  std::optional<detail::MemoryNode *> lookupNode(const std::string &Path);
};
} // namespace vfs
} // namespace vc