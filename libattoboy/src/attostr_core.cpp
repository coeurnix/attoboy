#include "attostr_internal.h"

namespace attoboy {

String::String() {
  impl = (StringImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                 sizeof(StringImpl));
  impl->data = AllocString(0);
  impl->len = 0;
}

String::String(const char *str) {
  impl = (StringImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                 sizeof(StringImpl));

  if (!str) {
    const wchar_t *nullStr = L"null";
    int len = 4;
    impl->data = AllocString(len);
    if (impl->data) {
      lstrcpyW(impl->data, nullStr);
      impl->len = len;
    } else {
      impl->len = 0;
    }
    return;
  }

  int num_chars = MultiByteToWideChar(CP_ACP, 0, str, -1, nullptr, 0);
  if (num_chars > 0) {
    impl->data = (LPWSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                   num_chars * sizeof(WCHAR));
    if (impl->data) {
      MultiByteToWideChar(CP_ACP, 0, str, -1, impl->data, num_chars);
      impl->len = num_chars - 1;
    } else {
      impl->len = 0;
    }
  } else {
    impl->data = AllocString(0);
    impl->len = 0;
  }
}

String::String(const wchar_t *str) {
  impl = (StringImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                 sizeof(StringImpl));

  if (!str) {
    const wchar_t *nullStr = L"null";
    int len = 4;
    impl->data = AllocString(len);
    if (impl->data) {
      lstrcpyW(impl->data, nullStr);
      impl->len = len;
    } else {
      impl->len = 0;
    }
    return;
  }

  int len = lstrlenW(str);
  impl->data = AllocString(len);
  if (impl->data) {
    lstrcpyW(impl->data, str);
    impl->len = len;
  } else {
    impl->len = 0;
  }
}

String::String(const String &other) {
  impl = (StringImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                 sizeof(StringImpl));

  if (other.impl) {
    ReadLockGuard guard(&other.impl->lock);
    if (other.impl->data) {
      int len = other.impl->len;
      impl->data = AllocString(len);
      if (impl->data) {
        lstrcpyW(impl->data, other.impl->data);
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

int String::length() const {
  if (!impl)
    return 0;
  ReadLockGuard guard(&impl->lock);
  return impl->len;
}

String String::duplicate() const {
  return String(*this);
}

const wchar_t *String::c_str() const {
  return (impl && impl->data) ? impl->data : L"";
}

} // namespace attoboy
