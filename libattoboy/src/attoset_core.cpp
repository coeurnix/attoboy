#include "attoset_internal.h"

namespace attoboy {

Set::Set() {
  impl = (SetImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                              sizeof(SetImpl));
  if (impl) {
    new (impl) SetImpl();
  }
}

Set::Set(int capacity) {
  impl = (SetImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                              sizeof(SetImpl));
  if (impl) {
    new (impl) SetImpl();
    impl->values = List(capacity);
  }
}

Set::Set(const Set &other) {
  impl = (SetImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                              sizeof(SetImpl));
  if (!impl)
    return;

  new (impl) SetImpl();

  if (other.impl) {
    ReadLockGuard guard(&other.impl->lock);
    impl->values = other.impl->values.duplicate();
  }
}

Set::Set(const List &list) {
  impl = (SetImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                              sizeof(SetImpl));
  if (!impl)
    return;

  new (impl) SetImpl();

  int len = list.length();
  for (int i = 0; i < len; i++) {
    ValueType type = list.typeAt(i);
    switch (type) {
    case TYPE_BOOL:
      put_impl(list.at<bool>(i));
      break;
    case TYPE_INT:
      put_impl(list.at<int>(i));
      break;
    case TYPE_FLOAT:
      put_impl(list.at<float>(i));
      break;
    case TYPE_STRING:
      put_impl(list.at<String>(i));
      break;
    case TYPE_LIST:
      put_impl(list.at<List>(i));
      break;
    case TYPE_MAP:
      put_impl(list.at<Map>(i));
      break;
    case TYPE_SET:
      put_impl(list.at<Set>(i));
      break;
    default:
      break;
    }
  }
}

Set::~Set() {
  if (impl) {
    impl->~SetImpl();
    HeapFree(GetProcessHeap(), 0, impl);
  }
}

Set &Set::operator=(const Set &other) {
  if (this != &other) {
    Set temp(other);
    SetImpl *oldImpl = impl;
    impl = temp.impl;
    temp.impl = oldImpl;
  }
  return *this;
}

int Set::length() const {
  if (!impl)
    return 0;
  ReadLockGuard guard(&impl->lock);
  return impl->values.length();
}

bool Set::isEmpty() const {
  if (!impl)
    return true;
  ReadLockGuard guard(&impl->lock);
  return impl->values.isEmpty();
}

Set &Set::clear() {
  if (!impl)
    return *this;
  WriteLockGuard guard(&impl->lock);
  impl->values.clear();
  return *this;
}

Set Set::duplicate() const {
  return Set(*this);
}

List Set::toList() const {
  if (!impl)
    return List();
  ReadLockGuard guard(&impl->lock);
  return impl->values.duplicate();
}

Set &Set::setUnion(const Set &other) {
  if (!impl || !other.impl)
    return *this;

  if (this == &other)
    return *this;

  List otherValues = other.toList();
  int len = otherValues.length();

  for (int i = 0; i < len; i++) {
    ValueType type = otherValues.typeAt(i);
    switch (type) {
    case TYPE_BOOL:
      put_impl(otherValues.at<bool>(i));
      break;
    case TYPE_INT:
      put_impl(otherValues.at<int>(i));
      break;
    case TYPE_FLOAT:
      put_impl(otherValues.at<float>(i));
      break;
    case TYPE_STRING:
      put_impl(otherValues.at<String>(i));
      break;
    case TYPE_LIST:
      put_impl(otherValues.at<List>(i));
      break;
    case TYPE_MAP:
      put_impl(otherValues.at<Map>(i));
      break;
    case TYPE_SET:
      put_impl(otherValues.at<Set>(i));
      break;
    default:
      break;
    }
  }

  return *this;
}

Set &Set::intersect(const Set &other) {
  if (!impl || !other.impl)
    return *this;

  if (this == &other)
    return *this;

  List currentValues = toList();
  clear();

  int len = currentValues.length();
  for (int i = 0; i < len; i++) {
    ValueType type = currentValues.typeAt(i);
    bool shouldKeep = false;

    switch (type) {
    case TYPE_BOOL:
      shouldKeep = other.contains<bool>(currentValues.at<bool>(i));
      if (shouldKeep)
        put_impl(currentValues.at<bool>(i));
      break;
    case TYPE_INT:
      shouldKeep = other.contains<int>(currentValues.at<int>(i));
      if (shouldKeep)
        put_impl(currentValues.at<int>(i));
      break;
    case TYPE_FLOAT:
      shouldKeep = other.contains<float>(currentValues.at<float>(i));
      if (shouldKeep)
        put_impl(currentValues.at<float>(i));
      break;
    case TYPE_STRING:
      shouldKeep = other.contains<String>(currentValues.at<String>(i));
      if (shouldKeep)
        put_impl(currentValues.at<String>(i));
      break;
    default:
      break;
    }
  }

  return *this;
}

Set &Set::subtract(const Set &other) {
  if (!impl || !other.impl)
    return *this;

  if (this == &other) {
    clear();
    return *this;
  }

  List otherValues = other.toList();
  int len = otherValues.length();

  for (int i = 0; i < len; i++) {
    ValueType type = otherValues.typeAt(i);
    switch (type) {
    case TYPE_BOOL:
      remove_impl(otherValues.at<bool>(i));
      break;
    case TYPE_INT:
      remove_impl(otherValues.at<int>(i));
      break;
    case TYPE_FLOAT:
      remove_impl(otherValues.at<float>(i));
      break;
    case TYPE_STRING:
      remove_impl(otherValues.at<String>(i));
      break;
    default:
      break;
    }
  }

  return *this;
}

} // namespace attoboy
