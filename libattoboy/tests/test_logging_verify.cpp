#include "attoboy/attoboy.h"

using namespace attoboy;

void atto_main() {
  String logPath = ATTO_TEXT("logging_verification.txt");

  // Clean up any existing file
  Path p(logPath);
  if (p.exists()) {
    p.deleteFile();
  }

  // Test file logging
  EnableLoggingToFile(logPath);
  Log(ATTO_TEXT("Message 1: Plain log"));
  LogInfo(ATTO_TEXT("Message 2: Info log"));
  LogWarning(ATTO_TEXT("Message 3: Warning log"));
  LogError(ATTO_TEXT("Message 4: Error log"));

  // Switch to console
  EnableLoggingToConsole();

  // Verify file exists and contains data
  if (p.exists()) {
    String contents = p.readToString();
    if (contents.length() > 0 &&
        contents.contains(ATTO_TEXT("Message 1")) &&
        contents.contains(ATTO_TEXT("INFO")) &&
        contents.contains(ATTO_TEXT("WARNING")) &&
        contents.contains(ATTO_TEXT("ERROR"))) {
      // Leave file for inspection - test passed
      Exit(0);
    }
  }

  Exit(1);
}
