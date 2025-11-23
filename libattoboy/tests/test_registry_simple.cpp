#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError(ATTO_TEXT("Test failed: "), message);                             \
    errorCount++;                                                              \
  }

void atto_main() {
  Log(ATTO_TEXT("Running Registry simple tests..."));

  String testKeyPath = ATTO_TEXT("HKEY_CURRENT_USER\\Software\\AttoTest");
  Registry reg(testKeyPath);

  if (reg.exists()) {
    Log(ATTO_TEXT("Cleaning up existing test key..."));
    reg.remove(true);
  }

  TEST(!reg.exists(), ATTO_TEXT("Key should not exist initially"));

  TEST(reg.create(), ATTO_TEXT("Should create key successfully"));
  TEST(reg.exists(), ATTO_TEXT("Key should exist after creation"));

  TEST(reg.setStringValue(ATTO_TEXT("TestString"), ATTO_TEXT("Hello World")),
       ATTO_TEXT("Should set string value"));
  TEST(reg.valueExists(ATTO_TEXT("TestString")),
       ATTO_TEXT("String value should exist"));

  const ATTO_CHAR *strValue = reg.getStringValue(ATTO_TEXT("TestString"));
  TEST(strValue != nullptr, ATTO_TEXT("Should retrieve string value"));
  if (strValue) {
    String retrieved(strValue);
    TEST(retrieved == ATTO_TEXT("Hello World"),
         ATTO_TEXT("String value should match"));
  }

  TEST(reg.setIntegerValue(ATTO_TEXT("TestInt"), 42),
       ATTO_TEXT("Should set integer value"));
  TEST(reg.valueExists(ATTO_TEXT("TestInt")), ATTO_TEXT("Integer value should exist"));

  unsigned int intValue = reg.getIntegerValue(ATTO_TEXT("TestInt"));
  TEST(intValue == 42, ATTO_TEXT("Integer value should match"));

  Buffer testBuf;
  testBuf.append((const unsigned char *)"TestData", 8);
  TEST(reg.setBinaryValue(ATTO_TEXT("TestBinary"), testBuf),
       ATTO_TEXT("Should set binary value"));
  TEST(reg.valueExists(ATTO_TEXT("TestBinary")),
       ATTO_TEXT("Binary value should exist"));

  List valueNames = reg.listValueNames();
  TEST(valueNames.length() == 3,
       ATTO_TEXT("Should have 3 values (TestString, TestInt, TestBinary)"));

  TEST(reg.deleteValue(ATTO_TEXT("TestInt")),
       ATTO_TEXT("Should delete integer value"));
  TEST(!reg.valueExists(ATTO_TEXT("TestInt")),
       ATTO_TEXT("Deleted value should not exist"));

  valueNames = reg.listValueNames();
  TEST(valueNames.length() == 2,
       ATTO_TEXT("Should have 2 values after deletion"));

  String subkeyPath = testKeyPath + ATTO_TEXT("\\Subkey1");
  Registry subkey1(subkeyPath);
  TEST(subkey1.create(), ATTO_TEXT("Should create subkey"));

  List subkeys = reg.listSubkeys();
  TEST(subkeys.length() == 1, ATTO_TEXT("Should have 1 subkey"));
  TEST(subkeys.at<String>(0) == ATTO_TEXT("Subkey1"),
       ATTO_TEXT("Subkey name should match"));

  TEST(reg.remove(true), ATTO_TEXT("Should remove key recursively"));
  TEST(!reg.exists(), ATTO_TEXT("Key should not exist after removal"));
  TEST(!subkey1.exists(), ATTO_TEXT("Subkey should not exist after removal"));

  if (errorCount == 0) {
    Log(ATTO_TEXT("All registry tests passed!"));
    Exit(0);
  } else {
    LogError(errorCount, ATTO_TEXT(" test(s) failed"));
    Exit(errorCount);
  }
}
