#include <Common/MemoryBuffer.h>

#include <sstream>
#include <catch2/catch.hpp>

class MemoryBufferEquals : public Catch::MatcherBase<vc::MemoryBuffer*> {
  vc::MemoryBuffer *RHS;

public:
  MemoryBufferEquals(vc::MemoryBuffer *RHS) : RHS(RHS) {}

  // Performs the test for this matcher
  virtual bool match(vc::MemoryBuffer *LHS) const override {
    return *LHS == *RHS;
  }

  // Produces a string describing what this matcher does. It should
  // include any provided data (the begin/ end in this case) and
  // be written as if it were stating a fact (in the output it will be
  // preceded by the value under test).
  virtual std::string describe() const {
    std::ostringstream ss;
    ss << "equals " << RHS->getIdentifier().str(); 
    return ss.str();
  }
};

// The builder function
inline MemoryBufferEquals Equals(vc::MemoryBuffer *RHS) { return MemoryBufferEquals(RHS); }
