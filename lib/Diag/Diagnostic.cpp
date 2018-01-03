#include <Diag/Diagnostic.h>

using namespace vc;

static const char *DiagnosticStrings[] = {
  "Invalid diagnostic",
#define DIAG(Type, ID, Text) Text
#include "Diag/DiagnosticsAll.def"
};

const char *Diagnostic::getString() const {
  return DiagnosticStrings[(uint32_t)ID];
};