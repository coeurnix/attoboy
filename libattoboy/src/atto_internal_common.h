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

} // namespace attoboy
