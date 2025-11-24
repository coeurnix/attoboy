#include "attoboy/attoboy.h"
#include "attostring_internal.h"
#include <windows.h>

namespace attoboy {

static HANDLE g_logFileHandle = nullptr;
static bool g_logToFile = false;

static String GetCurrentDatetimeString() {
  SYSTEMTIME st;
  GetLocalTime(&st);
  ATTO_WCHAR buf[32];
  wsprintfA(buf, "%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
  return String(static_cast<const ATTO_CHAR *>(buf));
}

static void PrintString(const String &s) {
  if (g_logToFile && g_logFileHandle) {
    DWORD written;
    DWORD bytesToWrite = s.length() * sizeof(ATTO_CHAR);
    WriteFile(g_logFileHandle, s.c_str(), bytesToWrite, &written, nullptr);
    FlushFileBuffers(g_logFileHandle);
  } else {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD written;
    WriteConsoleA(hOut, s.c_str(), s.length(), &written, nullptr);
  }
}

void EnableLoggingToFile(const String &path) {
  if (g_logFileHandle) {
    CloseHandle(g_logFileHandle);
    g_logFileHandle = nullptr;
  }

  g_logFileHandle = CreateFile(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ,
                                nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

  if (g_logFileHandle != INVALID_HANDLE_VALUE) {
    SetFilePointer(g_logFileHandle, 0, nullptr, FILE_END);
    g_logToFile = true;
  } else {
    g_logFileHandle = nullptr;
    g_logToFile = false;
  }
}

void EnableLoggingToConsole() {
  if (g_logFileHandle) {
    CloseHandle(g_logFileHandle);
    g_logFileHandle = nullptr;
  }

  g_logToFile = false;
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
