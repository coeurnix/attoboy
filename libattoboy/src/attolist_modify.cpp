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

void List::prepend_impl(float value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  for (int i = impl->size; i > 0; i--) {
    impl->items[i] = impl->items[i - 1];
  }

  impl->items[0].type = TYPE_FLOAT;
  impl->items[0].floatVal = value;
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

void List::prepend_impl(const Map &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  for (int i = impl->size; i > 0; i--) {
    impl->items[i] = impl->items[i - 1];
  }

  impl->items[0].type = TYPE_MAP;
  impl->items[0].mapVal = AllocMap(value);
  impl->size++;
}

void List::prepend_impl(const Set &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  for (int i = impl->size; i > 0; i--) {
    impl->items[i] = impl->items[i - 1];
  }

  impl->items[0].type = TYPE_SET;
  impl->items[0].setVal = AllocSet(value);
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

void List::insert_impl(int index, float value) {
  if (!impl)
    return;

  WriteLockGuard guard(&impl->lock);

  if (index < 0)
    index = 0;

  if (index >= impl->size) {
    if (!EnsureCapacity(impl, impl->size + 1))
      return;
    impl->items[impl->size].type = TYPE_FLOAT;
    impl->items[impl->size].floatVal = value;
    impl->size++;
    return;
  }

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  for (int i = impl->size; i > index; i--) {
    impl->items[i] = impl->items[i - 1];
  }

  impl->items[index].type = TYPE_FLOAT;
  impl->items[index].floatVal = value;
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

void List::insert_impl(int index, const Map &value) {
  if (!impl)
    return;

  WriteLockGuard guard(&impl->lock);

  if (index < 0)
    index = 0;

  if (index >= impl->size) {
    if (!EnsureCapacity(impl, impl->size + 1))
      return;
    impl->items[impl->size].type = TYPE_MAP;
    impl->items[impl->size].mapVal = AllocMap(value);
    impl->size++;
    return;
  }

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  for (int i = impl->size; i > index; i--) {
    impl->items[i] = impl->items[i - 1];
  }

  impl->items[index].type = TYPE_MAP;
  impl->items[index].mapVal = AllocMap(value);
  impl->size++;
}

void List::insert_impl(int index, const Set &value) {
  if (!impl)
    return;

  WriteLockGuard guard(&impl->lock);

  if (index < 0)
    index = 0;

  if (index >= impl->size) {
    if (!EnsureCapacity(impl, impl->size + 1))
      return;
    impl->items[impl->size].type = TYPE_SET;
    impl->items[impl->size].setVal = AllocSet(value);
    impl->size++;
    return;
  }

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  for (int i = impl->size; i > index; i--) {
    impl->items[i] = impl->items[i - 1];
  }

  impl->items[index].type = TYPE_SET;
  impl->items[index].setVal = AllocSet(value);
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
  else if (impl->items[lastIndex].type == TYPE_FLOAT)
    result = (int)impl->items[lastIndex].floatVal;

  FreeItemContents(&impl->items[lastIndex]);
  impl->size--;
  return result;
}

template <> float List::pop<float>() {
  if (!impl)
    return 0.0f;
  WriteLockGuard guard(&impl->lock);

  if (impl->size == 0)
    return 0.0f;

  int lastIndex = impl->size - 1;
  float result = 0.0f;

  if (impl->items[lastIndex].type == TYPE_FLOAT)
    result = impl->items[lastIndex].floatVal;
  else if (impl->items[lastIndex].type == TYPE_INT)
    result = (float)impl->items[lastIndex].intVal;

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

template <> Map List::pop<Map>() {
  if (!impl)
    return Map();
  WriteLockGuard guard(&impl->lock);

  if (impl->size == 0)
    return Map();

  int lastIndex = impl->size - 1;
  Map result;

  if (impl->items[lastIndex].type == TYPE_MAP &&
      impl->items[lastIndex].mapVal) {
    result = *(Map *)impl->items[lastIndex].mapVal;
  }

  FreeItemContents(&impl->items[lastIndex]);
  impl->size--;
  return result;
}

template <> Set List::pop<Set>() {
  if (!impl)
    return Set();
  WriteLockGuard guard(&impl->lock);

  if (impl->size == 0)
    return Set();

  int lastIndex = impl->size - 1;
  Set result;

  if (impl->items[lastIndex].type == TYPE_SET &&
      impl->items[lastIndex].setVal) {
    result = *(Set *)impl->items[lastIndex].setVal;
  }

  FreeItemContents(&impl->items[lastIndex]);
  impl->size--;
  return result;
}

} // namespace attoboy
