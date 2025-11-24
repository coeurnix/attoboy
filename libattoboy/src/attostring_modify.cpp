#include "attostring_internal.h"

namespace attoboy {

String String::append(const String &substring) const {
  if (!impl)
    return String();
  ReadLockGuard guard(&impl->lock);
  if (!substring.impl || substring.impl->len == 0)
    return String(*this);

  int newLen = impl->len + substring.impl->len;
  ATTO_LPSTR newData = AllocString(newLen);

  ATTO_LSTRCPY(newData, impl->data);
  ATTO_LSTRCPY(newData + impl->len, substring.impl->data);

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
  ATTO_LPSTR newData = AllocString(newLen);

  ATTO_LSTRCPY(newData, substring.impl->data);
  ATTO_LSTRCPY(newData + substring.impl->len, impl->data);

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

  int charLen = countUTF8Characters(impl->data, impl->len);
  if (index < 0)
    index = charLen + index;
  if (index < 0)
    index = 0;
  if (index > charLen)
    index = charLen;

  int byteIndex = getCharacterByteIndex(impl->data, index, impl->len);
  if (byteIndex < 0)
    byteIndex = impl->len;

  int newLen = impl->len + substring.impl->len;
  ATTO_LPSTR newData = AllocString(newLen);

  MyStrNCpy(newData, impl->data, byteIndex);
  ATTO_LSTRCPY(newData + byteIndex, substring.impl->data);
  ATTO_LSTRCPY(newData + byteIndex + substring.impl->len,
               impl->data + byteIndex);

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

  int charLen = countUTF8Characters(impl->data, impl->len);
  if (start < 0)
    start = charLen + start;
  if (start < 0)
    start = 0;
  if (start >= charLen)
    return String(*this);

  int actualEnd;
  if (end == -1)
    actualEnd = charLen;
  else {
    if (end < 0)
      actualEnd = charLen + end;
    else
      actualEnd = end;
  }

  if (actualEnd < start)
    actualEnd = start;
  if (actualEnd > charLen)
    actualEnd = charLen;

  int startByte = getCharacterByteIndex(impl->data, start, impl->len);
  int endByte = getCharacterByteIndex(impl->data, actualEnd, impl->len);
  if (startByte < 0 || endByte < 0 || endByte < startByte)
    return String(*this);

  int removeLen = endByte - startByte;
  if (removeLen <= 0)
    return String(*this);

  int newLen = impl->len - removeLen;
  ATTO_LPSTR newData = AllocString(newLen);

  if (startByte > 0) {
    MyStrNCpy(newData, impl->data, startByte);
  }
  if (endByte < impl->len) {
    ATTO_LSTRCPY(newData + startByte, impl->data + endByte);
  }
  newData[newLen] = ATTO_TEXT('\0');

  String result;
  FreeString(result.impl->data);
  result.impl->data = newData;
  result.impl->len = newLen;
  return result;
}

String String::operator+(const String &other) const { return append(other); }

} // namespace attoboy
