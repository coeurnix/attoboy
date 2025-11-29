#include "attoset_internal.h"
#include "attostring_internal.h"

namespace attoboy {

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

template <> bool Set::contains<float>(float value) const {
  if (!impl)
    return false;
  ReadLockGuard guard(&impl->lock);
  return impl->values.contains<float>(value);
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

SetValueView::SetValueView() : set(nullptr), index(0) {}

SetValueView::SetValueView(const Set *setPtr, int idx)
    : set(setPtr), index(idx) {}

SetValueView::operator bool() const {
  if (!set || !set->impl)
    return false;
  ReadLockGuard guard(&set->impl->lock);
  return set->impl->values.at<bool>(index);
}

SetValueView::operator int() const {
  if (!set || !set->impl)
    return 0;
  ReadLockGuard guard(&set->impl->lock);
  return set->impl->values.at<int>(index);
}

SetValueView::operator float() const {
  if (!set || !set->impl)
    return 0.0f;
  ReadLockGuard guard(&set->impl->lock);
  return set->impl->values.at<float>(index);
}

SetValueView::operator String() const {
  if (!set || !set->impl)
    return String();
  ReadLockGuard guard(&set->impl->lock);
  return set->impl->values.at<String>(index);
}

SetValueView::operator List() const {
  if (!set || !set->impl)
    return List();
  ReadLockGuard guard(&set->impl->lock);
  return set->impl->values.at<List>(index);
}

SetValueView::operator Map() const {
  if (!set || !set->impl)
    return Map();
  ReadLockGuard guard(&set->impl->lock);
  return set->impl->values.at<Map>(index);
}

SetValueView::operator Set() const {
  if (!set || !set->impl)
    return Set();
  ReadLockGuard guard(&set->impl->lock);
  return set->impl->values.at<Set>(index);
}

SetValueView Set::at(int index) const { return SetValueView(this, index); }

SetValueView Set::operator[](int index) const {
  return SetValueView(this, index);
}

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

void Set::remove_impl(float value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->values.find<float>(value);
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
    case TYPE_FLOAT:
      found = other.contains<float>(myValues.at<float>(i));
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
