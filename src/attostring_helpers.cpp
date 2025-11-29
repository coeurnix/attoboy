#include "attostring_internal.h"

namespace attoboy {

void MyStrNCpy(ATTO_WCHAR *dest, const ATTO_WCHAR *src, int count) {
  for (int i = 0; i < count; i++) {
    if (*src) {
      *dest++ = *src++;
    } else {
      *dest++ = '\0';
    }
  }
}

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

UINT ParseEncodingToCodePage(const String &encoding) {
  if (encoding.isEmpty() || encoding.equals("utf-8")) {
    return CP_UTF8;
  }

  if (encoding.equals("ansi")) {
    return CP_ACP;
  }

  if (encoding.equals("utf-16") || encoding.equals("unicode")) {
    return 1200;
  }

  if (encoding.equals("utf-16be")) {
    return 1201;
  }

  if (encoding.equals("utf-7")) {
    return CP_UTF7;
  }

  if (encoding.length() > 2 && (encoding.substring(0, 2).equals("cp") ||
                                encoding.substring(0, 2).equals("CP"))) {
    String numStr = encoding.substring(2);
    if (numStr.isNumber()) {
      int cp = numStr.toInteger();
      if (cp > 0 && cp <= 65535) {
        return (UINT)cp;
      }
    }
  }

  if (encoding.isNumber()) {
    int cp = encoding.toInteger();
    if (cp > 0 && cp <= 65535) {
      return (UINT)cp;
    }
  }

  return CP_ACP;
}

char *ConvertUTF8ToCodePage(const char *utf8, int utf8_len, UINT codePage) {
  if (!utf8 || utf8_len <= 0)
    return nullptr;

  int wideLen = MultiByteToWideChar(CP_UTF8, 0, utf8, utf8_len, nullptr, 0);
  if (wideLen == 0)
    return nullptr;

  wchar_t *wideBuf =
      (wchar_t *)HeapAlloc(GetProcessHeap(), 0, wideLen * sizeof(wchar_t));
  if (!wideBuf)
    return nullptr;

  if (MultiByteToWideChar(CP_UTF8, 0, utf8, utf8_len, wideBuf, wideLen) == 0) {
    HeapFree(GetProcessHeap(), 0, wideBuf);
    return nullptr;
  }

  int targetLen = WideCharToMultiByte(codePage, 0, wideBuf, wideLen, nullptr, 0,
                                      nullptr, nullptr);
  if (targetLen == 0) {
    HeapFree(GetProcessHeap(), 0, wideBuf);
    return nullptr;
  }

  char *targetBuf =
      (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, targetLen + 1);
  if (!targetBuf) {
    HeapFree(GetProcessHeap(), 0, wideBuf);
    return nullptr;
  }

  if (WideCharToMultiByte(codePage, 0, wideBuf, wideLen, targetBuf, targetLen,
                          nullptr, nullptr) == 0) {
    HeapFree(GetProcessHeap(), 0, wideBuf);
    HeapFree(GetProcessHeap(), 0, targetBuf);
    return nullptr;
  }

  HeapFree(GetProcessHeap(), 0, wideBuf);
  return targetBuf;
}

String ConvertCodePageToUTF8(const char *data, int data_len, UINT codePage) {
  if (!data || data_len <= 0)
    return String();

  int wideLen = MultiByteToWideChar(codePage, 0, data, data_len, nullptr, 0);
  if (wideLen == 0)
    return String();

  wchar_t *wideBuf =
      (wchar_t *)HeapAlloc(GetProcessHeap(), 0, wideLen * sizeof(wchar_t));
  if (!wideBuf)
    return String();

  if (MultiByteToWideChar(codePage, 0, data, data_len, wideBuf, wideLen) == 0) {
    HeapFree(GetProcessHeap(), 0, wideBuf);
    return String();
  }

  int utf8Len = WideCharToMultiByte(CP_UTF8, 0, wideBuf, wideLen, nullptr, 0,
                                    nullptr, nullptr);
  if (utf8Len == 0) {
    HeapFree(GetProcessHeap(), 0, wideBuf);
    return String();
  }

  char *utf8Buf =
      (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, utf8Len + 1);
  if (!utf8Buf) {
    HeapFree(GetProcessHeap(), 0, wideBuf);
    return String();
  }

  if (WideCharToMultiByte(CP_UTF8, 0, wideBuf, wideLen, utf8Buf, utf8Len,
                          nullptr, nullptr) == 0) {
    HeapFree(GetProcessHeap(), 0, wideBuf);
    HeapFree(GetProcessHeap(), 0, utf8Buf);
    return String();
  }

  HeapFree(GetProcessHeap(), 0, wideBuf);

  String result = String::FromCStr(utf8Buf, utf8Len);
  HeapFree(GetProcessHeap(), 0, utf8Buf);
  return result;
}

} // namespace attoboy
