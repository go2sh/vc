#ifndef VC_FORMAT_FORMATTYPES_H
#define VC_FORMAT_FORMATTYPES_H
namespace vc {
namespace format {
struct Penalty {
  explicit Penalty(unsigned Value) : Value(Value) {}

  Penalty operator+(const Penalty &RHS) { return Penalty(Value + RHS.Value); }
  void operator+=(const Penalty &RHS) { Value += RHS.Value; }
  bool operator==(const Penalty &RHS) { return Value == RHS.Value; }

private:
  unsigned Value = 0;
};

} // namespace format
} // namespace vc
#endif