#include "attoboy/attoboy.h"

using namespace attoboy;

void atto_main() {
  EnableLoggingToFile("test.log", true);
  Exit(0);
}
