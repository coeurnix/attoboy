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
  Log(ATTO_TEXT("Running environment variable tests..."));

  String testName = String(ATTO_TEXT("ATTOBOY_TEST_VAR"));
  String testValue = String(ATTO_TEXT("Hello World"));

  bool setResult = SetEnv(testName, testValue);
  TEST(setResult, ATTO_TEXT("SetEnv should return true on success"));

  String getValue = GetEnv(testName);
  TEST(getValue.equals(testValue),
       ATTO_TEXT("GetEnv should return the value set by SetEnv"));

  String nonExistent = GetEnv(String(ATTO_TEXT("ATTOBOY_NONEXISTENT_VAR_12345")));
  TEST(nonExistent.isEmpty(),
       ATTO_TEXT("GetEnv should return empty string for non-existent variable"));

  // Report results
  if (errorCount == 0) {
    Log(ATTO_TEXT("All environment variable tests passed!"));
    Exit(0);
  } else {
    LogError(errorCount, ATTO_TEXT(" test(s) failed"));
    Exit(errorCount);
  }
}
