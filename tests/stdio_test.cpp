#include <fstream>
#include <iostream>
#include <string>

#include "Common/MemoryBuffer.h"
#include "Common/SourceManager.h"
#include "Common/StringRef.h"
#include "Common/TokenKinds.h"
#include "Diag/DiagnosticConsumer.h"
#include "Diag/DiagnosticEngine.h"
#include "Parse/Lexer.h"
#include "Parse/Parser.h"
#include "Parse/Token.h"

using namespace vc;

class StdConsumer : public DiagnosticConsumer {
  SourceManager Mgr;

public:
  StdConsumer(const SourceManager &Mgr) : Mgr(Mgr) {}
  virtual void handleDiagnostic(const Diagnostic &Diag) {
    auto data = Mgr.getDecomposedLocation(Diag.getLocation());
    std::cout << Mgr.getPath(data.first) << ":";
    std::cout << Mgr.getLineNumber(data.first, data.second) << ":"
              << Mgr.getColumnNumber(data.first, data.second) << ":";
    DiagnosticEngine::formatDiagnosticText(std::cout, Diag.getString(),
                                           Diag.getArguments());
    std::cout << std::endl;
  }
};

int main(int argc, char **argv) {
  auto MemBuf = MemoryBuffer::getSTDIN();

  SourceManager SrcMgr;
  SourceFile File = SrcMgr.createSourceFile(std::move(MemBuf));
  SourceLocation Loc = SourceLocation::fromRawEncoding(1);
  DiagnosticEngine Engine;
  StdConsumer Consumer(SrcMgr);
  Engine.addConsumer(&Consumer);
  Lexer lexer(Engine, Loc, SrcMgr.getBuffer(File));

  Token Result;
  do {
    lexer.lex(Result);
  } while (Result.getKind() != tok::TokenKind::eof);

  return 0;
}