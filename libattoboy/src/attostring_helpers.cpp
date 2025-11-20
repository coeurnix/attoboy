#include "attostring_internal.h"

namespace attoboy {

// Custom wcsncpy replacement
void MyWcsNCpy(WCHAR *dest, const WCHAR *src, int count) {
  for (int i = 0; i < count; i++) {
    if (*src) {
      *dest++ = *src++;
    } else {
      *dest++ = L'\0';
    }
  }
}

// Custom wcsncmp replacement
int MyWcsNCmp(const WCHAR *s1, const WCHAR *s2, int count) {
  for (int i = 0; i < count; i++) {
    if (*s1 != *s2)
      return *s1 - *s2;
    if (*s1 == 0)
      return 0;
    s1++;
    s2++;
  }
  return 0;
}

// Custom wcsstr replacement
WCHAR *MyWcsStr(const WCHAR *haystack, const WCHAR *needle) {
  if (!*needle)
    return (WCHAR *)haystack;

  for (; *haystack; haystack++) {
    if (*haystack == *needle) {
      const WCHAR *h = haystack;
      const WCHAR *n = needle;
      while (*h && *n && *h == *n) {
        h++;
        n++;
      }
      if (!*n)
        return (WCHAR *)haystack;
    }
  }
  return nullptr;
}

} // namespace attoboy
