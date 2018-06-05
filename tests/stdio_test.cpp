#include <fstream>
#include <iostream>
#include <string>

#include "Diag/DiagnosticConsumer.h"
#include "Diag/DiagnosticEngine.h"
#include "Common/SourceManager.h"
#include "Common/TokenKinds.h"
#include "Parse/Lexer.h"
#include "Parse/Parser.h"

using namespace vc;

class StdConsumer : public DiagnosticConsumer {
    SourceManager Mgr;
public:
    StdConsumer(const SourceManager &Mgr) : Mgr(Mgr) {}
    virtual void handleDiagnostic(const Diagnostic & Diag) {
        auto data = Mgr.getDecomposedLocation(Diag.getLocation());
        std::cout << Mgr.getLineNumber(data.first, data.second) << ":" << Mgr.getColumnNumber(data.first, data.second) << ":";
        DiagnosticEngine::formatDiagnosticText(std::cout, Diag.getString(), Diag.getArguments());
        std::cout << std::endl;
    }
};

int main(int argc, char ** argv) {
    SourceManager SrcMgr;
    SourceFile File = SrcMgr.createSourceFile("tests/test.vhd");
    SourceLocation Loc = SourceLocation::fromRawEncoding(1);
    DiagnosticEngine Engine;
    StdConsumer Consumer(SrcMgr);
    Engine.addConsumer(&Consumer);
    Lexer lexer(Engine, Loc, SrcMgr.getBuffer(File));
    Parser P(Engine, &lexer);
    P.parseDesignFile();
    
    return 0;
}