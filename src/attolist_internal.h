#pragma once
#include "atto_internal_common.h"
#include "attoboy/attoboy.h"
#include <new>
#include <windows.h>

namespace attoboy {

struct ListImpl;
class Map;
class Set;

struct ListItem {
  ValueType type;
  union {
    bool boolVal;
    int intVal;
    float floatVal;
    String *stringVal;
    List *listVal;
    void *mapVal;
    void *setVal;
  };

  ListItem() {
    type = TYPE_NULL;
    floatVal = 0.0f;
  }
};

struct ListImpl {
  ListItem *items;
  int size;
  int capacity;
  mutable SRWLOCK lock;
};

static inline ListItem *AllocItems(int capacity) {
  if (capacity <= 0)
    return nullptr;
  return (ListItem *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                               capacity * sizeof(ListItem));
}

static inline void FreeItems(ListItem *items) {
  if (items)
    HeapFree(GetProcessHeap(), 0, items);
}

static inline String *AllocString() {
  void *mem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(String));
  if (!mem)
    return nullptr;
  return new (mem) String();
}

static inline String *AllocString(const String &other) {
  void *mem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(String));
  if (!mem)
    return nullptr;
  return new (mem) String(other);
}

static inline String *AllocString(const char *str) {
  void *mem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(String));
  if (!mem)
    return nullptr;
  return new (mem) String(str);
}

static inline String *AllocString(const wchar_t *str) {
  void *mem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(String));
  if (!mem)
    return nullptr;
  return new (mem) String(str);
}

static inline void FreeString(String *str) {
  if (!str)
    return;
  str->~String();
  HeapFree(GetProcessHeap(), 0, str);
}

static inline List *AllocList() {
  void *mem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(List));
  if (!mem)
    return nullptr;
  return new (mem) List();
}

static inline List *AllocList(const List &other) {
  void *mem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(List));
  if (!mem)
    return nullptr;
  return new (mem) List(other);
}

static inline void FreeList(List *list) {
  if (!list)
    return;
  list->~List();
  HeapFree(GetProcessHeap(), 0, list);
}

Map *AllocMap();
Map *AllocMap(const Map &other);
void FreeMap(Map *map);

Set *AllocSet();
Set *AllocSet(const Set &other);
void FreeSet(Set *set);

static inline void FreeItemContents(ListItem *item) {
  if (!item)
    return;
  if (item->type == TYPE_STRING && item->stringVal) {
    FreeString(item->stringVal);
    item->stringVal = nullptr;
  } else if (item->type == TYPE_LIST && item->listVal) {
    FreeList(item->listVal);
    item->listVal = nullptr;
  } else if (item->type == TYPE_MAP && item->mapVal) {
    FreeMap((Map *)item->mapVal);
    item->mapVal = nullptr;
  } else if (item->type == TYPE_SET && item->setVal) {
    FreeSet((Set *)item->setVal);
    item->setVal = nullptr;
  }
  item->type = TYPE_NULL;
}

static inline bool EnsureCapacity(ListImpl *impl, int requiredSize) {
  if (!impl || requiredSize <= impl->capacity)
    return true;

  int newCapacity = impl->capacity;
  if (newCapacity == 0)
    newCapacity = 8;

  while (newCapacity < requiredSize)
    newCapacity *= 2;

  ListItem *newItems = AllocItems(newCapacity);
  if (!newItems)
    return false;

  for (int i = 0; i < impl->size; i++) {
    newItems[i] = impl->items[i];
  }

  FreeItems(impl->items);
  impl->items = newItems;
  impl->capacity = newCapacity;
  return true;
}

static inline bool ItemsEqual(const ListItem *a, const ListItem *b) {
  if (!a || !b)
    return false;
  bool aIsNumeric = (a->type == TYPE_INT || a->type == TYPE_FLOAT);
  bool bIsNumeric = (b->type == TYPE_INT || b->type == TYPE_FLOAT);

  if (aIsNumeric && bIsNumeric) {
    float aVal = 0.0f, bVal = 0.0f;
    if (a->type == TYPE_INT)
      aVal = (float)a->intVal;
    else
      aVal = a->floatVal;

    if (b->type == TYPE_INT)
      bVal = (float)b->intVal;
    else
      bVal = b->floatVal;

    return aVal == bVal;
  }

  if (a->type != b->type)
    return false;

  switch (a->type) {
  case TYPE_BOOL:
    return a->boolVal == b->boolVal;
  case TYPE_STRING:
    if (a->stringVal && b->stringVal)
      return a->stringVal->equals(*b->stringVal);
    return a->stringVal == b->stringVal;
  case TYPE_NULL:
    return true;
  default:
    return false;
  }
}

static inline float ItemToFloat(const ListItem *item) {
  if (!item)
    return 0.0f;
  switch (item->type) {
  case TYPE_BOOL:
    return item->boolVal ? 1.0f : 0.0f;
  case TYPE_INT:
    return (float)item->intVal;
  case TYPE_FLOAT:
    return item->floatVal;
  case TYPE_STRING:
    if (item->stringVal)
      return item->stringVal->toFloat();
    return 0.0f;
  default:
    return 0.0f;
  }
}

static inline int CompareItems(const ListItem *a, const ListItem *b) {
  if (!a || !b)
    return 0;
  bool aIsNumeric =
      (a->type == TYPE_INT || a->type == TYPE_FLOAT || a->type == TYPE_BOOL);
  bool bIsNumeric =
      (b->type == TYPE_INT || b->type == TYPE_FLOAT || b->type == TYPE_BOOL);

  if (aIsNumeric && bIsNumeric) {
    float aVal = ItemToFloat(a);
    float bVal = ItemToFloat(b);
    if (aVal < bVal)
      return -1;
    if (aVal > bVal)
      return 1;
    return 0;
  }

  if (a->type == TYPE_STRING && b->type == TYPE_STRING) {
    if (a->stringVal && b->stringVal)
      return a->stringVal->compare(*b->stringVal);
    return 0;
  }

  if (a->type == TYPE_STRING || b->type == TYPE_STRING) {
    String aStr =
        (a->type == TYPE_STRING && a->stringVal) ? *a->stringVal : String();
    String bStr =
        (b->type == TYPE_STRING && b->stringVal) ? *b->stringVal : String();
    return aStr.compare(bStr);
  }

  return (int)a->type - (int)b->type;
}

} // namespace attoboy
