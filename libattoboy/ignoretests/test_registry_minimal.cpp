#include "attoboy/attoboy.h"

using namespace attoboy;

void atto_main() {
  Log(ATTO_TEXT("Starting minimal registry test..."));

  String testKeyPath = ATTO_TEXT("HKEY_CURRENT_USER\\Software\\AttoTest");
  Log(ATTO_TEXT("Creating registry object..."));
  Registry reg(testKeyPath);

  Log(ATTO_TEXT("Test completed!"));
  Exit(0);
}
