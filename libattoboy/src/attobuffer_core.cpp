#include "attobuffer_internal.h"

namespace attoboy {

Buffer::Buffer() {
  impl = (BufferImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                 sizeof(BufferImpl));
  InitializeSRWLock(&impl->lock);
  impl->data = AllocBufferData(512);
  impl->size = 0;
  impl->capacity = 512;
}

Buffer::Buffer(int size) {
  impl = (BufferImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                 sizeof(BufferImpl));
  InitializeSRWLock(&impl->lock);

  if (size < 0)
    size = 0;

  int capacity = size > 0 ? size : 512;
  impl->data = AllocBufferData(capacity);
  impl->size = 0;
  impl->capacity = capacity;
}

Buffer::Buffer(const String &str) {
  impl = (BufferImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                 sizeof(BufferImpl));
  InitializeSRWLock(&impl->lock);

  const wchar_t *wstr = str.c_str();
  int len = str.length();
  int byteSize = len * sizeof(wchar_t);

  int capacity = byteSize > 512 ? byteSize : 512;
  impl->data = AllocBufferData(capacity);
  impl->capacity = capacity;
  impl->size = byteSize;

  if (impl->data && byteSize > 0) {
    unsigned char *src = (unsigned char *)wstr;
    for (int i = 0; i < byteSize; i++) {
      impl->data[i] = src[i];
    }
  }
}

Buffer::Buffer(const void *ptr, int size) {
  impl = (BufferImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                 sizeof(BufferImpl));
  InitializeSRWLock(&impl->lock);

  if (!ptr || size <= 0) {
    impl->data = AllocBufferData(512);
    impl->size = 0;
    impl->capacity = 512;
    return;
  }

  int capacity = size > 512 ? size : 512;
  impl->data = AllocBufferData(capacity);
  impl->capacity = capacity;
  impl->size = size;

  if (impl->data) {
    const unsigned char *src = (const unsigned char *)ptr;
    for (int i = 0; i < size; i++) {
      impl->data[i] = src[i];
    }
  }
}

Buffer::Buffer(const Buffer &other) {
  impl = (BufferImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                 sizeof(BufferImpl));
  InitializeSRWLock(&impl->lock);

  if (other.impl) {
    ReadLockGuard guard(&other.impl->lock);

    int capacity = other.impl->capacity > 512 ? other.impl->capacity : 512;
    impl->data = AllocBufferData(capacity);
    impl->capacity = capacity;
    impl->size = other.impl->size;

    if (impl->data && other.impl->data && other.impl->size > 0) {
      for (int i = 0; i < other.impl->size; i++) {
        impl->data[i] = other.impl->data[i];
      }
    }
  } else {
    impl->data = AllocBufferData(512);
    impl->size = 0;
    impl->capacity = 512;
  }
}

Buffer::~Buffer() {
  if (impl) {
    FreeBufferData(impl->data);
    HeapFree(GetProcessHeap(), 0, impl);
  }
}

Buffer &Buffer::operator=(const Buffer &other) {
  if (this == &other)
    return *this;

  if (impl) {
    WriteLockGuard guard(&impl->lock);

    if (other.impl) {
      ReadLockGuard otherGuard(&other.impl->lock);

      if (other.impl->size > impl->capacity) {
        FreeBufferData(impl->data);
        int capacity = other.impl->capacity > 512 ? other.impl->capacity : 512;
        impl->data = AllocBufferData(capacity);
        impl->capacity = capacity;
      }

      impl->size = other.impl->size;

      if (impl->data && other.impl->data && other.impl->size > 0) {
        for (int i = 0; i < other.impl->size; i++) {
          impl->data[i] = other.impl->data[i];
        }
      }
    } else {
      impl->size = 0;
    }
  }

  return *this;
}

int Buffer::length() const {
  if (!impl)
    return 0;
  ReadLockGuard guard(&impl->lock);
  return impl->size;
}

bool Buffer::isEmpty() const {
  if (!impl)
    return true;
  ReadLockGuard guard(&impl->lock);
  return impl->size == 0;
}

Buffer &Buffer::clear() {
  if (impl) {
    WriteLockGuard guard(&impl->lock);
    impl->size = 0;
  }
  return *this;
}

} // namespace attoboy
