#include <Common/MemoryBuffer.h>

#include <boost/filesystem.hpp>
#include <memory>
#include <optional>

namespace vc {
namespace vfs {
class Status {
  std::string FileName;
  boost::filesystem::file_status FileStatus;
  boost::uintmax_t FileSize;

public:
  Status() {}
  Status(const std::string &FileName,
         const boost::filesystem::file_status &FileStatus,
         const boost::uintmax_t FileSize = -1)
      : FileName(FileName), FileStatus(FileStatus), FileSize(FileSize) {}

  std::string getName() const { return FileName; }
  boost::filesystem::file_type getType() const { return FileStatus.type(); }
  boost::filesystem::perms getPermissions() const {
    return FileStatus.permissions();
  }
  boost::uintmax_t getSize() const { return FileSize; }

  bool isValid() {
    return FileStatus.type() != boost::filesystem::file_type::status_error;
  }
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
  void addFile(
      const std::string &FileName,
      const boost::filesystem::file_status &FileStatus,
      const boost::uintmax_t FileSize = -1,
      std::unique_ptr<MemoryBuffer> Buffer = std::make_unique<MemoryBuffer>());

  virtual std::unique_ptr<File> getFile(const std::string &FileName);
  virtual Status status(const std::string &FileName);
  virtual std::unique_ptr<MemoryBuffer> getBuffer(const std::string &FileName);

private:
  std::optional<detail::MemoryNode *> lookupNode(const std::string &Path);
};
} // namespace vfs
} // namespace vc