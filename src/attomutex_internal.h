#pragma once
#include "attoboy/attoboy.h"
#include "atto_internal_common.h"
#include <windows.h>

namespace attoboy {

struct MutexImpl {
  SRWLOCK lock;
  volatile LONG refCount;
};

} // namespace attoboy
