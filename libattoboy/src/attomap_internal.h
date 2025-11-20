#pragma once
#include "attolist_internal.h"
#include "attoboy/attoboy.h"

namespace attoboy {

struct MapImpl {
  List keys;
  List values;
  SRWLOCK lock;

  MapImpl() : keys(), values() { InitializeSRWLock(&lock); }
};

} // namespace attoboy
