#include "attolist_internal.h"

namespace attoboy {

List &List::sort(bool ascending) {
  if (!impl)
    return *this;
  WriteLockGuard guard(&impl->lock);

  if (impl->size <= 1)
    return *this;

  for (int i = 0; i < impl->size - 1; i++) {
    for (int j = 0; j < impl->size - i - 1; j++) {
      int comparison = CompareItems(&impl->items[j], &impl->items[j + 1]);

      bool shouldSwap = ascending ? (comparison > 0) : (comparison < 0);

      if (shouldSwap) {
        ListItem temp = impl->items[j];
        impl->items[j] = impl->items[j + 1];
        impl->items[j + 1] = temp;
      }
    }
  }

  return *this;
}

} // namespace attoboy
