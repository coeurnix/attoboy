#include "attopath_internal.h"
#include "attodatetime_internal.h"

namespace attoboy {

long long Path::getSize() const {
  if (!impl || !impl->pathStr)
    return 0;

  ReadLockGuard guard(&impl->lock);

  WIN32_FILE_ATTRIBUTE_DATA fileInfo;
  if (!GetFileAttributesEx(impl->pathStr, GetFileExInfoStandard, &fileInfo))
    return 0;

  if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    return 0;

  ULARGE_INTEGER size;
  size.LowPart = fileInfo.nFileSizeLow;
  size.HighPart = fileInfo.nFileSizeHigh;
  return (long long)size.QuadPart;
}

DateTime Path::getCreatedOn() const {
  if (!impl || !impl->pathStr)
    return DateTime(0LL);

  ReadLockGuard guard(&impl->lock);

  WIN32_FILE_ATTRIBUTE_DATA fileInfo;
  if (!GetFileAttributesEx(impl->pathStr, GetFileExInfoStandard, &fileInfo))
    return DateTime(0LL);

  ULARGE_INTEGER uli;
  uli.LowPart = fileInfo.ftCreationTime.dwLowDateTime;
  uli.HighPart = fileInfo.ftCreationTime.dwHighDateTime;

  long long ticks = (long long)uli.QuadPart;
  long long millisSinceEpoch = Div64(ticks, 10000LL) - 11644473600000LL;

  return DateTime(millisSinceEpoch);
}

DateTime Path::getModifiedOn() const {
  if (!impl || !impl->pathStr)
    return DateTime(0LL);

  ReadLockGuard guard(&impl->lock);

  WIN32_FILE_ATTRIBUTE_DATA fileInfo;
  if (!GetFileAttributesEx(impl->pathStr, GetFileExInfoStandard, &fileInfo))
    return DateTime(0LL);

  ULARGE_INTEGER uli;
  uli.LowPart = fileInfo.ftLastWriteTime.dwLowDateTime;
  uli.HighPart = fileInfo.ftLastWriteTime.dwHighDateTime;

  long long ticks = (long long)uli.QuadPart;
  long long millisSinceEpoch = Div64(ticks, 10000LL) - 11644473600000LL;

  return DateTime(millisSinceEpoch);
}

DateTime Path::getAccessedOn() const {
  if (!impl || !impl->pathStr)
    return DateTime(0LL);

  ReadLockGuard guard(&impl->lock);

  WIN32_FILE_ATTRIBUTE_DATA fileInfo;
  if (!GetFileAttributesEx(impl->pathStr, GetFileExInfoStandard, &fileInfo))
    return DateTime(0LL);

  ULARGE_INTEGER uli;
  uli.LowPart = fileInfo.ftLastAccessTime.dwLowDateTime;
  uli.HighPart = fileInfo.ftLastAccessTime.dwHighDateTime;

  long long ticks = (long long)uli.QuadPart;
  long long millisSinceEpoch = Div64(ticks, 10000LL) - 11644473600000LL;

  return DateTime(millisSinceEpoch);
}

bool Path::isReadOnly() const {
  if (!impl || !impl->pathStr)
    return false;

  ReadLockGuard guard(&impl->lock);
  DWORD attrs = GetFileAttributes(impl->pathStr);
  if (attrs == INVALID_FILE_ATTRIBUTES)
    return false;

  return (attrs & FILE_ATTRIBUTE_READONLY) != 0;
}

bool Path::setReadOnly(bool isReadOnly) const {
  if (!impl || !impl->pathStr)
    return false;

  ReadLockGuard guard(&impl->lock);
  DWORD attrs = GetFileAttributes(impl->pathStr);
  if (attrs == INVALID_FILE_ATTRIBUTES)
    return false;

  if (isReadOnly) {
    attrs |= FILE_ATTRIBUTE_READONLY;
  } else {
    attrs &= ~FILE_ATTRIBUTE_READONLY;
  }

  return SetFileAttributes(impl->pathStr, attrs) != 0;
}

bool Path::isHidden() const {
  if (!impl || !impl->pathStr)
    return false;

  ReadLockGuard guard(&impl->lock);
  DWORD attrs = GetFileAttributes(impl->pathStr);
  if (attrs == INVALID_FILE_ATTRIBUTES)
    return false;

  return (attrs & FILE_ATTRIBUTE_HIDDEN) != 0;
}

bool Path::setHidden(bool isHidden) const {
  if (!impl || !impl->pathStr)
    return false;

  ReadLockGuard guard(&impl->lock);
  DWORD attrs = GetFileAttributes(impl->pathStr);
  if (attrs == INVALID_FILE_ATTRIBUTES)
    return false;

  if (isHidden) {
    attrs |= FILE_ATTRIBUTE_HIDDEN;
  } else {
    attrs &= ~FILE_ATTRIBUTE_HIDDEN;
  }

  return SetFileAttributes(impl->pathStr, attrs) != 0;
}

} // namespace attoboy
