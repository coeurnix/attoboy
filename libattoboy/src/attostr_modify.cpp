#include "attostr_internal.h"

namespace attoboy {

String &String::append(const String &substring) {
  if (!impl)
    return *this;
  WriteLockGuard guard(&impl->lock);
  if (!substring.impl || substring.impl->len == 0)
    return *this;

  int newLen = impl->len + substring.impl->len;
  LPWSTR newData = AllocString(newLen);

  lstrcpyW(newData, impl->data);
  lstrcpyW(newData + impl->len, substring.impl->data);

  FreeString(impl->data);
  impl->data = newData;
  impl->len = newLen;
  return *this;
}

String &String::prepend(const String &substring) {
  if (!impl)
    return *this;
  WriteLockGuard guard(&impl->lock);
  if (!substring.impl || substring.impl->len == 0)
    return *this;

  int newLen = impl->len + substring.impl->len;
  LPWSTR newData = AllocString(newLen);

  lstrcpyW(newData, substring.impl->data);
  lstrcpyW(newData + substring.impl->len, impl->data);

  FreeString(impl->data);
  impl->data = newData;
  impl->len = newLen;
  return *this;
}

String &String::insert(int index, const String &substring) {
  if (!impl)
    return *this;
  WriteLockGuard guard(&impl->lock);
  if (!substring.impl || substring.impl->len == 0)
    return *this;
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

  FreeString(impl->data);
  impl->data = newData;
  impl->len = newLen;
  return *this;
}

String &String::remove(int start, int end) {
  if (!impl)
    return *this;
  WriteLockGuard guard(&impl->lock);
  if (impl->len == 0)
    return *this;

  if (start < 0)
    start = impl->len + start;
  if (start < 0)
    start = 0;
  if (start >= impl->len)
    return *this;

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
    return *this;

  int newLen = impl->len - removeLen;
  LPWSTR newData = AllocString(newLen);

  if (start > 0) {
    MyWcsNCpy(newData, impl->data, start);
  }
  if (actualEnd < impl->len) {
    lstrcpyW(newData + start, impl->data + actualEnd);
  }
  newData[newLen] = L'\0';

  FreeString(impl->data);
  impl->data = newData;
  impl->len = newLen;
  return *this;
}

} // namespace attoboy
