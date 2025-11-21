#include "attostring_internal.h"

// Needed for floating point support when not linking with CRT
extern "C" {
int _fltused = 0;
}

namespace attoboy {

// Helper for float to string conversion
static void FloatToString(float val, LPWSTR buffer, int maxLen) {
  if (val < 0) {
    *buffer++ = L'-';
    val = -val;
    maxLen--;
  }

  int intPart = (int)val;
  float fracPart = val - intPart;

  int len = wsprintfW(buffer, L"%d", intPart);
  buffer += len;
  maxLen -= len;

  if (maxLen > 1) {
    *buffer++ = L'.';
    maxLen--;

    LPWSTR firstDecimalDigit = buffer;
    for (int i = 0; i < 6 && maxLen > 0; i++) {
      fracPart *= 10;
      int digit = (int)fracPart;
      *buffer++ = L'0' + digit;
      fracPart -= digit;
      maxLen--;
    }

    // Strip trailing zeros, keeping at least one decimal digit
    buffer--;
    while (buffer > firstDecimalDigit && *buffer == L'0') {
      buffer--;
    }
    buffer++;
    *buffer = L'\0';
  }
}

String::String(float val) {
  impl = (StringImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                 sizeof(StringImpl));
  InitializeSRWLock(&impl->lock);
  WCHAR buf[64];
  FloatToString(val, buf, 64);
  int len = lstrlenW(buf);
  impl->data = AllocString(len);
  lstrcpyW(impl->data, buf);
  impl->len = len;
}

} // namespace attoboy
