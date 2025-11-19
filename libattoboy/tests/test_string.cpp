#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError("Test failed: ", message);                                        \
    errorCount++;                                                              \
  }

void atto_main() {
  Log("Running String tests...");

  // Constructors
  String s1;
  TEST(s1.length() == 0, "Empty constructor should create string with length 0");
  TEST(s1.isEmpty(), "Empty string should return true for isEmpty()");

  String s2("Hello");
  TEST(s2.length() == 5, "String(\"Hello\") should have length 5");

  String s3(s2);
  TEST(s3.length() == 5, "Copy constructor should preserve length");
  TEST(s3 == s2, "Copy constructor should create equal string");

  String sBool(true);
  TEST(sBool.length() == 4, "String(true) should have length 4");
  TEST(sBool == "true", "String(true) should equal \"true\"");

  String sInt(123);
  TEST(sInt.length() == 3, "String(123) should have length 3");
  TEST(sInt == "123", "String(123) should equal \"123\"");

  String sLong(1234567890LL);
  TEST(sLong.length() == 10, "String(1234567890LL) should have length 10");

  String sDouble(123.456);
  TEST(sDouble.length() >= 7, "String(123.456) should have length >= 7");
  TEST(sDouble.startsWith("123."), "String(123.456) should start with \"123.\"");

  String sWide(L"Wide String");
  TEST(sWide.length() == 11, "String(L\"Wide String\") should have length 11");
  TEST(sWide.startsWith(L"Wide"), "Wide string should start with \"Wide\"");

  // Basic info methods
  TEST(!s2.isEmpty(), "\"Hello\" should not be empty");
  TEST(s2.length() == 5, "\"Hello\".length() should be 5");

  // Character and substring access
  String c = s2.at(0);
  TEST(c.length() == 1, "at(0) should return single character");

  String lastChar = s2.at(-1);
  TEST(lastChar.length() == 1, "at(-1) should return single character");

  String sub = s2.substring(1, 4);
  TEST(sub.length() == 3, "substring(1, 4) of \"Hello\" should have length 3");
  TEST(sub == "ell", "substring(1, 4) should equal \"ell\"");

  String sub2 = s2.substring(1, -1);
  TEST(sub2.length() == 4, "substring(1, -1) should have length 4");
  TEST(sub2 == "ello", "substring(1, -1) should equal \"ello\"");

  String sub3 = s2.substring(1, -2);
  TEST(sub3.length() == 2, "substring(1, -2) should have length 2");
  TEST(sub3 == "el", "substring(1, -2) should equal \"el\"");

  String sDup = s2.duplicate();
  TEST(sDup == s2, "duplicate() should create equal string");
  TEST(sDup.length() == s2.length(), "duplicate() should preserve length");

  // Query operations
  TEST(s2.contains("ell"), "\"Hello\" should contain \"ell\"");
  TEST(!s2.contains("xyz"), "\"Hello\" should not contain \"xyz\"");

  TEST(s2.startsWith("H"), "\"Hello\" should start with \"H\"");
  TEST(s2.startsWith("Hel"), "\"Hello\" should start with \"Hel\"");
  TEST(!s2.startsWith("h"), "\"Hello\" should not start with \"h\" (case-sensitive)");

  TEST(s2.endsWith("o"), "\"Hello\" should end with \"o\"");
  TEST(s2.endsWith("llo"), "\"Hello\" should end with \"llo\"");
  TEST(!s2.endsWith("O"), "\"Hello\" should not end with \"O\" (case-sensitive)");

  String banana("banana");
  TEST(banana.count("a") == 3, "\"banana\" should contain 3 occurrences of \"a\"");
  TEST(banana.count("an") == 2, "\"banana\" should contain 2 occurrences of \"an\"");
  TEST(banana.count("x") == 0, "\"banana\" should contain 0 occurrences of \"x\"");

  // Number validation and conversion
  String sNum("123");
  TEST(sNum.isNumber(), "\"123\" should be a valid number");
  TEST(sNum.toInteger() == 123, "\"123\".toInteger() should be 123");
  TEST(sNum.toDouble() == 123.0, "\"123\".toDouble() should be 123.0");

  String sFloat("45.67");
  TEST(sFloat.isNumber(), "\"45.67\" should be a valid number");
  TEST(sFloat.toDouble() > 45.6 && sFloat.toDouble() < 45.7, "\"45.67\".toDouble() should be approximately 45.67");

  String sNotNum("abc");
  TEST(!sNotNum.isNumber(), "\"abc\" should not be a valid number");
  TEST(sNotNum.toInteger() == 0, "\"abc\".toInteger() should be 0");

  String sBool1("true");
  TEST(sBool1.toBool(), "\"true\".toBool() should be true");

  String sBool2("1");
  TEST(sBool2.toBool(), "\"1\".toBool() should be true");

  String sBool3("yes");
  TEST(sBool3.toBool(), "\"yes\".toBool() should be true");

  String sBool4("false");
  TEST(!sBool4.toBool(), "\"false\".toBool() should be false");

  // Modification operations - append
  String sApp("A");
  sApp.append("B");
  TEST(sApp == "AB", "append(\"B\") should create \"AB\"");
  TEST(sApp.length() == 2, "After append, length should be 2");

  // Modification operations - prepend
  String sPre("B");
  sPre.prepend("A");
  TEST(sPre == "AB", "prepend(\"A\") should create \"AB\"");
  TEST(sPre.length() == 2, "After prepend, length should be 2");

  // Modification operations - insert
  String sIns("AC");
  sIns.insert(1, "B");
  TEST(sIns == "ABC", "insert(1, \"B\") should create \"ABC\"");
  TEST(sIns.length() == 3, "After insert, length should be 3");

  // Modification operations - remove
  String sRem("ABCD");
  sRem.remove(1, 3);
  TEST(sRem == "AD", "remove(1, 3) should leave \"AD\"");
  TEST(sRem.length() == 2, "After remove, length should be 2");

  // Modification operations - replace
  String sRep("banana");
  sRep.replace("a", "o");
  TEST(sRep == "bonono", "replace(\"a\", \"o\") should create \"bonono\"");
  TEST(!sRep.contains("a"), "After replace, should not contain \"a\"");
  TEST(sRep.contains("o"), "After replace, should contain \"o\"");

  // Modification operations - trim
  String sTrim("  test  ");
  sTrim.trim();
  TEST(sTrim == "test", "trim() should remove whitespace");
  TEST(sTrim.length() == 4, "After trim, length should be 4");

  String sTrim2("test");
  sTrim2.trim();
  TEST(sTrim2 == "test", "trim() on string without whitespace should be unchanged");

  // Modification operations - upper/lower
  String sCase("HeLLo");
  sCase.lower();
  TEST(sCase == "hello", "lower() should convert to lowercase");
  TEST(!sCase.contains("L"), "After lower(), should not contain uppercase 'L'");

  sCase.upper();
  TEST(sCase == "HELLO", "upper() should convert to uppercase");
  TEST(!sCase.contains("e"), "After upper(), should not contain lowercase 'e'");

  // Modification operations - reverse
  String sRev("ABC");
  sRev.reverse();
  TEST(sRev == "CBA", "reverse() should reverse the string");
  TEST(sRev.startsWith("C"), "Reversed \"ABC\" should start with 'C'");
  TEST(sRev.endsWith("A"), "Reversed \"ABC\" should end with 'A'");

  // Modification operations - repeat
  String sRpt("A");
  sRpt.repeat(3);
  TEST(sRpt == "AAA", "repeat(3) should create \"AAA\"");
  TEST(sRpt.length() == 3, "After repeat(3), length should be 3");

  String sRpt2("AB");
  sRpt2.repeat(2);
  TEST(sRpt2 == "ABAB", "repeat(2) on \"AB\" should create \"ABAB\"");

  // Chaining
  String sChain("  hello  ");
  sChain.trim().upper().append("!");
  TEST(sChain == "HELLO!", "Chaining trim().upper().append() should work");

  // Equality and comparison
  String sEq1("Test");
  String sEq2("Test");
  String sEq3("Other");

  TEST(sEq1.equals(sEq2), "equals() should return true for equal strings");
  TEST(!sEq1.equals(sEq3), "equals() should return false for different strings");

  TEST(sEq1 == sEq2, "operator== should return true for equal strings");
  TEST(!(sEq1 == sEq3), "operator== should return false for different strings");

  TEST(!(sEq1 != sEq2), "operator!= should return false for equal strings");
  TEST(sEq1 != sEq3, "operator!= should return true for different strings");

  // Hash
  TEST(sEq1.hash() == sEq2.hash(), "Equal strings should have equal hashes");
  TEST(String().hash() == 0, "Empty string hash should be 0");

  // Integer promotion
  int iVal = 42;
  String sPromoted(iVal);
  TEST(sPromoted == "42", "String(int) should create \"42\"");
  TEST(sPromoted.length() == 2, "String(42) should have length 2");

  short shVal = 100;
  String sShort(shVal);
  TEST(sShort == "100", "String(short) should create \"100\"");

  // Report results
  if (errorCount == 0) {
    Log("All String tests passed!");
    Exit(0);
  } else {
    LogError(errorCount, " test(s) failed");
    Exit(1);
  }
}
