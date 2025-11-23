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
#ifdef UNICODE
  wsprintfW(buf, L"%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
#else
  wsprintfA(buf, "%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
#endif
  return String(static_cast<const ATTO_CHAR *>(buf));
}

static void PrintString(const String &s) {
  if (g_logToFile && g_logFileHandle) {
    DWORD written;
#ifdef UNICODE
    int utf8Len = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), s.length(), nullptr, 0, nullptr, nullptr);
    if (utf8Len > 0) {
      char *utf8Buf = (char *)HeapAlloc(GetProcessHeap(), 0, utf8Len);
      if (utf8Buf) {
        WideCharToMultiByte(CP_UTF8, 0, s.c_str(), s.length(), utf8Buf, utf8Len, nullptr, nullptr);
        WriteFile(g_logFileHandle, utf8Buf, utf8Len, &written, nullptr);
        HeapFree(GetProcessHeap(), 0, utf8Buf);
      }
    }
#else
    WriteFile(g_logFileHandle, s.c_str(), s.length(), &written, nullptr);
#endif
    FlushFileBuffers(g_logFileHandle);
  } else {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD written;
#ifdef UNICODE
    WriteConsoleW(hOut, s.c_str(), s.length(), &written, nullptr);
#else
    WriteConsoleA(hOut, s.c_str(), s.length(), &written, nullptr);
#endif
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
