#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError(ATTO_TEXT("Test failed: "), message);                             \
    errorCount++;                                                              \
  }

void atto_main() {
  Log(ATTO_TEXT("Running Subprocess simple tests..."));

  int exitCode = Subprocess::Run(Path(ATTO_TEXT("cmd.exe")), ATTO_TEXT("/c"),
                                  ATTO_TEXT("exit 0"));
  TEST(exitCode == 0, ATTO_TEXT("Run should return exit code 0"));

  exitCode = Subprocess::Run(Path(ATTO_TEXT("cmd.exe")), ATTO_TEXT("/c"),
                              ATTO_TEXT("exit 42"));
  TEST(exitCode == 42, ATTO_TEXT("Run should return exit code 42"));

  String output = Subprocess::Capture(Path(ATTO_TEXT("cmd.exe")), ATTO_TEXT("/c"),
                                       ATTO_TEXT("echo Hello"));
  TEST(output.contains(ATTO_TEXT("Hello")),
       ATTO_TEXT("Capture should contain 'Hello'"));

  Subprocess proc(Path(ATTO_TEXT("cmd.exe")), ATTO_TEXT("/c"),
                  ATTO_TEXT("echo Test"));
  TEST(proc.isValid(), ATTO_TEXT("Subprocess should be valid"));

  exitCode = proc.wait();
  TEST(exitCode == 0, ATTO_TEXT("Subprocess should exit with code 0"));

  if (errorCount == 0) {
    Log(ATTO_TEXT("All Subprocess simple tests passed!"));
    Exit(0);
  } else {
    LogError(errorCount, ATTO_TEXT(" test(s) failed"));
    Exit(errorCount);
  }
}
