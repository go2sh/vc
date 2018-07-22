namespace vc {
namespace vfs {
class File {
  std::string Name;

  public:
    std::string getName() const {return Name;}
    virtual std::unique_ptr<vc::MemoryBuffer> getBuffer();

  
};

class FileSystem {};
} // namespace vfs

} // namespace vc