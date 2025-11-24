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

struct RegistryImpl {
  ATTO_LPSTR keyPath;
  int keyPathLen;
  HKEY hKey;
  HKEY hRootKey;
  ATTO_LPSTR subKeyPath;
  int subKeyPathLen;
  SRWLOCK lock;

  RegistryImpl()
      : keyPath(nullptr), keyPathLen(0), hKey(nullptr), hRootKey(nullptr),
        subKeyPath(nullptr), subKeyPathLen(0) {
    InitializeSRWLock(&lock);
  }
};

static inline ATTO_LPSTR AllocRegistryString(int len) {
  return (ATTO_LPSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                               (len + 1) * sizeof(ATTO_WCHAR));
}

static inline void FreeRegistryString(ATTO_LPSTR str) {
  if (str)
    HeapFree(GetProcessHeap(), 0, str);
}

static inline HKEY ParseRootKey(const ATTO_CHAR *path, int *prefixLen) {
  if (!path || !prefixLen)
    return nullptr;

  *prefixLen = 0;

  const ATTO_CHAR *hkcu = ATTO_TEXT("HKEY_CURRENT_USER");
  const ATTO_CHAR *hklm = ATTO_TEXT("HKEY_LOCAL_MACHINE");
  const ATTO_CHAR *hkcr = ATTO_TEXT("HKEY_CLASSES_ROOT");
  const ATTO_CHAR *hku = ATTO_TEXT("HKEY_USERS");
  const ATTO_CHAR *hkcc = ATTO_TEXT("HKEY_CURRENT_CONFIG");

  int hkcuLen = ATTO_LSTRLEN(hkcu);
  int hklmLen = ATTO_LSTRLEN(hklm);
  int hkcrLen = ATTO_LSTRLEN(hkcr);
  int hkuLen = ATTO_LSTRLEN(hku);
  int hkccLen = ATTO_LSTRLEN(hkcc);

  int pathLen = ATTO_LSTRLEN(path);

  if (pathLen >= hkcuLen) {
    bool match = true;
    for (int i = 0; i < hkcuLen; i++) {
      ATTO_CHAR c1 = path[i];
      ATTO_CHAR c2 = hkcu[i];
      if (c1 >= 'a' && c1 <= 'z')
        c1 = c1 - 'a' + 'A';
      if (c2 >= 'a' && c2 <= 'z')
        c2 = c2 - 'a' + 'A';
      if (c1 != c2) {
        match = false;
        break;
      }
    }
    if (match) {
      *prefixLen = hkcuLen;
      return HKEY_CURRENT_USER;
    }
  }
  if (pathLen >= hklmLen) {
    bool match = true;
    for (int i = 0; i < hklmLen; i++) {
      ATTO_CHAR c1 = path[i];
      ATTO_CHAR c2 = hklm[i];
      if (c1 >= 'a' && c1 <= 'z')
        c1 = c1 - 'a' + 'A';
      if (c2 >= 'a' && c2 <= 'z')
        c2 = c2 - 'a' + 'A';
      if (c1 != c2) {
        match = false;
        break;
      }
    }
    if (match) {
      *prefixLen = hklmLen;
      return HKEY_LOCAL_MACHINE;
    }
  }
  if (pathLen >= hkcrLen) {
    bool match = true;
    for (int i = 0; i < hkcrLen; i++) {
      ATTO_CHAR c1 = path[i];
      ATTO_CHAR c2 = hkcr[i];
      if (c1 >= 'a' && c1 <= 'z')
        c1 = c1 - 'a' + 'A';
      if (c2 >= 'a' && c2 <= 'z')
        c2 = c2 - 'a' + 'A';
      if (c1 != c2) {
        match = false;
        break;
      }
    }
    if (match) {
      *prefixLen = hkcrLen;
      return HKEY_CLASSES_ROOT;
    }
  }
  if (pathLen >= hkuLen) {
    bool match = true;
    for (int i = 0; i < hkuLen; i++) {
      ATTO_CHAR c1 = path[i];
      ATTO_CHAR c2 = hku[i];
      if (c1 >= 'a' && c1 <= 'z')
        c1 = c1 - 'a' + 'A';
      if (c2 >= 'a' && c2 <= 'z')
        c2 = c2 - 'a' + 'A';
      if (c1 != c2) {
        match = false;
        break;
      }
    }
    if (match) {
      *prefixLen = hkuLen;
      return HKEY_USERS;
    }
  }
  if (pathLen >= hkccLen) {
    bool match = true;
    for (int i = 0; i < hkccLen; i++) {
      ATTO_CHAR c1 = path[i];
      ATTO_CHAR c2 = hkcc[i];
      if (c1 >= 'a' && c1 <= 'z')
        c1 = c1 - 'a' + 'A';
      if (c2 >= 'a' && c2 <= 'z')
        c2 = c2 - 'a' + 'A';
      if (c1 != c2) {
        match = false;
        break;
      }
    }
    if (match) {
      *prefixLen = hkccLen;
      return HKEY_CURRENT_CONFIG;
    }
  }

  return nullptr;
}

} // namespace attoboy
