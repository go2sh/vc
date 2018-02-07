#include <Diag/Diagnostic.h>

using namespace vc;

enum class vc::DiagID : uint32_t {
  invalid_diagnostic,
  #define DIAG(Type, ID, Text) ID,
  #include "Diag/DiagnosticsAll.def"
  NUM_DIAGS
};

namespace vc {
  namespace diag {
    #define DIAG(Type, ID, Text) \
    vc::DiagID ID = vc::DiagID::ID;
    #include "Diag/DiagnosticsAll.def"
  }
}

static const char *DiagnosticStrings[] = {
  "Invalid diagnostic",
#define DIAG(Type, ID, Text) Text,
#include "Diag/DiagnosticsAll.def"
  ""
};

const char *Diagnostic::getString() const {
  return DiagnosticStrings[(uint32_t)ID];
};

const SourceLocation Diagnostic::getLocation() const {
  return Location;
}