#include "attobuffer_internal.h"
#include "attostring_internal.h"

namespace attoboy {

Buffer &Buffer::append(const String &str) {
  const ATTO_CHAR *wstr = str.c_str();
  int len = str.length();
  int byteSize = len * sizeof(ATTO_CHAR);

  if (byteSize <= 0)
    return *this;

  WriteLockGuard guard(&impl->lock);

  if (!EnsureBufferCapacity(impl, impl->size + byteSize))
    return *this;

  unsigned char *src = (unsigned char *)wstr;
  for (int i = 0; i < byteSize; i++) {
    impl->data[impl->size + i] = src[i];
  }
  impl->size += byteSize;

  return *this;
}

Buffer &Buffer::append(const Buffer &other) {
  if (!other.impl)
    return *this;

  ReadLockGuard otherGuard(&other.impl->lock);
  int otherSize = other.impl->size;

  if (otherSize <= 0)
    return *this;

  WriteLockGuard guard(&impl->lock);

  if (!EnsureBufferCapacity(impl, impl->size + otherSize))
    return *this;

  for (int i = 0; i < otherSize; i++) {
    impl->data[impl->size + i] = other.impl->data[i];
  }
  impl->size += otherSize;

  return *this;
}

Buffer &Buffer::append(const unsigned char *ptr, int size) {
  if (!ptr || size <= 0)
    return *this;

  WriteLockGuard guard(&impl->lock);

  if (!EnsureBufferCapacity(impl, impl->size + size))
    return *this;

  for (int i = 0; i < size; i++) {
    impl->data[impl->size + i] = ptr[i];
  }
  impl->size += size;

  return *this;
}

Buffer &Buffer::prepend(const String &str) {
  const ATTO_CHAR *wstr = str.c_str();
  int len = str.length();
  int byteSize = len * sizeof(ATTO_CHAR);

  if (byteSize <= 0)
    return *this;

  WriteLockGuard guard(&impl->lock);

  if (!EnsureBufferCapacity(impl, impl->size + byteSize))
    return *this;

  for (int i = impl->size - 1; i >= 0; i--) {
    impl->data[i + byteSize] = impl->data[i];
  }

  unsigned char *src = (unsigned char *)wstr;
  for (int i = 0; i < byteSize; i++) {
    impl->data[i] = src[i];
  }
  impl->size += byteSize;

  return *this;
}

Buffer &Buffer::prepend(const Buffer &other) {
  if (!other.impl)
    return *this;

  ReadLockGuard otherGuard(&other.impl->lock);
  int otherSize = other.impl->size;

  if (otherSize <= 0)
    return *this;

  WriteLockGuard guard(&impl->lock);

  if (!EnsureBufferCapacity(impl, impl->size + otherSize))
    return *this;

  for (int i = impl->size - 1; i >= 0; i--) {
    impl->data[i + otherSize] = impl->data[i];
  }

  for (int i = 0; i < otherSize; i++) {
    impl->data[i] = other.impl->data[i];
  }
  impl->size += otherSize;

  return *this;
}

Buffer &Buffer::prepend(const unsigned char *ptr, int size) {
  if (!ptr || size <= 0)
    return *this;

  WriteLockGuard guard(&impl->lock);

  if (!EnsureBufferCapacity(impl, impl->size + size))
    return *this;

  for (int i = impl->size - 1; i >= 0; i--) {
    impl->data[i + size] = impl->data[i];
  }

  for (int i = 0; i < size; i++) {
    impl->data[i] = ptr[i];
  }
  impl->size += size;

  return *this;
}

Buffer &Buffer::insert(int index, const String &str) {
  const ATTO_CHAR *wstr = str.c_str();
  int len = str.length();
  int byteSize = len * sizeof(ATTO_CHAR);

  if (byteSize <= 0)
    return *this;

  WriteLockGuard guard(&impl->lock);

  if (index < 0)
    index = 0;
  if (index > impl->size)
    index = impl->size;

  if (!EnsureBufferCapacity(impl, impl->size + byteSize))
    return *this;

  for (int i = impl->size - 1; i >= index; i--) {
    impl->data[i + byteSize] = impl->data[i];
  }

  unsigned char *src = (unsigned char *)wstr;
  for (int i = 0; i < byteSize; i++) {
    impl->data[index + i] = src[i];
  }
  impl->size += byteSize;

  return *this;
}

Buffer &Buffer::insert(int index, const Buffer &other) {
  if (!other.impl)
    return *this;

  ReadLockGuard otherGuard(&other.impl->lock);
  int otherSize = other.impl->size;

  if (otherSize <= 0)
    return *this;

  WriteLockGuard guard(&impl->lock);

  if (index < 0)
    index = 0;
  if (index > impl->size)
    index = impl->size;

  if (!EnsureBufferCapacity(impl, impl->size + otherSize))
    return *this;

  for (int i = impl->size - 1; i >= index; i--) {
    impl->data[i + otherSize] = impl->data[i];
  }

  for (int i = 0; i < otherSize; i++) {
    impl->data[index + i] = other.impl->data[i];
  }
  impl->size += otherSize;

  return *this;
}

Buffer &Buffer::insert(int index, const unsigned char *ptr, int size) {
  if (!ptr || size <= 0)
    return *this;

  WriteLockGuard guard(&impl->lock);

  if (index < 0)
    index = 0;
  if (index > impl->size)
    index = impl->size;

  if (!EnsureBufferCapacity(impl, impl->size + size))
    return *this;

  for (int i = impl->size - 1; i >= index; i--) {
    impl->data[i + size] = impl->data[i];
  }

  for (int i = 0; i < size; i++) {
    impl->data[index + i] = ptr[i];
  }
  impl->size += size;

  return *this;
}

} // namespace attoboy
