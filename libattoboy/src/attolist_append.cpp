#include "attolist_internal.h"

namespace attoboy {

void List::append_impl(bool value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  impl->items[impl->size].type = TYPE_BOOL;
  impl->items[impl->size].boolVal = value;
  impl->size++;
}

void List::append_impl(int value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  impl->items[impl->size].type = TYPE_INT;
  impl->items[impl->size].intVal = value;
  impl->size++;
}

void List::append_impl(float value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  impl->items[impl->size].type = TYPE_FLOAT;
  impl->items[impl->size].floatVal = value;
  impl->size++;
}

void List::append_impl(const ATTO_CHAR *value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  impl->items[impl->size].type = TYPE_STRING;
  impl->items[impl->size].stringVal = AllocString(value);
  impl->size++;
}

void List::append_impl(const String &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  impl->items[impl->size].type = TYPE_STRING;
  impl->items[impl->size].stringVal = AllocString(value);
  impl->size++;
}

void List::append_impl(const List &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  impl->items[impl->size].type = TYPE_LIST;
  impl->items[impl->size].listVal = AllocList(value);
  impl->size++;
}

void List::append_impl(const Map &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  impl->items[impl->size].type = TYPE_MAP;
  impl->items[impl->size].mapVal = AllocMap(value);
  impl->size++;
}

void List::append_impl(const Set &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  impl->items[impl->size].type = TYPE_SET;
  impl->items[impl->size].setVal = AllocSet(value);
  impl->size++;
}

} // namespace attoboy
