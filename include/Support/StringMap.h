#ifndef VC_SUPPORT_STRINGMAP_H
#define VC_SUPPORT_STRINGMAP_H

#include <iterator>
namespace vc {
using hash_type = unsigned;
template <typename T>
class StringMapIterator
    : public std::iterator<std::forward_iterator_tag, std::pair<StringRef, T>> {

};

template <typename T> class StringMapEntry {
  enum class EntryType { Empty, Tombstone, Entry };
  std::pair<StringRef, T> Value;
  hash_type Hash;
};

template <typename T, class Allocator = std::allocator<StringMapEntry<T>>> class StringMap {
  using iterator = StringMapIterator<T>;
  using size_type = std::size_t;

  size_type Buckets;
  size_type Elements;
  size_type Tombstones;

  StringMapEntry *Table;

  Allocator EntryAllocator;
public:
  StringMap() {
    NumBuckets = 64;
    NumElements = 0;
    NumTombstones = 0;

    Table = EntryAllocator.allocate();
  }
  std::pair<iterator, bool> insert(std::pair<StringRef, T> &Value);

private:
  void Init(size_type Buckets = 0);
};
} // namespace vc
#endif // !VC_SUPPORT_STRINGMAP_H