#include "attostr_internal.h"

namespace attoboy {

String String::substring(int start, int end) const {
  if (!impl)
    return String();
  ReadLockGuard guard(&impl->lock);
  if (!impl->data)
    return String();

  int len = impl->len;
  if (start < 0)
    start = len + start;
  if (start < 0)
    start = 0;
  if (start > len)
    start = len;

  int actualEnd;
  if (end == -1) {
    actualEnd = len;
  } else {
    if (end < 0)
      actualEnd = len + end;
    else
      actualEnd = end;
  }

  if (actualEnd < start)
    actualEnd = start;
  if (actualEnd > len)
    actualEnd = len;

  int newLen = actualEnd - start;
  if (newLen <= 0)
    return String();

  String result;
  FreeString(result.impl->data);
  result.impl->data = AllocString(newLen);

  MyWcsNCpy(result.impl->data, impl->data + start, newLen);
  result.impl->data[newLen] = L'\0';
  result.impl->len = newLen;
  return result;
}

String String::at(int index) const {
  if (!impl)
    return String();
  ReadLockGuard guard(&impl->lock);
  if (!impl->data)
    return String();
  if (index < 0)
    index = impl->len + index;
  if (index < 0 || index >= impl->len)
    return String();

  String result;
  FreeString(result.impl->data);
  result.impl->data = AllocString(1);
  result.impl->data[0] = impl->data[index];
  result.impl->data[1] = L'\0';
  result.impl->len = 1;
  return result;
}

bool String::contains(const String &substring) const {
  if (!impl)
    return false;
  ReadLockGuard guard(&impl->lock);
  if (!impl->data || !substring.impl || !substring.impl->data)
    return false;
  if (substring.impl->len == 0)
    return true;
  return MyWcsStr(impl->data, substring.impl->data) != nullptr;
}

double String::toDouble() const {
  if (!impl)
    return 0.0;
  ReadLockGuard guard(&impl->lock);
  if (!impl->data)
    return 0.0;
  double res = 0.0;
  int sign = 1;
  WCHAR *p = impl->data;

  while (*p == L' ')
    p++;
  if (*p == L'-') {
    sign = -1;
    p++;
  } else if (*p == L'+')
    p++;

  while (*p >= L'0' && *p <= L'9') {
    res = res * 10.0 + (*p - L'0');
    p++;
  }

  if (*p == L'.') {
    p++;
    double frac = 0.1;
    while (*p >= L'0' && *p <= L'9') {
      res += (*p - L'0') * frac;
      frac *= 0.1;
      p++;
    }
  }

  return res * sign;
}

long long String::toInteger() const {
  if (!impl)
    return 0;
  ReadLockGuard guard(&impl->lock);
  if (!impl->data)
    return 0;
  long long res = 0;
  int sign = 1;
  WCHAR *p = impl->data;

  while (*p == L' ')
    p++;
  if (*p == L'-') {
    sign = -1;
    p++;
  } else if (*p == L'+')
    p++;

  while (*p >= L'0' && *p <= L'9') {
    res = res * 10 + (*p - L'0');
    p++;
  }
  return res * sign;
}

bool String::toBool() const {
  if (!impl)
    return false;
  ReadLockGuard guard(&impl->lock);
  if (!impl->data)
    return false;
  if (lstrcmpiW(impl->data, L"true") == 0)
    return true;
  if (lstrcmpiW(impl->data, L"t") == 0)
    return true;
  if (lstrcmpiW(impl->data, L"1") == 0)
    return true;
  if (lstrcmpiW(impl->data, L"yes") == 0)
    return true;
  if (lstrcmpiW(impl->data, L"on") == 0)
    return true;
  return false;
}

bool String::isNumber() const {
  if (!impl)
    return false;
  ReadLockGuard guard(&impl->lock);
  if (!impl->data || impl->len == 0)
    return false;
  WCHAR *p = impl->data;
  while (*p == L' ')
    p++;
  if (*p == L'-' || *p == L'+')
    p++;
  if (!*p)
    return false;

  bool hasDigit = false;
  bool hasDot = false;

  while (*p) {
    if (*p >= L'0' && *p <= L'9') {
      hasDigit = true;
    } else if (*p == L'.') {
      if (hasDot)
        return false;
      hasDot = true;
    } else {
      return false;
    }
    p++;
  }
  return hasDigit;
}

bool String::startsWith(const String &substring) const {
  if (!impl)
    return false;
  ReadLockGuard guard(&impl->lock);
  if (!substring.impl)
    return false;
  if (substring.impl->len > impl->len)
    return false;
  if (substring.impl->len == 0)
    return true;
  return MyWcsNCmp(impl->data, substring.impl->data, substring.impl->len) == 0;
}

bool String::endsWith(const String &substring) const {
  if (!impl)
    return false;
  ReadLockGuard guard(&impl->lock);
  if (!substring.impl)
    return false;
  if (substring.impl->len > impl->len)
    return false;
  if (substring.impl->len == 0)
    return true;
  return MyWcsNCmp(impl->data + impl->len - substring.impl->len,
                   substring.impl->data, substring.impl->len) == 0;
}

bool String::isEmpty() const {
  if (!impl)
    return true;
  ReadLockGuard guard(&impl->lock);
  return impl->len == 0;
}

int String::count(const String &substring) const {
  if (!impl)
    return 0;
  ReadLockGuard guard(&impl->lock);
  if (!substring.impl || substring.impl->len == 0)
    return 0;
  int count = 0;
  WCHAR *p = impl->data;
  while ((p = MyWcsStr(p, substring.impl->data)) != nullptr) {
    count++;
    p += substring.impl->len;
  }
  return count;
}

bool String::equals(const String &other) const {
  if (!impl)
    return false;
  ReadLockGuard guard(&impl->lock);
  if (!other.impl)
    return false;
  if (impl->len != other.impl->len)
    return false;
  if (impl->len == 0)
    return true;
  return MyWcsNCmp(impl->data, other.impl->data, impl->len) == 0;
}

bool String::operator==(const String &other) const { return equals(other); }

bool String::operator!=(const String &other) const { return !equals(other); }

int String::hash() const {
  if (!impl)
    return 0;
  ReadLockGuard guard(&impl->lock);
  if (!impl->data || impl->len == 0)
    return 0;

  unsigned int hash = 5381;
  WCHAR *p = impl->data;
  int c;

  while ((c = *p++)) {
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  }

  return (int)hash;
}

} // namespace attoboy
