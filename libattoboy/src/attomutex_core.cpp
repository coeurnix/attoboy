#include "attomutex_internal.h"

namespace attoboy {

Mutex::Mutex() {
  impl = (MutexImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MutexImpl));
  if (impl) {
    InitializeSRWLock(&impl->lock);
    impl->refCount = 1;
  }
}

Mutex::Mutex(const Mutex &other) {
  impl = other.impl;
  if (impl) {
    InterlockedIncrement(&impl->refCount);
  }
}

Mutex::~Mutex() {
  if (impl) {
    if (InterlockedDecrement(&impl->refCount) == 0) {
      HeapFree(GetProcessHeap(), 0, impl);
    }
  }
}

Mutex &Mutex::operator=(const Mutex &other) {
  if (this != &other) {
    if (impl && InterlockedDecrement(&impl->refCount) == 0) {
      HeapFree(GetProcessHeap(), 0, impl);
    }
    impl = other.impl;
    if (impl) {
      InterlockedIncrement(&impl->refCount);
    }
  }
  return *this;
}

void Mutex::lock() {
  if (impl) {
    AcquireSRWLockExclusive(&impl->lock);
  }
}

void Mutex::unlock() {
  if (impl) {
    ReleaseSRWLockExclusive(&impl->lock);
  }
}

bool Mutex::tryLock() {
  if (impl) {
    return TryAcquireSRWLockExclusive(&impl->lock) != 0;
  }
  return false;
}

} // namespace attoboy
