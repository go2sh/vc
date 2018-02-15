#ifndef VC_DIAG_DIAGNOSTIC_H
#define VC_DIAG_DIAGNOSTIC_H

#include "Common/StringRef.h"
#include "Common/SourceLocation.h"

#include <vector>

namespace vc {
enum class DiagID : uint32_t;

class DiagnosticBuilder;

enum class DiagnosticArgumentKind {
  String
};

class DiagnosticArgument {
  DiagnosticArgumentKind Kind;
  union {
    StringRef S;
  };
public:
  DiagnosticArgument(StringRef S) : Kind(DiagnosticArgumentKind::String), S(S) {}
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
  const SourceLocation getLocation() const;
};
}; // namespace vc
#endif