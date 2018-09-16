#include <Common/TextBuffer.h>

#include <string>

#define CATCH_CONFIG_MAIN
#include <catch/catch.hpp>
using namespace Catch::Matchers;

TEST_CASE("GapTextBuffer simple operations", "[GapTextBuffer]") {

  std::string TestText = "Test 12345";
  std::string InsertText = "asd";
  vc::GapTextBuffer Buffer(TestText.data(), TestText.size());

  SECTION("init text buffer") {
    REQUIRE(Buffer.size() == 10);
    REQUIRE(Buffer.capacity() == 20);
    REQUIRE_THAT(Buffer.text(), Equals("Test 12345"));
  }

  SECTION("insert text") {
    Buffer.insertText(4, InsertText.data(), InsertText.length());
    REQUIRE(Buffer.size() == 13);
    REQUIRE(Buffer.capacity() == 20);
    REQUIRE_THAT(Buffer.text(), Equals("Testasd 12345"));
  }

  SECTION("delete text") {
    Buffer.deleteText(1, 2);
    REQUIRE(Buffer.size() == 8);
    REQUIRE(Buffer.capacity() == 20);
    REQUIRE_THAT(Buffer.text(), Equals("Tt 12345"));
  }

  SECTION("replace text") {
    std::string ReplaceText = "UnitTest";
    Buffer.replaceText(0, 4, ReplaceText.data(), ReplaceText.length());
    REQUIRE(Buffer.size() == 14);
    REQUIRE(Buffer.capacity() == 20);
    REQUIRE_THAT(Buffer.text(), Equals("UnitTest 12345"));
  }
}