add_library(vcFormat
    Format.cpp
    FormatToken.cpp
    FormatTokenLexer.cpp
    FormatLineParser.cpp
    FormatLineFormater.cpp
    FormatTokenIndenter.cpp
    FormatCalculator.cpp
    EditManager.cpp
)

target_link_libraries(vcFormat vcSupport)