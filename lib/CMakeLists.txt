add_library(vcSupport STATIC
    Support/FileSystem.cpp
    Support/SourceEdit.cpp
)
target_include_directories(vcSupport PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(vcSupport ${Boost_LIBRARIES})

add_library(vc
#    Support/StringMap.cpp
    Format/Format.cpp
    Format/FormatToken.cpp
    Format/FormatTokenLexer.cpp
    Format/FormatLineParser.cpp
    Common/TextBuffer.cpp
    Common/MemoryBuffer.cpp
    Common/SourceManager.cpp
    Common/CharInfo.cpp
    Common/TokenKinds.cpp
    Common/StringRef.cpp
    Diag/Diagnostic.cpp
    Diag/DiagnosticEngine.cpp
    Parse/Token.cpp
    Parse/Lexer.cpp
    Parse/Parser.cpp
    Parse/ParseName.cpp
    Parse/ParseExpr.cpp
    Parse/ParseType.cpp
    Parse/ParseStmt.cpp
    Parse/ParseDecl.cpp
    Parse/ParseUnit.cpp
)

add_subdirectory(Format)