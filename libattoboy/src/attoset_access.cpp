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

bool Set::compare(const Set &other) const {
  if (!impl && !other.impl)
    return true;
  if (!impl || !other.impl)
    return false;

  ReadLockGuard guard1(&impl->lock);
  ReadLockGuard guard2(&other.impl->lock);

  if (impl->values.length() != other.impl->values.length())
    return false;

  List myValues = impl->values.duplicate();
  int len = myValues.length();

  for (int i = 0; i < len; i++) {
    ValueType type = myValues.typeAt(i);
    bool found = false;

    switch (type) {
    case TYPE_BOOL:
      found = other.contains<bool>(myValues.at<bool>(i));
      break;
    case TYPE_INT:
      found = other.contains<int>(myValues.at<int>(i));
      break;
    case TYPE_LONG_LONG:
      found = other.contains<long long>(myValues.at<long long>(i));
      break;
    case TYPE_DOUBLE:
      found = other.contains<double>(myValues.at<double>(i));
      break;
    case TYPE_STRING:
      found = other.contains<String>(myValues.at<String>(i));
      break;
    default:
      break;
    }

    if (!found)
      return false;
  }

  return true;
}

bool Set::operator==(const Set &other) const { return compare(other); }

bool Set::operator!=(const Set &other) const { return !compare(other); }

} // namespace attoboy
