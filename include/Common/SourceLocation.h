#ifndef VC_BASIC_SOURCE_LOCATION_H
#define VC_BASIC_SOURCE_LOCATION_H

#include <stdint.h>

namespace vc {

class SourceFile {
  uint32_t ID = 0;

public:
  static SourceFile fromRawEncoding(uint32_t ID) {
    SourceFile SF;
    SF.ID = ID;
    return SF;
  }
};

class SourceLocation {
  uint32_t ID = 0;

public:
  bool isValid() { return ID != 0; }
  bool isInvalid() { return ID == 0; }

  SourceLocation getLocWithOffset(int32_t Offset) {
    SourceLocation Loc;
    Loc.ID = ID + Offset;
    return Loc;
  }

  uint32_t getRawEnconding() { return ID; }

  static SourceLocation fromRawEncoding(uint32_t ID) {
    SourceLocation Loc;
    Loc.ID = ID;
    return Loc;
  }
};

class SourceRange {
  SourceLocation Begin;
  SourceLocation End;

public:
  SourceRange(SourceLocation Begin) : Begin(Begin), End(Begin) {}
  SourceRange(SourceLocation Begin, SourceLocation End)
      : Begin(Begin), End(End) {}

  SourceLocation getBegin() { return Begin; }
  SourceLocation getEnd() { return End; }

  void setBegin(SourceLocation Loc) { Begin = Loc; }
  void setEnd(SourceLocation Loc) { End = Loc; }
};
} // namespace vc

#endif