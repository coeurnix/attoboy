#include "attoboy/attoboy.h"
#include <windows.h>

// Implement new/delete for no-CRT environment
void *operator new(size_t size) { return HeapAlloc(GetProcessHeap(), 0, size); }

void operator delete(void *ptr) noexcept {
  if (ptr)
    HeapFree(GetProcessHeap(), 0, ptr);
}

void operator delete(void *ptr, size_t) noexcept {
  if (ptr)
    HeapFree(GetProcessHeap(), 0, ptr);
}

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError("Test failed: ", message);                                        \
    errorCount++;                                                              \
  }

void atto_main() {
  Log("Running Map tests...");

  // Test constructors
  Map m1;
  TEST(m1.length() == 0, "Empty constructor should create map with length 0");
  TEST(m1.isEmpty(), "Empty map should return true for isEmpty()");

  Map m2(16);
  TEST(m2.length() == 0, "Capacity constructor should create map with length 0");
  TEST(m2.isEmpty(), "Capacity map should return true for isEmpty()");

  // Test put and get with different types
  Map m3;
  m3.put(1, String("one"));
  TEST(m3.length() == 1, "put() should increase length to 1");
  TEST(m3.hasKey(1), "hasKey() should find the key");
  TEST((m3.get<int, String>(1).equals(String("one"))), "get() should retrieve value");

  m3.put(2, String("two"));
  TEST(m3.length() == 2, "put() should increase length to 2");
  TEST(m3.hasKey(2), "hasKey() should find second key");
  TEST((m3.get<int, String>(2).equals(String("two"))), "get() should retrieve second value");

  // Test update existing key
  m3.put(1, String("ONE"));
  TEST(m3.length() == 2, "put() with existing key should not increase length");
  TEST((m3.get<int, String>(1).equals(String("ONE"))), "put() should update value");

  // Test chaining
  Map m4;
  m4.put(1, 10).put(2, 20).put(3, 30);
  TEST(m4.length() == 3, "Chained put() should work");
  TEST((m4.get<int, int>(1) == 10), "First chained value");
  TEST((m4.get<int, int>(2) == 20), "Second chained value");
  TEST((m4.get<int, int>(3) == 30), "Third chained value");

  // Test with string keys
  Map m5;
  m5.put(String("name"), String("Alice"));
  m5.put(String("age"), 30);
  TEST(m5.length() == 2, "String keys should work");
  TEST((m5.get<String, String>(String("name")).equals(String("Alice"))), "String key retrieval");
  TEST((m5.get<String, int>(String("age")) == 30), "Mixed types with string key");

  // Test hasKey
  Map m6;
  m6.put(1, 100);
  TEST(m6.hasKey(1), "hasKey() should return true for existing key");
  TEST(!m6.hasKey(2), "hasKey() should return false for non-existing key");

  // Test typeAt
  Map m7;
  m7.put(1, 42);
  m7.put(2, String("test"));
  m7.put(3, 3.14);
  TEST(m7.typeAt(1) == TYPE_INT, "typeAt() should return TYPE_INT");
  TEST(m7.typeAt(2) == TYPE_STRING, "typeAt() should return TYPE_STRING");
  TEST(m7.typeAt(3) == TYPE_DOUBLE, "typeAt() should return TYPE_DOUBLE");
  TEST(m7.typeAt(99) == TYPE_INVALID, "typeAt() should return TYPE_INVALID for non-existing key");

  // Test remove
  Map m8;
  m8.put(1, 10).put(2, 20).put(3, 30);
  m8.remove(2);
  TEST(m8.length() == 2, "remove() should decrease length");
  TEST(!m8.hasKey(2), "remove() should remove the key");
  TEST(m8.hasKey(1), "remove() should not affect other keys");
  TEST(m8.hasKey(3), "remove() should not affect other keys");

  // Test remove non-existing key
  Map m9;
  m9.put(1, 10);
  m9.remove(99);
  TEST(m9.length() == 1, "remove() non-existing key should not change length");

  // Test clear
  Map m10;
  m10.put(1, 10).put(2, 20).put(3, 30);
  m10.clear();
  TEST(m10.length() == 0, "clear() should set length to 0");
  TEST(m10.isEmpty(), "clear() should make map empty");

  // Test keys()
  Map m11;
  m11.put(1, 10).put(2, 20).put(3, 30);
  List keys = m11.keys();
  TEST(keys.length() == 3, "keys() should return list with correct length");
  TEST(keys.contains(1), "keys() should contain first key");
  TEST(keys.contains(2), "keys() should contain second key");
  TEST(keys.contains(3), "keys() should contain third key");

  // Test values()
  Map m12;
  m12.put(1, 10).put(2, 20).put(3, 30);
  List values = m12.values();
  TEST(values.length() == 3, "values() should return list with correct length");
  TEST(values.contains(10), "values() should contain first value");
  TEST(values.contains(20), "values() should contain second value");
  TEST(values.contains(30), "values() should contain third value");

  // Test findValue
  Map m13;
  m13.put(String("a"), 100);
  m13.put(String("b"), 200);
  m13.put(String("c"), 300);
  String foundKey = m13.findValue<String, int>(200);
  TEST((foundKey.equals(String("b"))), "findValue() should find correct key");

  // Test merge
  Map m14;
  m14.put(1, 10).put(2, 20);
  Map m15;
  m15.put(3, 30).put(4, 40);
  m14.merge(m15);
  TEST(m14.length() == 4, "merge() should combine maps");
  TEST((m14.get<int, int>(1) == 10), "merge() should keep original keys");
  TEST((m14.get<int, int>(3) == 30), "merge() should add new keys");
  TEST((m14.get<int, int>(4) == 40), "merge() should add new keys");

  // Test merge with overlapping keys
  Map m16;
  m16.put(1, 10).put(2, 20);
  Map m17;
  m17.put(2, 99).put(3, 30);
  m16.merge(m17);
  TEST(m16.length() == 3, "merge() with overlapping keys");
  TEST((m16.get<int, int>(2) == 99), "merge() should update overlapping key");

  // Test duplicate
  Map m18;
  m18.put(1, 10).put(2, 20);
  Map m19 = m18.duplicate();
  TEST(m19.length() == 2, "duplicate() should copy length");
  TEST((m19.get<int, int>(1) == 10), "duplicate() should copy first pair");
  TEST((m19.get<int, int>(2) == 20), "duplicate() should copy second pair");

  // Test copy constructor
  Map m20;
  m20.put(String("x"), 100);
  Map m21(m20);
  TEST(m21.length() == 1, "Copy constructor should work");
  TEST((m21.get<String, int>(String("x")) == 100), "Copy constructor should copy values");

  // Test with bool keys
  Map m22;
  m22.put(true, String("yes"));
  m22.put(false, String("no"));
  TEST(m22.length() == 2, "Bool keys should work");
  TEST((m22.get<bool, String>(true).equals(String("yes"))), "Bool key true");
  TEST((m22.get<bool, String>(false).equals(String("no"))), "Bool key false");

  // Test with double keys
  Map m23;
  m23.put(1.5, String("one-half"));
  m23.put(2.5, String("two-half"));
  TEST(m23.length() == 2, "Double keys should work");
  TEST((m23.get<double, String>(1.5).equals(String("one-half"))), "Double key retrieval");

  // Test with long long keys
  Map m24;
  long long bigKey = 9223372036854775807LL;
  m24.put(bigKey, String("big"));
  TEST(m24.length() == 1, "Long long keys should work");
  TEST((m24.get<long long, String>(bigKey).equals(String("big"))), "Long long key retrieval");

  // Test nested containers
  List innerList;
  innerList.append(1).append(2).append(3);
  Map m25;
  m25.put(String("list"), innerList);
  TEST(m25.length() == 1, "Nested list values should work");
  List retrieved = m25.get<String, List>(String("list"));
  TEST(retrieved.length() == 3, "Retrieved nested list should have correct length");

  // Test nested maps
  Map innerMap;
  innerMap.put(1, 100);
  Map m26;
  m26.put(String("map"), innerMap);
  TEST(m26.length() == 1, "Nested map values should work");
  Map retrievedMap = m26.get<String, Map>(String("map"));
  TEST(retrievedMap.length() == 1, "Retrieved nested map should have correct length");

  // Test get with default value
  Map m27;
  m27.put(1, 10);
  TEST((m27.get<int, int>(1, 999) == 10), "get() with default should return value when key exists");
  TEST((m27.get<int, int>(99, 999) == 999), "get() with default should return default when key doesn't exist");

  // Test edge case: empty map operations
  Map empty;
  TEST(empty.length() == 0, "Empty map length");
  TEST(empty.isEmpty(), "Empty map isEmpty");
  TEST(!empty.hasKey(1), "Empty map hasKey() should return false");
  TEST((empty.get<int, int>(1) == 0), "Empty map get() should return default");
  List emptyKeys = empty.keys();
  TEST(emptyKeys.length() == 0, "Empty map keys() should return empty list");
  List emptyValues = empty.values();
  TEST(emptyValues.length() == 0, "Empty map values() should return empty list");

  // Test with c-string keys
  Map m28;
  m28.put("key1", 100);
  m28.put("key2", 200);
  TEST(m28.length() == 2, "C-string keys should work");
  TEST(m28.hasKey("key1"), "hasKey() with c-string");
  TEST((m28.get<String, int>(String("key1")) == 100), "C-string key retrieval");

  // Test with wide string keys
  Map m29;
  m29.put(L"wide", 999);
  TEST(m29.length() == 1, "Wide string keys should work");
  TEST(m29.hasKey(L"wide"), "hasKey() with wide string");

  // Report results
  if (errorCount == 0) {
    Log("All Map tests passed!");
    Exit(0);
  } else {
    LogError(errorCount, " test(s) failed");
    Exit(1);
  }
}
