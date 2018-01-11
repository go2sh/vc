#include <fstream>
#include <iostream>
#include <string>

#include "Diag/DiagnosticConsumer.h"
#include "Diag/DiagnosticEngine.h"
#include "Common/TokenKinds.h"
#include "Parse/Lexer.h"
#include "Parse/Parser.h"

using namespace vc;

class StdConsumer : public DiagnosticConsumer {
    virtual void handleDiagnostic(const Diagnostic & Diag) {
        std::cout << Diag.getString();
    }
};

int main(int argc, char ** argv) {
    unique_ptr<MemoryBuffer> Buf = MemoryBuffer::getFile("tests/test.vhd");
    SourceLocation Loc = SourceLocation::fromRawEncoding(1);
    DiagnosticEngine Engine;
    StdConsumer Consumer;
    Engine.addConsumer(&Consumer);
    Lexer lexer(Engine, Loc, Buf.get());
    Parser P(Engine, &lexer);
    P.parseDesignFile();
    
    return 0;
}