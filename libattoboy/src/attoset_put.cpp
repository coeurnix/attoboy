#include "attoset_internal.h"
#include "attostring_internal.h"

namespace attoboy {

void Set::put_impl(bool value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (!impl->values.contains<bool>(value)) {
    impl->values.append<bool>(value);
  }
}

void Set::put_impl(int value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (!impl->values.contains<int>(value)) {
    impl->values.append<int>(value);
  }
}

void Set::put_impl(float value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (!impl->values.contains<float>(value)) {
    impl->values.append<float>(value);
  }
}

void Set::put_impl(const char *value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (!impl->values.contains<const char *>(value)) {
    impl->values.append(value);
  }
}

void Set::put_impl(const String &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  if (!impl->values.contains<String>(value)) {
    impl->values.append(value);
  }
}

void Set::put_impl(const List &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  impl->values.append(value);
}

void Set::put_impl(const Map &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  impl->values.append(value);
}

void Set::put_impl(const Set &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  impl->values.append(value);
}

} // namespace attoboy
