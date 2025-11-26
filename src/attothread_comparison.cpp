#include "attothread_internal.h"

namespace attoboy {

bool Thread::equals(const Thread &other) const {
  if (!impl && !other.impl) {
    return true;
  }
  if (!impl || !other.impl) {
    return false;
  }
  return impl == other.impl;
}

bool Thread::operator==(const Thread &other) const {
  return equals(other);
}

bool Thread::operator!=(const Thread &other) const {
  return !equals(other);
}

} // namespace attoboy
