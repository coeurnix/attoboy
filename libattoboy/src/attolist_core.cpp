#include "attolist_internal.h"

namespace attoboy {

List::List() {
  impl = (ListImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                               sizeof(ListImpl));
  if (impl) {
    impl->items = AllocItems(8);
    impl->size = 0;
    impl->capacity = 8;
  }
}

List::List(int capacity) {
  impl = (ListImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                               sizeof(ListImpl));
  if (impl) {
    if (capacity < 0)
      capacity = 0;
    if (capacity == 0)
      capacity = 8;
    impl->items = AllocItems(capacity);
    impl->size = 0;
    impl->capacity = capacity;
  }
}

List::List(const List &other) {
  impl = (ListImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                               sizeof(ListImpl));
  if (!impl)
    return;

  if (other.impl) {
    ReadLockGuard guard(&other.impl->lock);

    impl->capacity = other.impl->capacity;
    impl->size = other.impl->size;
    impl->items = AllocItems(impl->capacity);

    if (impl->items) {
      for (int i = 0; i < impl->size; i++) {
        impl->items[i].type = other.impl->items[i].type;

        switch (other.impl->items[i].type) {
        case TYPE_BOOL:
          impl->items[i].boolVal = other.impl->items[i].boolVal;
          break;
        case TYPE_INT:
          impl->items[i].intVal = other.impl->items[i].intVal;
          break;
        case TYPE_LONG_LONG:
          impl->items[i].longLongVal = other.impl->items[i].longLongVal;
          break;
        case TYPE_DOUBLE:
          impl->items[i].doubleVal = other.impl->items[i].doubleVal;
          break;
        case TYPE_STRING:
          if (other.impl->items[i].stringVal) {
            impl->items[i].stringVal =
                AllocString(*other.impl->items[i].stringVal);
          } else {
            impl->items[i].stringVal = nullptr;
          }
          break;
        case TYPE_LIST:
          if (other.impl->items[i].listVal) {
            impl->items[i].listVal = AllocList(*other.impl->items[i].listVal);
          } else {
            impl->items[i].listVal = nullptr;
          }
          break;
        case TYPE_MAP:
          if (other.impl->items[i].mapVal) {
            impl->items[i].mapVal = AllocMap(*(Map *)other.impl->items[i].mapVal);
          } else {
            impl->items[i].mapVal = nullptr;
          }
          break;
        case TYPE_SET:
          if (other.impl->items[i].setVal) {
            impl->items[i].setVal = AllocSet(*(Set *)other.impl->items[i].setVal);
          } else {
            impl->items[i].setVal = nullptr;
          }
          break;
        default:
          break;
        }
      }
    } else {
      impl->size = 0;
      impl->capacity = 0;
    }
  } else {
    impl->items = AllocItems(8);
    impl->size = 0;
    impl->capacity = 8;
  }
}

List::List(const Set &set) {
  impl = (ListImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                               sizeof(ListImpl));
  if (!impl)
    return;

  impl->items = AllocItems(8);
  impl->size = 0;
  impl->capacity = 8;
  InitializeSRWLock(&impl->lock);

  List temp = set.toList();
  if (temp.impl) {
    ReadLockGuard guard(&temp.impl->lock);

    impl->capacity = temp.impl->capacity;
    impl->size = temp.impl->size;
    FreeItems(impl->items);
    impl->items = AllocItems(impl->capacity);

    if (impl->items) {
      for (int i = 0; i < impl->size; i++) {
        impl->items[i].type = temp.impl->items[i].type;

        switch (temp.impl->items[i].type) {
        case TYPE_BOOL:
          impl->items[i].boolVal = temp.impl->items[i].boolVal;
          break;
        case TYPE_INT:
          impl->items[i].intVal = temp.impl->items[i].intVal;
          break;
        case TYPE_LONG_LONG:
          impl->items[i].longLongVal = temp.impl->items[i].longLongVal;
          break;
        case TYPE_DOUBLE:
          impl->items[i].doubleVal = temp.impl->items[i].doubleVal;
          break;
        case TYPE_STRING:
          if (temp.impl->items[i].stringVal) {
            impl->items[i].stringVal =
                AllocString(*temp.impl->items[i].stringVal);
          } else {
            impl->items[i].stringVal = nullptr;
          }
          break;
        case TYPE_LIST:
          if (temp.impl->items[i].listVal) {
            impl->items[i].listVal = AllocList(*temp.impl->items[i].listVal);
          } else {
            impl->items[i].listVal = nullptr;
          }
          break;
        case TYPE_MAP:
          if (temp.impl->items[i].mapVal) {
            impl->items[i].mapVal = AllocMap(*(Map *)temp.impl->items[i].mapVal);
          } else {
            impl->items[i].mapVal = nullptr;
          }
          break;
        case TYPE_SET:
          if (temp.impl->items[i].setVal) {
            impl->items[i].setVal = AllocSet(*(Set *)temp.impl->items[i].setVal);
          } else {
            impl->items[i].setVal = nullptr;
          }
          break;
        default:
          break;
        }
      }
    } else {
      impl->size = 0;
      impl->capacity = 0;
    }
  }
}

List::~List() {
  if (impl) {
    if (impl->items) {
      for (int i = 0; i < impl->size; i++) {
        FreeItemContents(&impl->items[i]);
      }
      FreeItems(impl->items);
    }
    HeapFree(GetProcessHeap(), 0, impl);
  }
}

int List::length() const {
  if (!impl)
    return 0;
  ReadLockGuard guard(&impl->lock);
  return impl->size;
}

bool List::isEmpty() const {
  if (!impl)
    return true;
  ReadLockGuard guard(&impl->lock);
  return impl->size == 0;
}

List &List::clear() {
  if (!impl)
    return *this;
  WriteLockGuard guard(&impl->lock);

  for (int i = 0; i < impl->size; i++) {
    FreeItemContents(&impl->items[i]);
  }
  impl->size = 0;
  return *this;
}

} // namespace attoboy
