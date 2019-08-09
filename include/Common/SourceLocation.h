#ifndef VC_BASIC_SOURCE_LOCATION_H
#define VC_BASIC_SOURCE_LOCATION_H

#include <stdint.h>

namespace vc {

class SourceManager;

class SourceFile {
  uint32_t ID = 0;

public:
  static SourceFile fromRawEncoding(uint32_t ID) {
    SourceFile SF;
    SF.ID = ID;
    return SF;
  }

private:
  friend class SourceManager;
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

  uint32_t getRawEnconding() const { return ID; }

  static SourceLocation fromRawEncoding(uint32_t ID) {
    SourceLocation Loc;
    Loc.ID = ID;
    return Loc;
  }

  friend bool operator<(const SourceLocation &LHS, const SourceLocation &RHS) {
    return LHS.ID < RHS.ID;
  }
};

class SourceRange {
  SourceLocation Begin;
  SourceLocation End;

public:
  SourceRange() {}
  SourceRange(SourceLocation Begin) : Begin(Begin), End(Begin) {}
  SourceRange(SourceLocation Begin, SourceLocation End)
      : Begin(Begin), End(End) {}

  SourceLocation getBegin() const { return Begin; }
  SourceLocation getEnd() const { return End; }

  void setBegin(SourceLocation Loc) { Begin = Loc; }
  void setEnd(SourceLocation Loc) { End = Loc; }

  uint32_t length() const {
    return End.getRawEnconding() - Begin.getRawEnconding();
  }
};
} // namespace vc

#endif