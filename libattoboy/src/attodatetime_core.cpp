#include "attodatetime_internal.h"
#include <new>

namespace attoboy {

DateTime::DateTime() {
  impl = (DateTimeImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                   sizeof(DateTimeImpl));
  if (impl) {
    new (impl) DateTimeImpl();
    SYSTEMTIME st;
    GetSystemTime(&st);
    SystemTimeToFileTime(&st, &impl->fileTime);
  }
}

DateTime::DateTime(long long millisSinceEpoch) {
  impl = (DateTimeImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                   sizeof(DateTimeImpl));
  if (!impl)
    return;

  new (impl) DateTimeImpl();

  const long long UNIX_EPOCH_FILETIME = 116444736000000000LL;
  long long fileTime100ns =
      UNIX_EPOCH_FILETIME + (millisSinceEpoch * 10000LL);

  impl->fileTime.dwLowDateTime = (DWORD)fileTime100ns;
  impl->fileTime.dwHighDateTime = (DWORD)(fileTime100ns >> 32);
}

DateTime::DateTime(const String &iso8601) {
  impl = (DateTimeImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                   sizeof(DateTimeImpl));
  if (!impl)
    return;

  new (impl) DateTimeImpl();

  const wchar_t *str = iso8601.c_str();
  SYSTEMTIME st = {0};

  int len = lstrlenW(str);
  if (len >= 19) {
    st.wYear = (str[0] - L'0') * 1000 + (str[1] - L'0') * 100 +
               (str[2] - L'0') * 10 + (str[3] - L'0');
    st.wMonth = (str[5] - L'0') * 10 + (str[6] - L'0');
    st.wDay = (str[8] - L'0') * 10 + (str[9] - L'0');
    st.wHour = (str[11] - L'0') * 10 + (str[12] - L'0');
    st.wMinute = (str[14] - L'0') * 10 + (str[15] - L'0');
    st.wSecond = (str[17] - L'0') * 10 + (str[18] - L'0');

    if (len >= 24 && str[19] == L'.') {
      st.wMilliseconds = (str[20] - L'0') * 100 + (str[21] - L'0') * 10 +
                         (str[22] - L'0');
    }

    SystemTimeToFileTime(&st, &impl->fileTime);
  } else {
    GetSystemTime(&st);
    SystemTimeToFileTime(&st, &impl->fileTime);
  }
}

DateTime::DateTime(const DateTime &other) {
  impl = (DateTimeImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                   sizeof(DateTimeImpl));
  if (!impl)
    return;

  new (impl) DateTimeImpl();

  if (other.impl) {
    ReadLockGuard guard(&other.impl->lock);
    impl->fileTime = other.impl->fileTime;
  }
}

DateTime::~DateTime() {
  if (impl) {
    impl->~DateTimeImpl();
    HeapFree(GetProcessHeap(), 0, impl);
  }
}

DateTime &DateTime::operator=(const DateTime &other) {
  if (this == &other || !impl || !other.impl)
    return *this;

  WriteLockGuard guardThis(&impl->lock);
  ReadLockGuard guardOther(&other.impl->lock);
  impl->fileTime = other.impl->fileTime;

  return *this;
}

} // namespace attoboy
