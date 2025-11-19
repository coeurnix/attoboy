#include "attoboy/attoboy.h"
#include <windows.h>

namespace attoboy {

void Exit(int exitCode) { ExitProcess(exitCode); }

} // namespace attoboy
