#include <fstream>
#include <iostream>
#include <string>


#include "Common/TokenKinds.h"
#include "Lexer/Lexer.h"

using namespace vc;

int main(int argc, char ** argv) {
    std::ifstream in("tests/test.vhd", std::ios::in | std::ios::binary);
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(((int)in.tellg()) + 1);
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    contents[contents.size()-1] = 0;
    in.close();
    Lexer lexer(contents.data(), contents.size()-1);
    Token t;
    while (t.getTokenKind() != tok::eof) {
        lexer.Lex(t);
        std::cout << "Token: " << tok::getTokenName(t.getTokenKind()) << " Value: " << t.getValue() << std::endl;
    }
    return 0;
}