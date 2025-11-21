#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError("Test failed: ", message);                                        \
    errorCount++;                                                              \
  }

void atto_main() {
  Log("Starting Buffer simple tests...");

  // Test 1: Default constructor
  Log("Test 1: Default constructor");
  Buffer b1;
  TEST(b1.length() == 0, "Default buffer should be empty");
  Log("Test 1 passed");

  // Test 2: Constructor with size
  Log("Test 2: Constructor with size");
  Buffer b2(1024);
  TEST(b2.length() == 0, "Buffer(size) should start with length 0");
  Log("Test 2 passed");

  // Test 3: Constructor from String
  Log("Test 3: Constructor from String");
  String s1("Hi");
  Buffer b3(s1);
  Log("Buffer created from string");
  TEST(b3.length() == 4, "Buffer from 'Hi' should be 4 bytes");
  Log("Test 3 passed");

  // Test 4: toString
  Log("Test 4: toString");
  String s2 = b3.toString();
  Log("toString returned: ", s2);
  TEST(s2 == s1, "toString should match original");
  Log("Test 4 passed");

  // Report results
  Log("All simple tests completed");
  if (errorCount == 0) {
    Log("All tests passed!");
    Exit(0);
  } else {
    LogError(errorCount, " test(s) failed");
    Exit(errorCount);
  }
}
