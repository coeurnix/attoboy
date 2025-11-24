#include "attostring_internal.h"

namespace attoboy {

String::String() {
  impl = (StringImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                 sizeof(StringImpl));
  InitializeSRWLock(&impl->lock);
  impl->data = AllocString(0);
  impl->len = 0;
}

String::String(const ATTO_CHAR *str) {
  impl = (StringImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                 sizeof(StringImpl));
  InitializeSRWLock(&impl->lock);

  if (!str) {
    const ATTO_CHAR *nullStr = ATTO_TEXT("null");
    int len = 4;
    impl->data = AllocString(len);
    if (impl->data) {
      lstrcpyA(impl->data, nullStr);
      impl->len = len;
    } else {
      impl->len = 0;
    }
    return;
  }

  int len = lstrlenA(str);
  impl->data = AllocString(len);
  if (impl->data) {
    lstrcpyA(impl->data, str);
    impl->len = len;
  } else {
    impl->len = 0;
  }
}

String::String(const String &other) {
  impl = (StringImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                 sizeof(StringImpl));
  InitializeSRWLock(&impl->lock);

  if (other.impl) {
    ReadLockGuard guard(&other.impl->lock);
    if (other.impl->data) {
      int len = other.impl->len;
      impl->data = AllocString(len);
      if (impl->data) {
        lstrcpyA(impl->data, other.impl->data);
        impl->len = len;
      } else {
        impl->len = 0;
      }
    } else {
      impl->data = AllocString(0);
      impl->len = 0;
    }
  } else {
    impl->data = AllocString(0);
    impl->len = 0;
  }
}

String::~String() {
  if (impl) {
    FreeString(impl->data);
    HeapFree(GetProcessHeap(), 0, impl);
  }
}

String &String::operator=(const String &other) {
  if (this == &other) {
    return *this;
  }

  if (impl) {
    WriteLockGuard guard(&impl->lock);
    FreeString(impl->data);

    if (other.impl) {
      ReadLockGuard otherGuard(&other.impl->lock);
      int len = other.impl->len;
      impl->data = AllocString(len);
      if (impl->data) {
        lstrcpyA(impl->data, other.impl->data);
        impl->len = len;
      } else {
        impl->data = AllocString(0);
        impl->len = 0;
      }
    } else {
      impl->data = AllocString(0);
      impl->len = 0;
    }
  }

  return *this;
}

int String::length() const {
  if (!impl)
    return 0;
  ReadLockGuard guard(&impl->lock);
  return countUTF8Characters(impl->data, impl->len);
}

int String::byteLength() const {
  if (!impl)
    return 0;
  ReadLockGuard guard(&impl->lock);
  return impl->len;
}

String String::duplicate() const { return String(*this); }

const ATTO_CHAR *String::c_str() const {
  return (impl && impl->data) ? impl->data : ATTO_TEXT("");
}

} // namespace attoboy
