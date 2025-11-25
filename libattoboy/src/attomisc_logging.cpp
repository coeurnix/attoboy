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
    DWORD bytesToWrite = s.byteLength() * sizeof(ATTO_CHAR);
    WriteFile(g_logFileHandle, s.c_str(), bytesToWrite, &written, nullptr);
    FlushFileBuffers(g_logFileHandle);
  } else {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD written;
    DWORD bytesToWrite = s.byteLength() * sizeof(ATTO_CHAR);

    // Check if handle is a console; if not (e.g., redirected), use WriteFile
    DWORD mode;
    if (GetConsoleMode(hOut, &mode)) {
      WriteConsoleA(hOut, s.c_str(), s.byteLength(), &written, nullptr);
    } else {
      WriteFile(hOut, s.c_str(), bytesToWrite, &written, nullptr);
    }
  }
}

void EnableLoggingToFile(const String &path, bool truncate) {
  if (g_logFileHandle) {
    CloseHandle(g_logFileHandle);
    g_logFileHandle = nullptr;
  }

  DWORD creationDisposition = truncate ? CREATE_ALWAYS : OPEN_ALWAYS;
  g_logFileHandle = CreateFile(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ,
                                nullptr, creationDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);

  if (g_logFileHandle != INVALID_HANDLE_VALUE) {
    if (!truncate) {
      // Append mode: seek to end
      SetFilePointer(g_logFileHandle, 0, nullptr, FILE_END);
    }
    g_logToFile = true;
  } else {
    // File creation failed - fall back to console and report error
    g_logFileHandle = nullptr;
    g_logToFile = false;

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    const char *errMsg = "ERROR: Failed to create log file: ";
    DWORD written;
    WriteFile(hOut, errMsg, (DWORD)lstrlenA(errMsg), &written, nullptr);
    WriteFile(hOut, path.c_str(), path.byteLength(), &written, nullptr);
    WriteFile(hOut, "\n", 1, &written, nullptr);
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
