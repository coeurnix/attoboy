#include "attostr_internal.h"

namespace attoboy {

String &String::trim() {
  if (!impl)
    return *this;
  WriteLockGuard guard(&impl->lock);
  if (!impl->data || impl->len == 0)
    return *this;

  int start = 0;
  while (start < impl->len && impl->data[start] <= L' ')
    start++;

  int end = impl->len - 1;
  while (end > start && impl->data[end] <= L' ')
    end--;

  int newLen = end - start + 1;
  if (newLen < 0)
    newLen = 0;

  if (start > 0 || newLen < impl->len) {
    LPWSTR newData = AllocString(newLen);
    if (newLen > 0) {
      MyWcsNCpy(newData, impl->data + start, newLen);
    }
    newData[newLen] = L'\0';
    FreeString(impl->data);
    impl->data = newData;
    impl->len = newLen;
  }
  return *this;
}

String &String::replace(const String &target, const String &replacement) {
  if (!impl)
    return *this;
  WriteLockGuard guard(&impl->lock);
  if (!impl->data || !target.impl || target.impl->len == 0)
    return *this;

  int count = 0;
  WCHAR *p = impl->data;
  int targetLen = target.impl->len;
  int replLen = replacement.impl ? replacement.impl->len : 0;

  while ((p = MyWcsStr(p, target.impl->data)) != nullptr) {
    count++;
    p += targetLen;
  }

  if (count == 0)
    return *this;

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

  FreeString(impl->data);
  impl->data = newData;
  impl->len = newLen;

  return *this;
}

String &String::lower() {
  if (!impl)
    return *this;
  WriteLockGuard guard(&impl->lock);
  if (impl->data) {
    CharLowerW(impl->data);
  }
  return *this;
}

String &String::upper() {
  if (!impl)
    return *this;
  WriteLockGuard guard(&impl->lock);
  if (impl->data) {
    CharUpperW(impl->data);
  }
  return *this;
}

String &String::repeat(int count) {
  if (!impl)
    return *this;
  WriteLockGuard guard(&impl->lock);
  if (count < 0)
    return *this;
  if (count == 0) {
    FreeString(impl->data);
    impl->data = AllocString(0);
    impl->len = 0;
    return *this;
  }
  if (count == 1)
    return *this;

  int newLen = impl->len * count;
  LPWSTR newData = AllocString(newLen);

  for (int i = 0; i < count; i++) {
    lstrcpyW(newData + (i * impl->len), impl->data);
  }

  FreeString(impl->data);
  impl->data = newData;
  impl->len = newLen;
  return *this;
}

String &String::reverse() {
  if (!impl)
    return *this;
  WriteLockGuard guard(&impl->lock);
  if (impl->len <= 1)
    return *this;
  int start = 0;
  int end = impl->len - 1;
  while (start < end) {
    WCHAR temp = impl->data[start];
    impl->data[start] = impl->data[end];
    impl->data[end] = temp;
    start++;
    end--;
  }
  return *this;
}

} // namespace attoboy
