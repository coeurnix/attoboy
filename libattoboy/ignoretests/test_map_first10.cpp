#include "attoboy/attoboy.h"
#include <windows.h>

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError(ATTO_TEXT("Test failed: "), message);                                        \
    errorCount++;                                                              \
  }

void atto_main() {
  Log(ATTO_TEXT("Running Map tests..."));

  // Test constructors - lines 38-46
  Log(ATTO_TEXT("Test 1"));
  Map m1;
  TEST(m1.length() == 0, ATTO_TEXT("Empty constructor should create map with length 0"));
  TEST(m1.isEmpty(), ATTO_TEXT("Empty map should return true for isEmpty()"));

  Log(ATTO_TEXT("Test 2"));
  Map m2(16);
  TEST(m2.length() == 0,
       ATTO_TEXT("Capacity constructor should create map with length 0"));
  TEST(m2.isEmpty(), ATTO_TEXT("Capacity map should return true for isEmpty()"));

  // Test put and get with different types - lines 48-66
  Log(ATTO_TEXT("Test 3"));
  Map m3;
  m3.put(1, String(ATTO_TEXT("one")));
  TEST(m3.length() == 1, ATTO_TEXT("put() should increase length to 1"));
  TEST(m3.hasKey(1), ATTO_TEXT("hasKey() should find the key"));
  TEST((m3.get<int, String>(1).equals(String(ATTO_TEXT("one")))),
       ATTO_TEXT("get() should retrieve value"));

  Log(ATTO_TEXT("Test 4"));
  m3.put(2, String(ATTO_TEXT("two")));
  TEST(m3.length() == 2, ATTO_TEXT("put() should increase length to 2"));
  TEST(m3.hasKey(2), ATTO_TEXT("hasKey() should find second key"));
  TEST((m3.get<int, String>(2).equals(String(ATTO_TEXT("two")))),
       ATTO_TEXT("get() should retrieve second value"));

  // Test update existing key
  Log(ATTO_TEXT("Test 5"));
  m3.put(1, String(ATTO_TEXT("ONE")));
  TEST(m3.length() == 2, ATTO_TEXT("put() with existing key should not increase length"));
  TEST((m3.get<int, String>(1).equals(String(ATTO_TEXT("ONE")))),
       ATTO_TEXT("put() should update value"));

  // Report results
  if (errorCount == 0) {
    Log(ATTO_TEXT("All Map tests passed!"));
    Exit(0);
  } else {
    LogError(errorCount, ATTO_TEXT(" test(s) failed"));
    Exit(errorCount);
  }
}
