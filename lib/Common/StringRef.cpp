#include "Common/StringRef.h"

#include <cstring>

using namespace vc;

std::size_t StringRef::find(char C) {
    std::size_t i = 0;
    
    while (i < Length) {
        if (*(Data+i++)==C) return i;
    }

    return npos;
}