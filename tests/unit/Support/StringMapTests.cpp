#include "catch.hpp"

#include "Support/StringMap.h"

TEST_CASE("Find key") {
    vc::StringMap<unsigned> M;
    auto it = M.find(vc::StringRef("Test"));
    REQUIRE(it == M.end());
}