#include "attobuffer_internal.h"

namespace attoboy {

Buffer &Buffer::trim() {
  if (!impl)
    return *this;

  WriteLockGuard lock(&impl->lock);

  if (impl->size == impl->capacity)
    return *this;

  if (impl->size == 0) {
    if (impl->data) {
      FreeBufferData(impl->data);
      impl->data = nullptr;
    }
    impl->capacity = 0;
    return *this;
  }

  unsigned char *newData = AllocBufferData(impl->size);
  if (!newData)
    return *this;

  for (int i = 0; i < impl->size; i++)
    newData[i] = impl->data[i];

  if (impl->data)
    FreeBufferData(impl->data);

  impl->data = newData;
  impl->capacity = impl->size;

  return *this;
}

} // namespace attoboy
