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
  Log(ATTO_TEXT("Running extended String tests..."));

  // Test String(nullptr) - should create "null"
  const ATTO_CHAR *nullPtr = nullptr;
  String sNull(nullPtr);
  TEST(sNull == ATTO_TEXT("null"), ATTO_TEXT("String(nullptr) should create \"null\""));

  // Test lines()
  String sLines(ATTO_TEXT("line1\nline2\nline3"));
  List lines = sLines.lines();
  TEST(lines.length() == 3, ATTO_TEXT("lines() should split into 3 lines"));
  TEST(lines.at<String>(0) == ATTO_TEXT("line1"), ATTO_TEXT("First line should be \"line1\""));
  TEST(lines.at<String>(1) == ATTO_TEXT("line2"), ATTO_TEXT("Second line should be \"line2\""));
  TEST(lines.at<String>(2) == ATTO_TEXT("line3"), ATTO_TEXT("Third line should be \"line3\""));

  String sLinesWindows(ATTO_TEXT("line1\r\nline2\r\nline3"));
  List linesWindows = sLinesWindows.lines();
  TEST(linesWindows.length() == 3,
       ATTO_TEXT("lines() should handle Windows line endings"));
  TEST(linesWindows.at<String>(0) == ATTO_TEXT("line1"), ATTO_TEXT("First line should be \"line1\""));
  TEST(linesWindows.at<String>(1) == ATTO_TEXT("line2"),
       ATTO_TEXT("Second line should be \"line2\""));

  String sLinesMixed(ATTO_TEXT("line1\nline2\r\nline3"));
  List linesMixed = sLinesMixed.lines();
  TEST(linesMixed.length() == 3, ATTO_TEXT("lines() should handle mixed line endings"));

  String sLinesTrailing(ATTO_TEXT("line1\nline2\n"));
  List linesTrailing = sLinesTrailing.lines();
  TEST(linesTrailing.length() == 3, ATTO_TEXT("lines() should handle trailing newline"));
  TEST(linesTrailing.at<String>(2).isEmpty(),
       ATTO_TEXT("Last line after trailing newline should be empty"));

  // Test join()
  List listToJoin;
  listToJoin.append(ATTO_TEXT("apple")).append(ATTO_TEXT("banana")).append(ATTO_TEXT("cherry"));
  String joined = String(ATTO_TEXT(", ")).join(listToJoin);
  TEST(joined == ATTO_TEXT("apple, banana, cherry"),
       ATTO_TEXT("join() should join strings with separator"));

  List listMixedTypes;
  listMixedTypes.append(ATTO_TEXT("value")).append(42).append(true);
  String joinedMixed = String(ATTO_TEXT(" | ")).join(listMixedTypes);
  TEST(joinedMixed.contains(ATTO_TEXT("value")), ATTO_TEXT("join() should handle mixed types"));
  TEST(joinedMixed.contains(ATTO_TEXT("42")), ATTO_TEXT("join() should convert int to string"));
  TEST(joinedMixed.contains(ATTO_TEXT("true")), ATTO_TEXT("join() should convert bool to string"));

  List emptyList;
  String joinedEmpty = String(ATTO_TEXT(", ")).join(emptyList);
  TEST(joinedEmpty.isEmpty(),
       ATTO_TEXT("join() with empty list should return empty string"));

  // Test split(sep, max)
  String sSplit(ATTO_TEXT("a,b,c,d,e"));
  List splitParts = sSplit.split(ATTO_TEXT(","), 2);
  TEST(splitParts.length() == 3, ATTO_TEXT("split(sep, 2) should create 3 parts"));
  TEST(splitParts.at<String>(0) == ATTO_TEXT("a"), ATTO_TEXT("First part should be \"a\""));
  TEST(splitParts.at<String>(1) == ATTO_TEXT("b"), ATTO_TEXT("Second part should be \"b\""));
  TEST(splitParts.at<String>(2) == ATTO_TEXT("c,d,e"), ATTO_TEXT("Third part should be remainder"));

  String sSplitOne(ATTO_TEXT("a,b,c"));
  List splitOne = sSplitOne.split(ATTO_TEXT(","), 1);
  TEST(splitOne.length() == 2, ATTO_TEXT("split(sep, 1) should create 2 parts"));
  TEST(splitOne.at<String>(0) == ATTO_TEXT("a"), ATTO_TEXT("First part should be \"a\""));
  TEST(splitOne.at<String>(1) == ATTO_TEXT("b,c"), ATTO_TEXT("Second part should be remainder"));

  String sSplitMulti(ATTO_TEXT("apple::banana::cherry"));
  List splitMulti = sSplitMulti.split(ATTO_TEXT("::"), 1);
  TEST(splitMulti.length() == 2, ATTO_TEXT("split() should handle multi-char separator"));
  TEST(splitMulti.at<String>(0) == ATTO_TEXT("apple"), ATTO_TEXT("First part should be \"apple\""));
  TEST(splitMulti.at<String>(1) == ATTO_TEXT("banana::cherry"),
       ATTO_TEXT("Remainder should contain remaining separators"));

  // Test split() - whitespace
  String sWhitespace(ATTO_TEXT("  hello   world  test  "));
  List splitWs = sWhitespace.split();
  TEST(splitWs.length() == 3, ATTO_TEXT("split() should split by whitespace"));
  TEST(splitWs.at<String>(0) == ATTO_TEXT("hello"), ATTO_TEXT("First word should be \"hello\""));
  TEST(splitWs.at<String>(1) == ATTO_TEXT("world"), ATTO_TEXT("Second word should be \"world\""));
  TEST(splitWs.at<String>(2) == ATTO_TEXT("test"), ATTO_TEXT("Third word should be \"test\""));

  String sTabs(ATTO_TEXT("tab\there\tthere"));
  List splitTabs = sTabs.split();
  TEST(splitTabs.length() == 3, ATTO_TEXT("split() should handle tabs"));

  String sMultiWs(ATTO_TEXT("a  \t\n  b"));
  List splitMultiWs = sMultiWs.split();
  TEST(splitMultiWs.length() == 2,
       ATTO_TEXT("split() should treat consecutive whitespace as one"));

  // Test format(list)
  List formatList;
  formatList.append(ATTO_TEXT("World")).append(42);
  String formatted = String(ATTO_TEXT("Hello {0}! The answer is {1}.")).format(formatList);
  TEST(formatted == ATTO_TEXT("Hello World! The answer is 42."),
       ATTO_TEXT("format(list) should substitute indexed placeholders"));

  String formatOutOfBounds = String(ATTO_TEXT("{0} {5} {1}")).format(formatList);
  TEST(formatOutOfBounds.contains(ATTO_TEXT("{5}")),
       ATTO_TEXT("format(list) should leave out-of-bounds placeholders unchanged"));

  // Test format(map)
  Map formatMap;
  formatMap.put(ATTO_TEXT("name"), ATTO_TEXT("Alice")).put(ATTO_TEXT("age"), 30);
  String formattedMap = String(ATTO_TEXT("Name: {name}, Age: {age}")).format(formatMap);
  TEST(formattedMap == ATTO_TEXT("Name: Alice, Age: 30"),
       ATTO_TEXT("format(map) should substitute keyed placeholders"));

  String formatMissingKey = String(ATTO_TEXT("{name} {unknown}")).format(formatMap);
  TEST(formatMissingKey.contains(ATTO_TEXT("{unknown}")),
       ATTO_TEXT("format(map) should leave unknown keys unchanged"));

  // Test String(list) - JSON
  List jsonList;
  jsonList.append(ATTO_TEXT("hello")).append(42).append(true).append(3.14f);
  String jsonListStr(jsonList);
  TEST(jsonListStr.contains(ATTO_TEXT("\"hello\"")), ATTO_TEXT("String(list) should quote strings"));
  TEST(jsonListStr.contains(ATTO_TEXT("42")), ATTO_TEXT("String(list) should include integers"));
  TEST(jsonListStr.contains(ATTO_TEXT("true")), ATTO_TEXT("String(list) should include booleans"));
  TEST(jsonListStr.contains(ATTO_TEXT("3.1")), ATTO_TEXT("String(list) should include floats"));
  TEST(jsonListStr.startsWith(ATTO_TEXT("[")), ATTO_TEXT("String(list) should start with ["));
  TEST(jsonListStr.endsWith(ATTO_TEXT("]")), ATTO_TEXT("String(list) should end with ]"));

  List nestedList;
  List innerList;
  innerList.append(1).append(2);
  nestedList.append(ATTO_TEXT("outer")).append(innerList);
  String jsonNested(nestedList);
  TEST(jsonNested.contains(ATTO_TEXT("[1,2]")) || jsonNested.contains(ATTO_TEXT("[1,2]")),
       ATTO_TEXT("String(list) should handle nested lists"));

  // Test String(map) - JSON
  Map jsonMap;
  jsonMap.put(ATTO_TEXT("name"), ATTO_TEXT("Bob")).put(ATTO_TEXT("age"), 25).put(ATTO_TEXT("active"), true);
  String jsonMapStr(jsonMap);
  TEST(jsonMapStr.contains(ATTO_TEXT("\"name\"")), ATTO_TEXT("String(map) should quote keys"));
  TEST(jsonMapStr.contains(ATTO_TEXT("\"Bob\"")),
       ATTO_TEXT("String(map) should quote string values"));
  TEST(jsonMapStr.contains(ATTO_TEXT("25")), ATTO_TEXT("String(map) should include integer values"));
  TEST(jsonMapStr.contains(ATTO_TEXT("true")),
       ATTO_TEXT("String(map) should include boolean values"));
  TEST(jsonMapStr.startsWith(ATTO_TEXT("{")), ATTO_TEXT("String(map) should start with {"));
  TEST(jsonMapStr.endsWith(ATTO_TEXT("}")), ATTO_TEXT("String(map) should end with }"));

  // Test JSON escaping
  List escapeList;
  escapeList.append(ATTO_TEXT("quote\"here")).append(ATTO_TEXT("newline\nhere"));
  String escapedJson(escapeList);
  TEST(escapedJson.contains(ATTO_TEXT("\\\"")), ATTO_TEXT("String(list) should escape quotes"));
  TEST(escapedJson.contains(ATTO_TEXT("\\n")), ATTO_TEXT("String(list) should escape newlines"));

  // Test null handling in JSON
  List nullList;
  nullList.append(String());
  String jsonWithNull(nullList);
  // Empty String() might produce empty string or null depending on
  // implementation
  TEST(jsonWithNull.contains(ATTO_TEXT("\"\"")) || jsonWithNull.contains(ATTO_TEXT("null")),
       ATTO_TEXT("String(list) should handle null/empty values"));

  // Report results
  if (errorCount == 0) {
    Log(ATTO_TEXT("All extended String tests passed!"));
    Exit(0);
  } else {
    LogError(errorCount, ATTO_TEXT(" test(s) failed"));
    Exit(errorCount);
  }
}
