#pragma once
#include <windows.h>

namespace attoboy {

// RAII lock guards for thread safety (shared across all internal headers)
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

// UTF-8 <-> UTF-16 conversion utilities for Windows API boundaries
inline WCHAR* Utf8ToWide(const char* utf8, int* outLen = nullptr) {
  if (!utf8) return nullptr;
  int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, nullptr, 0);
  if (len <= 0) return nullptr;
  WCHAR* wide = (WCHAR*)HeapAlloc(GetProcessHeap(), 0, len * sizeof(WCHAR));
  if (!wide) return nullptr;
  MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wide, len);
  if (outLen) *outLen = len - 1;
  return wide;
}

inline char* WideToUtf8(const WCHAR* wide, int* outLen = nullptr) {
  if (!wide) return nullptr;
  int len = WideCharToMultiByte(CP_UTF8, 0, wide, -1, nullptr, 0, nullptr, nullptr);
  if (len <= 0) return nullptr;
  char* utf8 = (char*)HeapAlloc(GetProcessHeap(), 0, len);
  if (!utf8) return nullptr;
  WideCharToMultiByte(CP_UTF8, 0, wide, -1, utf8, len, nullptr, nullptr);
  if (outLen) *outLen = len - 1;
  return utf8;
}

inline void FreeConvertedString(void* str) {
  if (str) HeapFree(GetProcessHeap(), 0, str);
}

} // namespace attoboy
