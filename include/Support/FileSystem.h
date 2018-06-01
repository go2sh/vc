#include <boost/filesystem.hpp>

namespace vc {
namespace vfs {
class Status {
  std::string FileName;
  boost::filesystem::file_status FileStatus;
  boost::uintmax_t FileSize;

public:
  Status() {}
  Status(const boost::filesystem::path &Path) {
    if (Path.filename_is_dot()) {
      auto It = Path.rbegin();
      It++;
      assert(It != Path.rend() && "Path must be absolute to construct directory name");
      FileName = It->filename.string();
    } else {
      FileName = Path.filename().string();
    }
    FileSize = boost::filesystem::file_size(Path);
    FileStatus = boost::filesystem::status(Path);
  }

  Status(const std::string &FileName,
         const boost::filesystem::file_status &FileStatus,
         const boost::uintmax_t FileSize)
      : FileName(FileName), FileStatus(FileStatus), FileSize(FileSize) {}

  boost::filesystem::file_type getType() { return FileStatus.type; }
  boost::filesystem::perms getPermissions() { return FileStatus.permissions; }
  boost::uintmax_t getSize() { return FileSize; }

  bool isValid() {
    return FileStatus.type = boost::filesystem::file_type::status_error;
  }
};
} // namespace vfs
} // namespace vc