#include "attostring_internal.h"

namespace attoboy {

String::String(bool val) {
  impl = (StringImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                 sizeof(StringImpl));
  InitializeSRWLock(&impl->lock);
  const WCHAR *s = val ? L"true" : L"false";
  int len = lstrlenW(s);
  impl->data = AllocString(len);
  lstrcpyW(impl->data, s);
  impl->len = len;
}

String::String(int val) {
  impl = (StringImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                 sizeof(StringImpl));
  InitializeSRWLock(&impl->lock);
  WCHAR buf[32];
  wsprintfW(buf, L"%d", val);
  int len = lstrlenW(buf);
  impl->data = AllocString(len);
  lstrcpyW(impl->data, buf);
  impl->len = len;
}

String::String(long long val) {
  impl = (StringImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                 sizeof(StringImpl));
  InitializeSRWLock(&impl->lock);
  WCHAR buf[64];
  wsprintfW(buf, L"%I64d", val);
  int len = lstrlenW(buf);
  impl->data = AllocString(len);
  lstrcpyW(impl->data, buf);
  impl->len = len;
}

} // namespace attoboy
