#include "attoboy/attoboy.h"
#include <windows.h>

using namespace attoboy;

void atto_main() {
  OutputDebugStringW(L"[TEST] Starting test\n");
  Log("Creating List");

  List innerList;
  Log("Appending to List");
  innerList.append(1);
  Log("Creating Map");

  Map m;
  Log("About to put List into Map");
  m.put(String("list"), innerList);
  Log("Put succeeded");

  Exit(0);
}
