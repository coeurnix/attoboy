#include "attostring_internal.h"

namespace attoboy {

// Custom strncpy replacement
void MyStrNCpy(ATTO_WCHAR *dest, const ATTO_WCHAR *src, int count) {
  for (int i = 0; i < count; i++) {
    if (*src) {
      *dest++ = *src++;
    } else {
      *dest++ = '\0';
    }
  }
}

// Custom strncmp replacement
int MyStrNCmp(const ATTO_WCHAR *s1, const ATTO_WCHAR *s2, int count) {
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

// Custom strstr replacement
ATTO_WCHAR *MyStrStr(const ATTO_WCHAR *haystack, const ATTO_WCHAR *needle) {
  if (!*needle)
    return (ATTO_WCHAR *)haystack;

  for (; *haystack; haystack++) {
    if (*haystack == *needle) {
      const ATTO_WCHAR *h = haystack;
      const ATTO_WCHAR *n = needle;
      while (*h && *n && *h == *n) {
        h++;
        n++;
      }
      if (!*n)
        return (ATTO_WCHAR *)haystack;
    }
  }
  return nullptr;
}

} // namespace attoboy
