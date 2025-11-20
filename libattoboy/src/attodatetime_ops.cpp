#include "attodatetime_internal.h"

namespace attoboy {

DateTime &DateTime::add(long long milliseconds) {
  if (!impl)
    return *this;

  WriteLockGuard guard(&impl->lock);

  ULARGE_INTEGER uli;
  uli.LowPart = impl->fileTime.dwLowDateTime;
  uli.HighPart = impl->fileTime.dwHighDateTime;

  long long fileTime100ns = (long long)uli.QuadPart;
  fileTime100ns += (milliseconds * 10000LL);

  uli.QuadPart = (ULONGLONG)fileTime100ns;
  impl->fileTime.dwLowDateTime = uli.LowPart;
  impl->fileTime.dwHighDateTime = uli.HighPart;

  return *this;
}

long long DateTime::diff(const DateTime &other) const {
  if (!impl || !other.impl)
    return 0;

  ReadLockGuard guardThis(&impl->lock);
  ReadLockGuard guardOther(&other.impl->lock);

  ULARGE_INTEGER uliThis, uliOther;
  uliThis.LowPart = impl->fileTime.dwLowDateTime;
  uliThis.HighPart = impl->fileTime.dwHighDateTime;
  uliOther.LowPart = other.impl->fileTime.dwLowDateTime;
  uliOther.HighPart = other.impl->fileTime.dwHighDateTime;

  long long diff100ns = (long long)uliThis.QuadPart - (long long)uliOther.QuadPart;
  return Div64(diff100ns, 10000LL);
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

bool DateTime::operator==(const DateTime &other) const {
  return equals(other);
}

bool DateTime::operator!=(const DateTime &other) const {
  return !equals(other);
}

} // namespace attoboy
