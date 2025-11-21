#include "attoboy/attoboy.h"
#include "attostring_internal.h"
#include <windows.h>

namespace attoboy {

String GetEnv(const String &name) {
  const ATTO_CHAR *nameStr = name.c_str();
#ifdef UNICODE
  DWORD size = GetEnvironmentVariableW(nameStr, nullptr, 0);
#else
  DWORD size = GetEnvironmentVariableA(nameStr, nullptr, 0);
#endif

  if (size == 0) {
    return String();
  }

  ATTO_WCHAR *buffer = (ATTO_WCHAR *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                         size * sizeof(ATTO_WCHAR));
  if (!buffer) {
    return String();
  }

#ifdef UNICODE
  DWORD result = GetEnvironmentVariableW(nameStr, buffer, size);
#else
  DWORD result = GetEnvironmentVariableA(nameStr, buffer, size);
#endif
  String value;
  if (result > 0 && result < size) {
    value = String(buffer);
  }

  HeapFree(GetProcessHeap(), 0, buffer);
  return value;
}

bool SetEnv(const String &name, const String &value) {
  const ATTO_CHAR *nameStr = name.c_str();
  const ATTO_CHAR *valueStr = value.c_str();

#ifdef UNICODE
  BOOL result = SetEnvironmentVariableW(nameStr, valueStr);
#else
  BOOL result = SetEnvironmentVariableA(nameStr, valueStr);
#endif
  return result != 0;
}

} // namespace attoboy
