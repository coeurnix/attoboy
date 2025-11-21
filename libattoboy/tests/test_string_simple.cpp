#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError(ATTO_TEXT("Test failed: "), message);                                        \
    errorCount++;                                                              \
  }

void atto_main() {
  Log(ATTO_TEXT("Running String simple tests..."));

  // Test nullptr handling
  const char *nullPtr = nullptr;
  String sNull(nullPtr);
  TEST(sNull.equals(ATTO_TEXT("null")), ATTO_TEXT("String(nullptr) should create 'null' string"));

  // Test lines()
  String sLines(ATTO_TEXT("a\nb\nc"));
  List lines = sLines.lines();
  TEST(lines.length() == 3, ATTO_TEXT("lines() should split string into 3 parts"));

  // Test split()
  String sSplit(ATTO_TEXT("hello world"));
  List parts = sSplit.split();
  TEST(parts.length() == 2, ATTO_TEXT("split() should create 2 parts"));
  TEST(parts.at<String>(0) == ATTO_TEXT("hello"), ATTO_TEXT("first part should be 'hello'"));
  TEST(parts.at<String>(1) == ATTO_TEXT("world"), ATTO_TEXT("second part should be 'world'"));

  // Test join()
  List list;
  list.append(ATTO_TEXT("a")).append(ATTO_TEXT("b")).append(ATTO_TEXT("c"));
  String joined = String(ATTO_TEXT(",")).join(list);
  TEST(joined == ATTO_TEXT("a,b,c"), ATTO_TEXT("join() should concatenate with commas"));

  // Test format()
  List fmtList;
  fmtList.append(ATTO_TEXT("World"));
  String formatted = String(ATTO_TEXT("Hello {0}!")).format(fmtList);
  TEST(formatted == ATTO_TEXT("Hello World!"), ATTO_TEXT("format() should replace {0} with value"));

  // Test JSON List
  List jsonList;
  jsonList.append(ATTO_TEXT("test")).append(42);
  String json(jsonList);
  TEST(json.startsWith(ATTO_TEXT("[")), ATTO_TEXT("JSON string should start with '['"));
  TEST(json.contains(ATTO_TEXT("\"test\"")), ATTO_TEXT("JSON should contain quoted 'test'"));
  TEST(json.contains(ATTO_TEXT("42")), ATTO_TEXT("JSON should contain number 42"));

  // Test JSON Set
  Set jsonSet;
  jsonSet.put(1).put(2).put(3);
  String jsonSetStr(jsonSet);
  TEST(jsonSetStr.startsWith(ATTO_TEXT("[")), ATTO_TEXT("JSON set string should start with '['"));
  TEST(jsonSetStr.endsWith(ATTO_TEXT("]")), ATTO_TEXT("JSON set string should end with ']'"));
  TEST(jsonSetStr.contains(ATTO_TEXT("1")), ATTO_TEXT("JSON set should contain 1"));
  TEST(jsonSetStr.contains(ATTO_TEXT("2")), ATTO_TEXT("JSON set should contain 2"));
  TEST(jsonSetStr.contains(ATTO_TEXT("3")), ATTO_TEXT("JSON set should contain 3"));

  // Report results
  if (errorCount == 0) {
    Log(ATTO_TEXT("All simple tests passed!"));
    Exit(0);
  } else {
    LogError(errorCount, ATTO_TEXT(" test(s) failed"));
    Exit(errorCount);
  }
}
