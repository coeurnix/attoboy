#include "attobuffer_internal.h"
#include "attostring_internal.h"

namespace attoboy {

bool Buffer::compare(const Buffer &other) const {
  if (!impl || !other.impl)
    return (!impl && !other.impl);

  ReadLockGuard guard1(&impl->lock);
  ReadLockGuard guard2(&other.impl->lock);

  if (impl->size != other.impl->size)
    return false;

  for (int i = 0; i < impl->size; i++) {
    if (impl->data[i] != other.impl->data[i])
      return false;
  }

  return true;
}

bool Buffer::operator==(const Buffer &other) const { return compare(other); }

bool Buffer::operator!=(const Buffer &other) const { return !compare(other); }

Buffer Buffer::operator+(const String &str) const {
  Buffer result(*this);
  result.append(str);
  return result;
}

Buffer Buffer::operator+(const Buffer &other) const {
  Buffer result(*this);
  result.append(other);
  return result;
}

const unsigned char *Buffer::c_ptr(int *len) const {
  if (!impl || !len) {
    if (len)
      *len = 0;
    return nullptr;
  }

  ReadLockGuard guard(&impl->lock);
  *len = impl->size;
  if (impl->size == 0)
    return nullptr;
  return impl->data;
}

String Buffer::toString() const {
  if (!impl) {
    return String();
  }

  ReadLockGuard guard(&impl->lock);

  if (impl->size == 0) {
    return String();
  }

  // For UTF-8 strings stored as bytes, create String directly from the data
  return String::FromCStr((const char *)impl->data, impl->size);
}

int Buffer::hash() const {
  if (!impl)
    return 0;

  ReadLockGuard guard(&impl->lock);

  if (impl->size == 0)
    return 0;

  int hash = 0;
  for (int i = 0; i < impl->size; i++) {
    hash = ((hash << 5) - hash) + impl->data[i];
  }

  return hash;
}

} // namespace attoboy
