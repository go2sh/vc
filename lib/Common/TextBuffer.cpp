#include <Common/TextBuffer.h>

#include <cassert>
#include <cstdlib>
#include <cstring>

using namespace vc;

GapTextBuffer::GapTextBuffer(const char *Data, std::size_t Length) {
  initBuffer(Data, Length);
}

GapTextBuffer::~GapTextBuffer() {
  if (Buffer != nullptr) {
    std::free(Buffer);
  }
}

void GapTextBuffer::initBuffer(const char *Data, std::size_t Length) {
  Capacity = 2 * Length;
  Buffer = static_cast<char *>(std::malloc(Capacity));
  Size = Length;
  PreGap = Buffer;
  PostGap = Buffer + Length;
  std::memcpy(Buffer + Length, Data, Length);
}

void GapTextBuffer::resizeBuffer() {
  std::size_t PostLength = Capacity - (uintptr_t)(PostGap - Buffer);
  std::size_t NewCapacity = Capacity * 2;
  char *NewBuffer = static_cast<char *>(std::malloc(NewCapacity));

  std::memcpy(NewBuffer, Buffer, (uintptr_t)(PreGap - Buffer));
  std::memcpy(NewBuffer + (NewCapacity - PostLength),
              Buffer + (Capacity - PostLength), PostLength);
  Buffer = NewBuffer;
  Capacity = NewCapacity;
  PostGap = Buffer + (Capacity - PostLength);
}

std::string GapTextBuffer::text() const {
  std::string Text;

  Text.resize(Size);
  std::copy(Buffer, PreGap, Text.begin());
  std::copy(PostGap, Buffer + Capacity, Text.begin() + (PreGap - Buffer));

  return std::move(Text);
}

void GapTextBuffer::insertText(std::size_t Offset, const char *Data,
                               std::size_t Length) {
  assert(Offset < Size && "Offset out of range");

  if (Length == 0)
    return;

  if (Offset + Length > Capacity) {
    resizeBuffer();
  }

  moveGap((std::ptrdiff_t)(Buffer + Offset - PreGap));
  insertInGap(Data, Length);
}

void GapTextBuffer::deleteText(std::size_t Offset, std::size_t Length) {
  assert(Offset < Size && "Offset out of range");
  assert(Offset + Length <= Size && "Length out of range");

  if (Length == 0)
    return;

  if (Buffer + Offset + Length < PreGap) {
    moveGap((std::ptrdiff_t)(Buffer + Offset + Length - PreGap));
  } else if (Buffer + Offset >= PreGap) {
    moveGap((std::ptrdiff_t)(Buffer + Offset - PreGap));
  }
  unsigned PreShrink = (uintptr_t)(PreGap - Buffer) - Offset;
  shrinkGap(PreShrink, Length - PreShrink);
}

void GapTextBuffer::replaceText(std::size_t Offset, std::size_t ReplaceLengtht,
                                const char *Data, std::size_t Length) {
  assert(Offset < Size && "Offset out of range");
  assert(Offset + ReplaceLengtht <= Size && "ReplacementLength out of range");

  if (Size + Length - ReplaceLengtht > Capacity) {
    resizeBuffer();
  }

  deleteText(Offset, ReplaceLengtht);
  insertInGap(Data, Length);
}

void GapTextBuffer::moveGap(int Bytes) {
  if (Bytes < 0) {
    std::copy_backward(PreGap + Bytes, PreGap, PostGap);
  } else {
    std::copy(PostGap, PostGap + Bytes, PreGap);
  }

  PreGap += Bytes;
  PostGap += Bytes;
}

void GapTextBuffer::shrinkGap(unsigned Pre, unsigned Post) {
  PreGap -= Pre;
  PostGap += Post;
  Size = Size - Pre - Post;
}

void GapTextBuffer::insertInGap(const char *Data, std::size_t Length) {
  std::copy(Data, Data + Length, PreGap);
  PreGap += Length;
  Size += Length;
}