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
