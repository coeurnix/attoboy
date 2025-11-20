#include "attostring_internal.h"

namespace attoboy {

String String::trim() const {
  if (!impl)
    return String();
  ReadLockGuard guard(&impl->lock);
  if (!impl->data || impl->len == 0)
    return String(*this);

  int start = 0;
  while (start < impl->len && impl->data[start] <= L' ')
    start++;

  int end = impl->len - 1;
  while (end > start && impl->data[end] <= L' ')
    end--;

  int newLen = end - start + 1;
  if (newLen < 0)
    newLen = 0;

  LPWSTR newData = AllocString(newLen);
  if (newLen > 0) {
    MyWcsNCpy(newData, impl->data + start, newLen);
  }
  newData[newLen] = L'\0';

  String result;
  FreeString(result.impl->data);
  result.impl->data = newData;
  result.impl->len = newLen;
  return result;
}

String String::replace(const String &target, const String &replacement) const {
  if (!impl)
    return String();
  ReadLockGuard guard(&impl->lock);
  if (!impl->data || !target.impl || target.impl->len == 0)
    return String(*this);

  int count = 0;
  WCHAR *p = impl->data;
  int targetLen = target.impl->len;
  int replLen = replacement.impl ? replacement.impl->len : 0;

  while ((p = MyWcsStr(p, target.impl->data)) != nullptr) {
    count++;
    p += targetLen;
  }

  if (count == 0)
    return String(*this);

  int newLen = impl->len + count * (replLen - targetLen);
  LPWSTR newData = AllocString(newLen);

  WCHAR *src = impl->data;
  WCHAR *dst = newData;

  while (true) {
    WCHAR *found = MyWcsStr(src, target.impl->data);
    if (!found) {
      lstrcpyW(dst, src);
      break;
    }

    int segmentLen = (int)(found - src);
    MyWcsNCpy(dst, src, segmentLen);
    dst += segmentLen;

    if (replLen > 0) {
      lstrcpyW(dst, replacement.impl->data);
      dst += replLen;
    }

    src = found + targetLen;
  }

  String result;
  FreeString(result.impl->data);
  result.impl->data = newData;
  result.impl->len = newLen;
  return result;
}

String String::lower() const {
  if (!impl)
    return String();
  ReadLockGuard guard(&impl->lock);
  if (!impl->data)
    return String(*this);

  LPWSTR newData = AllocString(impl->len);
  lstrcpyW(newData, impl->data);
  CharLowerW(newData);

  String result;
  FreeString(result.impl->data);
  result.impl->data = newData;
  result.impl->len = impl->len;
  return result;
}

String String::upper() const {
  if (!impl)
    return String();
  ReadLockGuard guard(&impl->lock);
  if (!impl->data)
    return String(*this);

  LPWSTR newData = AllocString(impl->len);
  lstrcpyW(newData, impl->data);
  CharUpperW(newData);

  String result;
  FreeString(result.impl->data);
  result.impl->data = newData;
  result.impl->len = impl->len;
  return result;
}

String String::repeat(int count) const {
  if (!impl)
    return String();
  ReadLockGuard guard(&impl->lock);
  if (count < 0)
    return String(*this);
  if (count == 0)
    return String();
  if (count == 1)
    return String(*this);

  int newLen = impl->len * count;
  LPWSTR newData = AllocString(newLen);

  for (int i = 0; i < count; i++) {
    lstrcpyW(newData + (i * impl->len), impl->data);
  }

  String result;
  FreeString(result.impl->data);
  result.impl->data = newData;
  result.impl->len = newLen;
  return result;
}

String String::reverse() const {
  if (!impl)
    return String();
  ReadLockGuard guard(&impl->lock);
  if (impl->len <= 1)
    return String(*this);

  LPWSTR newData = AllocString(impl->len);
  for (int i = 0; i < impl->len; i++) {
    newData[i] = impl->data[impl->len - 1 - i];
  }
  newData[impl->len] = L'\0';

  String result;
  FreeString(result.impl->data);
  result.impl->data = newData;
  result.impl->len = impl->len;
  return result;
}

} // namespace attoboy
