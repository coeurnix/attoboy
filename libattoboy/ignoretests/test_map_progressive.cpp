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
#ifdef UNICODE
  OutputDebugStringW(ATTO_TEXT("[DEBUG] atto_main started\n"));
#else
  OutputDebugStringA(ATTO_TEXT("[DEBUG] atto_main started\n"));
#endif
  Log(ATTO_TEXT("Running Map tests..."));
#ifdef UNICODE
  OutputDebugStringW(ATTO_TEXT("[DEBUG] After first Log\n"));
#else
  OutputDebugStringA(ATTO_TEXT("[DEBUG] After first Log\n"));
#endif

  // Test constructors - lines 38-41 from test_map.cpp
  Log(ATTO_TEXT("Creating m1..."));
  Map m1;
  Log(ATTO_TEXT("Testing m1.length()..."));
  TEST(m1.length() == 0, ATTO_TEXT("Empty constructor should create map with length 0"));
  Log(ATTO_TEXT("After first TEST"));
  TEST(m1.isEmpty(), ATTO_TEXT("Empty map should return true for isEmpty()"));
  Log(ATTO_TEXT("After second TEST"));

  Log(ATTO_TEXT("Creating m2..."));
  Map m2(16);
  TEST(m2.length() == 0,
       ATTO_TEXT("Capacity constructor should create map with length 0"));
  TEST(m2.isEmpty(), ATTO_TEXT("Capacity map should return true for isEmpty()"));

  // Test put and get with different types - lines 48-66 from test_map.cpp
  Log(ATTO_TEXT("Creating m3..."));
  Map m3;
  Log(ATTO_TEXT("Putting first value..."));
  m3.put(1, String(ATTO_TEXT("one")));
  Log(ATTO_TEXT("After put"));
  TEST(m3.length() == 1, ATTO_TEXT("put() should increase length to 1"));
  TEST(m3.hasKey(1), ATTO_TEXT("hasKey() should find the key"));
  Log(ATTO_TEXT("About to get value..."));
  TEST((m3.get<int, String>(1).equals(String(ATTO_TEXT("one")))),
       ATTO_TEXT("get() should retrieve value"));
  Log(ATTO_TEXT("After get test"));

  Log(ATTO_TEXT("Putting second value..."));
  m3.put(2, String(ATTO_TEXT("two")));
  TEST(m3.length() == 2, ATTO_TEXT("put() should increase length to 2"));
  TEST(m3.hasKey(2), ATTO_TEXT("hasKey() should find second key"));
  TEST((m3.get<int, String>(2).equals(String(ATTO_TEXT("two")))),
       ATTO_TEXT("get() should retrieve second value"));

  // Test update existing key
  Log(ATTO_TEXT("Updating existing key..."));
  m3.put(1, String(ATTO_TEXT("ONE")));
  TEST(m3.length() == 2, ATTO_TEXT("put() with existing key should not increase length"));
  TEST((m3.get<int, String>(1).equals(String(ATTO_TEXT("ONE")))),
       ATTO_TEXT("put() should update value"));

  // Test chaining - lines 68-74
  Log(ATTO_TEXT("Test chaining..."));
  Map m4;
  m4.put(1, 10).put(2, 20).put(3, 30);
  TEST(m4.length() == 3, ATTO_TEXT("Chained put() should work"));
  TEST((m4.get<int, int>(1) == 10), ATTO_TEXT("First chained value"));
  TEST((m4.get<int, int>(2) == 20), ATTO_TEXT("Second chained value"));
  TEST((m4.get<int, int>(3) == 30), ATTO_TEXT("Third chained value"));

  // Test with string keys - lines 76-84
  Log(ATTO_TEXT("Test with string keys..."));
  Map m5;
  m5.put(String(ATTO_TEXT("name")), String(ATTO_TEXT("Alice")));
  m5.put(String(ATTO_TEXT("age")), 30);
  TEST(m5.length() == 2, ATTO_TEXT("String keys should work"));
  TEST((m5.get<String, String>(String(ATTO_TEXT("name"))).equals(String(ATTO_TEXT("Alice")))),
       ATTO_TEXT("String key retrieval"));
  TEST((m5.get<String, int>(String(ATTO_TEXT("age"))) == 30),
       ATTO_TEXT("Mixed types with string key"));

  // Test keys() - lines 125-132
  Log(ATTO_TEXT("Test keys()..."));
  Map m11;
  m11.put(1, 10).put(2, 20).put(3, 30);
  List keys = m11.keys();
  TEST(keys.length() == 3, ATTO_TEXT("keys() should return list with correct length"));
  TEST(keys.contains(1), ATTO_TEXT("keys() should contain first key"));
  TEST(keys.contains(2), ATTO_TEXT("keys() should contain second key"));
  TEST(keys.contains(3), ATTO_TEXT("keys() should contain third key"));

  // Test values() - lines 134-141
  Log(ATTO_TEXT("Test values()..."));
  Map m12;
  m12.put(1, 10).put(2, 20).put(3, 30);
  List values = m12.values();
  TEST(values.length() == 3, ATTO_TEXT("values() should return list with correct length"));
  TEST(values.contains(10), ATTO_TEXT("values() should contain first value"));
  TEST(values.contains(20), ATTO_TEXT("values() should contain second value"));
  TEST(values.contains(30), ATTO_TEXT("values() should contain third value"));

  // Test duplicate - lines 171-177
  Log(ATTO_TEXT("Test duplicate()..."));
  Map m18;
  m18.put(1, 10).put(2, 20);
  Map m19 = m18.duplicate();
  TEST(m19.length() == 2, ATTO_TEXT("duplicate() should copy length"));
  TEST((m19.get<int, int>(1) == 10), ATTO_TEXT("duplicate() should copy first pair"));
  TEST((m19.get<int, int>(2) == 20), ATTO_TEXT("duplicate() should copy second pair"));

  // Test copy constructor - lines 179-185
  Log(ATTO_TEXT("Test copy constructor..."));
  Map m20;
  m20.put(String(ATTO_TEXT("x")), 100);
  Map m21(m20);
  TEST(m21.length() == 1, ATTO_TEXT("Copy constructor should work"));
  TEST((m21.get<String, int>(String(ATTO_TEXT("x"))) == 100),
       ATTO_TEXT("Copy constructor should copy values"));

  Log(ATTO_TEXT("All tests passed before nested containers"));
  Exit(0);
}
