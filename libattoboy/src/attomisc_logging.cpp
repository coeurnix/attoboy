#include "attoboy/attoboy.h"
#include "attostring_internal.h"
#include <windows.h>

namespace attoboy {

static String GetCurrentDatetimeString() {
  SYSTEMTIME st;
  GetLocalTime(&st);
  ATTO_WCHAR buf[32];
#ifdef UNICODE
  wsprintfW(buf, L"%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
#else
  wsprintfA(buf, "%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
#endif
  return String(static_cast<const ATTO_CHAR *>(buf));
}

static void PrintString(const String &s) {
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD written;
#ifdef UNICODE
  WriteConsoleW(hOut, s.c_str(), s.length(), &written, nullptr);
#else
  WriteConsoleA(hOut, s.c_str(), s.length(), &written, nullptr);
#endif
}

namespace internal {
void LogImpl(const String *args, int count, const String &prefix) {
  if (!prefix.isEmpty()) {
    PrintString(prefix);
    PrintString(GetCurrentDatetimeString());
    PrintString(String(ATTO_TEXT(": ")));
  }

  for (int i = 0; i < count; i++) {
    PrintString(args[i]);
  }

  PrintString(String(ATTO_TEXT("\n")));
}
} // namespace internal

} // namespace attoboy
