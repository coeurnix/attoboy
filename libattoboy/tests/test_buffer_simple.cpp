#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError(ATTO_TEXT("Test failed: "), message);                             \
    errorCount++;                                                              \
  }

void atto_main() {
  Log(ATTO_TEXT("Starting Buffer simple tests..."));

  // Test 1: Default constructor
  Log(ATTO_TEXT("Test 1: Default constructor"));
  Buffer b1;
  TEST(b1.length() == 0, ATTO_TEXT("Default buffer should be empty"));
  Log(ATTO_TEXT("Test 1 passed"));

  // Test 2: Constructor with size
  Log(ATTO_TEXT("Test 2: Constructor with size"));
  Buffer b2(1024);
  TEST(b2.length() == 0, ATTO_TEXT("Buffer(size) should start with length 0"));
  Log(ATTO_TEXT("Test 2 passed"));

  // Test 3: Constructor from String
  Log(ATTO_TEXT("Test 3: Constructor from String"));
  String s1(ATTO_TEXT("Hi"));
  Buffer b3(s1);
  Log(ATTO_TEXT("Buffer created from string"));
#ifdef UNICODE
  TEST(b3.length() == 4,
       ATTO_TEXT("Buffer from 'Hi' should be 4 bytes (2 wchars)"));
#else
  TEST(b3.length() == 2,
       ATTO_TEXT("Buffer from 'Hi' should be 2 bytes (2 chars)"));
#endif
  Log(ATTO_TEXT("Test 3 passed"));

  // Test 4: toString
  Log(ATTO_TEXT("Test 4: toString"));
  String s2 = b3.toString();
  Log(ATTO_TEXT("toString returned: "), s2);
  TEST(s2 == s1, ATTO_TEXT("toString should match original"));
  Log(ATTO_TEXT("Test 4 passed"));

  // Report results
  Log(ATTO_TEXT("All simple tests completed"));
  if (errorCount == 0) {
    Log(ATTO_TEXT("All tests passed!"));
    Exit(0);
  } else {
    LogError(errorCount, ATTO_TEXT(" test(s) failed"));
    Exit(errorCount);
  }
}
