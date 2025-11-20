#include "attoset_internal.h"

namespace attoboy {

// Template specializations for contains()
template <> bool Set::contains<bool>(bool value) const {
  if (!impl)
    return false;
  ReadLockGuard guard(&impl->lock);
  return impl->values.contains<bool>(value);
}

template <> bool Set::contains<int>(int value) const {
  if (!impl)
    return false;
  ReadLockGuard guard(&impl->lock);
  return impl->values.contains<int>(value);
}

template <> bool Set::contains<long long>(long long value) const {
  if (!impl)
    return false;
  ReadLockGuard guard(&impl->lock);
  return impl->values.contains<long long>(value);
}

template <> bool Set::contains<double>(double value) const {
  if (!impl)
    return false;
  ReadLockGuard guard(&impl->lock);
  return impl->values.contains<double>(value);
}

template <> bool Set::contains<String>(String value) const {
  if (!impl)
    return false;
  ReadLockGuard guard(&impl->lock);
  return impl->values.contains<String>(value);
}

template <> bool Set::contains<const char *>(const char *value) const {
  if (!impl)
    return false;
  ReadLockGuard guard(&impl->lock);
  return impl->values.contains<const char *>(value);
}

template <> bool Set::contains<const wchar_t *>(const wchar_t *value) const {
  if (!impl)
    return false;
  ReadLockGuard guard(&impl->lock);
  return impl->values.contains<const wchar_t *>(value);
}

// Remove implementations
void Set::remove_impl(bool value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->values.find<bool>(value);
  if (index >= 0) {
    impl->values.remove(index);
  }
}

void Set::remove_impl(int value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->values.find<int>(value);
  if (index >= 0) {
    impl->values.remove(index);
  }
}

void Set::remove_impl(long long value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->values.find<long long>(value);
  if (index >= 0) {
    impl->values.remove(index);
  }
}

void Set::remove_impl(double value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->values.find<double>(value);
  if (index >= 0) {
    impl->values.remove(index);
  }
}

void Set::remove_impl(const char *value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->values.find<const char *>(value);
  if (index >= 0) {
    impl->values.remove(index);
  }
}

void Set::remove_impl(const wchar_t *value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->values.find<const wchar_t *>(value);
  if (index >= 0) {
    impl->values.remove(index);
  }
}

void Set::remove_impl(const String &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->values.find<String>(value);
  if (index >= 0) {
    impl->values.remove(index);
  }
}

} // namespace attoboy
