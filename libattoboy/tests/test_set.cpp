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
  Log(ATTO_TEXT("Running Set tests..."));

  // Test constructors
  Set s1;
  TEST(s1.length() == 0, ATTO_TEXT("Empty constructor should create set with length 0"));
  TEST(s1.isEmpty(), ATTO_TEXT("Empty set should return true for isEmpty()"));

  Set s2(16);
  TEST(s2.length() == 0,
       ATTO_TEXT("Capacity constructor should create set with length 0"));
  TEST(s2.isEmpty(), ATTO_TEXT("Capacity set should return true for isEmpty()"));

  // Test put with different types
  Set s3;
  s3.put(1);
  TEST(s3.length() == 1, ATTO_TEXT("put() should increase length to 1"));
  TEST(s3.contains(1), ATTO_TEXT("contains() should find the value"));

  s3.put(2);
  TEST(s3.length() == 2, ATTO_TEXT("put() should increase length to 2"));
  TEST(s3.contains(2), ATTO_TEXT("contains() should find second value"));

  // Test uniqueness - adding duplicate
  s3.put(1);
  TEST(s3.length() == 2, ATTO_TEXT("put() with duplicate should not increase length"));
  TEST(s3.contains(1), ATTO_TEXT("Duplicate value should still be contained"));

  // Test chaining
  Set s4;
  s4.put(1).put(2).put(3);
  TEST(s4.length() == 3, ATTO_TEXT("Chained put() should work"));
  TEST(s4.contains(1), ATTO_TEXT("First chained value"));
  TEST(s4.contains(2), ATTO_TEXT("Second chained value"));
  TEST(s4.contains(3), ATTO_TEXT("Third chained value"));

  // Test with duplicates in chain
  Set s5;
  s5.put(1).put(2).put(1).put(3).put(2);
  TEST(s5.length() == 3, ATTO_TEXT("Chained put() should ignore duplicates"));

  // Test with strings
  Set s6;
  s6.put(String(ATTO_TEXT("apple")));
  s6.put(String(ATTO_TEXT("banana")));
  s6.put(String(ATTO_TEXT("apple"))); // duplicate
  TEST(s6.length() == 2, ATTO_TEXT("String values should work with uniqueness"));
  TEST(s6.contains(String(ATTO_TEXT("apple"))), ATTO_TEXT("String contains() check"));
  TEST(s6.contains(String(ATTO_TEXT("banana"))), ATTO_TEXT("String contains() check 2"));

  // Test contains with non-existing value
  Set s7;
  s7.put(10).put(20).put(30);
  TEST(s7.contains(20), ATTO_TEXT("contains() should return true for existing value"));
  TEST(!s7.contains(99),
       ATTO_TEXT("contains() should return false for non-existing value"));

  // Test remove
  Set s8;
  s8.put(1).put(2).put(3);
  s8.remove(2);
  TEST(s8.length() == 2, ATTO_TEXT("remove() should decrease length"));
  TEST(!s8.contains(2), ATTO_TEXT("remove() should remove the value"));
  TEST(s8.contains(1), ATTO_TEXT("remove() should not affect other values"));
  TEST(s8.contains(3), ATTO_TEXT("remove() should not affect other values"));

  // Test remove non-existing value
  Set s9;
  s9.put(10);
  s9.remove(99);
  TEST(s9.length() == 1,
       ATTO_TEXT("remove() non-existing value should not change length"));

  // Test clear
  Set s10;
  s10.put(1).put(2).put(3);
  s10.clear();
  TEST(s10.length() == 0, ATTO_TEXT("clear() should set length to 0"));
  TEST(s10.isEmpty(), ATTO_TEXT("clear() should make set empty"));

  // Test toList
  Set s11;
  s11.put(10).put(20).put(30);
  List list = s11.toList();
  TEST(list.length() == 3, ATTO_TEXT("toList() should return list with correct length"));
  TEST(list.contains(10), ATTO_TEXT("toList() should contain first value"));
  TEST(list.contains(20), ATTO_TEXT("toList() should contain second value"));
  TEST(list.contains(30), ATTO_TEXT("toList() should contain third value"));

  // Test setUnion
  Set s12;
  s12.put(1).put(2);
  Set s13;
  s13.put(3).put(4);
  s12.setUnion(s13);
  TEST(s12.length() == 4, ATTO_TEXT("setUnion() should combine sets"));
  TEST(s12.contains(1), ATTO_TEXT("setUnion() should keep original values"));
  TEST(s12.contains(2), ATTO_TEXT("setUnion() should keep original values"));
  TEST(s12.contains(3), ATTO_TEXT("setUnion() should add new values"));
  TEST(s12.contains(4), ATTO_TEXT("setUnion() should add new values"));

  // Test setUnion with overlapping values
  Set s14;
  s14.put(1).put(2).put(3);
  Set s15;
  s15.put(2).put(3).put(4);
  s14.setUnion(s15);
  TEST(s14.length() == 4,
       ATTO_TEXT("setUnion() with overlapping values should not duplicate"));
  TEST(s14.contains(1), ATTO_TEXT("setUnion() overlap check 1"));
  TEST(s14.contains(2), ATTO_TEXT("setUnion() overlap check 2"));
  TEST(s14.contains(3), ATTO_TEXT("setUnion() overlap check 3"));
  TEST(s14.contains(4), ATTO_TEXT("setUnion() overlap check 4"));

  // Test intersect
  Set s38;
  s38.put(1).put(2).put(3);
  Set s39;
  s39.put(2).put(3).put(4);
  s38.intersect(s39);
  TEST(s38.length() == 2, ATTO_TEXT("intersect() should keep only common values"));
  TEST(!s38.contains(1), ATTO_TEXT("intersect() should remove non-common values"));
  TEST(s38.contains(2), ATTO_TEXT("intersect() should keep common value 2"));
  TEST(s38.contains(3), ATTO_TEXT("intersect() should keep common value 3"));
  TEST(!s38.contains(4), ATTO_TEXT("intersect() should not add new values"));

  // Test intersect with no overlap
  Set s40;
  s40.put(1).put(2);
  Set s41;
  s41.put(3).put(4);
  s40.intersect(s41);
  TEST(s40.length() == 0, ATTO_TEXT("intersect() with no overlap should be empty"));
  TEST(s40.isEmpty(), ATTO_TEXT("intersect() with no overlap isEmpty check"));

  // Test intersect with self
  Set s42;
  s42.put(1).put(2).put(3);
  s42.intersect(s42);
  TEST(s42.length() == 3, ATTO_TEXT("intersect(self) should keep all values"));
  TEST(s42.contains(1), ATTO_TEXT("intersect(self) check 1"));
  TEST(s42.contains(2), ATTO_TEXT("intersect(self) check 2"));
  TEST(s42.contains(3), ATTO_TEXT("intersect(self) check 3"));

  // Test subtract
  Set s43;
  s43.put(1).put(2).put(3).put(4);
  Set s44;
  s44.put(2).put(4);
  s43.subtract(s44);
  TEST(s43.length() == 2, ATTO_TEXT("subtract() should remove matching values"));
  TEST(s43.contains(1), ATTO_TEXT("subtract() should keep non-matching value 1"));
  TEST(!s43.contains(2), ATTO_TEXT("subtract() should remove matching value 2"));
  TEST(s43.contains(3), ATTO_TEXT("subtract() should keep non-matching value 3"));
  TEST(!s43.contains(4), ATTO_TEXT("subtract() should remove matching value 4"));

  // Test subtract with no overlap
  Set s45;
  s45.put(1).put(2);
  Set s46;
  s46.put(3).put(4);
  s45.subtract(s46);
  TEST(s45.length() == 2, ATTO_TEXT("subtract() with no overlap should keep all"));
  TEST(s45.contains(1), ATTO_TEXT("subtract() no overlap check 1"));
  TEST(s45.contains(2), ATTO_TEXT("subtract() no overlap check 2"));

  // Test subtract with self
  Set s47;
  s47.put(1).put(2).put(3);
  s47.subtract(s47);
  TEST(s47.length() == 0, ATTO_TEXT("subtract(self) should clear set"));
  TEST(s47.isEmpty(), ATTO_TEXT("subtract(self) isEmpty check"));

  // Test duplicate
  Set s16;
  s16.put(10).put(20);
  Set s17 = s16.duplicate();
  TEST(s17.length() == 2, ATTO_TEXT("duplicate() should copy length"));
  TEST(s17.contains(10), ATTO_TEXT("duplicate() should copy first value"));
  TEST(s17.contains(20), ATTO_TEXT("duplicate() should copy second value"));

  // Test copy constructor
  Set s18;
  s18.put(100).put(200);
  Set s19(s18);
  TEST(s19.length() == 2, ATTO_TEXT("Copy constructor should work"));
  TEST(s19.contains(100), ATTO_TEXT("Copy constructor should copy first value"));
  TEST(s19.contains(200), ATTO_TEXT("Copy constructor should copy second value"));

  // Test Set from List constructor
  List l1;
  l1.append(1).append(2).append(3).append(2).append(1);
  Set s20(l1);
  TEST(s20.length() == 3, ATTO_TEXT("Set from List should remove duplicates"));
  TEST(s20.contains(1), ATTO_TEXT("Set from List should contain value 1"));
  TEST(s20.contains(2), ATTO_TEXT("Set from List should contain value 2"));
  TEST(s20.contains(3), ATTO_TEXT("Set from List should contain value 3"));

  // Test List from Set constructor
  Set s21;
  s21.put(10).put(20).put(30);
  List l2(s21);
  TEST(l2.length() == 3, ATTO_TEXT("List from Set constructor should work"));
  TEST(l2.contains(10), ATTO_TEXT("List from Set should contain value 10"));
  TEST(l2.contains(20), ATTO_TEXT("List from Set should contain value 20"));
  TEST(l2.contains(30), ATTO_TEXT("List from Set should contain value 30"));

  // Test with bool values
  Set s22;
  s22.put(true);
  s22.put(false);
  s22.put(true); // duplicate
  TEST(s22.length() == 2, ATTO_TEXT("Bool values should work with uniqueness"));
  TEST(s22.contains(true), ATTO_TEXT("Bool contains() true"));
  TEST(s22.contains(false), ATTO_TEXT("Bool contains() false"));

  // Test with float values
  Set s23;
  s23.put(1.5f);
  s23.put(2.5f);
  s23.put(1.5f); // duplicate
  TEST(s23.length() == 2, ATTO_TEXT("Float values should work with uniqueness"));
  TEST(s23.contains(1.5f), ATTO_TEXT("Float contains() check"));
  TEST(s23.contains(2.5f), ATTO_TEXT("Float contains() check 2"));

  // Test with c-strings
  Set s25;
  s25.put(ATTO_TEXT("test"));
  s25.put(ATTO_TEXT("hello"));
  s25.put(ATTO_TEXT("test")); // duplicate
  TEST(s25.length() == 2, ATTO_TEXT("C-string values should work with uniqueness"));
  TEST(s25.contains(ATTO_TEXT("test")), ATTO_TEXT("C-string contains() check"));
  TEST(s25.contains(ATTO_TEXT("hello")), ATTO_TEXT("C-string contains() check 2"));

  // Test with wide strings
  Set s26;
  s26.put(ATTO_TEXT("wide"));
  s26.put(ATTO_TEXT("string"));
  s26.put(ATTO_TEXT("wide")); // duplicate
  TEST(s26.length() == 2, ATTO_TEXT("Wide string values should work with uniqueness"));
  TEST(s26.contains(ATTO_TEXT("wide")), ATTO_TEXT("Wide string contains() check"));

  // Test edge case: empty set operations
  Set empty;
  TEST(empty.length() == 0, ATTO_TEXT("Empty set length"));
  TEST(empty.isEmpty(), ATTO_TEXT("Empty set isEmpty"));
  TEST(!empty.contains(1), ATTO_TEXT("Empty set contains() should return false"));
  List emptyList = empty.toList();
  TEST(emptyList.length() == 0, ATTO_TEXT("Empty set toList() should return empty list"));

  // Test numeric coercion (int and float)
  Set s27;
  s27.put(5);
  TEST(s27.contains(5), ATTO_TEXT("Int value should be found"));
  TEST(s27.contains(5.0f),
       ATTO_TEXT("Int value should be found with float (numeric coercion)"));

  // Test List concat with Set
  List l3;
  l3.append(1).append(2);
  Set s28;
  s28.put(3).put(4);
  l3.concat(s28);
  TEST(l3.length() == 4, ATTO_TEXT("List concat(Set) should work"));
  TEST(l3.at<int>(0) == 1, ATTO_TEXT("Concat first item"));
  TEST(l3.at<int>(1) == 2, ATTO_TEXT("Concat second item"));
  TEST(l3.contains(3), ATTO_TEXT("Concat should add set values"));
  TEST(l3.contains(4), ATTO_TEXT("Concat should add set values"));

  // Report results
  if (errorCount == 0) {
    Log(ATTO_TEXT("All Set tests passed!"));
    Exit(0);
  } else {
    LogError(errorCount, ATTO_TEXT(" test(s) failed"));
    Exit(errorCount);
  }
}
