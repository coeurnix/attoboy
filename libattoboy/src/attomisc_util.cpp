#include "attoboy/attoboy.h"
#include <windows.h>

namespace attoboy {

void Exit(int exitCode) { ExitProcess(exitCode); }

void Sleep(int milliseconds) {
  if (milliseconds > 0) {
    ::Sleep(milliseconds);
  }
}

} // namespace attoboy
