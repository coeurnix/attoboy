#include "attoboy/attoboy.h"
#include <windows.h>

namespace attoboy {

static String GetCurrentDatetimeString() {
  SYSTEMTIME st;
  GetLocalTime(&st);
  WCHAR buf[32];
  wsprintfW(buf, L"%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
  return String(static_cast<const wchar_t *>(buf));
}

static void PrintString(const String &s) {
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD written;
  WriteConsoleW(hOut, s.c_str(), s.length(), &written, nullptr);
}

namespace internal {
void LogImpl(const String *args, int count, const String &prefix) {
  if (!prefix.isEmpty()) {
    PrintString(prefix);
    PrintString(GetCurrentDatetimeString());
    PrintString(String(": "));
  }

  for (int i = 0; i < count; i++) {
    PrintString(args[i]);
  }

  PrintString(String(L"\n"));
}
} // namespace internal

} // namespace attoboy
