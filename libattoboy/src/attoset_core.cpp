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
    case TYPE_LONG_LONG:
      put_impl(list.at<long long>(i));
      break;
    case TYPE_DOUBLE:
      put_impl(list.at<double>(i));
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

Set &Set::merge(const Set &other) {
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
    case TYPE_LONG_LONG:
      put_impl(otherValues.at<long long>(i));
      break;
    case TYPE_DOUBLE:
      put_impl(otherValues.at<double>(i));
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

__attribute__((weak)) Set *AllocSet() {
  void *mem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Set));
  if (!mem)
    return nullptr;
  return new (mem) Set();
}

__attribute__((weak)) Set *AllocSet(const Set &other) {
  void *mem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Set));
  if (!mem)
    return nullptr;
  return new (mem) Set(other);
}

__attribute__((weak)) void FreeSet(Set *set) {
  if (!set)
    return;
  set->~Set();
  HeapFree(GetProcessHeap(), 0, set);
}

} // namespace attoboy
