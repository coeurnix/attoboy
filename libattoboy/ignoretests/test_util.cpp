#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError(ATTO_TEXT("Test failed: "), message);                             \
    errorCount++;                                                              \
  }

void atto_main() {
  Log(ATTO_TEXT("Running utility function tests..."));

  String userName = GetUserName();
  TEST(!userName.isEmpty(), ATTO_TEXT("GetUserName should return non-empty string"));
  Log(ATTO_TEXT("User name: "), userName);

  String displayName = GetUserDisplayName();
  Log(ATTO_TEXT("Display name: "), displayName);

  int pid = GetProcessId();
  TEST(pid > 0, ATTO_TEXT("GetProcessId should return positive value"));
  Log(ATTO_TEXT("Process ID: "), pid);

  if (errorCount == 0) {
    Log(ATTO_TEXT("All utility function tests passed!"));
    Exit(0);
  } else {
    LogError(errorCount, ATTO_TEXT(" test(s) failed"));
    Exit(errorCount);
  }
}
