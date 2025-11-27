#include "attostring_internal.h"

namespace attoboy {

String String::trim() const {
  if (!impl)
    return String();
  ReadLockGuard guard(&impl->lock);
  if (!impl->data || impl->len == 0)
    return String(*this);

  int start = 0;
  while (start < impl->len && impl->data[start] <= ' ')
    start++;

  int end = impl->len - 1;
  while (end > start && impl->data[end] <= ' ')
    end--;

  int newLen = end - start + 1;
  if (newLen < 0)
    newLen = 0;

  ATTO_LPSTR newData = AllocString(newLen);
  if (newLen > 0) {
    MyStrNCpy(newData, impl->data + start, newLen);
  }
  newData[newLen] = '\0';

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
  ATTO_WCHAR *p = impl->data;
  int targetLen = target.impl->len;
  int replLen = replacement.impl ? replacement.impl->len : 0;

  while ((p = MyStrStr(p, target.impl->data)) != nullptr) {
    count++;
    p += targetLen;
  }

  if (count == 0)
    return String(*this);

  int newLen = impl->len + count * (replLen - targetLen);
  ATTO_LPSTR newData = AllocString(newLen);

  ATTO_WCHAR *src = impl->data;
  ATTO_WCHAR *dst = newData;

  while (true) {
    ATTO_WCHAR *found = MyStrStr(src, target.impl->data);
    if (!found) {
      ATTO_LSTRCPY(dst, src);
      break;
    }

    int segmentLen = (int)(found - src);
    MyStrNCpy(dst, src, segmentLen);
    dst += segmentLen;

    if (replLen > 0) {
      ATTO_LSTRCPY(dst, replacement.impl->data);
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

  ATTO_LPSTR newData = AllocString(impl->len);
  ATTO_LSTRCPY(newData, impl->data);
  ATTO_CHARLOWER(newData);

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

  ATTO_LPSTR newData = AllocString(impl->len);
  ATTO_LSTRCPY(newData, impl->data);
  ATTO_CHARUPPER(newData);

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
  ATTO_LPSTR newData = AllocString(newLen);

  for (int i = 0; i < count; i++) {
    ATTO_LSTRCPY(newData + (i * impl->len), impl->data);
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
  if (!impl->data || impl->len == 0)
    return String(*this);

  int charCount = countUTF8Characters(impl->data, impl->len);
  if (charCount <= 1)
    return String(*this);

  String result;
  for (int i = charCount - 1; i >= 0; i--) {
    String charStr = at(i);
    result = result + charStr;
  }
  return result;
}

} // namespace attoboy
