#include "attoboy/attoboy.h"
#include <windows.h>

namespace attoboy {

String GetEnv(const String &name) {
  const wchar_t *nameStr = name.c_str();
  DWORD size = GetEnvironmentVariableW(nameStr, nullptr, 0);

  if (size == 0) {
    return String();
  }

  wchar_t *buffer = (wchar_t *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                         size * sizeof(wchar_t));
  if (!buffer) {
    return String();
  }

  DWORD result = GetEnvironmentVariableW(nameStr, buffer, size);
  String value;
  if (result > 0 && result < size) {
    value = String(buffer);
  }

  HeapFree(GetProcessHeap(), 0, buffer);
  return value;
}

bool SetEnv(const String &name, const String &value) {
  const wchar_t *nameStr = name.c_str();
  const wchar_t *valueStr = value.c_str();

  BOOL result = SetEnvironmentVariableW(nameStr, valueStr);
  return result != 0;
}

} // namespace attoboy
