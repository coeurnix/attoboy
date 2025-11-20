#include "attoboy/attoboy.h"
#include <windows.h>

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError("Test failed: ", message);                                        \
    errorCount++;                                                              \
  }

void atto_main() {
  Log("Running Map tests...");

  // Test constructors - lines 38-46
  Log("Test 1");
  Map m1;
  TEST(m1.length() == 0, "Empty constructor should create map with length 0");
  TEST(m1.isEmpty(), "Empty map should return true for isEmpty()");

  Log("Test 2");
  Map m2(16);
  TEST(m2.length() == 0,
       "Capacity constructor should create map with length 0");
  TEST(m2.isEmpty(), "Capacity map should return true for isEmpty()");

  // Test put and get with different types - lines 48-66
  Log("Test 3");
  Map m3;
  m3.put(1, String("one"));
  TEST(m3.length() == 1, "put() should increase length to 1");
  TEST(m3.hasKey(1), "hasKey() should find the key");
  TEST((m3.get<int, String>(1).equals(String("one"))),
       "get() should retrieve value");

  Log("Test 4");
  m3.put(2, String("two"));
  TEST(m3.length() == 2, "put() should increase length to 2");
  TEST(m3.hasKey(2), "hasKey() should find second key");
  TEST((m3.get<int, String>(2).equals(String("two"))),
       "get() should retrieve second value");

  // Test update existing key
  Log("Test 5");
  m3.put(1, String("ONE"));
  TEST(m3.length() == 2, "put() with existing key should not increase length");
  TEST((m3.get<int, String>(1).equals(String("ONE"))),
       "put() should update value");

  // Report results
  if (errorCount == 0) {
    Log("All Map tests passed!");
    Exit(0);
  } else {
    LogError(errorCount, " test(s) failed");
    Exit(errorCount);
  }
}
