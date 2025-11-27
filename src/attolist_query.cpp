#include "attolist_internal.h"
#include "attostring_internal.h"

namespace attoboy {

template <> int List::find<bool>(bool value) const {
  if (!impl)
    return -1;
  ReadLockGuard guard(&impl->lock);

  ListItem searchItem;
  searchItem.type = TYPE_BOOL;
  searchItem.boolVal = value;

  for (int i = 0; i < impl->size; i++) {
    if (ItemsEqual(&impl->items[i], &searchItem))
      return i;
  }
  return -1;
}

template <> int List::find<int>(int value) const {
  if (!impl)
    return -1;
  ReadLockGuard guard(&impl->lock);

  ListItem searchItem;
  searchItem.type = TYPE_INT;
  searchItem.intVal = value;

  for (int i = 0; i < impl->size; i++) {
    if (ItemsEqual(&impl->items[i], &searchItem))
      return i;
  }
  return -1;
}

template <> int List::find<float>(float value) const {
  if (!impl)
    return -1;
  ReadLockGuard guard(&impl->lock);

  ListItem searchItem;
  searchItem.type = TYPE_FLOAT;
  searchItem.floatVal = value;

  for (int i = 0; i < impl->size; i++) {
    if (ItemsEqual(&impl->items[i], &searchItem))
      return i;
  }
  return -1;
}

template <> int List::find<String>(String value) const {
  if (!impl)
    return -1;
  ReadLockGuard guard(&impl->lock);

  ListItem searchItem;
  searchItem.type = TYPE_STRING;
  searchItem.stringVal = &value;

  for (int i = 0; i < impl->size; i++) {
    if (ItemsEqual(&impl->items[i], &searchItem))
      return i;
  }
  return -1;
}

template <> int List::find<const char *>(const char *value) const {
  String str(value);
  return find<String>(str);
}

template <> bool List::contains<bool>(bool value) const {
  return find<bool>(value) != -1;
}

template <> bool List::contains<int>(int value) const {
  return find<int>(value) != -1;
}

template <> bool List::contains<float>(float value) const {
  return find<float>(value) != -1;
}

template <> bool List::contains<String>(String value) const {
  return find<String>(value) != -1;
}

template <> bool List::contains<const char *>(const char *value) const {
  String str(value);
  return find<String>(str) != -1;
}

bool List::compare(const List &other) const {
  if (!impl && !other.impl)
    return true;
  if (!impl || !other.impl)
    return false;

  ReadLockGuard guard1(&impl->lock);
  ReadLockGuard guard2(&other.impl->lock);

  if (impl->size != other.impl->size)
    return false;

  for (int i = 0; i < impl->size; i++) {
    if (!ItemsEqual(&impl->items[i], &other.impl->items[i]))
      return false;
  }

  return true;
}

bool List::operator==(const List &other) const { return compare(other); }

bool List::operator!=(const List &other) const { return !compare(other); }

} // namespace attoboy
