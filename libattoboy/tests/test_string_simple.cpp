#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError("Test failed: ", message);                                        \
    errorCount++;                                                              \
  }

void atto_main() {
  Log("Running String simple tests...");

  // Test nullptr handling
  const char *nullPtr = nullptr;
  String sNull(nullPtr);
  TEST(sNull.equals("null"), "String(nullptr) should create 'null' string");

  // Test lines()
  String sLines("a\nb\nc");
  List lines = sLines.lines();
  TEST(lines.length() == 3, "lines() should split string into 3 parts");

  // Test split()
  String sSplit("hello world");
  List parts = sSplit.split();
  TEST(parts.length() == 2, "split() should create 2 parts");
  TEST(parts.at<String>(0) == "hello", "first part should be 'hello'");
  TEST(parts.at<String>(1) == "world", "second part should be 'world'");

  // Test join()
  List list;
  list.append("a").append("b").append("c");
  String joined = String(",").join(list);
  TEST(joined == "a,b,c", "join() should concatenate with commas");

  // Test format()
  List fmtList;
  fmtList.append("World");
  String formatted = String("Hello {0}!").format(fmtList);
  TEST(formatted == "Hello World!", "format() should replace {0} with value");

  // Test JSON List
  List jsonList;
  jsonList.append("test").append(42);
  String json(jsonList);
  TEST(json.startsWith("["), "JSON string should start with '['");
  TEST(json.contains("\"test\""), "JSON should contain quoted 'test'");
  TEST(json.contains("42"), "JSON should contain number 42");

  // Test JSON Set
  Set jsonSet;
  jsonSet.put(1).put(2).put(3);
  String jsonSetStr(jsonSet);
  TEST(jsonSetStr.startsWith("["), "JSON set string should start with '['");
  TEST(jsonSetStr.endsWith("]"), "JSON set string should end with ']'");
  TEST(jsonSetStr.contains("1"), "JSON set should contain 1");
  TEST(jsonSetStr.contains("2"), "JSON set should contain 2");
  TEST(jsonSetStr.contains("3"), "JSON set should contain 3");

  // Report results
  if (errorCount == 0) {
    Log("All simple tests passed!");
    Exit(0);
  } else {
    LogError(errorCount, " test(s) failed");
    Exit(errorCount);
  }
}
