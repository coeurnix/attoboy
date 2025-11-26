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

void* operator new(decltype(sizeof(0)) size) {
  return HeapAlloc(GetProcessHeap(), 0, size);
}

void* operator new[](decltype(sizeof(0)) size) {
  return HeapAlloc(GetProcessHeap(), 0, size);
}

void operator delete(void* ptr) noexcept {
  if (ptr) HeapFree(GetProcessHeap(), 0, ptr);
}

void operator delete[](void* ptr) noexcept {
  if (ptr) HeapFree(GetProcessHeap(), 0, ptr);
}

void operator delete(void* ptr, decltype(sizeof(0)) size) noexcept {
  if (ptr) HeapFree(GetProcessHeap(), 0, ptr);
}

void operator delete[](void* ptr, decltype(sizeof(0)) size) noexcept {
  if (ptr) HeapFree(GetProcessHeap(), 0, ptr);
}

#ifdef _MSC_VER

#pragma function(memset)
#pragma function(memcpy)

extern "C" {

void* memset(void* dest, int ch, unsigned int count) {
  unsigned char* d = static_cast<unsigned char*>(dest);
  while (count--) {
    *d++ = static_cast<unsigned char>(ch);
  }
  return dest;
}

void* memcpy(void* dest, const void* src, unsigned int count) {
  unsigned char* d = static_cast<unsigned char*>(dest);
  const unsigned char* s = static_cast<const unsigned char*>(src);
  while (count--) {
    *d++ = *s++;
  }
  return dest;
}

}
#endif
