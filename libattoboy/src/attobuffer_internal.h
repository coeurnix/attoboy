#pragma once
#include "attoboy/attoboy.h"
#include "atto_internal_common.h"
#include "attostring_internal.h"
#include <windows.h>

namespace attoboy {

struct BufferImpl {
  unsigned char *data;
  int size;
  int capacity;
  mutable SRWLOCK lock;
};

static inline unsigned char *AllocBufferData(int capacity) {
  if (capacity <= 0)
    return nullptr;
  return (unsigned char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                    capacity);
}

static inline void FreeBufferData(unsigned char *data) {
  if (data)
    HeapFree(GetProcessHeap(), 0, data);
}

static inline bool EnsureBufferCapacity(BufferImpl *impl, int requiredSize) {
  if (!impl || requiredSize <= impl->capacity)
    return true;

  int newCapacity = impl->capacity;
  if (newCapacity == 0)
    newCapacity = 512;

  while (newCapacity < requiredSize)
    newCapacity *= 2;

  unsigned char *newData = AllocBufferData(newCapacity);
  if (!newData)
    return false;

  if (impl->data && impl->size > 0) {
    for (int i = 0; i < impl->size; i++) {
      newData[i] = impl->data[i];
    }
  }

  FreeBufferData(impl->data);
  impl->data = newData;
  impl->capacity = newCapacity;
  return true;
}

} // namespace attoboy
