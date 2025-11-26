#include "attostring_internal.h"

namespace attoboy {

String::String(bool val) {
  impl = (StringImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                 sizeof(StringImpl));
  InitializeSRWLock(&impl->lock);
  const ATTO_WCHAR *s = val ? "true" : "false";
  int len = lstrlenA(s);
  impl->data = AllocString(len);
  lstrcpyA(impl->data, s);
  impl->len = len;
}

String::String(int val) {
  impl = (StringImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                 sizeof(StringImpl));
  InitializeSRWLock(&impl->lock);
  ATTO_WCHAR buf[32];
  wsprintfA(buf, "%d", val);
  int len = lstrlenA(buf);
  impl->data = AllocString(len);
  lstrcpyA(impl->data, buf);
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
    if (negative) {
      wsprintfA(buf, "-%u", (unsigned int)absVal);
    } else {
      wsprintfA(buf, "%u", (unsigned int)absVal);
    }
  } else {
    // For larger values, format as high and low parts
    // Using the fact that 2^32 = 4294967296
    unsigned int high = (unsigned int)(absVal >> 32);
    unsigned int low = (unsigned int)(absVal & 0xFFFFFFFFULL);

    // Approximate: value ≈ high * 4294967296 + low
    // Format both parts and combine
    if (negative) {
      wsprintfA(buf, "-%u%010u", high, low);
    } else {
      wsprintfA(buf, "%u%010u", high, low);
    }
  }

  int len = lstrlenA(buf);
  impl->data = AllocString(len);
  lstrcpyA(impl->data, buf);
  impl->len = len;
}

} // namespace attoboy
