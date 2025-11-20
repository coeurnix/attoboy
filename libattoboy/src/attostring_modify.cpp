#include "attostring_internal.h"

namespace attoboy {

String String::append(const String &substring) const {
  if (!impl)
    return String();
  ReadLockGuard guard(&impl->lock);
  if (!substring.impl || substring.impl->len == 0)
    return String(*this);

  int newLen = impl->len + substring.impl->len;
  LPWSTR newData = AllocString(newLen);

  lstrcpyW(newData, impl->data);
  lstrcpyW(newData + impl->len, substring.impl->data);

  String result;
  FreeString(result.impl->data);
  result.impl->data = newData;
  result.impl->len = newLen;
  return result;
}

String String::prepend(const String &substring) const {
  if (!impl)
    return String();
  ReadLockGuard guard(&impl->lock);
  if (!substring.impl || substring.impl->len == 0)
    return String(*this);

  int newLen = impl->len + substring.impl->len;
  LPWSTR newData = AllocString(newLen);

  lstrcpyW(newData, substring.impl->data);
  lstrcpyW(newData + substring.impl->len, impl->data);

  String result;
  FreeString(result.impl->data);
  result.impl->data = newData;
  result.impl->len = newLen;
  return result;
}

String String::insert(int index, const String &substring) const {
  if (!impl)
    return String();
  ReadLockGuard guard(&impl->lock);
  if (!substring.impl || substring.impl->len == 0)
    return String(*this);
  if (index < 0)
    index = impl->len + index;
  if (index < 0)
    index = 0;
  if (index > impl->len)
    index = impl->len;

  int newLen = impl->len + substring.impl->len;
  LPWSTR newData = AllocString(newLen);

  MyWcsNCpy(newData, impl->data, index);
  lstrcpyW(newData + index, substring.impl->data);
  lstrcpyW(newData + index + substring.impl->len, impl->data + index);

  String result;
  FreeString(result.impl->data);
  result.impl->data = newData;
  result.impl->len = newLen;
  return result;
}

String String::remove(int start, int end) const {
  if (!impl)
    return String();
  ReadLockGuard guard(&impl->lock);
  if (impl->len == 0)
    return String(*this);

  if (start < 0)
    start = impl->len + start;
  if (start < 0)
    start = 0;
  if (start >= impl->len)
    return String(*this);

  int actualEnd;
  if (end == -1)
    actualEnd = impl->len;
  else {
    if (end < 0)
      actualEnd = impl->len + end;
    else
      actualEnd = end;
  }

  if (actualEnd < start)
    actualEnd = start;
  if (actualEnd > impl->len)
    actualEnd = impl->len;

  int removeLen = actualEnd - start;
  if (removeLen <= 0)
    return String(*this);

  int newLen = impl->len - removeLen;
  LPWSTR newData = AllocString(newLen);

  if (start > 0) {
    MyWcsNCpy(newData, impl->data, start);
  }
  if (actualEnd < impl->len) {
    lstrcpyW(newData + start, impl->data + actualEnd);
  }
  newData[newLen] = L'\0';

  String result;
  FreeString(result.impl->data);
  result.impl->data = newData;
  result.impl->len = newLen;
  return result;
}

String String::operator+(const String &other) const {
  return append(other);
}

} // namespace attoboy
