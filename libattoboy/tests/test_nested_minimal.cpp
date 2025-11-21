#include "attoboy/attoboy.h"
#include <windows.h>

using namespace attoboy;

void atto_main() {
#ifdef UNICODE
  OutputDebugStringW(ATTO_TEXT("[TEST] Starting test\n"));
#else
  OutputDebugStringA(ATTO_TEXT("[TEST] Starting test\n"));
#endif
  Log(ATTO_TEXT("Creating List"));

  List innerList;
  Log(ATTO_TEXT("Appending to List"));
  innerList.append(1);
  Log(ATTO_TEXT("Creating Map"));

  Map m;
  Log(ATTO_TEXT("About to put List into Map"));
  m.put(String(ATTO_TEXT("list")), innerList);
  Log(ATTO_TEXT("Put succeeded"));

  Exit(0);
}
