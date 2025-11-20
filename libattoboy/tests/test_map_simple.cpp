#include "attoboy/attoboy.h"
#include <windows.h>

void *operator new(size_t size) { return HeapAlloc(GetProcessHeap(), 0, size); }

void operator delete(void *ptr) noexcept {
  if (ptr)
    HeapFree(GetProcessHeap(), 0, ptr);
}

void operator delete(void *ptr, size_t) noexcept {
  if (ptr)
    HeapFree(GetProcessHeap(), 0, ptr);
}

using namespace attoboy;

void atto_main() {
  Log("Testing Map constructor...");

  Map m1;
  Log("Map created successfully");

  Log("Testing put...");
  m1.put(1, 10);
  Log("Put successful");

  Log("Testing length...");
  int len = m1.length();
  Log("Length: ", len);

  Exit(0);
}
