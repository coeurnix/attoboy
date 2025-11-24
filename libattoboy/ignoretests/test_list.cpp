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
  Log(ATTO_TEXT("Running List tests..."));

  // Test constructors
  List l1;
  TEST(l1.length() == 0, ATTO_TEXT("Empty constructor should create list with length 0"));
  TEST(l1.isEmpty(), ATTO_TEXT("Empty list should return true for isEmpty()"));

  List l2(16);
  TEST(l2.length() == 0,
       ATTO_TEXT("Capacity constructor should create list with length 0"));
  TEST(l2.isEmpty(), ATTO_TEXT("Capacity list should return true for isEmpty()"));

  // Test append with different types
  List l3;
  l3.append(true);
  TEST(l3.length() == 1, ATTO_TEXT("Append bool should increase length to 1"));
  TEST(l3.typeAt(0) == TYPE_BOOL, ATTO_TEXT("First item should be TYPE_BOOL"));
  TEST(l3.at<bool>(0) == true, ATTO_TEXT("First item should be true"));

  l3.append(42);
  TEST(l3.length() == 2, ATTO_TEXT("Append int should increase length to 2"));
  TEST(l3.typeAt(1) == TYPE_INT, ATTO_TEXT("Second item should be TYPE_INT"));
  TEST(l3.at<int>(1) == 42, ATTO_TEXT("Second item should be 42"));

  l3.append(3.14f);
  TEST(l3.length() == 3, ATTO_TEXT("Append float should increase length to 3"));
  TEST(l3.typeAt(2) == TYPE_FLOAT, ATTO_TEXT("Third item should be TYPE_FLOAT"));
  TEST(ApproxEqual(l3.at<float>(2), 3.14f), ATTO_TEXT("Third item should be 3.14"));

  l3.append(String(ATTO_TEXT("hello")));
  TEST(l3.length() == 4, ATTO_TEXT("Append String should increase length to 4"));
  TEST(l3.typeAt(3) == TYPE_STRING, ATTO_TEXT("Fourth item should be TYPE_STRING"));
  TEST(l3.at<String>(3).equals(String(ATTO_TEXT("hello"))),
       ATTO_TEXT("Fourth item should be 'hello'"));

  // Test chaining
  List l4;
  l4.append(1).append(2).append(3);
  TEST(l4.length() == 3, ATTO_TEXT("Chained append should work"));
  TEST(l4.at<int>(0) == 1, ATTO_TEXT("First chained item should be 1"));
  TEST(l4.at<int>(1) == 2, ATTO_TEXT("Second chained item should be 2"));
  TEST(l4.at<int>(2) == 3, ATTO_TEXT("Third chained item should be 3"));

  // Test prepend
  List l5;
  l5.prepend(3).prepend(2).prepend(1);
  TEST(l5.length() == 3, ATTO_TEXT("Prepend should work"));
  TEST(l5.at<int>(0) == 1, ATTO_TEXT("First prepended item should be 1"));
  TEST(l5.at<int>(1) == 2, ATTO_TEXT("Second prepended item should be 2"));
  TEST(l5.at<int>(2) == 3, ATTO_TEXT("Third prepended item should be 3"));

  // Test insert
  List l6;
  l6.append(1).append(3);
  l6.insert(1, 2);
  TEST(l6.length() == 3, ATTO_TEXT("Insert should increase length"));
  TEST(l6.at<int>(0) == 1, ATTO_TEXT("First item should be 1"));
  TEST(l6.at<int>(1) == 2, ATTO_TEXT("Inserted item should be 2"));
  TEST(l6.at<int>(2) == 3, ATTO_TEXT("Third item should be 3"));

  // Test insert with negative index (should prepend)
  List l7;
  l7.append(2).append(3);
  l7.insert(-1, 1);
  TEST(l7.length() == 3, ATTO_TEXT("Insert with negative index should prepend"));
  TEST(l7.at<int>(0) == 1, ATTO_TEXT("Negative insert should prepend to front"));

  // Test insert beyond end (should append)
  List l8;
  l8.append(1).append(2);
  l8.insert(100, 3);
  TEST(l8.length() == 3, ATTO_TEXT("Insert beyond end should append"));
  TEST(l8.at<int>(2) == 3, ATTO_TEXT("Beyond-end insert should append to end"));

  // Test operator[]
  List l9;
  l9.append(10).append(20).append(30);
  TEST(l9.operator[]<int>(0) == 10, ATTO_TEXT("operator[] should work for index 0"));
  TEST(l9.operator[]<int>(1) == 20, ATTO_TEXT("operator[] should work for index 1"));
  TEST(l9.operator[]<int>(2) == 30, ATTO_TEXT("operator[] should work for index 2"));

  // Test set
  List l10;
  l10.append(1).append(2).append(3);
  l10.set(1, 99);
  TEST(l10.at<int>(1) == 99, ATTO_TEXT("set() should update value"));
  TEST(l10.length() == 3, ATTO_TEXT("set() should not change length"));

  // Test set on empty list (should append)
  List l11;
  l11.set(0, 42);
  TEST(l11.length() == 1, ATTO_TEXT("set() on empty list should append"));
  TEST(l11.at<int>(0) == 42, ATTO_TEXT("set() on empty list should set value"));

  // Test remove
  List l12;
  l12.append(1).append(2).append(3);
  l12.remove(1);
  TEST(l12.length() == 2, ATTO_TEXT("remove() should decrease length"));
  TEST(l12.at<int>(0) == 1, ATTO_TEXT("First item should still be 1"));
  TEST(l12.at<int>(1) == 3, ATTO_TEXT("Second item should now be 3"));

  // Test pop
  List l13;
  l13.append(1).append(2).append(3);
  int popped = l13.pop<int>();
  TEST(popped == 3, ATTO_TEXT("pop() should return last value"));
  TEST(l13.length() == 2, ATTO_TEXT("pop() should decrease length"));
  TEST(l13.at<int>(1) == 2, ATTO_TEXT("Last remaining item should be 2"));

  // Test clear
  List l14;
  l14.append(1).append(2).append(3);
  l14.clear();
  TEST(l14.length() == 0, ATTO_TEXT("clear() should set length to 0"));
  TEST(l14.isEmpty(), ATTO_TEXT("clear() should make list empty"));

  // Test find with numeric coercion
  List l15;
  l15.append(1).append(2).append(3);
  TEST(l15.find(2) == 1, ATTO_TEXT("find() should find int value"));
  TEST(l15.find(2.0f) == 1,
       ATTO_TEXT("find() should find with numeric coercion (int == float)"));
  TEST(l15.find(99) == -1, ATTO_TEXT("find() should return -1 for not found"));

  // Test find with strings
  List l16;
  l16.append(String(ATTO_TEXT("a"))).append(String(ATTO_TEXT("b"))).append(String(ATTO_TEXT("c")));
  TEST(l16.find(String(ATTO_TEXT("b"))) == 1, ATTO_TEXT("find() should find String value"));
  TEST(l16.find(String(ATTO_TEXT("z"))) == -1,
       ATTO_TEXT("find() should return -1 for String not found"));

  // Test contains
  List l17;
  l17.append(10).append(20).append(30);
  TEST(l17.contains(20), ATTO_TEXT("contains() should return true for existing value"));
  TEST(!l17.contains(99),
       ATTO_TEXT("contains() should return false for non-existing value"));
  TEST(l17.contains(20.0f), ATTO_TEXT("contains() should work with numeric coercion"));

  // Test reverse
  List l18;
  l18.append(1).append(2).append(3);
  l18.reverse();
  TEST(l18.at<int>(0) == 3, ATTO_TEXT("reverse() should reverse order - first"));
  TEST(l18.at<int>(1) == 2, ATTO_TEXT("reverse() should reverse order - middle"));
  TEST(l18.at<int>(2) == 1, ATTO_TEXT("reverse() should reverse order - last"));

  // Test concat
  List l19;
  l19.append(1).append(2);
  List l20;
  l20.append(3).append(4);
  l19.concat(l20);
  TEST(l19.length() == 4, ATTO_TEXT("concat() should combine lists"));
  TEST(l19.at<int>(0) == 1, ATTO_TEXT("concat() first item"));
  TEST(l19.at<int>(1) == 2, ATTO_TEXT("concat() second item"));
  TEST(l19.at<int>(2) == 3, ATTO_TEXT("concat() third item"));
  TEST(l19.at<int>(3) == 4, ATTO_TEXT("concat() fourth item"));

  // Test slice
  List l21;
  l21.append(1).append(2).append(3).append(4).append(5);
  List sliced = l21.slice(1, 4);
  TEST(sliced.length() == 3, ATTO_TEXT("slice() should create correct length list"));
  TEST(sliced.at<int>(0) == 2, ATTO_TEXT("slice() first item should be correct"));
  TEST(sliced.at<int>(1) == 3, ATTO_TEXT("slice() second item should be correct"));
  TEST(sliced.at<int>(2) == 4, ATTO_TEXT("slice() third item should be correct"));

  // Test slice with negative start (should clamp to 0)
  List sliced2 = l21.slice(-1, 3);
  TEST(sliced2.length() == 3, ATTO_TEXT("slice() with negative start should clamp"));
  TEST(sliced2.at<int>(0) == 1, ATTO_TEXT("slice() with negative start - first item"));

  // Test slice with end beyond length (should clamp)
  List sliced3 = l21.slice(2, 100);
  TEST(sliced3.length() == 3, ATTO_TEXT("slice() with large end should clamp"));
  TEST(sliced3.at<int>(0) == 3, ATTO_TEXT("slice() with large end - first item"));

  // Test duplicate
  List l22;
  l22.append(1).append(2).append(3);
  List l23 = l22.duplicate();
  TEST(l23.length() == 3, ATTO_TEXT("duplicate() should copy length"));
  TEST(l23.at<int>(0) == 1, ATTO_TEXT("duplicate() should copy first item"));
  TEST(l23.at<int>(1) == 2, ATTO_TEXT("duplicate() should copy second item"));
  TEST(l23.at<int>(2) == 3, ATTO_TEXT("duplicate() should copy third item"));

  // Test copy constructor
  List l24;
  l24.append(10).append(20);
  List l25(l24);
  TEST(l25.length() == 2, ATTO_TEXT("Copy constructor should copy length"));
  TEST(l25.at<int>(0) == 10, ATTO_TEXT("Copy constructor should copy first item"));
  TEST(l25.at<int>(1) == 20, ATTO_TEXT("Copy constructor should copy second item"));

  // Test sort ascending with numbers
  List l26;
  l26.append(3).append(1).append(4).append(2);
  l26.sort(true);
  TEST(l26.at<int>(0) == 1, ATTO_TEXT("sort() ascending - first"));
  TEST(l26.at<int>(1) == 2, ATTO_TEXT("sort() ascending - second"));
  TEST(l26.at<int>(2) == 3, ATTO_TEXT("sort() ascending - third"));
  TEST(l26.at<int>(3) == 4, ATTO_TEXT("sort() ascending - fourth"));

  // Test sort descending
  List l27;
  l27.append(3).append(1).append(4).append(2);
  l27.sort(false);
  TEST(l27.at<int>(0) == 4, ATTO_TEXT("sort() descending - first"));
  TEST(l27.at<int>(1) == 3, ATTO_TEXT("sort() descending - second"));
  TEST(l27.at<int>(2) == 2, ATTO_TEXT("sort() descending - third"));
  TEST(l27.at<int>(3) == 1, ATTO_TEXT("sort() descending - fourth"));

  // Test sort with strings
  List l28;
  l28.append(String(ATTO_TEXT("charlie"))).append(String(ATTO_TEXT("alice"))).append(String(ATTO_TEXT("bob")));
  l28.sort(true);
  TEST(l28.at<String>(0).equals(String(ATTO_TEXT("alice"))), ATTO_TEXT("sort() strings - first"));
  TEST(l28.at<String>(1).equals(String(ATTO_TEXT("bob"))), ATTO_TEXT("sort() strings - second"));
  TEST(l28.at<String>(2).equals(String(ATTO_TEXT("charlie"))), ATTO_TEXT("sort() strings - third"));

  // Test sort with mixed numeric types (numeric coercion)
  List l29;
  l29.append(3.5f).append(1).append(2.2f).append(4);
  l29.sort(true);
  TEST(ApproxEqual(l29.at<float>(0), 1.0f), ATTO_TEXT("sort() mixed numeric - first"));
  TEST(ApproxEqual(l29.at<float>(1), 2.2f), ATTO_TEXT("sort() mixed numeric - second"));
  TEST(ApproxEqual(l29.at<float>(2), 3.5f), ATTO_TEXT("sort() mixed numeric - third"));
  TEST(ApproxEqual(l29.at<float>(3), 4.0f), ATTO_TEXT("sort() mixed numeric - fourth"));

  // Test nested lists
  List inner1;
  inner1.append(1).append(2);
  List inner2;
  inner2.append(3).append(4);
  List outer;
  outer.append(inner1).append(inner2);
  TEST(outer.length() == 2, ATTO_TEXT("Nested lists should work"));
  TEST(outer.typeAt(0) == TYPE_LIST, ATTO_TEXT("First nested item should be TYPE_LIST"));
  List retrieved = outer.at<List>(0);
  TEST(retrieved.length() == 2,
       ATTO_TEXT("Retrieved nested list should have correct length"));
  TEST(retrieved.at<int>(0) == 1, ATTO_TEXT("Nested list first item"));
  TEST(retrieved.at<int>(1) == 2, ATTO_TEXT("Nested list second item"));

  // Test typeAt with invalid index
  List l30;
  l30.append(1);
  TEST(l30.typeAt(0) == TYPE_INT, ATTO_TEXT("typeAt() should return correct type"));
  TEST(l30.typeAt(-1) == TYPE_INVALID,
       ATTO_TEXT("typeAt() with negative index should return TYPE_INVALID"));
  TEST(l30.typeAt(100) == TYPE_INVALID,
       ATTO_TEXT("typeAt() with large index should return TYPE_INVALID"));

  // Test edge case: empty list operations
  List empty;
  TEST(empty.length() == 0, ATTO_TEXT("Empty list length"));
  TEST(empty.isEmpty(), ATTO_TEXT("Empty list isEmpty"));
  TEST(empty.at<int>(0) == 0, ATTO_TEXT("Empty list at() should return default"));
  TEST(empty.find(1) == -1, ATTO_TEXT("Empty list find() should return -1"));
  TEST(!empty.contains(1), ATTO_TEXT("Empty list contains() should return false"));
  empty.reverse();
  TEST(empty.length() == 0, ATTO_TEXT("Empty list reverse() should not crash"));
  empty.sort();
  TEST(empty.length() == 0, ATTO_TEXT("Empty list sort() should not crash"));

  // Test with c-style strings
  List l32;
  l32.append(ATTO_TEXT("test"));
  TEST(l32.typeAt(0) == TYPE_STRING,
       ATTO_TEXT("C-string should be converted to TYPE_STRING"));
  TEST(l32.at<String>(0).equals(String(ATTO_TEXT("test"))),
       ATTO_TEXT("C-string should be stored as String"));

  // Test with wide strings
  List l33;
  l33.append(ATTO_TEXT("wide"));
  TEST(l33.typeAt(0) == TYPE_STRING,
       ATTO_TEXT("Wide string should be converted to TYPE_STRING"));
  TEST(l33.at<String>(0).equals(String(ATTO_TEXT("wide"))),
       ATTO_TEXT("Wide string should be stored as String"));

  // Report results
  if (errorCount == 0) {
    Log(ATTO_TEXT("All List tests passed!"));
    Exit(0);
  } else {
    LogError(errorCount, ATTO_TEXT(" test(s) failed"));
    Exit(errorCount);
  }
}
