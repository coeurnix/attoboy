#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError(ATTO_TEXT("Test failed: "), message);                                        \
    errorCount++;                                                              \
  }

void atto_main() {
  Log(ATTO_TEXT("Running String tests..."));

  // Constructors
  String s1;
  TEST(s1.length() == 0,
       ATTO_TEXT("Empty constructor should create string with length 0"));
  TEST(s1.isEmpty(), ATTO_TEXT("Empty string should return true for isEmpty()"));

  String s2(ATTO_TEXT("Hello"));
  TEST(s2.length() == 5, ATTO_TEXT("String(\"Hello\") should have length 5"));

  String s3(s2);
  TEST(s3.length() == 5, ATTO_TEXT("Copy constructor should preserve length"));
  TEST(s3 == s2, ATTO_TEXT("Copy constructor should create equal string"));

  String sBool(true);
  TEST(sBool.length() == 4, ATTO_TEXT("String(true) should have length 4"));
  TEST(sBool == ATTO_TEXT("true"), ATTO_TEXT("String(true) should equal \"true\""));

  String sInt(123);
  TEST(sInt.length() == 3, ATTO_TEXT("String(123) should have length 3"));
  TEST(sInt == ATTO_TEXT("123"), ATTO_TEXT("String(123) should equal \"123\""));

  String sLong(1234567890);
  TEST(sLong.length() == 10, ATTO_TEXT("String(1234567890) should have length 10"));

  String sFloat(123.456f);
  TEST(sFloat.length() >= 7, ATTO_TEXT("String(123.456f) should have length >= 7"));
  TEST(sFloat.startsWith(ATTO_TEXT("123.")),
       ATTO_TEXT("String(123.456f) should start with \"123.\""));

  String sWide(ATTO_TEXT("Wide String"));
  TEST(sWide.length() == 11, ATTO_TEXT("String(L\"Wide String\") should have length 11"));
  TEST(sWide.startsWith(ATTO_TEXT("Wide")), ATTO_TEXT("Wide string should start with \"Wide\""));

  // Basic info methods
  TEST(!s2.isEmpty(), ATTO_TEXT("\"Hello\" should not be empty"));
  TEST(s2.length() == 5, ATTO_TEXT("\"Hello\".length() should be 5"));

  // Character and substring access
  String c = s2.at(0);
  TEST(c.length() == 1, ATTO_TEXT("at(0) should return single character"));

  String lastChar = s2.at(-1);
  TEST(lastChar.length() == 1, ATTO_TEXT("at(-1) should return single character"));

  String sub = s2.substring(1, 4);
  TEST(sub.length() == 3, ATTO_TEXT("substring(1, 4) of \"Hello\" should have length 3"));
  TEST(sub == ATTO_TEXT("ell"), ATTO_TEXT("substring(1, 4) should equal \"ell\""));

  String sub2 = s2.substring(1, -1);
  TEST(sub2.length() == 4, ATTO_TEXT("substring(1, -1) should have length 4"));
  TEST(sub2 == ATTO_TEXT("ello"), ATTO_TEXT("substring(1, -1) should equal \"ello\""));

  String sub3 = s2.substring(1, -2);
  TEST(sub3.length() == 2, ATTO_TEXT("substring(1, -2) should have length 2"));
  TEST(sub3 == ATTO_TEXT("el"), ATTO_TEXT("substring(1, -2) should equal \"el\""));

  String sDup = s2.duplicate();
  TEST(sDup == s2, ATTO_TEXT("duplicate() should create equal string"));
  TEST(sDup.length() == s2.length(), ATTO_TEXT("duplicate() should preserve length"));

  // Query operations
  TEST(s2.contains(ATTO_TEXT("ell")), ATTO_TEXT("\"Hello\" should contain \"ell\""));
  TEST(!s2.contains(ATTO_TEXT("xyz")), ATTO_TEXT("\"Hello\" should not contain \"xyz\""));

  TEST(s2.startsWith(ATTO_TEXT("H")), ATTO_TEXT("\"Hello\" should start with \"H\""));
  TEST(s2.startsWith(ATTO_TEXT("Hel")), ATTO_TEXT("\"Hello\" should start with \"Hel\""));
  TEST(!s2.startsWith(ATTO_TEXT("h")),
       ATTO_TEXT("\"Hello\" should not start with \"h\" (case-sensitive)"));

  TEST(s2.endsWith(ATTO_TEXT("o")), ATTO_TEXT("\"Hello\" should end with \"o\""));
  TEST(s2.endsWith(ATTO_TEXT("llo")), ATTO_TEXT("\"Hello\" should end with \"llo\""));
  TEST(!s2.endsWith(ATTO_TEXT("O")),
       ATTO_TEXT("\"Hello\" should not end with \"O\" (case-sensitive)"));

  String banana(ATTO_TEXT("banana"));
  TEST(banana.count(ATTO_TEXT("a")) == 3,
       ATTO_TEXT("\"banana\" should contain 3 occurrences of \"a\""));
  TEST(banana.count(ATTO_TEXT("an")) == 2,
       ATTO_TEXT("\"banana\" should contain 2 occurrences of \"an\""));
  TEST(banana.count(ATTO_TEXT("x")) == 0,
       ATTO_TEXT("\"banana\" should contain 0 occurrences of \"x\""));

  // Number validation and conversion
  String sNum(ATTO_TEXT("123"));
  TEST(sNum.isNumber(), ATTO_TEXT("\"123\" should be a valid number"));
  TEST(sNum.toInteger() == 123, ATTO_TEXT("\"123\".toInteger() should be 123"));
  TEST(sNum.toFloat() == 123.0f, ATTO_TEXT("\"123\".toFloat() should be 123.0"));

  String sFloatStr(ATTO_TEXT("45.67"));
  TEST(sFloatStr.isNumber(), ATTO_TEXT("\"45.67\" should be a valid number"));
  TEST(sFloatStr.toFloat() > 45.6f && sFloatStr.toFloat() < 45.7f,
       ATTO_TEXT("\"45.67\".toFloat() should be approximately 45.67"));

  String sNotNum(ATTO_TEXT("abc"));
  TEST(!sNotNum.isNumber(), ATTO_TEXT("\"abc\" should not be a valid number"));
  TEST(sNotNum.toInteger() == 0, ATTO_TEXT("\"abc\".toInteger() should be 0"));

  String sBool1(ATTO_TEXT("true"));
  TEST(sBool1.toBool(), ATTO_TEXT("\"true\".toBool() should be true"));

  String sBool2(ATTO_TEXT("1"));
  TEST(sBool2.toBool(), ATTO_TEXT("\"1\".toBool() should be true"));

  String sBool3(ATTO_TEXT("yes"));
  TEST(sBool3.toBool(), ATTO_TEXT("\"yes\".toBool() should be true"));

  String sBool4(ATTO_TEXT("false"));
  TEST(!sBool4.toBool(), ATTO_TEXT("\"false\".toBool() should be false"));

  // Modification operations - append
  String sApp(ATTO_TEXT("A"));
  String sAppResult = sApp.append(ATTO_TEXT("B"));
  TEST(sAppResult == ATTO_TEXT("AB"), ATTO_TEXT("append(\"B\") should create \"AB\""));
  TEST(sAppResult.length() == 2, ATTO_TEXT("After append, length should be 2"));
  TEST(sApp == ATTO_TEXT("A"), ATTO_TEXT("Original string should be unchanged"));

  // Modification operations - prepend
  String sPre(ATTO_TEXT("B"));
  String sPreResult = sPre.prepend(ATTO_TEXT("A"));
  TEST(sPreResult == ATTO_TEXT("AB"), ATTO_TEXT("prepend(\"A\") should create \"AB\""));
  TEST(sPreResult.length() == 2, ATTO_TEXT("After prepend, length should be 2"));
  TEST(sPre == ATTO_TEXT("B"), ATTO_TEXT("Original string should be unchanged"));

  // Modification operations - insert
  String sIns(ATTO_TEXT("AC"));
  String sInsResult = sIns.insert(1, ATTO_TEXT("B"));
  TEST(sInsResult == ATTO_TEXT("ABC"), ATTO_TEXT("insert(1, \"B\") should create \"ABC\""));
  TEST(sInsResult.length() == 3, ATTO_TEXT("After insert, length should be 3"));
  TEST(sIns == ATTO_TEXT("AC"), ATTO_TEXT("Original string should be unchanged"));

  // Modification operations - remove
  String sRem(ATTO_TEXT("ABCD"));
  String sRemResult = sRem.remove(1, 3);
  TEST(sRemResult == ATTO_TEXT("AD"), ATTO_TEXT("remove(1, 3) should leave \"AD\""));
  TEST(sRemResult.length() == 2, ATTO_TEXT("After remove, length should be 2"));
  TEST(sRem == ATTO_TEXT("ABCD"), ATTO_TEXT("Original string should be unchanged"));

  // Modification operations - replace
  String sRep(ATTO_TEXT("banana"));
  String sRepResult = sRep.replace(ATTO_TEXT("a"), ATTO_TEXT("o"));
  TEST(sRepResult == ATTO_TEXT("bonono"),
       ATTO_TEXT("replace(\"a\", \"o\") should create \"bonono\""));
  TEST(!sRepResult.contains(ATTO_TEXT("a")), ATTO_TEXT("After replace, should not contain \"a\""));
  TEST(sRepResult.contains(ATTO_TEXT("o")), ATTO_TEXT("After replace, should contain \"o\""));
  TEST(sRep == ATTO_TEXT("banana"), ATTO_TEXT("Original string should be unchanged"));

  // Modification operations - trim
  String sTrim(ATTO_TEXT("  test  "));
  String sTrimResult = sTrim.trim();
  TEST(sTrimResult == ATTO_TEXT("test"), ATTO_TEXT("trim() should remove whitespace"));
  TEST(sTrimResult.length() == 4, ATTO_TEXT("After trim, length should be 4"));
  TEST(sTrim == ATTO_TEXT("  test  "), ATTO_TEXT("Original string should be unchanged"));

  String sTrim2(ATTO_TEXT("test"));
  String sTrim2Result = sTrim2.trim();
  TEST(sTrim2Result == ATTO_TEXT("test"),
       ATTO_TEXT("trim() on string without whitespace should be unchanged"));

  // Modification operations - upper/lower
  String sCase(ATTO_TEXT("HeLLo"));
  String sCaseLower = sCase.lower();
  TEST(sCaseLower == ATTO_TEXT("hello"), ATTO_TEXT("lower() should convert to lowercase"));
  TEST(!sCaseLower.contains(ATTO_TEXT("L")),
       ATTO_TEXT("After lower(), should not contain uppercase 'L'"));
  TEST(sCase == ATTO_TEXT("HeLLo"), ATTO_TEXT("Original string should be unchanged"));

  String sCaseUpper = sCaseLower.upper();
  TEST(sCaseUpper == ATTO_TEXT("HELLO"), ATTO_TEXT("upper() should convert to uppercase"));
  TEST(!sCaseUpper.contains(ATTO_TEXT("e")),
       ATTO_TEXT("After upper(), should not contain lowercase 'e'"));

  // Modification operations - reverse
  String sRev(ATTO_TEXT("ABC"));
  String sRevResult = sRev.reverse();
  TEST(sRevResult == ATTO_TEXT("CBA"), ATTO_TEXT("reverse() should reverse the string"));
  TEST(sRevResult.startsWith(ATTO_TEXT("C")), ATTO_TEXT("Reversed \"ABC\" should start with 'C'"));
  TEST(sRevResult.endsWith(ATTO_TEXT("A")), ATTO_TEXT("Reversed \"ABC\" should end with 'A'"));
  TEST(sRev == ATTO_TEXT("ABC"), ATTO_TEXT("Original string should be unchanged"));

  // Modification operations - repeat
  String sRpt(ATTO_TEXT("A"));
  String sRptResult = sRpt.repeat(3);
  TEST(sRptResult == ATTO_TEXT("AAA"), ATTO_TEXT("repeat(3) should create \"AAA\""));
  TEST(sRptResult.length() == 3, ATTO_TEXT("After repeat(3), length should be 3"));
  TEST(sRpt == ATTO_TEXT("A"), ATTO_TEXT("Original string should be unchanged"));

  String sRpt2(ATTO_TEXT("AB"));
  String sRpt2Result = sRpt2.repeat(2);
  TEST(sRpt2Result == ATTO_TEXT("ABAB"), ATTO_TEXT("repeat(2) on \"AB\" should create \"ABAB\""));
  TEST(sRpt2 == ATTO_TEXT("AB"), ATTO_TEXT("Original string should be unchanged"));

  // Chaining
  String sChain(ATTO_TEXT("  hello  "));
  String sChainResult = sChain.trim().upper().append(ATTO_TEXT("!"));
  TEST(sChainResult == ATTO_TEXT("HELLO!"),
       ATTO_TEXT("Chaining trim().upper().append() should work"));
  TEST(sChain == ATTO_TEXT("  hello  "),
       ATTO_TEXT("Original string should be unchanged after chaining"));

  // + operator concatenation
  String sPlus1(ATTO_TEXT("Hello"));
  String sPlus2(ATTO_TEXT(" World"));
  String sPlusResult = sPlus1 + sPlus2;
  TEST(sPlusResult == ATTO_TEXT("Hello World"), ATTO_TEXT("operator+ should concatenate strings"));
  TEST(sPlusResult.length() == 11,
       ATTO_TEXT("Concatenated string should have correct length"));
  TEST(sPlus1 == ATTO_TEXT("Hello"),
       ATTO_TEXT("Original string should be unchanged after operator+"));
  TEST(sPlus2 == ATTO_TEXT(" World"),
       ATTO_TEXT("Second original string should be unchanged after operator+"));

  String sPlusEmpty1(ATTO_TEXT(""));
  String sPlusEmpty2(ATTO_TEXT("test"));
  String sPlusEmptyResult = sPlusEmpty1 + sPlusEmpty2;
  TEST(sPlusEmptyResult == ATTO_TEXT("test"),
       ATTO_TEXT("operator+ with empty first string should work"));
  TEST(sPlusEmptyResult.length() == 4,
       ATTO_TEXT("Result should have length of non-empty string"));

  String sPlusEmptyResult2 = sPlusEmpty2 + sPlusEmpty1;
  TEST(sPlusEmptyResult2 == ATTO_TEXT("test"),
       ATTO_TEXT("operator+ with empty second string should work"));

  String sPlusLong1(ATTO_TEXT("This is "));
  String sPlusLong2(ATTO_TEXT("a longer test"));
  String sPlusLongResult = sPlusLong1 + sPlusLong2;
  TEST(sPlusLongResult == ATTO_TEXT("This is a longer test"),
       ATTO_TEXT("operator+ should work with longer strings"));
  TEST(sPlusLongResult.startsWith(ATTO_TEXT("This is ")),
       ATTO_TEXT("Result should start with first string"));

  // Equality and comparison
  String sEq1(ATTO_TEXT("Test"));
  String sEq2(ATTO_TEXT("Test"));
  String sEq3(ATTO_TEXT("Other"));

  TEST(sEq1.equals(sEq2), ATTO_TEXT("equals() should return true for equal strings"));
  TEST(!sEq1.equals(sEq3),
       ATTO_TEXT("equals() should return false for different strings"));

  TEST(sEq1 == sEq2, ATTO_TEXT("operator== should return true for equal strings"));
  TEST(!(sEq1 == sEq3), ATTO_TEXT("operator== should return false for different strings"));

  TEST(!(sEq1 != sEq2), ATTO_TEXT("operator!= should return false for equal strings"));
  TEST(sEq1 != sEq3, ATTO_TEXT("operator!= should return true for different strings"));

  // String comparison (compare)
  String sCmp1(ATTO_TEXT("apple"));
  String sCmp2(ATTO_TEXT("apple"));
  String sCmp3(ATTO_TEXT("banana"));
  String sCmp4(ATTO_TEXT("appl"));
  String sCmp5(ATTO_TEXT("application"));

  TEST(sCmp1.compare(sCmp2) == 0,
       ATTO_TEXT("compare() should return 0 for equal strings"));
  TEST(sCmp1.compare(sCmp3) < 0, ATTO_TEXT("\"apple\" should be less than \"banana\""));
  TEST(sCmp3.compare(sCmp1) > 0, ATTO_TEXT("\"banana\" should be greater than \"apple\""));
  TEST(sCmp1.compare(sCmp4) > 0,
       ATTO_TEXT("\"apple\" should be greater than \"appl\" (longer)"));
  TEST(sCmp4.compare(sCmp1) < 0,
       ATTO_TEXT("\"appl\" should be less than \"apple\" (shorter)"));
  TEST(sCmp1.compare(sCmp5) < 0,
       ATTO_TEXT("\"apple\" should be less than \"application\""));
  TEST(sCmp5.compare(sCmp1) > 0,
       ATTO_TEXT("\"application\" should be greater than \"apple\""));

  String sEmpty1;
  String sEmpty2;
  String sNonEmpty(ATTO_TEXT("text"));

  TEST(sEmpty1.compare(sEmpty2) == 0, ATTO_TEXT("Empty strings should compare equal"));
  TEST(sEmpty1.compare(sNonEmpty) < 0,
       ATTO_TEXT("Empty string should be less than non-empty"));
  TEST(sNonEmpty.compare(sEmpty1) > 0,
       ATTO_TEXT("Non-empty string should be greater than empty"));

  String sCase1(ATTO_TEXT("abc"));
  String sCase2(ATTO_TEXT("ABC"));
  TEST(sCase1.compare(sCase2) != 0, ATTO_TEXT("compare() should be case-sensitive"));
  TEST(sCase2.compare(sCase1) < 0,
       ATTO_TEXT("\"ABC\" should be less than \"abc\" (uppercase < lowercase)"));

  // Hash
  TEST(sEq1.hash() == sEq2.hash(), ATTO_TEXT("Equal strings should have equal hashes"));
  TEST(String().hash() == 0, ATTO_TEXT("Empty string hash should be 0"));

  // Integer promotion
  int iVal = 42;
  String sPromoted(iVal);
  TEST(sPromoted == ATTO_TEXT("42"), ATTO_TEXT("String(int) should create \"42\""));
  TEST(sPromoted.length() == 2, ATTO_TEXT("String(42) should have length 2"));

  short shVal = 100;
  String sShort(shVal);
  TEST(sShort == ATTO_TEXT("100"), ATTO_TEXT("String(short) should create \"100\""));

  // Report results
  if (errorCount == 0) {
    Log(ATTO_TEXT("All String tests passed!"));
    Exit(0);
  } else {
    LogError(errorCount, ATTO_TEXT(" test(s) failed"));
    Exit(errorCount);
  }
}
