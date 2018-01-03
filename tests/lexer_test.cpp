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
    std::ifstream in("tests/test.vhd", std::ios::in | std::ios::binary);
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(((int)in.tellg()) + 1);
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    contents[contents.size()-1] = 0;
    in.close();
    DiagnosticEngine Engine;
    StdConsumer Consumer;
    Engine.addConsumer(&Consumer);
    Lexer lexer(Engine, contents.data(), contents.size()-1);
    Parser P(Engine, &lexer);
    P.parseDesignFile();
    
    return 0;
}