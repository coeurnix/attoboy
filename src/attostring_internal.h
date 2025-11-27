#pragma once
#include "atto_internal_common.h"
#include "attoboy/attoboy.h"
#include <windows.h>


namespace attoboy {

using ATTO_LPSTR = LPSTR;
using ATTO_WCHAR = CHAR;
#define ATTO_LSTRCPY lstrcpyA
#define ATTO_LSTRLEN lstrlenA
#define ATTO_LSTRCMPI lstrcmpiA
#define ATTO_CHARUPPER CharUpperA
#define ATTO_CHARLOWER CharLowerA

struct StringImpl {
  ATTO_LPSTR data;
  int len;
  mutable SRWLOCK lock;
};

static inline ATTO_LPSTR AllocString(int len) {
  return (ATTO_LPSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                               (len + 1) * sizeof(ATTO_WCHAR));
}

static inline void FreeString(ATTO_LPSTR str) {
  if (str)
    HeapFree(GetProcessHeap(), 0, str);
}

void MyStrNCpy(ATTO_WCHAR *dest, const ATTO_WCHAR *src, int count);
int MyStrNCmp(const ATTO_WCHAR *s1, const ATTO_WCHAR *s2, int count);
ATTO_WCHAR *MyStrStr(const ATTO_WCHAR *haystack, const ATTO_WCHAR *needle);

} // namespace attoboy
