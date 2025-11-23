#include "attoboy/attoboy.h"
#include "attostring_internal.h"
#include <windows.h>
#include <lmcons.h>

#define SECURITY_WIN32
#include <security.h>
#pragma comment(lib, "secur32.lib")

#ifdef GetUserName
#undef GetUserName
#endif

namespace attoboy {

void Exit(int exitCode) { ExitProcess(exitCode); }

void Sleep(int milliseconds) {
  if (milliseconds > 0) {
    ::Sleep(milliseconds);
  }
}

String GetUserName() {
#ifdef UNICODE
  WCHAR buffer[UNLEN + 1];
  DWORD size = UNLEN + 1;
  if (::GetUserNameW(buffer, &size)) {
    return String(buffer);
  }
#else
  CHAR buffer[UNLEN + 1];
  DWORD size = UNLEN + 1;
  if (::GetUserNameA(buffer, &size)) {
    return String(buffer);
  }
#endif
  return String();
}

String GetUserDisplayName() {
#ifdef UNICODE
  WCHAR buffer[256];
  DWORD size = 256;
  if (::GetUserNameExW(NameDisplay, buffer, &size)) {
    return String(buffer);
  }
#else
  CHAR buffer[256];
  DWORD size = 256;
  if (::GetUserNameExA(NameDisplay, buffer, &size)) {
    return String(buffer);
  }
#endif
  return String();
}

int GetProcessId() {
  return (int)::GetCurrentProcessId();
}

} // namespace attoboy
