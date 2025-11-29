#include "attolist_internal.h"
#include "attostring_internal.h"

namespace attoboy {

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
  if (impl->items[index].type == TYPE_FLOAT)
    return (int)impl->items[index].floatVal;
  return 0;
}

template <> float List::at<float>(int index) const {
  if (!impl)
    return 0.0f;
  ReadLockGuard guard(&impl->lock);

  if (impl->size == 0)
    return 0.0f;
  index = ClampIndex(index, impl->size);

  if (impl->items[index].type == TYPE_FLOAT)
    return impl->items[index].floatVal;
  if (impl->items[index].type == TYPE_INT)
    return (float)impl->items[index].intVal;
  return 0.0f;
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

template <> bool List::operator[]<bool>(int index) const {
  return at<bool>(index);
}

template <> int List::operator[]<int>(int index) const {
  return at<int>(index);
}

template <> float List::operator[]<float>(int index) const {
  return at<float>(index);
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

ListValueView::ListValueView() : list(nullptr), index(0) {}

ListValueView::ListValueView(const List *listPtr, int idx)
    : list(listPtr), index(idx) {}

ListValueView::operator bool() const {
  if (!list)
    return false;
  return list->at<bool>(index);
}

ListValueView::operator int() const {
  if (!list)
    return 0;
  return list->at<int>(index);
}

ListValueView::operator float() const {
  if (!list)
    return 0.0f;
  return list->at<float>(index);
}

ListValueView::operator String() const {
  if (!list)
    return String();
  return list->at<String>(index);
}

ListValueView::operator List() const {
  if (!list)
    return List();
  return list->at<List>(index);
}

ListValueView::operator Map() const {
  if (!list)
    return Map();
  return list->at<Map>(index);
}

ListValueView::operator Set() const {
  if (!list)
    return Set();
  return list->at<Set>(index);
}

ListValueView List::at(int index) const { return ListValueView(this, index); }

ListValueView List::operator[](int index) const {
  return ListValueView(this, index);
}

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

void List::set_impl(int index, float value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (impl->size == 0) {
    if (!EnsureCapacity(impl, 1))
      return;
    impl->items[0].type = TYPE_FLOAT;
    impl->items[0].floatVal = value;
    impl->size = 1;
    return;
  }

  index = ClampIndex(index, impl->size);
  FreeItemContents(&impl->items[index]);
  impl->items[index].type = TYPE_FLOAT;
  impl->items[index].floatVal = value;
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
