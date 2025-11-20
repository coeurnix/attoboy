#include "attoboy/attoboy.h"
#include <windows.h>

void operator delete(void *ptr, size_t) noexcept {
  if (ptr)
    HeapFree(GetProcessHeap(), 0, ptr);
}

using namespace attoboy;

void atto_main() {
  Log("Test 1: Empty constructor");
  Map m1;
  Log("Test 1 passed");

  Log("Test 2: Capacity constructor");
  Map m2(16);
  Log("Test 2 passed");

  Log("Test 3: put String value");
  Map m3;
  m3.put(1, String("one"));
  Log("Test 3 passed");

  Log("Test 4: length check");
  int len = m3.length();
  Log("Length: ", len);
  Log("Test 4 passed");

  Log("Test 5: hasKey");
  bool hasIt = m3.hasKey(1);
  Log("hasKey: ", hasIt ? "true" : "false");
  Log("Test 5 passed");

  Log("Test 6: get String value");
  String result = m3.get<int, String>(1);
  Log("Test 6 passed");

  Log("Test 7: String equals check");
  bool eq = result.equals(String("one"));
  Log("Equals: ", eq ? "true" : "false");
  Log("Test 7 passed");

  Exit(0);
}
