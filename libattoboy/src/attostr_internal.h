#pragma once
#include "attoboy/attoboy.h"
#include <windows.h>

namespace attoboy {

struct StringImpl {
  LPWSTR data;
  int len;
  SRWLOCK lock;
};

// RAII lock guards for thread safety
class ReadLockGuard {
  SRWLOCK *lock;

public:
  ReadLockGuard(SRWLOCK *l) : lock(l) { AcquireSRWLockShared(lock); }
  ~ReadLockGuard() { ReleaseSRWLockShared(lock); }
};

class WriteLockGuard {
  SRWLOCK *lock;

public:
  WriteLockGuard(SRWLOCK *l) : lock(l) { AcquireSRWLockExclusive(lock); }
  ~WriteLockGuard() { ReleaseSRWLockExclusive(lock); }
};

// Inline helper functions for string allocation
static inline LPWSTR AllocString(int len) {
  return (LPWSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                           (len + 1) * sizeof(WCHAR));
}

static inline void FreeString(LPWSTR str) {
  if (str)
    HeapFree(GetProcessHeap(), 0, str);
}

// Shared string helper functions (implemented in attostr_helpers.cpp)
void MyWcsNCpy(WCHAR *dest, const WCHAR *src, int count);
int MyWcsNCmp(const WCHAR *s1, const WCHAR *s2, int count);
WCHAR *MyWcsStr(const WCHAR *haystack, const WCHAR *needle);

} // namespace attoboy
