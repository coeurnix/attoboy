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
  OutputDebugStringW(L"[DEBUG] atto_main started\n");
  Log("Running Map tests...");
  OutputDebugStringW(L"[DEBUG] After first Log\n");

  // Test constructors - lines 38-41 from test_map.cpp
  Log("Creating m1...");
  Map m1;
  Log("Testing m1.length()...");
  TEST(m1.length() == 0, "Empty constructor should create map with length 0");
  Log("After first TEST");
  TEST(m1.isEmpty(), "Empty map should return true for isEmpty()");
  Log("After second TEST");

  Log("Creating m2...");
  Map m2(16);
  TEST(m2.length() == 0,
       "Capacity constructor should create map with length 0");
  TEST(m2.isEmpty(), "Capacity map should return true for isEmpty()");

  // Test put and get with different types - lines 48-66 from test_map.cpp
  Log("Creating m3...");
  Map m3;
  Log("Putting first value...");
  m3.put(1, String("one"));
  Log("After put");
  TEST(m3.length() == 1, "put() should increase length to 1");
  TEST(m3.hasKey(1), "hasKey() should find the key");
  Log("About to get value...");
  TEST((m3.get<int, String>(1).equals(String("one"))),
       "get() should retrieve value");
  Log("After get test");

  Log("Putting second value...");
  m3.put(2, String("two"));
  TEST(m3.length() == 2, "put() should increase length to 2");
  TEST(m3.hasKey(2), "hasKey() should find second key");
  TEST((m3.get<int, String>(2).equals(String("two"))),
       "get() should retrieve second value");

  // Test update existing key
  Log("Updating existing key...");
  m3.put(1, String("ONE"));
  TEST(m3.length() == 2, "put() with existing key should not increase length");
  TEST((m3.get<int, String>(1).equals(String("ONE"))),
       "put() should update value");

  // Test chaining - lines 68-74
  Log("Test chaining...");
  Map m4;
  m4.put(1, 10).put(2, 20).put(3, 30);
  TEST(m4.length() == 3, "Chained put() should work");
  TEST((m4.get<int, int>(1) == 10), "First chained value");
  TEST((m4.get<int, int>(2) == 20), "Second chained value");
  TEST((m4.get<int, int>(3) == 30), "Third chained value");

  // Test with string keys - lines 76-84
  Log("Test with string keys...");
  Map m5;
  m5.put(String("name"), String("Alice"));
  m5.put(String("age"), 30);
  TEST(m5.length() == 2, "String keys should work");
  TEST((m5.get<String, String>(String("name")).equals(String("Alice"))),
       "String key retrieval");
  TEST((m5.get<String, int>(String("age")) == 30),
       "Mixed types with string key");

  // Test keys() - lines 125-132
  Log("Test keys()...");
  Map m11;
  m11.put(1, 10).put(2, 20).put(3, 30);
  List keys = m11.keys();
  TEST(keys.length() == 3, "keys() should return list with correct length");
  TEST(keys.contains(1), "keys() should contain first key");
  TEST(keys.contains(2), "keys() should contain second key");
  TEST(keys.contains(3), "keys() should contain third key");

  // Test values() - lines 134-141
  Log("Test values()...");
  Map m12;
  m12.put(1, 10).put(2, 20).put(3, 30);
  List values = m12.values();
  TEST(values.length() == 3, "values() should return list with correct length");
  TEST(values.contains(10), "values() should contain first value");
  TEST(values.contains(20), "values() should contain second value");
  TEST(values.contains(30), "values() should contain third value");

  // Test duplicate - lines 171-177
  Log("Test duplicate()...");
  Map m18;
  m18.put(1, 10).put(2, 20);
  Map m19 = m18.duplicate();
  TEST(m19.length() == 2, "duplicate() should copy length");
  TEST((m19.get<int, int>(1) == 10), "duplicate() should copy first pair");
  TEST((m19.get<int, int>(2) == 20), "duplicate() should copy second pair");

  // Test copy constructor - lines 179-185
  Log("Test copy constructor...");
  Map m20;
  m20.put(String("x"), 100);
  Map m21(m20);
  TEST(m21.length() == 1, "Copy constructor should work");
  TEST((m21.get<String, int>(String("x")) == 100),
       "Copy constructor should copy values");

  Log("All tests passed before nested containers");
  Exit(0);
}
