#include "attolist_internal.h"
#include "attomap_internal.h"
#include "attoset_internal.h"

namespace attoboy {

Map *AllocMap() {
  void *mem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Map));
  if (!mem)
    return nullptr;
  return new (mem) Map();
}

Map *AllocMap(const Map &other) {
  void *mem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Map));
  if (!mem)
    return nullptr;
  return new (mem) Map(other);
}

void FreeMap(Map *map) {
  if (!map)
    return;
  map->~Map();
  HeapFree(GetProcessHeap(), 0, map);
}

Set *AllocSet() {
  void *mem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Set));
  if (!mem)
    return nullptr;
  return new (mem) Set();
}

Set *AllocSet(const Set &other) {
  void *mem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Set));
  if (!mem)
    return nullptr;
  return new (mem) Set(other);
}

void FreeSet(Set *set) {
  if (!set)
    return;
  set->~Set();
  HeapFree(GetProcessHeap(), 0, set);
}

} // namespace attoboy
