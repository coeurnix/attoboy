#include "attodatetime_internal.h"

namespace attoboy {

long long DateTime::timestamp() const {
  if (!impl)
    return 0;

  ReadLockGuard guard(&impl->lock);

  ULARGE_INTEGER uli;
  uli.LowPart = impl->fileTime.dwLowDateTime;
  uli.HighPart = impl->fileTime.dwHighDateTime;

  const long long UNIX_EPOCH_FILETIME = 116444736000000000LL;
  long long fileTime100ns = (long long)uli.QuadPart;
  long long millisSinceEpoch = Div64(fileTime100ns - UNIX_EPOCH_FILETIME, 10000LL);

  return millisSinceEpoch;
}

String DateTime::toString() const {
  if (!impl)
    return String();

  ReadLockGuard guard(&impl->lock);

  SYSTEMTIME st;
  if (!FileTimeToSystemTime(&impl->fileTime, &st)) {
    return String();
  }

  wchar_t buffer[128];
  wsprintfW(buffer, L"%04d-%02d-%02dT%02d:%02d:%02d.%03dZ", st.wYear, st.wMonth,
            st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

  return String(buffer);
}

} // namespace attoboy
