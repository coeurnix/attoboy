#include "attostring_internal.h"

// Needed for floating point support when not linking with CRT
extern "C" {
int _fltused = 0;
}

namespace attoboy {

// Helper for float to string conversion
static void FloatToString(float val, ATTO_LPSTR buffer, int maxLen) {
  if (val < 0) {
    *buffer++ = ATTO_TEXT('-');
    val = -val;
    maxLen--;
  }

  int intPart = (int)val;
  float fracPart = val - intPart;

#ifdef UNICODE
  int len = wsprintfW(buffer, L"%d", intPart);
#else
  int len = wsprintfA(buffer, "%d", intPart);
#endif
  buffer += len;
  maxLen -= len;

  if (maxLen > 1) {
    *buffer++ = ATTO_TEXT('.');
    maxLen--;

    ATTO_LPSTR firstDecimalDigit = buffer;
    for (int i = 0; i < 6 && maxLen > 0; i++) {
      fracPart *= 10;
      int digit = (int)fracPart;
      *buffer++ = ATTO_TEXT('0') + digit;
      fracPart -= digit;
      maxLen--;
    }

    // Strip trailing zeros, keeping at least one decimal digit
    buffer--;
    while (buffer > firstDecimalDigit && *buffer == ATTO_TEXT('0')) {
      buffer--;
    }
    buffer++;
    *buffer = ATTO_TEXT('\0');
  }
}

String::String(float val) {
  impl = (StringImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                 sizeof(StringImpl));
  InitializeSRWLock(&impl->lock);
  ATTO_WCHAR buf[64];
  FloatToString(val, buf, 64);
#ifdef UNICODE
  int len = lstrlenW(buf);
#else
  int len = lstrlenA(buf);
#endif
  impl->data = AllocString(len);
#ifdef UNICODE
  lstrcpyW(impl->data, buf);
#else
  lstrcpyA(impl->data, buf);
#endif
  impl->len = len;
}

} // namespace attoboy
