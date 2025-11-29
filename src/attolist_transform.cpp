#include "attolist_internal.h"

namespace attoboy {

List &List::reverse() {
  if (!impl)
    return *this;
  WriteLockGuard guard(&impl->lock);

  if (impl->size <= 1)
    return *this;

  for (int i = 0; i < impl->size / 2; i++) {
    int j = impl->size - 1 - i;
    ListItem temp = impl->items[i];
    impl->items[i] = impl->items[j];
    impl->items[j] = temp;
  }

  return *this;
}

List &List::concat(const List &other) {
  if (!impl)
    return *this;
  if (!other.impl)
    return *this;

  WriteLockGuard guardThis(&impl->lock);
  ReadLockGuard guardOther(&other.impl->lock);

  if (other.impl->size == 0)
    return *this;

  int newSize = impl->size + other.impl->size;
  if (!EnsureCapacity(impl, newSize))
    return *this;

  for (int i = 0; i < other.impl->size; i++) {
    int targetIndex = impl->size + i;
    impl->items[targetIndex].type = other.impl->items[i].type;

    switch (other.impl->items[i].type) {
    case TYPE_BOOL:
      impl->items[targetIndex].boolVal = other.impl->items[i].boolVal;
      break;
    case TYPE_INT:
      impl->items[targetIndex].intVal = other.impl->items[i].intVal;
      break;
    case TYPE_FLOAT:
      impl->items[targetIndex].floatVal = other.impl->items[i].floatVal;
      break;
    case TYPE_STRING:
      if (other.impl->items[i].stringVal) {
        impl->items[targetIndex].stringVal =
            AllocString(*other.impl->items[i].stringVal);
      } else {
        impl->items[targetIndex].stringVal = nullptr;
      }
      break;
    case TYPE_LIST:
      if (other.impl->items[i].listVal) {
        impl->items[targetIndex].listVal =
            AllocList(*other.impl->items[i].listVal);
      } else {
        impl->items[targetIndex].listVal = nullptr;
      }
      break;
    case TYPE_MAP:
      if (other.impl->items[i].mapVal) {
        impl->items[targetIndex].mapVal =
            AllocMap(*(Map *)other.impl->items[i].mapVal);
      } else {
        impl->items[targetIndex].mapVal = nullptr;
      }
      break;
    case TYPE_SET:
      if (other.impl->items[i].setVal) {
        impl->items[targetIndex].setVal =
            AllocSet(*(Set *)other.impl->items[i].setVal);
      } else {
        impl->items[targetIndex].setVal = nullptr;
      }
      break;
    default:
      break;
    }
  }

  impl->size = newSize;
  return *this;
}

List &List::concat(const Set &set) {
  List temp = set.toList();
  return concat(temp);
}

List List::slice(int start, int end) const {
  if (!impl)
    return List();

  ReadLockGuard guard(&impl->lock);

  if (impl->size == 0)
    return List();

  if (start < 0)
    start = 0;
  if (end < 0)
    end = impl->size;

  if (start >= impl->size)
    return List();
  if (end > impl->size)
    end = impl->size;
  if (start >= end)
    return List();

  int sliceSize = end - start;
  List result(sliceSize);

  if (!result.impl || !result.impl->items)
    return List();

  for (int i = 0; i < sliceSize; i++) {
    int sourceIndex = start + i;
    result.impl->items[i].type = impl->items[sourceIndex].type;

    switch (impl->items[sourceIndex].type) {
    case TYPE_BOOL:
      result.impl->items[i].boolVal = impl->items[sourceIndex].boolVal;
      break;
    case TYPE_INT:
      result.impl->items[i].intVal = impl->items[sourceIndex].intVal;
      break;
    case TYPE_FLOAT:
      result.impl->items[i].floatVal = impl->items[sourceIndex].floatVal;
      break;
    case TYPE_STRING:
      if (impl->items[sourceIndex].stringVal) {
        result.impl->items[i].stringVal =
            AllocString(*impl->items[sourceIndex].stringVal);
      } else {
        result.impl->items[i].stringVal = nullptr;
      }
      break;
    case TYPE_LIST:
      if (impl->items[sourceIndex].listVal) {
        result.impl->items[i].listVal =
            AllocList(*impl->items[sourceIndex].listVal);
      } else {
        result.impl->items[i].listVal = nullptr;
      }
      break;
    case TYPE_MAP:
      if (impl->items[sourceIndex].mapVal) {
        result.impl->items[i].mapVal =
            AllocMap(*(Map *)impl->items[sourceIndex].mapVal);
      } else {
        result.impl->items[i].mapVal = nullptr;
      }
      break;
    case TYPE_SET:
      if (impl->items[sourceIndex].setVal) {
        result.impl->items[i].setVal =
            AllocSet(*(Set *)impl->items[sourceIndex].setVal);
      } else {
        result.impl->items[i].setVal = nullptr;
      }
      break;
    default:
      break;
    }
  }

  result.impl->size = sliceSize;
  return result;
}

List List::duplicate() const { return List(*this); }

} // namespace attoboy
