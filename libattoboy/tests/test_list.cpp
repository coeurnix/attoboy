#include "attoboy/attoboy.h"
#include <windows.h>

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError("Test failed: ", message);                                        \
    errorCount++;                                                              \
  }

// Helper for approximate floating point equality
static inline bool ApproxEqual(double a, double b, double epsilon = 0.0001) {
  double diff = a - b;
  if (diff < 0)
    diff = -diff;
  return diff < epsilon;
}

void atto_main() {
  Log("Running List tests...");

  // Test constructors
  List l1;
  TEST(l1.length() == 0, "Empty constructor should create list with length 0");
  TEST(l1.isEmpty(), "Empty list should return true for isEmpty()");

  List l2(16);
  TEST(l2.length() == 0,
       "Capacity constructor should create list with length 0");
  TEST(l2.isEmpty(), "Capacity list should return true for isEmpty()");

  // Test append with different types
  List l3;
  l3.append(true);
  TEST(l3.length() == 1, "Append bool should increase length to 1");
  TEST(l3.typeAt(0) == TYPE_BOOL, "First item should be TYPE_BOOL");
  TEST(l3.at<bool>(0) == true, "First item should be true");

  l3.append(42);
  TEST(l3.length() == 2, "Append int should increase length to 2");
  TEST(l3.typeAt(1) == TYPE_INT, "Second item should be TYPE_INT");
  TEST(l3.at<int>(1) == 42, "Second item should be 42");

  l3.append(3.14);
  TEST(l3.length() == 3, "Append double should increase length to 3");
  TEST(l3.typeAt(2) == TYPE_DOUBLE, "Third item should be TYPE_DOUBLE");
  TEST(ApproxEqual(l3.at<double>(2), 3.14), "Third item should be 3.14");

  l3.append(String("hello"));
  TEST(l3.length() == 4, "Append String should increase length to 4");
  TEST(l3.typeAt(3) == TYPE_STRING, "Fourth item should be TYPE_STRING");
  TEST(l3.at<String>(3).equals(String("hello")),
       "Fourth item should be 'hello'");

  // Test chaining
  List l4;
  l4.append(1).append(2).append(3);
  TEST(l4.length() == 3, "Chained append should work");
  TEST(l4.at<int>(0) == 1, "First chained item should be 1");
  TEST(l4.at<int>(1) == 2, "Second chained item should be 2");
  TEST(l4.at<int>(2) == 3, "Third chained item should be 3");

  // Test prepend
  List l5;
  l5.prepend(3).prepend(2).prepend(1);
  TEST(l5.length() == 3, "Prepend should work");
  TEST(l5.at<int>(0) == 1, "First prepended item should be 1");
  TEST(l5.at<int>(1) == 2, "Second prepended item should be 2");
  TEST(l5.at<int>(2) == 3, "Third prepended item should be 3");

  // Test insert
  List l6;
  l6.append(1).append(3);
  l6.insert(1, 2);
  TEST(l6.length() == 3, "Insert should increase length");
  TEST(l6.at<int>(0) == 1, "First item should be 1");
  TEST(l6.at<int>(1) == 2, "Inserted item should be 2");
  TEST(l6.at<int>(2) == 3, "Third item should be 3");

  // Test insert with negative index (should prepend)
  List l7;
  l7.append(2).append(3);
  l7.insert(-1, 1);
  TEST(l7.length() == 3, "Insert with negative index should prepend");
  TEST(l7.at<int>(0) == 1, "Negative insert should prepend to front");

  // Test insert beyond end (should append)
  List l8;
  l8.append(1).append(2);
  l8.insert(100, 3);
  TEST(l8.length() == 3, "Insert beyond end should append");
  TEST(l8.at<int>(2) == 3, "Beyond-end insert should append to end");

  // Test operator[]
  List l9;
  l9.append(10).append(20).append(30);
  TEST(l9.operator[]<int>(0) == 10, "operator[] should work for index 0");
  TEST(l9.operator[]<int>(1) == 20, "operator[] should work for index 1");
  TEST(l9.operator[]<int>(2) == 30, "operator[] should work for index 2");

  // Test set
  List l10;
  l10.append(1).append(2).append(3);
  l10.set(1, 99);
  TEST(l10.at<int>(1) == 99, "set() should update value");
  TEST(l10.length() == 3, "set() should not change length");

  // Test set on empty list (should append)
  List l11;
  l11.set(0, 42);
  TEST(l11.length() == 1, "set() on empty list should append");
  TEST(l11.at<int>(0) == 42, "set() on empty list should set value");

  // Test remove
  List l12;
  l12.append(1).append(2).append(3);
  l12.remove(1);
  TEST(l12.length() == 2, "remove() should decrease length");
  TEST(l12.at<int>(0) == 1, "First item should still be 1");
  TEST(l12.at<int>(1) == 3, "Second item should now be 3");

  // Test pop
  List l13;
  l13.append(1).append(2).append(3);
  int popped = l13.pop<int>();
  TEST(popped == 3, "pop() should return last value");
  TEST(l13.length() == 2, "pop() should decrease length");
  TEST(l13.at<int>(1) == 2, "Last remaining item should be 2");

  // Test clear
  List l14;
  l14.append(1).append(2).append(3);
  l14.clear();
  TEST(l14.length() == 0, "clear() should set length to 0");
  TEST(l14.isEmpty(), "clear() should make list empty");

  // Test find with numeric coercion
  List l15;
  l15.append(1).append(2).append(3);
  TEST(l15.find(2) == 1, "find() should find int value");
  TEST(l15.find(2.0) == 1,
       "find() should find with numeric coercion (int == double)");
  TEST(l15.find(99) == -1, "find() should return -1 for not found");

  // Test find with strings
  List l16;
  l16.append(String("a")).append(String("b")).append(String("c"));
  TEST(l16.find(String("b")) == 1, "find() should find String value");
  TEST(l16.find(String("z")) == -1,
       "find() should return -1 for String not found");

  // Test contains
  List l17;
  l17.append(10).append(20).append(30);
  TEST(l17.contains(20), "contains() should return true for existing value");
  TEST(!l17.contains(99),
       "contains() should return false for non-existing value");
  TEST(l17.contains(20.0), "contains() should work with numeric coercion");

  // Test reverse
  List l18;
  l18.append(1).append(2).append(3);
  l18.reverse();
  TEST(l18.at<int>(0) == 3, "reverse() should reverse order - first");
  TEST(l18.at<int>(1) == 2, "reverse() should reverse order - middle");
  TEST(l18.at<int>(2) == 1, "reverse() should reverse order - last");

  // Test concat
  List l19;
  l19.append(1).append(2);
  List l20;
  l20.append(3).append(4);
  l19.concat(l20);
  TEST(l19.length() == 4, "concat() should combine lists");
  TEST(l19.at<int>(0) == 1, "concat() first item");
  TEST(l19.at<int>(1) == 2, "concat() second item");
  TEST(l19.at<int>(2) == 3, "concat() third item");
  TEST(l19.at<int>(3) == 4, "concat() fourth item");

  // Test slice
  List l21;
  l21.append(1).append(2).append(3).append(4).append(5);
  List sliced = l21.slice(1, 4);
  TEST(sliced.length() == 3, "slice() should create correct length list");
  TEST(sliced.at<int>(0) == 2, "slice() first item should be correct");
  TEST(sliced.at<int>(1) == 3, "slice() second item should be correct");
  TEST(sliced.at<int>(2) == 4, "slice() third item should be correct");

  // Test slice with negative start (should clamp to 0)
  List sliced2 = l21.slice(-1, 3);
  TEST(sliced2.length() == 3, "slice() with negative start should clamp");
  TEST(sliced2.at<int>(0) == 1, "slice() with negative start - first item");

  // Test slice with end beyond length (should clamp)
  List sliced3 = l21.slice(2, 100);
  TEST(sliced3.length() == 3, "slice() with large end should clamp");
  TEST(sliced3.at<int>(0) == 3, "slice() with large end - first item");

  // Test duplicate
  List l22;
  l22.append(1).append(2).append(3);
  List l23 = l22.duplicate();
  TEST(l23.length() == 3, "duplicate() should copy length");
  TEST(l23.at<int>(0) == 1, "duplicate() should copy first item");
  TEST(l23.at<int>(1) == 2, "duplicate() should copy second item");
  TEST(l23.at<int>(2) == 3, "duplicate() should copy third item");

  // Test copy constructor
  List l24;
  l24.append(10).append(20);
  List l25(l24);
  TEST(l25.length() == 2, "Copy constructor should copy length");
  TEST(l25.at<int>(0) == 10, "Copy constructor should copy first item");
  TEST(l25.at<int>(1) == 20, "Copy constructor should copy second item");

  // Test sort ascending with numbers
  List l26;
  l26.append(3).append(1).append(4).append(2);
  l26.sort(true);
  TEST(l26.at<int>(0) == 1, "sort() ascending - first");
  TEST(l26.at<int>(1) == 2, "sort() ascending - second");
  TEST(l26.at<int>(2) == 3, "sort() ascending - third");
  TEST(l26.at<int>(3) == 4, "sort() ascending - fourth");

  // Test sort descending
  List l27;
  l27.append(3).append(1).append(4).append(2);
  l27.sort(false);
  TEST(l27.at<int>(0) == 4, "sort() descending - first");
  TEST(l27.at<int>(1) == 3, "sort() descending - second");
  TEST(l27.at<int>(2) == 2, "sort() descending - third");
  TEST(l27.at<int>(3) == 1, "sort() descending - fourth");

  // Test sort with strings
  List l28;
  l28.append(String("charlie")).append(String("alice")).append(String("bob"));
  l28.sort(true);
  TEST(l28.at<String>(0).equals(String("alice")), "sort() strings - first");
  TEST(l28.at<String>(1).equals(String("bob")), "sort() strings - second");
  TEST(l28.at<String>(2).equals(String("charlie")), "sort() strings - third");

  // Test sort with mixed numeric types (numeric coercion)
  List l29;
  l29.append(3.5).append(1).append(2.2).append(4);
  l29.sort(true);
  TEST(ApproxEqual(l29.at<double>(0), 1.0), "sort() mixed numeric - first");
  TEST(ApproxEqual(l29.at<double>(1), 2.2), "sort() mixed numeric - second");
  TEST(ApproxEqual(l29.at<double>(2), 3.5), "sort() mixed numeric - third");
  TEST(ApproxEqual(l29.at<double>(3), 4.0), "sort() mixed numeric - fourth");

  // Test nested lists
  List inner1;
  inner1.append(1).append(2);
  List inner2;
  inner2.append(3).append(4);
  List outer;
  outer.append(inner1).append(inner2);
  TEST(outer.length() == 2, "Nested lists should work");
  TEST(outer.typeAt(0) == TYPE_LIST, "First nested item should be TYPE_LIST");
  List retrieved = outer.at<List>(0);
  TEST(retrieved.length() == 2,
       "Retrieved nested list should have correct length");
  TEST(retrieved.at<int>(0) == 1, "Nested list first item");
  TEST(retrieved.at<int>(1) == 2, "Nested list second item");

  // Test typeAt with invalid index
  List l30;
  l30.append(1);
  TEST(l30.typeAt(0) == TYPE_INT, "typeAt() should return correct type");
  TEST(l30.typeAt(-1) == TYPE_INVALID,
       "typeAt() with negative index should return TYPE_INVALID");
  TEST(l30.typeAt(100) == TYPE_INVALID,
       "typeAt() with large index should return TYPE_INVALID");

  // Test edge case: empty list operations
  List empty;
  TEST(empty.length() == 0, "Empty list length");
  TEST(empty.isEmpty(), "Empty list isEmpty");
  TEST(empty.at<int>(0) == 0, "Empty list at() should return default");
  TEST(empty.find(1) == -1, "Empty list find() should return -1");
  TEST(!empty.contains(1), "Empty list contains() should return false");
  empty.reverse();
  TEST(empty.length() == 0, "Empty list reverse() should not crash");
  empty.sort();
  TEST(empty.length() == 0, "Empty list sort() should not crash");

  // Test long long type
  List l31;
  long long bigNum = 9223372036854775807LL;
  l31.append(bigNum);
  TEST(l31.typeAt(0) == TYPE_LONG_LONG,
       "Append long long should set TYPE_LONG_LONG");
  TEST(l31.at<long long>(0) == bigNum, "Long long should be stored correctly");

  // Test with c-style strings
  List l32;
  l32.append("test");
  TEST(l32.typeAt(0) == TYPE_STRING,
       "C-string should be converted to TYPE_STRING");
  TEST(l32.at<String>(0).equals(String("test")),
       "C-string should be stored as String");

  // Test with wide strings
  List l33;
  l33.append(L"wide");
  TEST(l33.typeAt(0) == TYPE_STRING,
       "Wide string should be converted to TYPE_STRING");
  TEST(l33.at<String>(0).equals(String(L"wide")),
       "Wide string should be stored as String");

  // Report results
  if (errorCount == 0) {
    Log("All List tests passed!");
    Exit(0);
  } else {
    LogError(errorCount, " test(s) failed");
    Exit(errorCount);
  }
}
