#ifndef VC_DIAG_DIAGNOSTIC_H
#define VC_DIAG_DIAGNOSTIC_H

#include <cassert>
#include <string_view>
#include <vector>

#include "Common/SourceLocation.h"


namespace vc {
enum class DiagID : uint32_t;

class DiagnosticBuilder;

enum class DiagnosticArgumentKind { String };

class DiagnosticArgument {
  DiagnosticArgumentKind Kind;
  union {
    std::string_view S;
  };

public:
  DiagnosticArgument(std::string_view S)
      : Kind(DiagnosticArgumentKind::String), S(S) {}

  DiagnosticArgumentKind getKind() const { return Kind; }
  std::string_view getAsString() const {
    assert(Kind == DiagnosticArgumentKind::String);
    return S;
  }
};

class Diagnostic {
  friend class DiagnosticBuilder;
  DiagID ID;
  SourceLocation Location;
  std::vector<SourceRange> Ranges;
  std::vector<DiagnosticArgument> Arguments;

public:
  Diagnostic(DiagID ID) : ID(ID) {}

  const char *getString() const;
  SourceLocation getLocation() const { return Location; }
  const std::vector<DiagnosticArgument> &getArguments() const {
    return Arguments;
  }
};
}; // namespace vc
#endif