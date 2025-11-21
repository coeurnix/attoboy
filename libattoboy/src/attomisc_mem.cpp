#include "attoboy/attoboy.h"
#include <windows.h>

namespace attoboy {

void *Alloc(int size) {
  if (size <= 0) {
    return nullptr;
  }
  return HeapAlloc(GetProcessHeap(), 0, size);
}

void *Realloc(void *ptr, int size) {
  if (size <= 0) {
    if (ptr != nullptr) {
      HeapFree(GetProcessHeap(), 0, ptr);
    }
    return nullptr;
  }
  if (ptr == nullptr) {
    return HeapAlloc(GetProcessHeap(), 0, size);
  }
  return HeapReAlloc(GetProcessHeap(), 0, ptr, size);
}

void Free(void *ptr) {
  if (ptr != nullptr) {
    HeapFree(GetProcessHeap(), 0, ptr);
  }
}

} // namespace attoboy
