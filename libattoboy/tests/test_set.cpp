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
  Log("Running Set tests...");

  // Test constructors
  Set s1;
  TEST(s1.length() == 0, "Empty constructor should create set with length 0");
  TEST(s1.isEmpty(), "Empty set should return true for isEmpty()");

  Set s2(16);
  TEST(s2.length() == 0,
       "Capacity constructor should create set with length 0");
  TEST(s2.isEmpty(), "Capacity set should return true for isEmpty()");

  // Test put with different types
  Set s3;
  s3.put(1);
  TEST(s3.length() == 1, "put() should increase length to 1");
  TEST(s3.contains(1), "contains() should find the value");

  s3.put(2);
  TEST(s3.length() == 2, "put() should increase length to 2");
  TEST(s3.contains(2), "contains() should find second value");

  // Test uniqueness - adding duplicate
  s3.put(1);
  TEST(s3.length() == 2, "put() with duplicate should not increase length");
  TEST(s3.contains(1), "Duplicate value should still be contained");

  // Test chaining
  Set s4;
  s4.put(1).put(2).put(3);
  TEST(s4.length() == 3, "Chained put() should work");
  TEST(s4.contains(1), "First chained value");
  TEST(s4.contains(2), "Second chained value");
  TEST(s4.contains(3), "Third chained value");

  // Test with duplicates in chain
  Set s5;
  s5.put(1).put(2).put(1).put(3).put(2);
  TEST(s5.length() == 3, "Chained put() should ignore duplicates");

  // Test with strings
  Set s6;
  s6.put(String("apple"));
  s6.put(String("banana"));
  s6.put(String("apple")); // duplicate
  TEST(s6.length() == 2, "String values should work with uniqueness");
  TEST(s6.contains(String("apple")), "String contains() check");
  TEST(s6.contains(String("banana")), "String contains() check 2");

  // Test contains with non-existing value
  Set s7;
  s7.put(10).put(20).put(30);
  TEST(s7.contains(20), "contains() should return true for existing value");
  TEST(!s7.contains(99),
       "contains() should return false for non-existing value");

  // Test remove
  Set s8;
  s8.put(1).put(2).put(3);
  s8.remove(2);
  TEST(s8.length() == 2, "remove() should decrease length");
  TEST(!s8.contains(2), "remove() should remove the value");
  TEST(s8.contains(1), "remove() should not affect other values");
  TEST(s8.contains(3), "remove() should not affect other values");

  // Test remove non-existing value
  Set s9;
  s9.put(10);
  s9.remove(99);
  TEST(s9.length() == 1,
       "remove() non-existing value should not change length");

  // Test clear
  Set s10;
  s10.put(1).put(2).put(3);
  s10.clear();
  TEST(s10.length() == 0, "clear() should set length to 0");
  TEST(s10.isEmpty(), "clear() should make set empty");

  // Test toList
  Set s11;
  s11.put(10).put(20).put(30);
  List list = s11.toList();
  TEST(list.length() == 3, "toList() should return list with correct length");
  TEST(list.contains(10), "toList() should contain first value");
  TEST(list.contains(20), "toList() should contain second value");
  TEST(list.contains(30), "toList() should contain third value");

  // Test setUnion
  Set s12;
  s12.put(1).put(2);
  Set s13;
  s13.put(3).put(4);
  s12.setUnion(s13);
  TEST(s12.length() == 4, "setUnion() should combine sets");
  TEST(s12.contains(1), "setUnion() should keep original values");
  TEST(s12.contains(2), "setUnion() should keep original values");
  TEST(s12.contains(3), "setUnion() should add new values");
  TEST(s12.contains(4), "setUnion() should add new values");

  // Test setUnion with overlapping values
  Set s14;
  s14.put(1).put(2).put(3);
  Set s15;
  s15.put(2).put(3).put(4);
  s14.setUnion(s15);
  TEST(s14.length() == 4,
       "setUnion() with overlapping values should not duplicate");
  TEST(s14.contains(1), "setUnion() overlap check 1");
  TEST(s14.contains(2), "setUnion() overlap check 2");
  TEST(s14.contains(3), "setUnion() overlap check 3");
  TEST(s14.contains(4), "setUnion() overlap check 4");

  // Test intersect
  Set s38;
  s38.put(1).put(2).put(3);
  Set s39;
  s39.put(2).put(3).put(4);
  s38.intersect(s39);
  TEST(s38.length() == 2, "intersect() should keep only common values");
  TEST(!s38.contains(1), "intersect() should remove non-common values");
  TEST(s38.contains(2), "intersect() should keep common value 2");
  TEST(s38.contains(3), "intersect() should keep common value 3");
  TEST(!s38.contains(4), "intersect() should not add new values");

  // Test intersect with no overlap
  Set s40;
  s40.put(1).put(2);
  Set s41;
  s41.put(3).put(4);
  s40.intersect(s41);
  TEST(s40.length() == 0, "intersect() with no overlap should be empty");
  TEST(s40.isEmpty(), "intersect() with no overlap isEmpty check");

  // Test intersect with self
  Set s42;
  s42.put(1).put(2).put(3);
  s42.intersect(s42);
  TEST(s42.length() == 3, "intersect(self) should keep all values");
  TEST(s42.contains(1), "intersect(self) check 1");
  TEST(s42.contains(2), "intersect(self) check 2");
  TEST(s42.contains(3), "intersect(self) check 3");

  // Test subtract
  Set s43;
  s43.put(1).put(2).put(3).put(4);
  Set s44;
  s44.put(2).put(4);
  s43.subtract(s44);
  TEST(s43.length() == 2, "subtract() should remove matching values");
  TEST(s43.contains(1), "subtract() should keep non-matching value 1");
  TEST(!s43.contains(2), "subtract() should remove matching value 2");
  TEST(s43.contains(3), "subtract() should keep non-matching value 3");
  TEST(!s43.contains(4), "subtract() should remove matching value 4");

  // Test subtract with no overlap
  Set s45;
  s45.put(1).put(2);
  Set s46;
  s46.put(3).put(4);
  s45.subtract(s46);
  TEST(s45.length() == 2, "subtract() with no overlap should keep all");
  TEST(s45.contains(1), "subtract() no overlap check 1");
  TEST(s45.contains(2), "subtract() no overlap check 2");

  // Test subtract with self
  Set s47;
  s47.put(1).put(2).put(3);
  s47.subtract(s47);
  TEST(s47.length() == 0, "subtract(self) should clear set");
  TEST(s47.isEmpty(), "subtract(self) isEmpty check");

  // Test duplicate
  Set s16;
  s16.put(10).put(20);
  Set s17 = s16.duplicate();
  TEST(s17.length() == 2, "duplicate() should copy length");
  TEST(s17.contains(10), "duplicate() should copy first value");
  TEST(s17.contains(20), "duplicate() should copy second value");

  // Test copy constructor
  Set s18;
  s18.put(100).put(200);
  Set s19(s18);
  TEST(s19.length() == 2, "Copy constructor should work");
  TEST(s19.contains(100), "Copy constructor should copy first value");
  TEST(s19.contains(200), "Copy constructor should copy second value");

  // Test Set from List constructor
  List l1;
  l1.append(1).append(2).append(3).append(2).append(1);
  Set s20(l1);
  TEST(s20.length() == 3, "Set from List should remove duplicates");
  TEST(s20.contains(1), "Set from List should contain value 1");
  TEST(s20.contains(2), "Set from List should contain value 2");
  TEST(s20.contains(3), "Set from List should contain value 3");

  // Test List from Set constructor
  Set s21;
  s21.put(10).put(20).put(30);
  List l2(s21);
  TEST(l2.length() == 3, "List from Set constructor should work");
  TEST(l2.contains(10), "List from Set should contain value 10");
  TEST(l2.contains(20), "List from Set should contain value 20");
  TEST(l2.contains(30), "List from Set should contain value 30");

  // Test with bool values
  Set s22;
  s22.put(true);
  s22.put(false);
  s22.put(true); // duplicate
  TEST(s22.length() == 2, "Bool values should work with uniqueness");
  TEST(s22.contains(true), "Bool contains() true");
  TEST(s22.contains(false), "Bool contains() false");

  // Test with double values
  Set s23;
  s23.put(1.5);
  s23.put(2.5);
  s23.put(1.5); // duplicate
  TEST(s23.length() == 2, "Double values should work with uniqueness");
  TEST(s23.contains(1.5), "Double contains() check");
  TEST(s23.contains(2.5), "Double contains() check 2");

  // Test with long long values
  Set s24;
  long long bigVal = 9223372036854775807LL;
  s24.put(bigVal);
  s24.put(bigVal); // duplicate
  TEST(s24.length() == 1, "Long long values should work with uniqueness");
  TEST(s24.contains(bigVal), "Long long contains() check");

  // Test with c-strings
  Set s25;
  s25.put("test");
  s25.put("hello");
  s25.put("test"); // duplicate
  TEST(s25.length() == 2, "C-string values should work with uniqueness");
  TEST(s25.contains("test"), "C-string contains() check");
  TEST(s25.contains("hello"), "C-string contains() check 2");

  // Test with wide strings
  Set s26;
  s26.put(L"wide");
  s26.put(L"string");
  s26.put(L"wide"); // duplicate
  TEST(s26.length() == 2, "Wide string values should work with uniqueness");
  TEST(s26.contains(L"wide"), "Wide string contains() check");

  // Test edge case: empty set operations
  Set empty;
  TEST(empty.length() == 0, "Empty set length");
  TEST(empty.isEmpty(), "Empty set isEmpty");
  TEST(!empty.contains(1), "Empty set contains() should return false");
  List emptyList = empty.toList();
  TEST(emptyList.length() == 0, "Empty set toList() should return empty list");

  // Test numeric coercion (int and double)
  Set s27;
  s27.put(5);
  TEST(s27.contains(5), "Int value should be found");
  TEST(s27.contains(5.0),
       "Int value should be found with double (numeric coercion)");

  // Test List concat with Set
  List l3;
  l3.append(1).append(2);
  Set s28;
  s28.put(3).put(4);
  l3.concat(s28);
  TEST(l3.length() == 4, "List concat(Set) should work");
  TEST(l3.at<int>(0) == 1, "Concat first item");
  TEST(l3.at<int>(1) == 2, "Concat second item");
  TEST(l3.contains(3), "Concat should add set values");
  TEST(l3.contains(4), "Concat should add set values");

  // Report results
  if (errorCount == 0) {
    Log("All Set tests passed!");
    Exit(0);
  } else {
    LogError(errorCount, " test(s) failed");
    Exit(errorCount);
  }
}
