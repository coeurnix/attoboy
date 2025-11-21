#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError("Test failed: ", message);                                        \
    errorCount++;                                                              \
  }

void atto_main() {
  Log("Running variadic constructor and operator tests...");

  // Test String variadic constructor
  String s1("Hello", " ", "World");
  TEST(s1 == "Hello World", "String variadic constructor (3 args)");

  String s2("The", " answer", " is", " ", 42);
  TEST(s2 == "The answer is 42", "String variadic constructor with int");

  String s3(1, 2, 3);
  TEST(s3 == "123", "String variadic constructor with multiple ints");

  // Test List variadic constructor
  List list1(1, 2, 3, 4, 5);
  TEST(list1.length() == 5, "List variadic constructor length");
  TEST(list1.at<int>(0) == 1, "List variadic [0]");
  TEST(list1.at<int>(4) == 5, "List variadic [4]");

  List list2("hello", 42, true, 3.14f);
  TEST(list2.length() == 4, "List variadic mixed types length");
  TEST(list2.at<String>(0) == "hello", "List variadic mixed types [0]");
  TEST(list2.at<int>(1) == 42, "List variadic mixed types [1]");
  TEST(list2.at<bool>(2) == true, "List variadic mixed types [2]");

  // Test Set variadic constructor
  Set set1(1, 2, 3, 4, 5);
  TEST(set1.length() == 5, "Set variadic constructor length");
  TEST(set1.contains<int>(1), "Set variadic contains 1");
  TEST(set1.contains<int>(5), "Set variadic contains 5");

  Set set2(1, 2, 2, 3, 3, 3);
  TEST(set2.length() == 3, "Set variadic removes duplicates");
  TEST(set2.contains<int>(1), "Set variadic dedupe contains 1");
  TEST(set2.contains<int>(2), "Set variadic dedupe contains 2");
  TEST(set2.contains<int>(3), "Set variadic dedupe contains 3");

  // Test Map variadic constructor
  Map map1("name", "Alice", "age", 30, "active", true);
  TEST(map1.length() == 3, "Map variadic constructor length");
  TEST((map1.get<String, String>("name") == "Alice"), "Map variadic get name");
  TEST((map1.get<String, int>("age") == 30), "Map variadic get age");
  TEST((map1.get<String, bool>("active") == true), "Map variadic get active");

  Map map2(1, "one", 2, "two", 3);
  TEST(map2.length() == 3, "Map variadic odd args length");
  TEST((map2.get<int, String>(1) == "one"), "Map variadic odd args [1]");
  TEST((map2.get<int, String>(2) == "two"), "Map variadic odd args [2]");

  // Test List + operator
  List list3;
  list3.append(1).append(2);
  List list4 = list3 + 3;
  TEST(list4.length() == 3, "List + operator length");
  TEST(list4.at<int>(0) == 1, "List + operator [0]");
  TEST(list4.at<int>(1) == 2, "List + operator [1]");
  TEST(list4.at<int>(2) == 3, "List + operator [2]");

  List list5 = list4 + "hello";
  TEST(list5.length() == 4, "List + string length");
  TEST(list5.at<String>(3) == "hello", "List + string value");

  // Test Set + operator
  Set set3;
  set3.put(1).put(2);
  Set set4 = set3 + 3;
  TEST(set4.length() == 3, "Set + operator length");
  TEST(set4.contains<int>(1), "Set + operator contains 1");
  TEST(set4.contains<int>(2), "Set + operator contains 2");
  TEST(set4.contains<int>(3), "Set + operator contains 3");

  Set set5 = set4 + 2;
  TEST(set5.length() == 3, "Set + duplicate length");

  // Test List compare and operators
  List listA;
  listA.append(1).append(2).append(3);
  List listB;
  listB.append(1).append(2).append(3);
  List listC;
  listC.append(1).append(2).append(4);
  List listD;
  listD.append(1).append(2);

  TEST(listA.compare(listB), "List compare equal");
  TEST(!listA.compare(listC), "List compare different values");
  TEST(!listA.compare(listD), "List compare different length");
  TEST(listA == listB, "List == operator");
  TEST(listA != listC, "List != operator");

  // Test Set compare and operators
  Set setA;
  setA.put(1).put(2).put(3);
  Set setB;
  setB.put(3).put(1).put(2);
  Set setC;
  setC.put(1).put(2).put(4);
  Set setD;
  setD.put(1).put(2);

  TEST(setA.compare(setB), "Set compare equal (different order)");
  TEST(!setA.compare(setC), "Set compare different values");
  TEST(!setA.compare(setD), "Set compare different length");
  TEST(setA == setB, "Set == operator");
  TEST(setA != setC, "Set != operator");

  // Test Map compare and operators
  Map mapA;
  mapA.put("a", 1).put("b", 2);
  Map mapB;
  mapB.put("a", 1).put("b", 2);
  Map mapC;
  mapC.put("a", 1).put("b", 3);
  Map mapD;
  mapD.put("a", 1);

  TEST(mapA.compare(mapB), "Map compare equal");
  TEST(!mapA.compare(mapC), "Map compare different values");
  TEST(!mapA.compare(mapD), "Map compare different length");
  TEST(mapA == mapB, "Map == operator");
  TEST(mapA != mapC, "Map != operator");

  // Report results
  if (errorCount == 0) {
    Log("All variadic constructor and operator tests passed!");
    Exit(0);
  } else {
    LogError(errorCount, " test(s) failed");
    Exit(errorCount);
  }
}
