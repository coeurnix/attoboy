#include "attodatetime_internal.h"

namespace attoboy {

DateTime &DateTime::add(long long milliseconds) {
  if (!impl)
    return *this;

  WriteLockGuard guard(&impl->lock);

#ifdef _MSC_VER
  // MSVC 32-bit build: avoid 64-bit multiplication that triggers __allmul
  ULARGE_INTEGER uli;
  uli.LowPart = impl->fileTime.dwLowDateTime;
  uli.HighPart = impl->fileTime.dwHighDateTime;

  // Add (milliseconds * 10000) to the FILETIME
  ULARGE_INTEGER add100ns;
  ULARGE_INTEGER ms;
  ms.QuadPart = (ULONGLONG)milliseconds;
  add100ns.QuadPart = ms.QuadPart * 10000ULL;

  ULARGE_INTEGER result;
  result.QuadPart = uli.QuadPart + add100ns.QuadPart;

  impl->fileTime.dwLowDateTime = result.LowPart;
  impl->fileTime.dwHighDateTime = result.HighPart;

#else
  // Non-MSVC builds: use the original implementation
  ULARGE_INTEGER uli;
  uli.LowPart = impl->fileTime.dwLowDateTime;
  uli.HighPart = impl->fileTime.dwHighDateTime;

  long long fileTime100ns = (long long)uli.QuadPart;
  fileTime100ns += (milliseconds * 10000LL);

  uli.QuadPart = (ULONGLONG)fileTime100ns;
  impl->fileTime.dwLowDateTime = uli.LowPart;
  impl->fileTime.dwHighDateTime = uli.HighPart;
#endif

  return *this;
}

long long DateTime::diff(const DateTime &other) const {
  if (!impl || !other.impl)
    return 0;

  ReadLockGuard guardThis(&impl->lock);
  ReadLockGuard guardOther(&other.impl->lock);

#ifdef _MSC_VER
  // MSVC 32-bit build: avoid 64-bit subtraction that triggers intrinsics
  ULARGE_INTEGER uliThis, uliOther;
  uliThis.LowPart = impl->fileTime.dwLowDateTime;
  uliThis.HighPart = impl->fileTime.dwHighDateTime;
  uliOther.LowPart = other.impl->fileTime.dwLowDateTime;
  uliOther.HighPart = other.impl->fileTime.dwHighDateTime;

  // Compute difference in 100ns units
  ULARGE_INTEGER diff100ns;
  diff100ns.QuadPart = uliThis.QuadPart - uliOther.QuadPart;

  // Convert to milliseconds
  return Div64((long long)diff100ns.QuadPart, 10000LL);

#else
  // Non-MSVC builds: use the original implementation
  ULARGE_INTEGER uliThis, uliOther;
  uliThis.LowPart = impl->fileTime.dwLowDateTime;
  uliThis.HighPart = impl->fileTime.dwHighDateTime;
  uliOther.LowPart = other.impl->fileTime.dwLowDateTime;
  uliOther.HighPart = other.impl->fileTime.dwHighDateTime;

  long long diff100ns =
      (long long)uliThis.QuadPart - (long long)uliOther.QuadPart;
  return Div64(diff100ns, 10000LL);
#endif
}

int DateTime::compare(const DateTime &other) const {
  if (!impl || !other.impl)
    return 0;

  ReadLockGuard guardThis(&impl->lock);
  ReadLockGuard guardOther(&other.impl->lock);

  LONG result = CompareFileTime(&impl->fileTime, &other.impl->fileTime);
  return (int)result;
}

bool DateTime::equals(const DateTime &other) const {
  return compare(other) == 0;
}

bool DateTime::operator==(const DateTime &other) const { return equals(other); }

bool DateTime::operator!=(const DateTime &other) const {
  return !equals(other);
}

} // namespace attoboy
