#include "attoboy/attoboy.h"

using namespace attoboy;

void atto_main() {
  String s(ATTO_TEXT("Hello from Attoboy!"));
  Log(s);
  Exit(0);
}
