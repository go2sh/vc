#ifndef VC_SUPPORT_STRINGMAP_H
#define VC_SUPPORT_STRINGMAP_H
#include "Common/StringRef.h"

#include <utility>

namespace vc {

std::size_t hash(const char *Data, std::size_t Length) {
  std::size_t hash = 5381;

  while (Length--)
    hash = ((hash << 5) + hash) + *(Data++); /* hash * 33 + c */

  return hash;
}

template <typename T> class StringMapEntry {
public:
  std::pair<StringRef, T> Value;
  unsigned Hash;
};

template <typename T>
class StringMapIterator
    : public std::iterator<std::forward_iterator_tag, std::pair<StringRef, T>> {
  StringMapEntry<T> **EntryPtr;

public:
  StringMapIterator(StringMapEntry<T> **EntryPtr) : EntryPtr(EntryPtr) {}
  bool operator==(const StringMapIterator<T> &RHS) const {
    return EntryPtr == RHS.EntryPtr;
  }
  bool operator!=(const StringMapIterator<T> &RHS) const {
    return EntryPtr != RHS.EntryPtr;
  }
};

template <typename T, class EntryAlloc = std::allocator<StringMapEntry<T>>,
          class CharAlloc = std::allocator<char>>
class StringMap {
  /* Type definitions */
  using hash_type = unsigned;
  using size_type = std::size_t;
  using value_type = std::pair<StringRef, T>;
  typedef StringMapIterator<T> iterator;

  /* Map size */
  size_type Buckets;
  size_type Tombstones;
  size_type Entries;

  /** Entry table
   *
   * A pointer to each entry in the hash map is stored in this entry table.
   * This allows a easy lookup, resize functions as well as easy detection of
   * empty or tombstone buckets.
   */
  StringMapEntry<T> **EntryTable;

  /* Allocators */
  EntryAlloc EntryAllocator;
  CharAlloc CharAllocator;

public:
  StringMap() { Init(); }

  bool empty() { return Entries == 0; }

  std::pair<iterator,bool> insert(const value_type &Value) {
    if ()
    std::size_t Offset = LookupKey(Key);
  }

  iterator find(StringRef Key) {
    std::size_t Offset = LookupOffset(Key);
    if (EntryTable[Offset] && EntryTable[Offset] != getTombstone()) {
      return iterator(&EntryTable[Offset]);
    }
    return iterator(&EntryTable[Buckets]);
  }

  iterator begin() { return iterator(&EntryTable[0]); }
  iterator end() { return iterator(&EntryTable[Buckets]); }

private:
  void Init(size_type InitBuckets = 0) {
    Buckets = InitBuckets ? InitBuckets : 16;
    Entries = Tombstones = 0;

    EntryTable = static_cast<StringMapEntry<T> **>(
        std::calloc(Buckets, sizeof(StringMapEntry<T> *)));
  }

  /** Lookup the offset for a key
   *
   * The table must either contain the key or have enought space
   * to find a free offset.
   */
  std::size_t LookupOffset(StringRef Key) {
    hash_type Hash = hash(Key.data(), Key.length());
    std::size_t Offset, i = 0;
    std::size_t LastTombstone = -1;

    while (true) {
      // Calculate offset with quadratic probing
      Offset = (Hash + i * i) & (Buckets - 1);

      if (EntryTable[Offset] == getEmpty()) {
        // Return the tombstone if there was one
        if (LastTombstone != -1) {
          Offset = LastTombstone;
        }
        break;
      }

      if (EntryTable[Offset]->Value.first == Key) {
        break;
      } else {
        // Check for a tombstone and store it
        if (EntryTable[Offset] == getTombstone()) {
          LastTombstone = Offset;
        }
        i++;
      }
    }

    return Offset;
  }

  StringMapEntry<T> *getTombstone() {
    uintptr_t Temp = static_cast<uintptr_t>(-1);
    return reinterpret_cast<StringMapEntry<T> *>(Temp);
  }

  StringMapEntry<T> *getEmpty() { return nullptr; }
};
} // namespace vc

#endif