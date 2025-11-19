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

void List::append_impl(long long value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  impl->items[impl->size].type = TYPE_LONG_LONG;
  impl->items[impl->size].longLongVal = value;
  impl->size++;
}

void List::append_impl(double value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  impl->items[impl->size].type = TYPE_DOUBLE;
  impl->items[impl->size].doubleVal = value;
  impl->size++;
}

void List::append_impl(const char *value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (!EnsureCapacity(impl, impl->size + 1))
    return;

  impl->items[impl->size].type = TYPE_STRING;
  impl->items[impl->size].stringVal = AllocString(value);
  impl->size++;
}

void List::append_impl(const wchar_t *value) {
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

} // namespace attoboy
