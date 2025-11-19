#include "attolist_internal.h"

namespace attoboy {

// Prepend implementations
void List::prepend_impl(bool value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  // Shift all items down by one
  for (int i = impl->size; i > 0; i--) {
    impl->items[i] = impl->items[i - 1];
  }

  impl->items[0].type = TYPE_BOOL;
  impl->items[0].boolVal = value;
  impl->size++;
}

void List::prepend_impl(int value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  for (int i = impl->size; i > 0; i--) {
    impl->items[i] = impl->items[i - 1];
  }

  impl->items[0].type = TYPE_INT;
  impl->items[0].intVal = value;
  impl->size++;
}

void List::prepend_impl(long long value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  for (int i = impl->size; i > 0; i--) {
    impl->items[i] = impl->items[i - 1];
  }

  impl->items[0].type = TYPE_LONG_LONG;
  impl->items[0].longLongVal = value;
  impl->size++;
}

void List::prepend_impl(double value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  for (int i = impl->size; i > 0; i--) {
    impl->items[i] = impl->items[i - 1];
  }

  impl->items[0].type = TYPE_DOUBLE;
  impl->items[0].doubleVal = value;
  impl->size++;
}

void List::prepend_impl(const char *value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  for (int i = impl->size; i > 0; i--) {
    impl->items[i] = impl->items[i - 1];
  }

  impl->items[0].type = TYPE_STRING;
  impl->items[0].stringVal = AllocString(value);
  impl->size++;
}

void List::prepend_impl(const wchar_t *value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  for (int i = impl->size; i > 0; i--) {
    impl->items[i] = impl->items[i - 1];
  }

  impl->items[0].type = TYPE_STRING;
  impl->items[0].stringVal = AllocString(value);
  impl->size++;
}

void List::prepend_impl(const String &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  for (int i = impl->size; i > 0; i--) {
    impl->items[i] = impl->items[i - 1];
  }

  impl->items[0].type = TYPE_STRING;
  impl->items[0].stringVal = AllocString(value);
  impl->size++;
}

void List::prepend_impl(const List &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  for (int i = impl->size; i > 0; i--) {
    impl->items[i] = impl->items[i - 1];
  }

  impl->items[0].type = TYPE_LIST;
  impl->items[0].listVal = AllocList(value);
  impl->size++;
}

// Insert implementations
void List::insert_impl(int index, bool value) {
  if (!impl)
    return;

  WriteLockGuard guard(&impl->lock);

  if (index < 0)
    index = 0;

  if (index >= impl->size) {
    if (!EnsureCapacity(impl, impl->size + 1))
      return;
    impl->items[impl->size].type = TYPE_BOOL;
    impl->items[impl->size].boolVal = value;
    impl->size++;
    return;
  }

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  for (int i = impl->size; i > index; i--) {
    impl->items[i] = impl->items[i - 1];
  }

  impl->items[index].type = TYPE_BOOL;
  impl->items[index].boolVal = value;
  impl->size++;
}

void List::insert_impl(int index, int value) {
  if (!impl)
    return;

  WriteLockGuard guard(&impl->lock);

  if (index < 0)
    index = 0;

  if (index >= impl->size) {
    if (!EnsureCapacity(impl, impl->size + 1))
      return;
    impl->items[impl->size].type = TYPE_INT;
    impl->items[impl->size].intVal = value;
    impl->size++;
    return;
  }

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  for (int i = impl->size; i > index; i--) {
    impl->items[i] = impl->items[i - 1];
  }

  impl->items[index].type = TYPE_INT;
  impl->items[index].intVal = value;
  impl->size++;
}

void List::insert_impl(int index, long long value) {
  if (!impl)
    return;

  WriteLockGuard guard(&impl->lock);

  if (index < 0)
    index = 0;

  if (index >= impl->size) {
    if (!EnsureCapacity(impl, impl->size + 1))
      return;
    impl->items[impl->size].type = TYPE_LONG_LONG;
    impl->items[impl->size].longLongVal = value;
    impl->size++;
    return;
  }

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  for (int i = impl->size; i > index; i--) {
    impl->items[i] = impl->items[i - 1];
  }

  impl->items[index].type = TYPE_LONG_LONG;
  impl->items[index].longLongVal = value;
  impl->size++;
}

void List::insert_impl(int index, double value) {
  if (!impl)
    return;

  WriteLockGuard guard(&impl->lock);

  if (index < 0)
    index = 0;

  if (index >= impl->size) {
    if (!EnsureCapacity(impl, impl->size + 1))
      return;
    impl->items[impl->size].type = TYPE_DOUBLE;
    impl->items[impl->size].doubleVal = value;
    impl->size++;
    return;
  }

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  for (int i = impl->size; i > index; i--) {
    impl->items[i] = impl->items[i - 1];
  }

  impl->items[index].type = TYPE_DOUBLE;
  impl->items[index].doubleVal = value;
  impl->size++;
}

void List::insert_impl(int index, const char *value) {
  if (!impl)
    return;

  WriteLockGuard guard(&impl->lock);

  if (index < 0)
    index = 0;

  if (index >= impl->size) {
    if (!EnsureCapacity(impl, impl->size + 1))
      return;
    impl->items[impl->size].type = TYPE_STRING;
    impl->items[impl->size].stringVal = AllocString(value);
    impl->size++;
    return;
  }

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  for (int i = impl->size; i > index; i--) {
    impl->items[i] = impl->items[i - 1];
  }

  impl->items[index].type = TYPE_STRING;
  impl->items[index].stringVal = AllocString(value);
  impl->size++;
}

void List::insert_impl(int index, const wchar_t *value) {
  if (!impl)
    return;

  WriteLockGuard guard(&impl->lock);

  if (index < 0)
    index = 0;

  if (index >= impl->size) {
    if (!EnsureCapacity(impl, impl->size + 1))
      return;
    impl->items[impl->size].type = TYPE_STRING;
    impl->items[impl->size].stringVal = AllocString(value);
    impl->size++;
    return;
  }

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  for (int i = impl->size; i > index; i--) {
    impl->items[i] = impl->items[i - 1];
  }

  impl->items[index].type = TYPE_STRING;
  impl->items[index].stringVal = AllocString(value);
  impl->size++;
}

void List::insert_impl(int index, const String &value) {
  if (!impl)
    return;

  WriteLockGuard guard(&impl->lock);

  if (index < 0)
    index = 0;

  if (index >= impl->size) {
    if (!EnsureCapacity(impl, impl->size + 1))
      return;
    impl->items[impl->size].type = TYPE_STRING;
    impl->items[impl->size].stringVal = AllocString(value);
    impl->size++;
    return;
  }

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  for (int i = impl->size; i > index; i--) {
    impl->items[i] = impl->items[i - 1];
  }

  impl->items[index].type = TYPE_STRING;
  impl->items[index].stringVal = AllocString(value);
  impl->size++;
}

void List::insert_impl(int index, const List &value) {
  if (!impl)
    return;

  WriteLockGuard guard(&impl->lock);

  if (index < 0)
    index = 0;

  if (index >= impl->size) {
    if (!EnsureCapacity(impl, impl->size + 1))
      return;
    impl->items[impl->size].type = TYPE_LIST;
    impl->items[impl->size].listVal = AllocList(value);
    impl->size++;
    return;
  }

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  for (int i = impl->size; i > index; i--) {
    impl->items[i] = impl->items[i - 1];
  }

  impl->items[index].type = TYPE_LIST;
  impl->items[index].listVal = AllocList(value);
  impl->size++;
}

// Remove implementation
List &List::remove(int index) {
  if (!impl)
    return *this;
  WriteLockGuard guard(&impl->lock);

  if (impl->size == 0)
    return *this;

  if (index < 0)
    index = 0;
  if (index >= impl->size)
    index = impl->size - 1;

  FreeItemContents(&impl->items[index]);

  for (int i = index; i < impl->size - 1; i++) {
    impl->items[i] = impl->items[i + 1];
  }

  impl->size--;
  return *this;
}

// Pop template specializations
template <> bool List::pop<bool>() {
  if (!impl)
    return false;
  WriteLockGuard guard(&impl->lock);

  if (impl->size == 0)
    return false;

  int lastIndex = impl->size - 1;
  bool result = false;

  if (impl->items[lastIndex].type == TYPE_BOOL)
    result = impl->items[lastIndex].boolVal;

  FreeItemContents(&impl->items[lastIndex]);
  impl->size--;
  return result;
}

template <> int List::pop<int>() {
  if (!impl)
    return 0;
  WriteLockGuard guard(&impl->lock);

  if (impl->size == 0)
    return 0;

  int lastIndex = impl->size - 1;
  int result = 0;

  if (impl->items[lastIndex].type == TYPE_INT)
    result = impl->items[lastIndex].intVal;
  else if (impl->items[lastIndex].type == TYPE_LONG_LONG)
    result = (int)impl->items[lastIndex].longLongVal;
  else if (impl->items[lastIndex].type == TYPE_DOUBLE)
    result = (int)impl->items[lastIndex].doubleVal;

  FreeItemContents(&impl->items[lastIndex]);
  impl->size--;
  return result;
}

template <> long long List::pop<long long>() {
  if (!impl)
    return 0LL;
  WriteLockGuard guard(&impl->lock);

  if (impl->size == 0)
    return 0LL;

  int lastIndex = impl->size - 1;
  long long result = 0LL;

  if (impl->items[lastIndex].type == TYPE_LONG_LONG)
    result = impl->items[lastIndex].longLongVal;
  else if (impl->items[lastIndex].type == TYPE_INT)
    result = (long long)impl->items[lastIndex].intVal;
  else if (impl->items[lastIndex].type == TYPE_DOUBLE)
    result = (long long)impl->items[lastIndex].doubleVal;

  FreeItemContents(&impl->items[lastIndex]);
  impl->size--;
  return result;
}

template <> double List::pop<double>() {
  if (!impl)
    return 0.0;
  WriteLockGuard guard(&impl->lock);

  if (impl->size == 0)
    return 0.0;

  int lastIndex = impl->size - 1;
  double result = 0.0;

  if (impl->items[lastIndex].type == TYPE_DOUBLE)
    result = impl->items[lastIndex].doubleVal;
  else if (impl->items[lastIndex].type == TYPE_INT)
    result = (double)impl->items[lastIndex].intVal;
  else if (impl->items[lastIndex].type == TYPE_LONG_LONG)
    result = (double)impl->items[lastIndex].longLongVal;

  FreeItemContents(&impl->items[lastIndex]);
  impl->size--;
  return result;
}

template <> String List::pop<String>() {
  if (!impl)
    return String();
  WriteLockGuard guard(&impl->lock);

  if (impl->size == 0)
    return String();

  int lastIndex = impl->size - 1;
  String result;

  if (impl->items[lastIndex].type == TYPE_STRING &&
      impl->items[lastIndex].stringVal) {
    result = *impl->items[lastIndex].stringVal;
  }

  FreeItemContents(&impl->items[lastIndex]);
  impl->size--;
  return result;
}

template <> List List::pop<List>() {
  if (!impl)
    return List();
  WriteLockGuard guard(&impl->lock);

  if (impl->size == 0)
    return List();

  int lastIndex = impl->size - 1;
  List result;

  if (impl->items[lastIndex].type == TYPE_LIST &&
      impl->items[lastIndex].listVal) {
    result = *impl->items[lastIndex].listVal;
  }

  FreeItemContents(&impl->items[lastIndex]);
  impl->size--;
  return result;
}

} // namespace attoboy
