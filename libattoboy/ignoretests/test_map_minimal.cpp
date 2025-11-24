#include "attoboy/attoboy.h"
#include <windows.h>

void operator delete(void *ptr, size_t) noexcept {
  if (ptr)
    HeapFree(GetProcessHeap(), 0, ptr);
}

using namespace attoboy;

void atto_main() {
  Log(ATTO_TEXT("Test 1: Empty constructor"));
  Map m1;
  Log(ATTO_TEXT("Test 1 passed"));

  Log(ATTO_TEXT("Test 2: Capacity constructor"));
  Map m2(16);
  Log(ATTO_TEXT("Test 2 passed"));

  Log(ATTO_TEXT("Test 3: put String value"));
  Map m3;
  m3.put(1, String(ATTO_TEXT("one")));
  Log(ATTO_TEXT("Test 3 passed"));

  Log(ATTO_TEXT("Test 4: length check"));
  int len = m3.length();
  Log(ATTO_TEXT("Length: "), len);
  Log(ATTO_TEXT("Test 4 passed"));

  Log(ATTO_TEXT("Test 5: hasKey"));
  bool hasIt = m3.hasKey(1);
  Log(ATTO_TEXT("hasKey: "), hasIt ? ATTO_TEXT("true") : ATTO_TEXT("false"));
  Log(ATTO_TEXT("Test 5 passed"));

  Log(ATTO_TEXT("Test 6: get String value"));
  String result = m3.get<int, String>(1);
  Log(ATTO_TEXT("Test 6 passed"));

  Log(ATTO_TEXT("Test 7: String equals check"));
  bool eq = result.equals(String(ATTO_TEXT("one")));
  Log(ATTO_TEXT("Equals: "), eq ? ATTO_TEXT("true") : ATTO_TEXT("false"));
  Log(ATTO_TEXT("Test 7 passed"));

  Exit(0);
}
