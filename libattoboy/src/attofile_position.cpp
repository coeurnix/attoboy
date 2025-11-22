#include "attofile_internal.h"

namespace attoboy {

bool File::setPosition(long long pos) {
  if (!impl || !impl->isOpen || !impl->isValid)
    return false;

  WriteLockGuard lock(&impl->lock);

  if (impl->type != FILE_TYPE_REGULAR)
    return false;

  LARGE_INTEGER distance;
  distance.QuadPart = pos;

  return SetFilePointerEx(impl->handle, distance, nullptr, FILE_BEGIN) != 0;
}

long long File::getPosition() const {
  if (!impl || !impl->isOpen || !impl->isValid)
    return -1;

  ReadLockGuard lock(&impl->lock);

  if (impl->type != FILE_TYPE_REGULAR)
    return -1;

  LARGE_INTEGER distance;
  distance.QuadPart = 0;

  LARGE_INTEGER newPos;
  if (!SetFilePointerEx(impl->handle, distance, &newPos, FILE_CURRENT))
    return -1;

  return newPos.QuadPart;
}

long long File::getEndPosition() const {
  if (!impl || !impl->isOpen || !impl->isValid)
    return -1;

  ReadLockGuard lock(&impl->lock);

  if (impl->type != FILE_TYPE_REGULAR)
    return -1;

  LARGE_INTEGER fileSize;
  if (!GetFileSizeEx(impl->handle, &fileSize))
    return -1;

  return fileSize.QuadPart;
}

bool File::isAtEnd() const {
  if (!impl || !impl->isOpen || !impl->isValid)
    return false;

  ReadLockGuard lock(&impl->lock);

  if (impl->type != FILE_TYPE_REGULAR)
    return false;

  LARGE_INTEGER fileSize;
  if (!GetFileSizeEx(impl->handle, &fileSize))
    return false;

  LARGE_INTEGER distance;
  distance.QuadPart = 0;

  LARGE_INTEGER currentPos;
  if (!SetFilePointerEx(impl->handle, distance, &currentPos, FILE_CURRENT))
    return false;

  return currentPos.QuadPart >= fileSize.QuadPart;
}

} // namespace attoboy
