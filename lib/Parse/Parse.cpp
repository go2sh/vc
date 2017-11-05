#include "Parser/Parser.h"
#include "AST/Context.h"

using namespace vc;
bool Parser::ParseDesignFile() {
    bool hasDesignUnit = false;
    do {
      Context *Ctx = new Context();
      SetContext(Ctx);
    } while (hasDesignUnit);
    return hasDesign
}