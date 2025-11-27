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

String::String(char val) {
  impl = (StringImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                 sizeof(StringImpl));
  InitializeSRWLock(&impl->lock);
  char buf[2] = {val, '\0'};
  impl->data = AllocString(1);
  if (impl->data) {
    lstrcpyA(impl->data, buf);
    impl->len = 1;
  } else {
    impl->len = 0;
  }
}

String::String(long long val) {
  impl = (StringImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                 sizeof(StringImpl));
  InitializeSRWLock(&impl->lock);
  ATTO_WCHAR buf[32];

  bool negative = val < 0;
  unsigned long long absVal =
      negative ? -(unsigned long long)val : (unsigned long long)val;

  if (absVal <= 0xFFFFFFFFULL) {
    if (negative) {
      wsprintfA(buf, "-%u", (unsigned int)absVal);
    } else {
      wsprintfA(buf, "%u", (unsigned int)absVal);
    }
  } else {
    unsigned int high = (unsigned int)(absVal >> 32);
    unsigned int low = (unsigned int)(absVal & 0xFFFFFFFFULL);

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
