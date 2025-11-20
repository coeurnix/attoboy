#pragma once
#include "attoboy/attoboy.h"
#include "atto_internal_common.h"
#include <new> // For placement new
#include <windows.h>


namespace attoboy {

// Forward declaration for internal use
struct ListImpl;
class Map;
class Set;

struct ListItem {
  ValueType type;
  union {
    bool boolVal;
    int intVal;
    long long longLongVal;
    double doubleVal;
    String *stringVal;
    List *listVal;
    void *mapVal;
    void *setVal;
  };

  ListItem() {
    type = TYPE_NULL;
    // Zero all 8 bytes of union
    doubleVal = 0.0;
  }
};

struct ListImpl {
  ListItem *items;
  int size;
  int capacity;
  SRWLOCK lock;
};

// Inline helper functions for list item allocation
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

// Helper functions for String allocation/deallocation without new/delete
static inline String *AllocString() {
  void *mem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(String));
  if (!mem)
    return nullptr;
  return new (mem) String(); // Placement new
}

static inline String *AllocString(const String &other) {
  void *mem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(String));
  if (!mem)
    return nullptr;
  return new (mem) String(other); // Placement new with copy constructor
}

static inline String *AllocString(const char *str) {
  void *mem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(String));
  if (!mem)
    return nullptr;
  return new (mem) String(str); // Placement new
}

static inline String *AllocString(const wchar_t *str) {
  void *mem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(String));
  if (!mem)
    return nullptr;
  return new (mem) String(str); // Placement new
}

static inline void FreeString(String *str) {
  if (!str)
    return;
  str->~String(); // Explicitly call destructor
  HeapFree(GetProcessHeap(), 0, str);
}

// Helper functions for List allocation/deallocation without new/delete
static inline List *AllocList() {
  void *mem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(List));
  if (!mem)
    return nullptr;
  return new (mem) List(); // Placement new
}

static inline List *AllocList(const List &other) {
  void *mem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(List));
  if (!mem)
    return nullptr;
  return new (mem) List(other); // Placement new with copy constructor
}

static inline void FreeList(List *list) {
  if (!list)
    return;
  list->~List(); // Explicitly call destructor
  HeapFree(GetProcessHeap(), 0, list);
}

// Helper functions for Map allocation/deallocation without new/delete
// Implemented in attolist_mapset.cpp as weak symbols to allow linker elimination
// when Map/Set are not used
__attribute__((weak)) Map *AllocMap();
__attribute__((weak)) Map *AllocMap(const Map &other);
__attribute__((weak)) void FreeMap(Map *map);

// Helper functions for Set allocation/deallocation without new/delete
// Implemented in attolist_mapset.cpp as weak symbols to allow linker elimination
// when Map/Set are not used
__attribute__((weak)) Set *AllocSet();
__attribute__((weak)) Set *AllocSet(const Set &other);
__attribute__((weak)) void FreeSet(Set *set);

// Helper function to free a single item's contents
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

// Helper function to ensure capacity
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

  // Copy existing items
  for (int i = 0; i < impl->size; i++) {
    newItems[i] = impl->items[i];
  }

  FreeItems(impl->items);
  impl->items = newItems;
  impl->capacity = newCapacity;
  return true;
}

// Helper function to compare two ListItems with numeric coercion
static inline bool ItemsEqual(const ListItem *a, const ListItem *b) {
  if (!a || !b)
    return false;

  // Handle numeric coercion: int, long long, and double can be equal
  bool aIsNumeric = (a->type == TYPE_INT || a->type == TYPE_LONG_LONG ||
                     a->type == TYPE_DOUBLE);
  bool bIsNumeric = (b->type == TYPE_INT || b->type == TYPE_LONG_LONG ||
                     b->type == TYPE_DOUBLE);

  if (aIsNumeric && bIsNumeric) {
    // Convert both to double for comparison
    double aVal = 0.0, bVal = 0.0;
    if (a->type == TYPE_INT)
      aVal = (double)a->intVal;
    else if (a->type == TYPE_LONG_LONG)
      aVal = (double)a->longLongVal;
    else
      aVal = a->doubleVal;

    if (b->type == TYPE_INT)
      bVal = (double)b->intVal;
    else if (b->type == TYPE_LONG_LONG)
      bVal = (double)b->longLongVal;
    else
      bVal = b->doubleVal;

    return aVal == bVal;
  }

  // For non-numeric types, types must match exactly
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

// Helper function to convert item to double for sorting
static inline double ItemToDouble(const ListItem *item) {
  if (!item)
    return 0.0;
  switch (item->type) {
  case TYPE_BOOL:
    return item->boolVal ? 1.0 : 0.0;
  case TYPE_INT:
    return (double)item->intVal;
  case TYPE_LONG_LONG:
    return (double)item->longLongVal;
  case TYPE_DOUBLE:
    return item->doubleVal;
  case TYPE_STRING:
    if (item->stringVal)
      return item->stringVal->toDouble();
    return 0.0;
  default:
    return 0.0;
  }
}

// Helper function to compare two items for sorting
static inline int CompareItems(const ListItem *a, const ListItem *b) {
  if (!a || !b)
    return 0;

  // Try numeric comparison first
  bool aIsNumeric = (a->type == TYPE_INT || a->type == TYPE_LONG_LONG ||
                     a->type == TYPE_DOUBLE || a->type == TYPE_BOOL);
  bool bIsNumeric = (b->type == TYPE_INT || b->type == TYPE_LONG_LONG ||
                     b->type == TYPE_DOUBLE || b->type == TYPE_BOOL);

  if (aIsNumeric && bIsNumeric) {
    double aVal = ItemToDouble(a);
    double bVal = ItemToDouble(b);
    if (aVal < bVal)
      return -1;
    if (aVal > bVal)
      return 1;
    return 0;
  }

  // If both are strings, compare as strings
  if (a->type == TYPE_STRING && b->type == TYPE_STRING) {
    if (a->stringVal && b->stringVal)
      return a->stringVal->compare(*b->stringVal);
    return 0;
  }

  // Try converting to string for comparison
  if (a->type == TYPE_STRING || b->type == TYPE_STRING) {
    String aStr =
        (a->type == TYPE_STRING && a->stringVal) ? *a->stringVal : String();
    String bStr =
        (b->type == TYPE_STRING && b->stringVal) ? *b->stringVal : String();
    return aStr.compare(bStr);
  }

  // Otherwise, order by type
  return (int)a->type - (int)b->type;
}

} // namespace attoboy
