add_library(vcSupport STATIC
    Support/FileSystem.cpp
    Support/SourceEdit.cpp
    $<$<PLATFORM_ID:Windows>:Support/ConfigFolders/ConfigFoldersWin32.cpp>
    $<$<PLATFORM_ID:Linux>:Support/ConfigFolders/ConfigFoldersPosix.cpp>
)

add_library(vcParse
    Parse/TokenKinds.cpp
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

add_library(vc
    Common/TextBuffer.cpp
    Common/MemoryBuffer.cpp
    Common/SourceManager.cpp
    Common/CharInfo.cpp
    Common/OperatorPrecedence.cpp
    Diag/Diagnostic.cpp
    Diag/DiagnosticEngine.cpp

)

add_subdirectory(Format)