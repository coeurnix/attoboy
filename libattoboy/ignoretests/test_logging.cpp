#include "attoboy/attoboy.h"

using namespace attoboy;

void atto_main() {
  // Test console logging (default)
  Log(ATTO_TEXT("Test 1: Console logging"));
  LogInfo(ATTO_TEXT("This is an info message"));
  LogWarning(ATTO_TEXT("This is a warning"));
  LogError(ATTO_TEXT("This is an error"));

  // Test file logging
  String logPath = ATTO_TEXT("test_log_output.txt");
  EnableLoggingToFile(logPath);

  Log(ATTO_TEXT("Test 2: File logging"));
  LogInfo(ATTO_TEXT("This info goes to file"));
  LogWarning(ATTO_TEXT("This warning goes to file"));
  LogError(ATTO_TEXT("This error goes to file"));

  // Switch back to console
  EnableLoggingToConsole();
  Log(ATTO_TEXT("Test 3: Back to console logging"));
  LogInfo(ATTO_TEXT("This info is on console again"));

  // Check if file was created and read it before switching back to console
  Path logFilePath(logPath);
  if (logFilePath.exists()) {
    String fileContents = logFilePath.readToString();

    // Now we're back on console, so this will print
    Log(ATTO_TEXT("SUCCESS: Log file was created"));
    Log(ATTO_TEXT("File contents length: "), fileContents.length());

    if (fileContents.contains(ATTO_TEXT("Test 2")) &&
        fileContents.contains(ATTO_TEXT("This info goes to file"))) {
      Log(ATTO_TEXT("SUCCESS: File contains expected content"));
    } else {
      LogError(ATTO_TEXT("FAIL: File content mismatch"));
      Exit(1);
    }

    // Clean up
    logFilePath.deleteFile();
  } else {
    LogError(ATTO_TEXT("FAIL: Log file was not created"));
    Exit(1);
  }

  Log(ATTO_TEXT("All logging tests passed!"));
  Exit(0);
}
