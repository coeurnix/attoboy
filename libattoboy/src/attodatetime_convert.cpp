#include "attodatetime_internal.h"
#include "attostring_internal.h"

namespace attoboy {

long long DateTime::timestamp() const {
  if (!impl)
    return 0;

  ReadLockGuard guard(&impl->lock);

#ifdef _MSC_VER
  ULARGE_INTEGER uli;
  uli.LowPart = impl->fileTime.dwLowDateTime;
  uli.HighPart = impl->fileTime.dwHighDateTime;

  const ULARGE_INTEGER UNIX_EPOCH_FILETIME = {
      {0xD53E8000, 0x019DB1DE}}; // 116444736000000000LL

  ULARGE_INTEGER fileTimeSinceEpoch;
  fileTimeSinceEpoch.QuadPart = uli.QuadPart - UNIX_EPOCH_FILETIME.QuadPart;

  long long millisSinceEpoch =
      Div64((long long)fileTimeSinceEpoch.QuadPart, 10000LL);

  return millisSinceEpoch;

#else
  ULARGE_INTEGER uli;
  uli.LowPart = impl->fileTime.dwLowDateTime;
  uli.HighPart = impl->fileTime.dwHighDateTime;

  const long long UNIX_EPOCH_FILETIME = 116444736000000000LL;
  long long fileTime100ns = (long long)uli.QuadPart;
  long long millisSinceEpoch =
      Div64(fileTime100ns - UNIX_EPOCH_FILETIME, 10000LL);

  return millisSinceEpoch;
#endif
}

String DateTime::toString() const {
  if (!impl)
    return String();

  ReadLockGuard guard(&impl->lock);

  SYSTEMTIME st;
  if (!FileTimeToSystemTime(&impl->fileTime, &st)) {
    return String();
  }

  ATTO_WCHAR buffer[128];
#ifdef UNICODE
  wsprintfW(buffer, L"%04d-%02d-%02dT%02d:%02d:%02d.%03dZ", st.wYear, st.wMonth,
            st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#else
  wsprintfA(buffer, "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ", st.wYear, st.wMonth,
            st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#endif

  return String(buffer);
}

} // namespace attoboy
