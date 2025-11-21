#include "attobuffer_internal.h"

namespace attoboy {

Buffer Buffer::slice(int start, int end) const {
  if (!impl) {
    return Buffer();
  }

  ReadLockGuard guard(&impl->lock);

  if (start < 0)
    start = 0;
  if (start > impl->size)
    start = impl->size;

  if (end == -1 || end > impl->size)
    end = impl->size;
  if (end < start)
    end = start;

  int sliceSize = end - start;

  if (sliceSize <= 0) {
    return Buffer();
  }

  Buffer result(sliceSize);
  result.impl->size = sliceSize;

  for (int i = 0; i < sliceSize; i++) {
    result.impl->data[i] = impl->data[start + i];
  }

  return result;
}

Buffer &Buffer::remove(int start, int end) {
  if (!impl)
    return *this;

  WriteLockGuard guard(&impl->lock);

  if (start < 0)
    start = 0;
  if (start >= impl->size)
    return *this;

  if (end == -1 || end > impl->size)
    end = impl->size;
  if (end <= start)
    return *this;

  int removeSize = end - start;
  int remainingSize = impl->size - end;

  for (int i = 0; i < remainingSize; i++) {
    impl->data[start + i] = impl->data[end + i];
  }

  impl->size -= removeSize;

  return *this;
}

Buffer &Buffer::reverse() {
  if (!impl)
    return *this;

  WriteLockGuard guard(&impl->lock);

  if (impl->size <= 1)
    return *this;

  int left = 0;
  int right = impl->size - 1;

  while (left < right) {
    unsigned char temp = impl->data[left];
    impl->data[left] = impl->data[right];
    impl->data[right] = temp;
    left++;
    right--;
  }

  return *this;
}

} // namespace attoboy
