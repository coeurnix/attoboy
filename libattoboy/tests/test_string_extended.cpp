#include "attoboy/attoboy.h"
#include <windows.h>

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError("Test failed: ", message);                                        \
    errorCount++;                                                              \
  }

void atto_main() {
  Log("Running extended String tests...");

  // Test String(nullptr) - should create "null"
  const char *nullChar = nullptr;
  const wchar_t *nullWchar = nullptr;
  String sNullChar(nullChar);
  String sNullWchar(nullWchar);
  TEST(sNullChar == "null", "String(nullptr char) should create \"null\"");
  TEST(sNullWchar == "null", "String(nullptr wchar_t) should create \"null\"");

  // Test lines()
  String sLines("line1\nline2\nline3");
  List lines = sLines.lines();
  TEST(lines.length() == 3, "lines() should split into 3 lines");
  TEST(lines.at<String>(0) == "line1", "First line should be \"line1\"");
  TEST(lines.at<String>(1) == "line2", "Second line should be \"line2\"");
  TEST(lines.at<String>(2) == "line3", "Third line should be \"line3\"");

  String sLinesWindows("line1\r\nline2\r\nline3");
  List linesWindows = sLinesWindows.lines();
  TEST(linesWindows.length() == 3,
       "lines() should handle Windows line endings");
  TEST(linesWindows.at<String>(0) == "line1", "First line should be \"line1\"");
  TEST(linesWindows.at<String>(1) == "line2",
       "Second line should be \"line2\"");

  String sLinesMixed("line1\nline2\r\nline3");
  List linesMixed = sLinesMixed.lines();
  TEST(linesMixed.length() == 3, "lines() should handle mixed line endings");

  String sLinesTrailing("line1\nline2\n");
  List linesTrailing = sLinesTrailing.lines();
  TEST(linesTrailing.length() == 3, "lines() should handle trailing newline");
  TEST(linesTrailing.at<String>(2).isEmpty(),
       "Last line after trailing newline should be empty");

  // Test join()
  List listToJoin;
  listToJoin.append("apple").append("banana").append("cherry");
  String joined = String(", ").join(listToJoin);
  TEST(joined == "apple, banana, cherry",
       "join() should join strings with separator");

  List listMixedTypes;
  listMixedTypes.append("value").append(42).append(true);
  String joinedMixed = String(" | ").join(listMixedTypes);
  TEST(joinedMixed.contains("value"), "join() should handle mixed types");
  TEST(joinedMixed.contains("42"), "join() should convert int to string");
  TEST(joinedMixed.contains("true"), "join() should convert bool to string");

  List emptyList;
  String joinedEmpty = String(", ").join(emptyList);
  TEST(joinedEmpty.isEmpty(),
       "join() with empty list should return empty string");

  // Test split(sep, max)
  String sSplit("a,b,c,d,e");
  List splitParts = sSplit.split(",", 2);
  TEST(splitParts.length() == 3, "split(sep, 2) should create 3 parts");
  TEST(splitParts.at<String>(0) == "a", "First part should be \"a\"");
  TEST(splitParts.at<String>(1) == "b", "Second part should be \"b\"");
  TEST(splitParts.at<String>(2) == "c,d,e", "Third part should be remainder");

  String sSplitOne("a,b,c");
  List splitOne = sSplitOne.split(",", 1);
  TEST(splitOne.length() == 2, "split(sep, 1) should create 2 parts");
  TEST(splitOne.at<String>(0) == "a", "First part should be \"a\"");
  TEST(splitOne.at<String>(1) == "b,c", "Second part should be remainder");

  String sSplitMulti("apple::banana::cherry");
  List splitMulti = sSplitMulti.split("::", 1);
  TEST(splitMulti.length() == 2, "split() should handle multi-char separator");
  TEST(splitMulti.at<String>(0) == "apple", "First part should be \"apple\"");
  TEST(splitMulti.at<String>(1) == "banana::cherry",
       "Remainder should contain remaining separators");

  // Test split() - whitespace
  String sWhitespace("  hello   world  test  ");
  List splitWs = sWhitespace.split();
  TEST(splitWs.length() == 3, "split() should split by whitespace");
  TEST(splitWs.at<String>(0) == "hello", "First word should be \"hello\"");
  TEST(splitWs.at<String>(1) == "world", "Second word should be \"world\"");
  TEST(splitWs.at<String>(2) == "test", "Third word should be \"test\"");

  String sTabs("tab\there\tthere");
  List splitTabs = sTabs.split();
  TEST(splitTabs.length() == 3, "split() should handle tabs");

  String sMultiWs("a  \t\n  b");
  List splitMultiWs = sMultiWs.split();
  TEST(splitMultiWs.length() == 2,
       "split() should treat consecutive whitespace as one");

  // Test format(list)
  List formatList;
  formatList.append("World").append(42);
  String formatted = String("Hello {0}! The answer is {1}.").format(formatList);
  TEST(formatted == "Hello World! The answer is 42.",
       "format(list) should substitute indexed placeholders");

  String formatOutOfBounds = String("{0} {5} {1}").format(formatList);
  TEST(formatOutOfBounds.contains("{5}"),
       "format(list) should leave out-of-bounds placeholders unchanged");

  // Test format(map)
  Map formatMap;
  formatMap.put("name", "Alice").put("age", 30);
  String formattedMap = String("Name: {name}, Age: {age}").format(formatMap);
  TEST(formattedMap == "Name: Alice, Age: 30",
       "format(map) should substitute keyed placeholders");

  String formatMissingKey = String("{name} {unknown}").format(formatMap);
  TEST(formatMissingKey.contains("{unknown}"),
       "format(map) should leave unknown keys unchanged");

  // Test String(list) - JSON
  List jsonList;
  jsonList.append("hello").append(42).append(true).append(3.14f);
  String jsonListStr(jsonList);
  TEST(jsonListStr.contains("\"hello\""), "String(list) should quote strings");
  TEST(jsonListStr.contains("42"), "String(list) should include integers");
  TEST(jsonListStr.contains("true"), "String(list) should include booleans");
  TEST(jsonListStr.contains("3.1"), "String(list) should include floats");
  TEST(jsonListStr.startsWith("["), "String(list) should start with [");
  TEST(jsonListStr.endsWith("]"), "String(list) should end with ]");

  List nestedList;
  List innerList;
  innerList.append(1).append(2);
  nestedList.append("outer").append(innerList);
  String jsonNested(nestedList);
  TEST(jsonNested.contains("[1,2]") || jsonNested.contains("[1,2]"),
       "String(list) should handle nested lists");

  // Test String(map) - JSON
  Map jsonMap;
  jsonMap.put("name", "Bob").put("age", 25).put("active", true);
  String jsonMapStr(jsonMap);
  TEST(jsonMapStr.contains("\"name\""), "String(map) should quote keys");
  TEST(jsonMapStr.contains("\"Bob\""),
       "String(map) should quote string values");
  TEST(jsonMapStr.contains("25"), "String(map) should include integer values");
  TEST(jsonMapStr.contains("true"),
       "String(map) should include boolean values");
  TEST(jsonMapStr.startsWith("{"), "String(map) should start with {");
  TEST(jsonMapStr.endsWith("}"), "String(map) should end with }");

  // Test JSON escaping
  List escapeList;
  escapeList.append("quote\"here").append("newline\nhere");
  String escapedJson(escapeList);
  TEST(escapedJson.contains("\\\""), "String(list) should escape quotes");
  TEST(escapedJson.contains("\\n"), "String(list) should escape newlines");

  // Test null handling in JSON
  List nullList;
  nullList.append(String());
  String jsonWithNull(nullList);
  // Empty String() might produce empty string or null depending on
  // implementation
  TEST(jsonWithNull.contains("\"\"") || jsonWithNull.contains("null"),
       "String(list) should handle null/empty values");

  // Report results
  if (errorCount == 0) {
    Log("All extended String tests passed!");
    Exit(0);
  } else {
    LogError(errorCount, " test(s) failed");
    Exit(errorCount);
  }
}
