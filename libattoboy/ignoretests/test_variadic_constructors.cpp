#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError(ATTO_TEXT("Test failed: "), message);                                        \
    errorCount++;                                                              \
  }

void atto_main() {
  Log(ATTO_TEXT("Running variadic constructor and operator tests..."));

  // Test String variadic constructor
  String s1(ATTO_TEXT("Hello"), ATTO_TEXT(" "), ATTO_TEXT("World"));
  TEST(s1 == ATTO_TEXT("Hello World"), ATTO_TEXT("String variadic constructor (3 args)"));

  String s2(ATTO_TEXT("The"), ATTO_TEXT(" answer"), ATTO_TEXT(" is"), ATTO_TEXT(" "), 42);
  TEST(s2 == ATTO_TEXT("The answer is 42"), ATTO_TEXT("String variadic constructor with int"));

  String s3(1, 2, 3);
  TEST(s3 == ATTO_TEXT("123"), ATTO_TEXT("String variadic constructor with multiple ints"));

  // Test List variadic constructor
  List list1(1, 2, 3, 4, 5);
  TEST(list1.length() == 5, ATTO_TEXT("List variadic constructor length"));
  TEST(list1.at<int>(0) == 1, ATTO_TEXT("List variadic [0]"));
  TEST(list1.at<int>(4) == 5, ATTO_TEXT("List variadic [4]"));

  List list2(ATTO_TEXT("hello"), 42, true, 3.14f);
  TEST(list2.length() == 4, ATTO_TEXT("List variadic mixed types length"));
  TEST(list2.at<String>(0) == ATTO_TEXT("hello"), ATTO_TEXT("List variadic mixed types [0]"));
  TEST(list2.at<int>(1) == 42, ATTO_TEXT("List variadic mixed types [1]"));
  TEST(list2.at<bool>(2) == true, ATTO_TEXT("List variadic mixed types [2]"));

  // Test Set variadic constructor
  Set set1(1, 2, 3, 4, 5);
  TEST(set1.length() == 5, ATTO_TEXT("Set variadic constructor length"));
  TEST(set1.contains<int>(1), ATTO_TEXT("Set variadic contains 1"));
  TEST(set1.contains<int>(5), ATTO_TEXT("Set variadic contains 5"));

  Set set2(1, 2, 2, 3, 3, 3);
  TEST(set2.length() == 3, ATTO_TEXT("Set variadic removes duplicates"));
  TEST(set2.contains<int>(1), ATTO_TEXT("Set variadic dedupe contains 1"));
  TEST(set2.contains<int>(2), ATTO_TEXT("Set variadic dedupe contains 2"));
  TEST(set2.contains<int>(3), ATTO_TEXT("Set variadic dedupe contains 3"));

  // Test Map variadic constructor
  Map map1(ATTO_TEXT("name"), ATTO_TEXT("Alice"), ATTO_TEXT("age"), 30, ATTO_TEXT("active"), true);
  TEST(map1.length() == 3, ATTO_TEXT("Map variadic constructor length"));
  TEST((map1.get<String, String>(ATTO_TEXT("name")) == ATTO_TEXT("Alice")), ATTO_TEXT("Map variadic get name"));
  TEST((map1.get<String, int>(ATTO_TEXT("age")) == 30), ATTO_TEXT("Map variadic get age"));
  TEST((map1.get<String, bool>(ATTO_TEXT("active")) == true), ATTO_TEXT("Map variadic get active"));

  Map map2(1, ATTO_TEXT("one"), 2, ATTO_TEXT("two"), 3);
  TEST(map2.length() == 3, ATTO_TEXT("Map variadic odd args length"));
  TEST((map2.get<int, String>(1) == ATTO_TEXT("one")), ATTO_TEXT("Map variadic odd args [1]"));
  TEST((map2.get<int, String>(2) == ATTO_TEXT("two")), ATTO_TEXT("Map variadic odd args [2]"));

  // Test List + operator
  List list3;
  list3.append(1).append(2);
  List list4 = list3 + 3;
  TEST(list4.length() == 3, ATTO_TEXT("List + operator length"));
  TEST(list4.at<int>(0) == 1, ATTO_TEXT("List + operator [0]"));
  TEST(list4.at<int>(1) == 2, ATTO_TEXT("List + operator [1]"));
  TEST(list4.at<int>(2) == 3, ATTO_TEXT("List + operator [2]"));

  List list5 = list4 + ATTO_TEXT("hello");
  TEST(list5.length() == 4, ATTO_TEXT("List + string length"));
  TEST(list5.at<String>(3) == ATTO_TEXT("hello"), ATTO_TEXT("List + string value"));

  // Test Set + operator
  Set set3;
  set3.put(1).put(2);
  Set set4 = set3 + 3;
  TEST(set4.length() == 3, ATTO_TEXT("Set + operator length"));
  TEST(set4.contains<int>(1), ATTO_TEXT("Set + operator contains 1"));
  TEST(set4.contains<int>(2), ATTO_TEXT("Set + operator contains 2"));
  TEST(set4.contains<int>(3), ATTO_TEXT("Set + operator contains 3"));

  Set set5 = set4 + 2;
  TEST(set5.length() == 3, ATTO_TEXT("Set + duplicate length"));

  // Test List compare and operators
  List listA;
  listA.append(1).append(2).append(3);
  List listB;
  listB.append(1).append(2).append(3);
  List listC;
  listC.append(1).append(2).append(4);
  List listD;
  listD.append(1).append(2);

  TEST(listA.compare(listB), ATTO_TEXT("List compare equal"));
  TEST(!listA.compare(listC), ATTO_TEXT("List compare different values"));
  TEST(!listA.compare(listD), ATTO_TEXT("List compare different length"));
  TEST(listA == listB, ATTO_TEXT("List == operator"));
  TEST(listA != listC, ATTO_TEXT("List != operator"));

  // Test Set compare and operators
  Set setA;
  setA.put(1).put(2).put(3);
  Set setB;
  setB.put(3).put(1).put(2);
  Set setC;
  setC.put(1).put(2).put(4);
  Set setD;
  setD.put(1).put(2);

  TEST(setA.compare(setB), ATTO_TEXT("Set compare equal (different order)"));
  TEST(!setA.compare(setC), ATTO_TEXT("Set compare different values"));
  TEST(!setA.compare(setD), ATTO_TEXT("Set compare different length"));
  TEST(setA == setB, ATTO_TEXT("Set == operator"));
  TEST(setA != setC, ATTO_TEXT("Set != operator"));

  // Test Map compare and operators
  Map mapA;
  mapA.put(ATTO_TEXT("a"), 1).put(ATTO_TEXT("b"), 2);
  Map mapB;
  mapB.put(ATTO_TEXT("a"), 1).put(ATTO_TEXT("b"), 2);
  Map mapC;
  mapC.put(ATTO_TEXT("a"), 1).put(ATTO_TEXT("b"), 3);
  Map mapD;
  mapD.put(ATTO_TEXT("a"), 1);

  TEST(mapA.compare(mapB), ATTO_TEXT("Map compare equal"));
  TEST(!mapA.compare(mapC), ATTO_TEXT("Map compare different values"));
  TEST(!mapA.compare(mapD), ATTO_TEXT("Map compare different length"));
  TEST(mapA == mapB, ATTO_TEXT("Map == operator"));
  TEST(mapA != mapC, ATTO_TEXT("Map != operator"));

  // Report results
  if (errorCount == 0) {
    Log(ATTO_TEXT("All variadic constructor and operator tests passed!"));
    Exit(0);
  } else {
    LogError(errorCount, ATTO_TEXT(" test(s) failed"));
    Exit(errorCount);
  }
}
