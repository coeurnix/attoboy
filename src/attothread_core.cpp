#include "attothread_internal.h"

namespace attoboy {

static DWORD WINAPI ThreadProc(LPVOID param) {
  ThreadImpl *impl = (ThreadImpl *)param;
  if (impl) {
    void *ret = nullptr;
    if (impl->func) {
      ret = impl->func(impl->arg);
    }
    WriteLockGuard guard(&impl->lock);
    impl->returnValue = ret;
    impl->completed = true;
  }
  return 0;
}

Thread::Thread(void *(*func)(void *), void *arg) {
  impl = (ThreadImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                 sizeof(ThreadImpl));
  if (impl) {
    InitializeSRWLock(&impl->lock);
    impl->func = func;
    impl->arg = arg;
    impl->returnValue = nullptr;
    impl->completed = false;
    impl->cancelled = false;
    impl->refCount = 1;
    impl->handle =
        CreateThread(nullptr, 0, ThreadProc, impl, 0, &impl->threadId);
    if (!impl->handle) {
      impl->completed = true;
    }
  }
}

Thread::Thread(const Thread &other) {
  impl = other.impl;
  if (impl) {
    InterlockedIncrement(&impl->refCount);
  }
}

Thread::~Thread() {
  if (impl) {
    if (InterlockedDecrement(&impl->refCount) == 0) {
      if (impl->handle) {
        CloseHandle(impl->handle);
      }
      HeapFree(GetProcessHeap(), 0, impl);
    }
  }
}

Thread &Thread::operator=(const Thread &other) {
  if (this != &other) {
    if (impl && InterlockedDecrement(&impl->refCount) == 0) {
      if (impl->handle) {
        CloseHandle(impl->handle);
      }
      HeapFree(GetProcessHeap(), 0, impl);
    }
    impl = other.impl;
    if (impl) {
      InterlockedIncrement(&impl->refCount);
    }
  }
  return *this;
}

void *Thread::await() {
  if (!impl || !impl->handle) {
    return nullptr;
  }

  {
    ReadLockGuard guard(&impl->lock);
    if (impl->cancelled || impl->completed) {
      return impl->returnValue;
    }
  }

  WaitForSingleObject(impl->handle, INFINITE);

  ReadLockGuard guard(&impl->lock);
  return impl->returnValue;
}

void Thread::cancel() {
  if (!impl || !impl->handle) {
    return;
  }

  WriteLockGuard guard(&impl->lock);
  if (!impl->cancelled && !impl->completed) {
    TerminateThread(impl->handle, 1);
    impl->cancelled = true;
  }
}

bool Thread::isRunning() const {
  if (!impl || !impl->handle) {
    return false;
  }

  ReadLockGuard guard(&impl->lock);
  if (impl->cancelled || impl->completed) {
    return false;
  }

  DWORD exitCode;
  if (GetExitCodeThread(impl->handle, &exitCode)) {
    return exitCode == STILL_ACTIVE;
  }
  return false;
}

} // namespace attoboy
