#include "atto_internal_common.h"
#include "attoboy/attoboy.h"
#include "attostring_internal.h"
#include <lmcons.h>
#include <windows.h>

#define SECURITY_WIN32
#include <security.h>
#pragma comment(lib, "secur32.lib")

#ifdef GetUserName
#undef GetUserName
#endif

namespace attoboy {

void Exit(int exitCode) {
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut != INVALID_HANDLE_VALUE) {
    FlushFileBuffers(hOut);
  }

  HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
  if (hIn != INVALID_HANDLE_VALUE) {
    FlushConsoleInputBuffer(hIn);
  }

  ExitProcess(exitCode);
}

void Sleep(int milliseconds) {
  if (milliseconds > 0) {
    ::Sleep(milliseconds);
  }
}

String GetUserName() {
  WCHAR buffer[UNLEN + 1];
  DWORD size = UNLEN + 1;
  if (::GetUserNameW(buffer, &size)) {
    char *nameUtf8 = WideToUtf8(buffer);
    if (nameUtf8) {
      String result = String(nameUtf8);
      FreeConvertedString(nameUtf8);
      return result;
    }
  }
  return String();
}

String GetUserDisplayName() {
  WCHAR buffer[256];
  DWORD size = 256;
  if (::GetUserNameExW(NameDisplay, buffer, &size)) {
    char *nameUtf8 = WideToUtf8(buffer);
    if (nameUtf8) {
      String result = String(nameUtf8);
      FreeConvertedString(nameUtf8);
      return result;
    }
  }
  return String();
}

int GetProcessId() { return (int)::GetCurrentProcessId(); }

} // namespace attoboy
