#include "attostr_internal.h"

// Needed for floating point support when not linking with CRT
extern "C" {
int _fltused = 0;
}

namespace attoboy {

// Helper for double to string conversion
static void DoubleToString(double val, LPWSTR buffer, int maxLen) {
  if (val < 0) {
    *buffer++ = L'-';
    val = -val;
    maxLen--;
  }

  long long intPart = (long long)val;
  double fracPart = val - intPart;

  int len = wsprintfW(buffer, L"%I64d", intPart);
  buffer += len;
  maxLen -= len;

  if (maxLen > 1) {
    *buffer++ = L'.';
    maxLen--;

    for (int i = 0; i < 6 && maxLen > 0; i++) {
      fracPart *= 10;
      int digit = (int)fracPart;
      *buffer++ = L'0' + digit;
      fracPart -= digit;
      maxLen--;
    }
    *buffer = L'\0';
  }
}

String::String(double val) {
  impl = (StringImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                 sizeof(StringImpl));
  WCHAR buf[64];
  DoubleToString(val, buf, 64);
  int len = lstrlenW(buf);
  impl->data = AllocString(len);
  lstrcpyW(impl->data, buf);
  impl->len = len;
}

} // namespace attoboy
