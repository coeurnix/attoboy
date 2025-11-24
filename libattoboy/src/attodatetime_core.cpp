#include "attodatetime_internal.h"
#include "attostring_internal.h"
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

#ifdef _MSC_VER
  // MSVC 32-bit build: avoid 64-bit multiplication that triggers __allmul
  // Use ULARGE_INTEGER arithmetic which is safe in Windows API

  const ULARGE_INTEGER UNIX_EPOCH_FILETIME = {
      {0xD53E8000, 0x019DB1DE}}; // 116444736000000000LL

  // Convert to 100ns units by multiplying by 10000
  ULARGE_INTEGER millis100ns;
  millis100ns.QuadPart = (ULONGLONG)millisSinceEpoch * 10000;

  // Add the Unix epoch offset
  ULARGE_INTEGER fileTime100ns;
  fileTime100ns.QuadPart = UNIX_EPOCH_FILETIME.QuadPart + millis100ns.QuadPart;

  impl->fileTime.dwLowDateTime = fileTime100ns.LowPart;
  impl->fileTime.dwHighDateTime = fileTime100ns.HighPart;

#else
  // Non-MSVC builds: use the original simple implementation
  const long long UNIX_EPOCH_FILETIME = 116444736000000000LL;
  long long fileTime100ns = UNIX_EPOCH_FILETIME + (millisSinceEpoch * 10000LL);

  impl->fileTime.dwLowDateTime = (DWORD)fileTime100ns;
  impl->fileTime.dwHighDateTime = (DWORD)(fileTime100ns >> 32);
#endif
}

DateTime::DateTime(const String &iso8601) {
  impl = (DateTimeImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                   sizeof(DateTimeImpl));
  if (!impl)
    return;

  new (impl) DateTimeImpl();

  const ATTO_CHAR *str = iso8601.c_str();
  SYSTEMTIME st = {0};

#ifdef UNICODE
  int len = lstrlenW(str);
#else
  int len = lstrlenA(str);
#endif
  if (len >= 19) {
    st.wYear = (str[0] - ATTO_TEXT('0')) * 1000 +
               (str[1] - ATTO_TEXT('0')) * 100 +
               (str[2] - ATTO_TEXT('0')) * 10 + (str[3] - ATTO_TEXT('0'));
    st.wMonth = (str[5] - ATTO_TEXT('0')) * 10 + (str[6] - ATTO_TEXT('0'));
    st.wDay = (str[8] - ATTO_TEXT('0')) * 10 + (str[9] - ATTO_TEXT('0'));
    st.wHour = (str[11] - ATTO_TEXT('0')) * 10 + (str[12] - ATTO_TEXT('0'));
    st.wMinute = (str[14] - ATTO_TEXT('0')) * 10 + (str[15] - ATTO_TEXT('0'));
    st.wSecond = (str[17] - ATTO_TEXT('0')) * 10 + (str[18] - ATTO_TEXT('0'));

    if (len >= 24 && str[19] == ATTO_TEXT('.')) {
      st.wMilliseconds = (str[20] - ATTO_TEXT('0')) * 100 +
                         (str[21] - ATTO_TEXT('0')) * 10 +
                         (str[22] - ATTO_TEXT('0'));
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
