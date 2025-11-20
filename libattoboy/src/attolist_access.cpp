#include "attolist_internal.h"

namespace attoboy {

// Helper function to clamp index to valid range
static inline int ClampIndex(int index, int size) {
  if (size == 0)
    return 0;
  if (index < 0)
    index = 0;
  if (index >= size)
    index = size - 1;
  return index;
}

ValueType List::typeAt(int index) const {
  if (!impl)
    return TYPE_INVALID;
  ReadLockGuard guard(&impl->lock);

  if (index < 0 || index >= impl->size)
    return TYPE_INVALID;

  return impl->items[index].type;
}

// Template specializations for at()
template <> bool List::at<bool>(int index) const {
  if (!impl)
    return false;
  ReadLockGuard guard(&impl->lock);

  if (impl->size == 0)
    return false;
  index = ClampIndex(index, impl->size);

  if (impl->items[index].type == TYPE_BOOL)
    return impl->items[index].boolVal;
  return false;
}

template <> int List::at<int>(int index) const {
  if (!impl)
    return 0;
  ReadLockGuard guard(&impl->lock);

  if (impl->size == 0)
    return 0;
  index = ClampIndex(index, impl->size);

  if (impl->items[index].type == TYPE_INT)
    return impl->items[index].intVal;
  if (impl->items[index].type == TYPE_LONG_LONG)
    return (int)impl->items[index].longLongVal;
  if (impl->items[index].type == TYPE_DOUBLE)
    return (int)impl->items[index].doubleVal;
  return 0;
}

template <> long long List::at<long long>(int index) const {
  if (!impl)
    return 0LL;
  ReadLockGuard guard(&impl->lock);

  if (impl->size == 0)
    return 0LL;
  index = ClampIndex(index, impl->size);

  if (impl->items[index].type == TYPE_LONG_LONG)
    return impl->items[index].longLongVal;
  if (impl->items[index].type == TYPE_INT)
    return (long long)impl->items[index].intVal;
  if (impl->items[index].type == TYPE_DOUBLE)
    return (long long)impl->items[index].doubleVal;
  return 0LL;
}

template <> double List::at<double>(int index) const {
  if (!impl)
    return 0.0;
  ReadLockGuard guard(&impl->lock);

  if (impl->size == 0)
    return 0.0;
  index = ClampIndex(index, impl->size);

  if (impl->items[index].type == TYPE_DOUBLE)
    return impl->items[index].doubleVal;
  if (impl->items[index].type == TYPE_INT)
    return (double)impl->items[index].intVal;
  if (impl->items[index].type == TYPE_LONG_LONG)
    return (double)impl->items[index].longLongVal;
  return 0.0;
}

template <> String List::at<String>(int index) const {
  if (!impl)
    return String();
  ReadLockGuard guard(&impl->lock);

  if (impl->size == 0)
    return String();
  index = ClampIndex(index, impl->size);

  if (impl->items[index].type == TYPE_STRING && impl->items[index].stringVal)
    return *impl->items[index].stringVal;
  return String();
}

template <> List List::at<List>(int index) const {
  if (!impl)
    return List();
  ReadLockGuard guard(&impl->lock);

  if (impl->size == 0)
    return List();
  index = ClampIndex(index, impl->size);

  if (impl->items[index].type == TYPE_LIST && impl->items[index].listVal)
    return *impl->items[index].listVal;
  return List();
}

template <> Map List::at<Map>(int index) const {
  if (!impl)
    return Map();
  ReadLockGuard guard(&impl->lock);

  if (impl->size == 0)
    return Map();
  index = ClampIndex(index, impl->size);

  if (impl->items[index].type == TYPE_MAP && impl->items[index].mapVal)
    return *(Map *)impl->items[index].mapVal;
  return Map();
}

template <> Set List::at<Set>(int index) const {
  if (!impl)
    return Set();
  ReadLockGuard guard(&impl->lock);

  if (impl->size == 0)
    return Set();
  index = ClampIndex(index, impl->size);

  if (impl->items[index].type == TYPE_SET && impl->items[index].setVal)
    return *(Set *)impl->items[index].setVal;
  return Set();
}

// Template specializations for operator[]
template <> bool List::operator[]<bool>(int index) const {
  return at<bool>(index);
}

template <> int List::operator[]<int>(int index) const {
  return at<int>(index);
}

template <> long long List::operator[]<long long>(int index) const {
  return at<long long>(index);
}

template <> double List::operator[]<double>(int index) const {
  return at<double>(index);
}

template <> String List::operator[]<String>(int index) const {
  return at<String>(index);
}

template <> List List::operator[]<List>(int index) const {
  return at<List>(index);
}

template <> Map List::operator[]<Map>(int index) const {
  return at<Map>(index);
}

template <> Set List::operator[]<Set>(int index) const {
  return at<Set>(index);
}

// Set implementations
void List::set_impl(int index, bool value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (impl->size == 0) {
    if (!EnsureCapacity(impl, 1))
      return;
    impl->items[0].type = TYPE_BOOL;
    impl->items[0].boolVal = value;
    impl->size = 1;
    return;
  }

  index = ClampIndex(index, impl->size);
  FreeItemContents(&impl->items[index]);
  impl->items[index].type = TYPE_BOOL;
  impl->items[index].boolVal = value;
}

void List::set_impl(int index, int value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (impl->size == 0) {
    if (!EnsureCapacity(impl, 1))
      return;
    impl->items[0].type = TYPE_INT;
    impl->items[0].intVal = value;
    impl->size = 1;
    return;
  }

  index = ClampIndex(index, impl->size);
  FreeItemContents(&impl->items[index]);
  impl->items[index].type = TYPE_INT;
  impl->items[index].intVal = value;
}

void List::set_impl(int index, long long value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (impl->size == 0) {
    if (!EnsureCapacity(impl, 1))
      return;
    impl->items[0].type = TYPE_LONG_LONG;
    impl->items[0].longLongVal = value;
    impl->size = 1;
    return;
  }

  index = ClampIndex(index, impl->size);
  FreeItemContents(&impl->items[index]);
  impl->items[index].type = TYPE_LONG_LONG;
  impl->items[index].longLongVal = value;
}

void List::set_impl(int index, double value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (impl->size == 0) {
    if (!EnsureCapacity(impl, 1))
      return;
    impl->items[0].type = TYPE_DOUBLE;
    impl->items[0].doubleVal = value;
    impl->size = 1;
    return;
  }

  index = ClampIndex(index, impl->size);
  FreeItemContents(&impl->items[index]);
  impl->items[index].type = TYPE_DOUBLE;
  impl->items[index].doubleVal = value;
}

void List::set_impl(int index, const char *value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (impl->size == 0) {
    if (!EnsureCapacity(impl, 1))
      return;
    impl->items[0].type = TYPE_STRING;
    impl->items[0].stringVal = AllocString(value);
    impl->size = 1;
    return;
  }

  index = ClampIndex(index, impl->size);
  FreeItemContents(&impl->items[index]);
  impl->items[index].type = TYPE_STRING;
  impl->items[index].stringVal = AllocString(value);
}

void List::set_impl(int index, const wchar_t *value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (impl->size == 0) {
    if (!EnsureCapacity(impl, 1))
      return;
    impl->items[0].type = TYPE_STRING;
    impl->items[0].stringVal = AllocString(value);
    impl->size = 1;
    return;
  }

  index = ClampIndex(index, impl->size);
  FreeItemContents(&impl->items[index]);
  impl->items[index].type = TYPE_STRING;
  impl->items[index].stringVal = AllocString(value);
}

void List::set_impl(int index, const String &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (impl->size == 0) {
    if (!EnsureCapacity(impl, 1))
      return;
    impl->items[0].type = TYPE_STRING;
    impl->items[0].stringVal = AllocString(value);
    impl->size = 1;
    return;
  }

  index = ClampIndex(index, impl->size);
  FreeItemContents(&impl->items[index]);
  impl->items[index].type = TYPE_STRING;
  impl->items[index].stringVal = AllocString(value);
}

void List::set_impl(int index, const List &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (impl->size == 0) {
    if (!EnsureCapacity(impl, 1))
      return;
    impl->items[0].type = TYPE_LIST;
    impl->items[0].listVal = AllocList(value);
    impl->size = 1;
    return;
  }

  index = ClampIndex(index, impl->size);
  FreeItemContents(&impl->items[index]);
  impl->items[index].type = TYPE_LIST;
  impl->items[index].listVal = AllocList(value);
}

void List::set_impl(int index, const Map &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (impl->size == 0) {
    if (!EnsureCapacity(impl, 1))
      return;
    impl->items[0].type = TYPE_MAP;
    impl->items[0].mapVal = AllocMap(value);
    impl->size = 1;
    return;
  }

  index = ClampIndex(index, impl->size);
  FreeItemContents(&impl->items[index]);
  impl->items[index].type = TYPE_MAP;
  impl->items[index].mapVal = AllocMap(value);
}

void List::set_impl(int index, const Set &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (impl->size == 0) {
    if (!EnsureCapacity(impl, 1))
      return;
    impl->items[0].type = TYPE_SET;
    impl->items[0].setVal = AllocSet(value);
    impl->size = 1;
    return;
  }

  index = ClampIndex(index, impl->size);
  FreeItemContents(&impl->items[index]);
  impl->items[index].type = TYPE_SET;
  impl->items[index].setVal = AllocSet(value);
}

} // namespace attoboy
