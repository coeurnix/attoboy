#pragma once
#include "attoboy/attoboy.h"
#include "atto_internal_common.h"
#include <windows.h>

namespace attoboy {

struct ThreadImpl {
  HANDLE handle;
  void* (*func)(void*);
  void *arg;
  void *returnValue;
  DWORD threadId;
  volatile bool completed;
  volatile bool cancelled;
  mutable SRWLOCK lock;
  volatile LONG refCount;
};

} // namespace attoboy
