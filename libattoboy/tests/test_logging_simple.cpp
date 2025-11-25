#include "attoboy/attoboy.h"

using namespace attoboy;

void atto_main() {
  EnableLoggingToFile(ATTO_TEXT("test.log"), true);
  Exit(0);
}
