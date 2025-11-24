#include "attoboy/attoboy.h"

using namespace attoboy;

void atto_main() {
  // Test 1: Create empty buffer
  Buffer b1;
  if (b1.length() != 0) {
    Exit(1);
  }

  // Test 2: Create buffer with size
  Buffer b2(100);
  if (b2.length() != 0) {
    Exit(2);
  }

  // Test 3: Create buffer from string
  String s(ATTO_TEXT("Test"));
  Buffer b3(s);
#ifdef UNICODE
  if (b3.length() != 8) { // 4 chars * 2 bytes per wchar
    Exit(3);
  }
#else
  if (b3.length() != 4) { // 4 chars * 1 byte per char
    Exit(3);
  }
#endif

  // Test 4: toString
  String result = b3.toString();
  if (!(result == s)) {
    Exit(4);
  }

  // Test 5: Copy constructor
  Buffer b4(b3);
  if (b4.length() != b3.length()) {
    Exit(5);
  }

  // Test 6: Assignment
  Buffer b5;
  b5 = b3;
  if (b5.length() != b3.length()) {
    Exit(6);
  }

  // Test 7: Clear
  Buffer b6(s);
  b6.clear();
  if (b6.length() != 0) {
    Exit(7);
  }

  // Test 8: Append string
  Buffer b7;
  b7.append(String(ATTO_TEXT("A")));
#ifdef UNICODE
  if (b7.length() != 2) {
    Exit(8);
  }
#else
  if (b7.length() != 1) {
    Exit(8);
  }
#endif

  // Test 9: Compare
  Buffer b8(s);
  Buffer b9(s);
  if (!b8.compare(b9)) {
    Exit(9);
  }

  // Test 10: Operators
  if (!(b8 == b9)) {
    Exit(10);
  }

  // All tests passed
  Exit(0);
}
