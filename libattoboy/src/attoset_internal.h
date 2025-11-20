#pragma once
#include "attolist_internal.h"
#include "attoboy/attoboy.h"

namespace attoboy {

struct SetImpl {
  List values;
  SRWLOCK lock;

  SetImpl() : values() { InitializeSRWLock(&lock); }
};

} // namespace attoboy
