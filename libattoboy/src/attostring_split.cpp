#include "attolist_internal.h"
#include "attostring_internal.h"


namespace attoboy {

static inline bool IsWhitespace(wchar_t c) {
  return c == L' ' || c == L'\t' || c == L'\n' || c == L'\r';
}

List String::split(const String &sep, int max) const {
  List result;

  if (!impl || !sep.impl) {
    result.append(*this);
    return result;
  }

  ReadLockGuard guard1(&impl->lock);
  ReadLockGuard guard2(&sep.impl->lock);

  if (impl->len == 0) {
    result.append(String());
    return result;
  }

  if (sep.impl->len == 0) {
    result.append(*this);
    return result;
  }

  if (max < 1) {
    max = 1;
  }

  const wchar_t *data = impl->data;
  const wchar_t *sepData = sep.impl->data;
  int sepLen = sep.impl->len;

  int start = 0;
  int splitCount = 0;

  for (int i = 0; i <= impl->len - sepLen; i++) {
    if (splitCount >= max) {
      break;
    }

    bool found = true;
    for (int j = 0; j < sepLen; j++) {
      if (data[i + j] != sepData[j]) {
        found = false;
        break;
      }
    }

    if (found) {
      LPWSTR part = AllocString(i - start);
      if (part) {
        MyWcsNCpy(part, data + start, i - start);
        part[i - start] = L'\0';
        result.append(String(part));
        FreeString(part);
      }
      i += sepLen - 1;
      start = i + 1;
      splitCount++;
    }
  }

  LPWSTR remainder = AllocString(impl->len - start);
  if (remainder) {
    MyWcsNCpy(remainder, data + start, impl->len - start);
    remainder[impl->len - start] = L'\0';
    result.append(String(remainder));
    FreeString(remainder);
  }

  return result;
}

List String::split() const {
  List result;

  if (!impl) {
    return result;
  }

  ReadLockGuard guard(&impl->lock);

  if (impl->len == 0) {
    return result;
  }

  const wchar_t *data = impl->data;
  int start = -1;

  for (int i = 0; i < impl->len; i++) {
    if (IsWhitespace(data[i])) {
      if (start >= 0) {
        LPWSTR part = AllocString(i - start);
        if (part) {
          MyWcsNCpy(part, data + start, i - start);
          part[i - start] = L'\0';
          result.append(String(part));
          FreeString(part);
        }
        start = -1;
      }
    } else {
      if (start < 0) {
        start = i;
      }
    }
  }

  if (start >= 0) {
    LPWSTR part = AllocString(impl->len - start);
    if (part) {
      MyWcsNCpy(part, data + start, impl->len - start);
      part[impl->len - start] = L'\0';
      result.append(String(part));
      FreeString(part);
    }
  }

  return result;
}

} // namespace attoboy
