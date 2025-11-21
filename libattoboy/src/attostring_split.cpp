#include "attolist_internal.h"
#include "attostring_internal.h"


namespace attoboy {

static inline bool IsWhitespace(ATTO_WCHAR c) {
  return c == ATTO_TEXT(' ') || c == ATTO_TEXT('\t') || c == ATTO_TEXT('\n') || c == ATTO_TEXT('\r');
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

  const ATTO_WCHAR *data = impl->data;
  const ATTO_WCHAR *sepData = sep.impl->data;
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
      ATTO_LPSTR part = AllocString(i - start);
      if (part) {
        MyStrNCpy(part, data + start, i - start);
        part[i - start] = ATTO_TEXT('\0');
        result.append(String(part));
        FreeString(part);
      }
      i += sepLen - 1;
      start = i + 1;
      splitCount++;
    }
  }

  ATTO_LPSTR remainder = AllocString(impl->len - start);
  if (remainder) {
    MyStrNCpy(remainder, data + start, impl->len - start);
    remainder[impl->len - start] = ATTO_TEXT('\0');
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

  const ATTO_WCHAR *data = impl->data;
  int start = -1;

  for (int i = 0; i < impl->len; i++) {
    if (IsWhitespace(data[i])) {
      if (start >= 0) {
        ATTO_LPSTR part = AllocString(i - start);
        if (part) {
          MyStrNCpy(part, data + start, i - start);
          part[i - start] = ATTO_TEXT('\0');
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
    ATTO_LPSTR part = AllocString(impl->len - start);
    if (part) {
      MyStrNCpy(part, data + start, impl->len - start);
      part[impl->len - start] = ATTO_TEXT('\0');
      result.append(String(part));
      FreeString(part);
    }
  }

  return result;
}

} // namespace attoboy
