#ifndef VC_COMMON_TEXTBUFFER_H
#define VC_COMMON_TEXTBUFFER_H
#include <string>

namespace vc {
class GapTextBuffer {
  char *Buffer;
  std::size_t Capacity;
  std::size_t Size;

  char *PreGap;
  char *PostGap;

public:
  GapTextBuffer(const char *Data, std::size_t Length);
  ~GapTextBuffer();

  void insertText(std::size_t Offset, const char *Data, std::size_t Length);
  void deleteText(std::size_t Offset, std::size_t Length);
  void replaceText(std::size_t Offset, std::size_t ReplaceLengtht,
                   const char *Data, std::size_t Length);

  std::size_t size() const { return Size; }
  std::size_t capacity() const { return Capacity; }
  std::string text() const;

private:
  void resizeBuffer();

  void moveGap(int Bytes);
  void shrinkGap(unsigned Pre, unsigned Post);
  void insertInGap(const char *Data, std::size_t Length);
};
} // namespace vc
#endif // !VC_COMMON_TEXTBUFFER_H