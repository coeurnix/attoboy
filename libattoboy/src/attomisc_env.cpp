#include "attoboy/attoboy.h"
#include "attostring_internal.h"
#include "atto_internal_common.h"
#include <windows.h>

namespace attoboy {

String GetEnv(const String &name) {
  const ATTO_CHAR *nameStr = name.c_str();
  WCHAR* nameWide = Utf8ToWide(nameStr);
  if (!nameWide) {
    return String();
  }

  DWORD size = GetEnvironmentVariableW(nameWide, nullptr, 0);
  if (size == 0) {
    FreeConvertedString(nameWide);
    return String();
  }

  WCHAR *buffer = (WCHAR *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                      size * sizeof(WCHAR));
  if (!buffer) {
    FreeConvertedString(nameWide);
    return String();
  }

  DWORD result = GetEnvironmentVariableW(nameWide, buffer, size);
  FreeConvertedString(nameWide);

  String value;
  if (result > 0 && result < size) {
    char* valueUtf8 = WideToUtf8(buffer);
    if (valueUtf8) {
      value = String(valueUtf8);
      FreeConvertedString(valueUtf8);
    }
  }

  HeapFree(GetProcessHeap(), 0, buffer);
  return value;
}

bool SetEnv(const String &name, const String &value) {
  const ATTO_CHAR *nameStr = name.c_str();
  const ATTO_CHAR *valueStr = value.c_str();

  WCHAR* nameWide = Utf8ToWide(nameStr);
  if (!nameWide) {
    return false;
  }

  WCHAR* valueWide = Utf8ToWide(valueStr);
  if (!valueWide) {
    FreeConvertedString(nameWide);
    return false;
  }

  BOOL result = SetEnvironmentVariableW(nameWide, valueWide);
  FreeConvertedString(nameWide);
  FreeConvertedString(valueWide);
  return result != 0;
}

} // namespace attoboy
