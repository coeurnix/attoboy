#include "attoboy/attoboy.h"
#include <windows.h>

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError(ATTO_TEXT("Test failed: "), message);                                        \
    errorCount++;                                                              \
  }

// Helper for approximate floating point equality
static inline bool ApproxEqual(float a, float b, float epsilon = 0.0001f) {
  float diff = a - b;
  if (diff < 0)
    diff = -diff;
  return diff < epsilon;
}

void atto_main() {
  Log(ATTO_TEXT("Running Map tests..."));

  // Test constructors
  Map m1;
  TEST(m1.length() == 0, ATTO_TEXT("Empty constructor should create map with length 0"));
  TEST(m1.isEmpty(), ATTO_TEXT("Empty map should return true for isEmpty()"));

  Log(ATTO_TEXT("Stage 1 passed"));

  Map m2(16);
  TEST(m2.length() == 0,
       ATTO_TEXT("Capacity constructor should create map with length 0"));
  TEST(m2.isEmpty(), ATTO_TEXT("Capacity map should return true for isEmpty()"));

  // Test put and get with different types
  Map m3;
  m3.put(1, String(ATTO_TEXT("one")));
  TEST(m3.length() == 1, ATTO_TEXT("put() should increase length to 1"));
  TEST(m3.hasKey(1), ATTO_TEXT("hasKey() should find the key"));
  TEST((m3.get<int, String>(1).equals(String(ATTO_TEXT("one")))),
       ATTO_TEXT("get() should retrieve value"));

  m3.put(2, String(ATTO_TEXT("two")));
  TEST(m3.length() == 2, ATTO_TEXT("put() should increase length to 2"));
  TEST(m3.hasKey(2), ATTO_TEXT("hasKey() should find second key"));
  TEST((m3.get<int, String>(2).equals(String(ATTO_TEXT("two")))),
       ATTO_TEXT("get() should retrieve second value"));

  // Test update existing key
  m3.put(1, String(ATTO_TEXT("ONE")));
  TEST(m3.length() == 2, ATTO_TEXT("put() with existing key should not increase length"));
  TEST((m3.get<int, String>(1).equals(String(ATTO_TEXT("ONE")))),
       ATTO_TEXT("put() should update value"));

  Log(ATTO_TEXT("Stage 2 passed"));

  // Test chaining
  Map m4;
  m4.put(1, 10).put(2, 20).put(3, 30);
  TEST(m4.length() == 3, ATTO_TEXT("Chained put() should work"));
  TEST((m4.get<int, int>(1) == 10), ATTO_TEXT("First chained value"));
  TEST((m4.get<int, int>(2) == 20), ATTO_TEXT("Second chained value"));
  TEST((m4.get<int, int>(3) == 30), ATTO_TEXT("Third chained value"));

  Log(ATTO_TEXT("Stage 3 passed"));

  // Test with string keys
  Map m5;
  m5.put(String(ATTO_TEXT("name")), String(ATTO_TEXT("Alice")));
  m5.put(String(ATTO_TEXT("age")), 30);
  TEST(m5.length() == 2, ATTO_TEXT("String keys should work"));
  TEST((m5.get<String, String>(String(ATTO_TEXT("name"))).equals(String(ATTO_TEXT("Alice")))),
       ATTO_TEXT("String key retrieval"));
  TEST((m5.get<String, int>(String(ATTO_TEXT("age"))) == 30),
       ATTO_TEXT("Mixed types with string key"));

  Log(ATTO_TEXT("Stage 4 passed"));

  // Test hasKey
  Map m6;
  m6.put(1, 100);
  TEST(m6.hasKey(1), ATTO_TEXT("hasKey() should return true for existing key"));
  TEST(!m6.hasKey(2), ATTO_TEXT("hasKey() should return false for non-existing key"));

  // Test typeAt
  Map m7;
  m7.put(1, 42);
  m7.put(2, String(ATTO_TEXT("test")));
  m7.put(3, 3.14f);
  TEST(m7.typeAt(1) == TYPE_INT, ATTO_TEXT("typeAt() should return TYPE_INT"));
  TEST(m7.typeAt(2) == TYPE_STRING, ATTO_TEXT("typeAt() should return TYPE_STRING"));
  TEST(m7.typeAt(3) == TYPE_FLOAT, ATTO_TEXT("typeAt() should return TYPE_FLOAT"));
  TEST(m7.typeAt(99) == TYPE_INVALID,
       ATTO_TEXT("typeAt() should return TYPE_INVALID for non-existing key"));

  // Test remove
  Map m8;
  m8.put(1, 10).put(2, 20).put(3, 30);
  m8.remove(2);
  TEST(m8.length() == 2, ATTO_TEXT("remove() should decrease length"));
  TEST(!m8.hasKey(2), ATTO_TEXT("remove() should remove the key"));
  TEST(m8.hasKey(1), ATTO_TEXT("remove() should not affect other keys"));
  TEST(m8.hasKey(3), ATTO_TEXT("remove() should not affect other keys"));

  // Test remove non-existing key
  Map m9;
  m9.put(1, 10);
  m9.remove(99);
  TEST(m9.length() == 1, ATTO_TEXT("remove() non-existing key should not change length"));

  Log(ATTO_TEXT("Stage 5 passed"));

  // Test clear
  Map m10;
  m10.put(1, 10).put(2, 20).put(3, 30);
  m10.clear();
  TEST(m10.length() == 0, ATTO_TEXT("clear() should set length to 0"));
  TEST(m10.isEmpty(), ATTO_TEXT("clear() should make map empty"));

  // Test keys()
  Map m11;
  m11.put(1, 10).put(2, 20).put(3, 30);
  List keys = m11.keys();
  TEST(keys.length() == 3, ATTO_TEXT("keys() should return list with correct length"));
  TEST(keys.contains(1), ATTO_TEXT("keys() should contain first key"));
  TEST(keys.contains(2), ATTO_TEXT("keys() should contain second key"));
  TEST(keys.contains(3), ATTO_TEXT("keys() should contain third key"));

  // Test values()
  Map m12;
  m12.put(1, 10).put(2, 20).put(3, 30);
  List values = m12.values();
  TEST(values.length() == 3, ATTO_TEXT("values() should return list with correct length"));
  TEST(values.contains(10), ATTO_TEXT("values() should contain first value"));
  TEST(values.contains(20), ATTO_TEXT("values() should contain second value"));
  TEST(values.contains(30), ATTO_TEXT("values() should contain third value"));

  Log(ATTO_TEXT("Stage 6 passed"));

  // Test findValue
  Map m13;
  m13.put(String(ATTO_TEXT("a")), 100);
  m13.put(String(ATTO_TEXT("b")), 200);
  m13.put(String(ATTO_TEXT("c")), 300);
  String foundKey = m13.findValue<String, int>(200);
  TEST((foundKey.equals(String(ATTO_TEXT("b")))), ATTO_TEXT("findValue() should find correct key"));

  // Test merge
  Map m14;
  m14.put(1, 10).put(2, 20);
  Map m15;
  m15.put(3, 30).put(4, 40);
  m14.merge(m15);
  TEST(m14.length() == 4, ATTO_TEXT("merge() should combine maps"));
  TEST((m14.get<int, int>(1) == 10), ATTO_TEXT("merge() should keep original keys"));
  TEST((m14.get<int, int>(3) == 30), ATTO_TEXT("merge() should add new keys"));
  TEST((m14.get<int, int>(4) == 40), ATTO_TEXT("merge() should add new keys"));

  // Test merge with overlapping keys
  Map m16;
  m16.put(1, 10).put(2, 20);
  Map m17;
  m17.put(2, 99).put(3, 30);
  m16.merge(m17);
  TEST(m16.length() == 3, ATTO_TEXT("merge() with overlapping keys"));
  TEST((m16.get<int, int>(2) == 99), ATTO_TEXT("merge() should update overlapping key"));

  Log(ATTO_TEXT("Stage 7 passed"));

  // Test duplicate
  Map m18;
  m18.put(1, 10).put(2, 20);
  Map m19 = m18.duplicate();
  TEST(m19.length() == 2, ATTO_TEXT("duplicate() should copy length"));
  TEST((m19.get<int, int>(1) == 10), ATTO_TEXT("duplicate() should copy first pair"));
  TEST((m19.get<int, int>(2) == 20), ATTO_TEXT("duplicate() should copy second pair"));

  // Test copy constructor
  Map m20;
  m20.put(String(ATTO_TEXT("x")), 100);
  Map m21(m20);
  TEST(m21.length() == 1, ATTO_TEXT("Copy constructor should work"));
  TEST((m21.get<String, int>(String(ATTO_TEXT("x"))) == 100),
       ATTO_TEXT("Copy constructor should copy values"));

  // Test with bool keys
  Map m22;
  m22.put(true, String(ATTO_TEXT("yes")));
  m22.put(false, String(ATTO_TEXT("no")));
  TEST(m22.length() == 2, ATTO_TEXT("Bool keys should work"));
  TEST((m22.get<bool, String>(true).equals(String(ATTO_TEXT("yes")))), ATTO_TEXT("Bool key true"));
  TEST((m22.get<bool, String>(false).equals(String(ATTO_TEXT("no")))), ATTO_TEXT("Bool key false"));

  // Test with float keys
  Map m23;
  m23.put(1.5f, String(ATTO_TEXT("one-half")));
  m23.put(2.5f, String(ATTO_TEXT("two-half")));
  TEST(m23.length() == 2, ATTO_TEXT("Float keys should work"));
  TEST((m23.get<float, String>(1.5f).equals(String(ATTO_TEXT("one-half")))),
       ATTO_TEXT("Float key retrieval"));

  Log(ATTO_TEXT("Stage 8 passed"));

  // Test nested containers
  List innerList;
  innerList.append(1).append(2).append(3);
  Map m25;
  m25.put(String(ATTO_TEXT("list")), innerList);
  TEST(m25.length() == 1, ATTO_TEXT("Nested list values should work"));
  List retrieved = m25.get<String, List>(String(ATTO_TEXT("list")));
  TEST(retrieved.length() == 3,
       ATTO_TEXT("Retrieved nested list should have correct length"));

  Log(ATTO_TEXT("Stage 9 passed"));

  // Test nested maps
  Map innerMap;
  innerMap.put(1, 100);
  Log(ATTO_TEXT("nested-map-A"));
  Map m26;
  m26.put(String(ATTO_TEXT("map")), innerMap);
  Log(ATTO_TEXT("nested-map-B"));
  TEST(m26.length() == 1, ATTO_TEXT("Nested map values should work"));
  Map retrievedMap = m26.get<String, Map>(String(ATTO_TEXT("map")));
  Log(ATTO_TEXT("nested-map-C"));
  TEST(retrievedMap.length() == 1,
       ATTO_TEXT("Retrieved nested map should have correct length"));

  Log(ATTO_TEXT("Stage 10 passed"));

  // Test get with default value
  Map m27;
  m27.put(1, 10);
  TEST((m27.get<int, int>(1, 999) == 10),
       ATTO_TEXT("get() with default should return value when key exists"));
  TEST((m27.get<int, int>(99, 999) == 999),
       ATTO_TEXT("get() with default should return default when key doesn't exist"));

  Log(ATTO_TEXT("Stage 11 passed"));

  // Test edge case: empty map operations
  Map empty;
  TEST(empty.length() == 0, ATTO_TEXT("Empty map length"));
  TEST(empty.isEmpty(), ATTO_TEXT("Empty map isEmpty"));
  TEST(!empty.hasKey(1), ATTO_TEXT("Empty map hasKey() should return false"));
  TEST((empty.get<int, int>(1) == 0), ATTO_TEXT("Empty map get() should return default"));
  List emptyKeys = empty.keys();
  TEST(emptyKeys.length() == 0, ATTO_TEXT("Empty map keys() should return empty list"));
  List emptyValues = empty.values();
  TEST(emptyValues.length() == 0,
       ATTO_TEXT("Empty map values() should return empty list"));

  Log(ATTO_TEXT("Stage 12 passed"));

  // Test with c-string keys
  Map m28;
  m28.put(ATTO_TEXT("key1"), 100);
  m28.put(ATTO_TEXT("key2"), 200);
  TEST(m28.length() == 2, ATTO_TEXT("C-string keys should work"));
  TEST(m28.hasKey(ATTO_TEXT("key1")), ATTO_TEXT("hasKey() with c-string"));
  TEST((m28.get<String, int>(String(ATTO_TEXT("key1"))) == 100), ATTO_TEXT("C-string key retrieval"));

  Log(ATTO_TEXT("Stage 13 passed"));

  // Test with wide string keys
  Map m29;
  m29.put(ATTO_TEXT("wide"), 999);
  TEST(m29.length() == 1, ATTO_TEXT("Wide string keys should work"));
  TEST(m29.hasKey(ATTO_TEXT("wide")), ATTO_TEXT("hasKey() with wide string"));

  // Report results
  if (errorCount == 0) {
    Log(ATTO_TEXT("All Map tests passed!"));
    Exit(0);
  } else {
    LogError(errorCount, ATTO_TEXT(" test(s) failed"));
    Exit(errorCount);
  }
}
