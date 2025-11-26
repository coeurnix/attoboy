#include "attofile_internal.h"

namespace attoboy {

bool File::equals(const File &other) const {
  if (!impl && !other.impl) {
    return true;
  }
  if (!impl || !other.impl) {
    return false;
  }
  return impl == other.impl;
}

bool File::operator==(const File &other) const {
  return equals(other);
}

bool File::operator!=(const File &other) const {
  return !equals(other);
}

} // namespace attoboy
