#include "attoboy/attoboy.h"

using namespace attoboy;

void atto_main() {
  // Test 1: Basic Log function with single string
  Log(ATTO_TEXT("Test 1: Basic log message"));

  // Test 2: Log with multiple arguments of different types
  Log(ATTO_TEXT("Test 2: Multiple args - String: "), ATTO_TEXT("hello"), ATTO_TEXT(" Int: "), 42,
      ATTO_TEXT(" Bool: "), true);

  // Test 3: Log with numeric types
  Log(ATTO_TEXT("Test 3: Numbers - "), 123, ATTO_TEXT(" "), 45.67f, ATTO_TEXT(" "), -99);

  // Test 4: LogInfo (should work with default INFO level)
  LogInfo(ATTO_TEXT("Test 4: Info message"));

  // Test 5: LogWarning (should work with default INFO level)
  LogWarning(ATTO_TEXT("Test 5: Warning message"));

  // Test 6: LogError (always enabled unless ATTOBOY_LOG_DISABLE)
  LogError(ATTO_TEXT("Test 6: Error message"));

  // Test 7: LogDebug (should NOT output with default INFO level)
  // This call compiles but produces no output
  LogDebug(ATTO_TEXT("Test 7: Debug message - you should NOT see this"));

  // Test 8: Multiple arguments with timestamps
  LogInfo(ATTO_TEXT("Test 8: Info with multiple args - "), ATTO_TEXT("value: "), 100);
  LogWarning(ATTO_TEXT("Test 9: Warning with calculation - "), ATTO_TEXT("result: "), 10 + 20);
  LogError(ATTO_TEXT("Test 10: Error with boolean - "), ATTO_TEXT("flag: "), false);

  // Test 11: Empty log
  Log();

  // Test 12: Single character
  Log(ATTO_TEXT("X"));

  // Test 13: Long message
  Log(ATTO_TEXT("Test 13: This is a longer message to verify that the logging system ")
      ATTO_TEXT("can handle messages of varying lengths without issues."));

  // Test 14: String objects
  String s1(ATTO_TEXT("Hello"));
  String s2(ATTO_TEXT(" World"));
  Log(ATTO_TEXT("Test 14: String objects - "), s1, s2);

  // Test 15: Chain of different log levels
  Log(ATTO_TEXT("Test 15: Starting sequence"));
  LogInfo(ATTO_TEXT("  -> Info level"));
  LogWarning(ATTO_TEXT("  -> Warning level"));
  LogError(ATTO_TEXT("  -> Error level"));
  Log(ATTO_TEXT("Test 15: Sequence complete"));

  // If we reach here, all tests passed (logging doesn't cause crashes)
  Log(ATTO_TEXT("All logging tests completed successfully!"));

  Exit(0);
}
