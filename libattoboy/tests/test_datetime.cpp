#include "attoboy/attoboy.h"
#include <windows.h>

void *operator new(size_t size) { return HeapAlloc(GetProcessHeap(), 0, size); }

void operator delete(void *ptr) noexcept {
  if (ptr)
    HeapFree(GetProcessHeap(), 0, ptr);
}

void operator delete(void *ptr, size_t) noexcept {
  if (ptr)
    HeapFree(GetProcessHeap(), 0, ptr);
}

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError("Test failed: ", message);                                        \
    errorCount++;                                                              \
  }

void atto_main() {
  Log("Running DateTime tests...");

  // Test default constructor (now)
  DateTime dt1;
  long long ts1 = dt1.timestamp();
  TEST(ts1 > 0, "Default constructor should create DateTime with positive timestamp");
  String str1 = dt1.toString();
  TEST(str1.length() > 0, "toString() should return non-empty string");
  TEST(str1.contains(L"T"), "ISO-8601 format should contain 'T'");
  TEST(str1.contains(L"Z"), "ISO-8601 format should end with 'Z'");

  // Test constructor from timestamp (Unix epoch)
  DateTime dt2(0LL);
  long long ts2 = dt2.timestamp();
  TEST(ts2 == 0, "DateTime(0) should have timestamp 0");
  String str2 = dt2.toString();
  TEST(str2.startsWith(L"1970-01-01"), "Unix epoch should be 1970-01-01");

  // Test constructor from timestamp (known date)
  long long millis2000 = 946684800000LL; // 2000-01-01 00:00:00 UTC
  DateTime dt3(millis2000);
  long long ts3 = dt3.timestamp();
  TEST(ts3 == millis2000, "DateTime should preserve timestamp");
  String str3 = dt3.toString();
  TEST(str3.startsWith(L"2000-01-01"), "Timestamp for 2000-01-01 should format correctly");

  // Test constructor from ISO-8601 string
  String isoStr = String(L"2024-06-15T12:30:45.123Z");
  DateTime dt4(isoStr);
  String str4 = dt4.toString();
  TEST(str4.equals(isoStr), "DateTime from ISO-8601 should round-trip");

  // Test copy constructor
  DateTime dt5(dt3);
  long long ts5 = dt5.timestamp();
  TEST(ts5 == millis2000, "Copy constructor should copy timestamp");
  TEST(dt5.equals(dt3), "Copy constructor should create equal DateTime");

  // Test assignment operator
  DateTime dt6;
  dt6 = dt3;
  TEST(dt6.equals(dt3), "Assignment operator should copy DateTime");
  TEST(dt6.timestamp() == dt3.timestamp(), "Assignment should copy timestamp");

  // Test add (positive)
  DateTime dt7(0LL);
  dt7.add(1000LL);
  TEST(dt7.timestamp() == 1000, "add(1000) should add 1 second");

  // Test add (negative)
  DateTime dt8(10000LL);
  dt8.add(-5000LL);
  TEST(dt8.timestamp() == 5000, "add(-5000) should subtract 5 seconds");

  // Test add chaining
  DateTime dt9(0LL);
  dt9.add(1000LL).add(2000LL).add(3000LL);
  TEST(dt9.timestamp() == 6000, "Chained add should work");

  // Test diff (positive)
  DateTime dtA(10000LL);
  DateTime dtB(5000LL);
  long long diffAB = dtA.diff(dtB);
  TEST(diffAB == 5000, "diff() should return positive value when this > other");

  // Test diff (negative)
  long long diffBA = dtB.diff(dtA);
  TEST(diffBA == -5000, "diff() should return negative value when this < other");

  // Test diff (equal)
  DateTime dtC(1000LL);
  DateTime dtD(1000LL);
  long long diffCD = dtC.diff(dtD);
  TEST(diffCD == 0, "diff() should return 0 when DateTimes are equal");

  // Test compare
  DateTime dtE(1000LL);
  DateTime dtF(2000LL);
  TEST(dtE.compare(dtF) < 0, "compare() should return negative when this < other");
  TEST(dtF.compare(dtE) > 0, "compare() should return positive when this > other");
  TEST(dtE.compare(dtE) == 0, "compare() should return 0 when equal");

  // Test equals
  DateTime dtG(12345LL);
  DateTime dtH(12345LL);
  DateTime dtI(54321LL);
  TEST(dtG.equals(dtH), "equals() should return true for equal DateTimes");
  TEST(!dtG.equals(dtI), "equals() should return false for different DateTimes");

  // Test operator ==
  TEST(dtG == dtH, "operator== should return true for equal DateTimes");
  TEST(!(dtG == dtI), "operator== should return false for different DateTimes");

  // Test operator !=
  TEST(dtG != dtI, "operator!= should return true for different DateTimes");
  TEST(!(dtG != dtH), "operator!= should return false for equal DateTimes");

  // Test ISO-8601 parsing edge cases
  DateTime dtJ(String(L"2020-12-31T23:59:59.999Z"));
  String strJ = dtJ.toString();
  TEST(strJ.contains(L"2020-12-31"), "ISO-8601 parsing should handle end of year");
  TEST(strJ.contains(L"23:59:59"), "ISO-8601 parsing should handle end of day");

  // Test timestamp precision (milliseconds)
  DateTime dtK(123456789LL);
  long long tsK = dtK.timestamp();
  TEST(tsK == 123456789LL, "Timestamp should preserve millisecond precision");

  // Test toString format
  DateTime dtL(String(L"1999-03-15T08:30:00.500Z"));
  String strL = dtL.toString();
  TEST(strL.startsWith(L"1999-03-15"), "toString() should format date correctly");
  TEST(strL.contains(L"08:30:00"), "toString() should format time correctly");
  TEST(strL.contains(L".500Z"), "toString() should format milliseconds correctly");

  // Test add with large values
  DateTime dtM(0LL);
  dtM.add(86400000LL); // Add 1 day in milliseconds
  String strM = dtM.toString();
  TEST(strM.startsWith(L"1970-01-02"), "add(86400000) should add 1 day");

  // Test negative timestamps (before Unix epoch) - if supported
  DateTime dtN(-86400000LL); // 1 day before epoch
  long long tsN = dtN.timestamp();
  TEST(tsN == -86400000LL, "Negative timestamps should be supported");

  // Test environment variables
  Log("\nTesting environment variables...");

  String testName = String(L"ATTOBOY_TEST_VAR");
  String testValue = String(L"Hello World");

  bool setResult = SetEnv(testName, testValue);
  TEST(setResult, "SetEnv should return true on success");

  String getValue = GetEnv(testName);
  TEST(getValue.equals(testValue), "GetEnv should return the value set by SetEnv");

  String nonExistent = GetEnv(String(L"ATTOBOY_NONEXISTENT_VAR_12345"));
  TEST(nonExistent.isEmpty(), "GetEnv should return empty string for non-existent variable");

  // Report results
  if (errorCount == 0) {
    Log("All DateTime tests passed!");
    Exit(0);
  } else {
    LogError(errorCount, " test(s) failed");
    Exit(1);
  }
}
