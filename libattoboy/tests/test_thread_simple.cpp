#include "attoboy/attoboy.h"

using namespace attoboy;

void atto_main() {
  Log(ATTO_TEXT("Test 1: Sleep function"));
  Sleep(100);
  Log(ATTO_TEXT("Sleep completed successfully"));

  Log(ATTO_TEXT("All tests passed!"));
  Exit(0);
}
