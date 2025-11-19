#include "attoboy/attoboy.h"

using namespace attoboy;

void atto_main() {
  // Test 1: Basic Log function with single string
  Log("Test 1: Basic log message");

  // Test 2: Log with multiple arguments of different types
  Log("Test 2: Multiple args - String: ", "hello", " Int: ", 42,
      " Bool: ", true);

  // Test 3: Log with numeric types
  Log("Test 3: Numbers - ", 123, " ", 45.67, " ", -99);

  // Test 4: LogInfo (should work with default INFO level)
  LogInfo("Test 4: Info message");

  // Test 5: LogWarning (should work with default INFO level)
  LogWarning("Test 5: Warning message");

  // Test 6: LogError (always enabled unless ATTOBOY_LOG_DISABLE)
  LogError("Test 6: Error message");

  // Test 7: LogDebug (should NOT output with default INFO level)
  // This call compiles but produces no output
  LogDebug("Test 7: Debug message - you should NOT see this");

  // Test 8: Multiple arguments with timestamps
  LogInfo("Test 8: Info with multiple args - ", "value: ", 100);
  LogWarning("Test 9: Warning with calculation - ", "result: ", 10 + 20);
  LogError("Test 10: Error with boolean - ", "flag: ", false);

  // Test 11: Empty log
  Log();

  // Test 12: Single character
  Log("X");

  // Test 13: Long message
  Log("Test 13: This is a longer message to verify that the logging system "
      "can handle messages of varying lengths without issues.");

  // Test 14: String objects
  String s1("Hello");
  String s2(" World");
  Log("Test 14: String objects - ", s1, s2);

  // Test 15: Chain of different log levels
  Log("Test 15: Starting sequence");
  LogInfo("  -> Info level");
  LogWarning("  -> Warning level");
  LogError("  -> Error level");
  Log("Test 15: Sequence complete");

  // If we reach here, all tests passed (logging doesn't cause crashes)
  Log("All logging tests completed successfully!");

  Exit(0);
}
