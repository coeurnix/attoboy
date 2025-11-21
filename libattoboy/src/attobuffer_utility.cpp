#include "attobuffer_internal.h"

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

String Buffer::toString() const {
  if (!impl) {
    return String();
  }

  ReadLockGuard guard(&impl->lock);

  if (impl->size == 0) {
    return String();
  }

  int wcharCount = impl->size / sizeof(wchar_t);

  if (wcharCount == 0) {
    return String();
  }

  wchar_t *temp = (wchar_t *)HeapAlloc(
      GetProcessHeap(), HEAP_ZERO_MEMORY, (wcharCount + 1) * sizeof(wchar_t));

  if (!temp) {
    return String();
  }

  for (int i = 0; i < wcharCount; i++) {
    int byteOffset = i * sizeof(wchar_t);
    wchar_t wc = 0;
    unsigned char *wcBytes = (unsigned char *)&wc;
    wcBytes[0] = impl->data[byteOffset];
    wcBytes[1] = impl->data[byteOffset + 1];
    temp[i] = wc;
  }
  temp[wcharCount] = 0;

  String result(temp);
  HeapFree(GetProcessHeap(), 0, temp);

  return result;
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
