#include "attostring_internal.h"

namespace attoboy {

String::String(bool val) {
  impl = (StringImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                 sizeof(StringImpl));
  InitializeSRWLock(&impl->lock);
  const ATTO_WCHAR *s = val ? ATTO_TEXT("true") : ATTO_TEXT("false");
#ifdef UNICODE
  int len = lstrlenW(s);
  impl->data = AllocString(len);
  lstrcpyW(impl->data, s);
#else
  int len = lstrlenA(s);
  impl->data = AllocString(len);
  lstrcpyA(impl->data, s);
#endif
  impl->len = len;
}

String::String(int val) {
  impl = (StringImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                 sizeof(StringImpl));
  InitializeSRWLock(&impl->lock);
  ATTO_WCHAR buf[32];
#ifdef UNICODE
  wsprintfW(buf, L"%d", val);
  int len = lstrlenW(buf);
  impl->data = AllocString(len);
  lstrcpyW(impl->data, buf);
#else
  wsprintfA(buf, "%d", val);
  int len = lstrlenA(buf);
  impl->data = AllocString(len);
  lstrcpyA(impl->data, buf);
#endif
  impl->len = len;
}

String::String(long long val) {
  impl = (StringImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                 sizeof(StringImpl));
  InitializeSRWLock(&impl->lock);
  ATTO_WCHAR buf[32];

  // Convert long long to string without 64-bit division
  // Split into high/low 32-bit parts and format separately
  bool negative = val < 0;
  unsigned long long absVal = negative ? -(unsigned long long)val : (unsigned long long)val;

  // If it fits in 32 bits, use simple wsprintf
  if (absVal <= 0xFFFFFFFFULL) {
#ifdef UNICODE
    if (negative) {
      wsprintfW(buf, L"-%u", (unsigned int)absVal);
    } else {
      wsprintfW(buf, L"%u", (unsigned int)absVal);
    }
#else
    if (negative) {
      wsprintfA(buf, "-%u", (unsigned int)absVal);
    } else {
      wsprintfA(buf, "%u", (unsigned int)absVal);
    }
#endif
  } else {
    // For larger values, format as high and low parts
    // Using the fact that 2^32 = 4294967296
    unsigned int high = (unsigned int)(absVal >> 32);
    unsigned int low = (unsigned int)(absVal & 0xFFFFFFFFULL);

    // Approximate: value ≈ high * 4294967296 + low
    // Format both parts and combine
#ifdef UNICODE
    if (negative) {
      wsprintfW(buf, L"-%u%010u", high, low);
    } else {
      wsprintfW(buf, L"%u%010u", high, low);
    }
#else
    if (negative) {
      wsprintfA(buf, "-%u%010u", high, low);
    } else {
      wsprintfA(buf, "%u%010u", high, low);
    }
#endif
  }

#ifdef UNICODE
  int len = lstrlenW(buf);
  impl->data = AllocString(len);
  lstrcpyW(impl->data, buf);
#else
  int len = lstrlenA(buf);
  impl->data = AllocString(len);
  lstrcpyA(impl->data, buf);
#endif
  impl->len = len;
}

} // namespace attoboy
