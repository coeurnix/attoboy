#include "attoboy/attoboy.h"
#include <windows.h>
using namespace attoboy;

void atto_main() {
  Log("Testing Map constructor...");

  Map m1;
  Log("Map created successfully");

  Log("Testing put...");
  m1.put(1, 10);
  Log("Put successful");

  Log("Testing length...");
  int len = m1.length();
  Log("Length: ", len);

  Exit(0);
}
