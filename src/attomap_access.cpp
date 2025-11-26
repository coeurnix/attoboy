#include "attomap_internal.h"
#include "attostring_internal.h"

namespace attoboy {

// Template specializations for get()
template <> bool Map::get<bool, bool>(bool key, bool defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<bool>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<bool>(index);
}

template <> int Map::get<bool, int>(bool key, int defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<bool>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<int>(index);
}

template <>
float Map::get<bool, float>(bool key, float defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<bool>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<float>(index);
}

template <>
String Map::get<bool, String>(bool key, String defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<bool>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<String>(index);
}

template <> List Map::get<bool, List>(bool key, List defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<bool>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<List>(index);
}

template <> Map Map::get<bool, Map>(bool key, Map defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<bool>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<Map>(index);
}

template <> Set Map::get<bool, Set>(bool key, Set defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<bool>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<Set>(index);
}

template <> bool Map::get<int, bool>(int key, bool defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<int>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<bool>(index);
}

template <> int Map::get<int, int>(int key, int defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<int>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<int>(index);
}

template <>
float Map::get<int, float>(int key, float defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<int>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<float>(index);
}

template <>
String Map::get<int, String>(int key, String defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<int>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<String>(index);
}

template <> List Map::get<int, List>(int key, List defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<int>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<List>(index);
}

template <> Map Map::get<int, Map>(int key, Map defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<int>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<Map>(index);
}

template <> Set Map::get<int, Set>(int key, Set defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<int>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<Set>(index);
}

template <>
bool Map::get<float, bool>(float key, bool defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<float>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<bool>(index);
}

template <> int Map::get<float, int>(float key, int defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<float>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<int>(index);
}

template <>
float Map::get<float, float>(float key, float defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<float>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<float>(index);
}

template <>
String Map::get<float, String>(float key, String defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<float>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<String>(index);
}

template <> List Map::get<float, List>(float key, List defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<float>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<List>(index);
}

template <> Map Map::get<float, Map>(float key, Map defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<float>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<Map>(index);
}

template <> Set Map::get<float, Set>(float key, Set defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<float>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<Set>(index);
}

template <>
bool Map::get<String, bool>(String key, bool defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<String>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<bool>(index);
}

template <> int Map::get<String, int>(String key, int defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<String>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<int>(index);
}

template <>
float Map::get<String, float>(String key, float defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<String>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<float>(index);
}

template <>
String Map::get<String, String>(String key, String defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<String>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<String>(index);
}

template <> List Map::get<String, List>(String key, List defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<String>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<List>(index);
}

template <> Map Map::get<String, Map>(String key, Map defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<String>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<Map>(index);
}

template <> Set Map::get<String, Set>(String key, Set defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<String>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<Set>(index);
}

// Template specializations for hasKey()
template <> bool Map::hasKey<bool>(bool key) const {
  if (!impl)
    return false;
  ReadLockGuard guard(&impl->lock);
  return impl->keys.find<bool>(key) >= 0;
}

template <> bool Map::hasKey<int>(int key) const {
  if (!impl)
    return false;
  ReadLockGuard guard(&impl->lock);
  return impl->keys.find<int>(key) >= 0;
}

template <> bool Map::hasKey<float>(float key) const {
  if (!impl)
    return false;
  ReadLockGuard guard(&impl->lock);
  return impl->keys.find<float>(key) >= 0;
}

template <> bool Map::hasKey<String>(String key) const {
  if (!impl)
    return false;
  ReadLockGuard guard(&impl->lock);
  return impl->keys.find<String>(key) >= 0;
}

template <> bool Map::hasKey<const char *>(const char *key) const {
  if (!impl)
    return false;
  ReadLockGuard guard(&impl->lock);
  return impl->keys.find<const char *>(key) >= 0;
}

// Template specializations for typeAt()
template <> ValueType Map::typeAt<bool>(bool key) const {
  if (!impl)
    return TYPE_INVALID;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<bool>(key);
  if (index < 0)
    return TYPE_INVALID;

  return impl->values.typeAt(index);
}

template <> ValueType Map::typeAt<int>(int key) const {
  if (!impl)
    return TYPE_INVALID;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<int>(key);
  if (index < 0)
    return TYPE_INVALID;

  return impl->values.typeAt(index);
}

template <> ValueType Map::typeAt<float>(float key) const {
  if (!impl)
    return TYPE_INVALID;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<float>(key);
  if (index < 0)
    return TYPE_INVALID;

  return impl->values.typeAt(index);
}

template <> ValueType Map::typeAt<String>(String key) const {
  if (!impl)
    return TYPE_INVALID;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<String>(key);
  if (index < 0)
    return TYPE_INVALID;

  return impl->values.typeAt(index);
}

template <> ValueType Map::typeAt<const char *>(const char *key) const {
  if (!impl)
    return TYPE_INVALID;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<const char *>(key);
  if (index < 0)
    return TYPE_INVALID;

  return impl->values.typeAt(index);
}

// Template specializations for findValue()
template <> bool Map::findValue<bool, bool>(bool value) const {
  if (!impl)
    return false;
  ReadLockGuard guard(&impl->lock);

  int index = impl->values.find<bool>(value);
  if (index < 0)
    return false;

  return impl->keys.at<bool>(index);
}

template <> int Map::findValue<int, bool>(bool value) const {
  if (!impl)
    return 0;
  ReadLockGuard guard(&impl->lock);

  int index = impl->values.find<bool>(value);
  if (index < 0)
    return 0;

  return impl->keys.at<int>(index);
}

template <> float Map::findValue<float, bool>(bool value) const {
  if (!impl)
    return 0.0f;
  ReadLockGuard guard(&impl->lock);

  int index = impl->values.find<bool>(value);
  if (index < 0)
    return 0.0f;

  return impl->keys.at<float>(index);
}

template <> String Map::findValue<String, bool>(bool value) const {
  if (!impl)
    return String();
  ReadLockGuard guard(&impl->lock);

  int index = impl->values.find<bool>(value);
  if (index < 0)
    return String();

  return impl->keys.at<String>(index);
}

template <> bool Map::findValue<bool, int>(int value) const {
  if (!impl)
    return false;
  ReadLockGuard guard(&impl->lock);

  int index = impl->values.find<int>(value);
  if (index < 0)
    return false;

  return impl->keys.at<bool>(index);
}

template <> int Map::findValue<int, int>(int value) const {
  if (!impl)
    return 0;
  ReadLockGuard guard(&impl->lock);

  int index = impl->values.find<int>(value);
  if (index < 0)
    return 0;

  return impl->keys.at<int>(index);
}

template <> float Map::findValue<float, int>(int value) const {
  if (!impl)
    return 0.0f;
  ReadLockGuard guard(&impl->lock);

  int index = impl->values.find<int>(value);
  if (index < 0)
    return 0.0f;

  return impl->keys.at<float>(index);
}

template <> String Map::findValue<String, int>(int value) const {
  if (!impl)
    return String();
  ReadLockGuard guard(&impl->lock);

  int index = impl->values.find<int>(value);
  if (index < 0)
    return String();

  return impl->keys.at<String>(index);
}

template <> bool Map::findValue<bool, float>(float value) const {
  if (!impl)
    return false;
  ReadLockGuard guard(&impl->lock);

  int index = impl->values.find<float>(value);
  if (index < 0)
    return false;

  return impl->keys.at<bool>(index);
}

template <> int Map::findValue<int, float>(float value) const {
  if (!impl)
    return 0;
  ReadLockGuard guard(&impl->lock);

  int index = impl->values.find<float>(value);
  if (index < 0)
    return 0;

  return impl->keys.at<int>(index);
}

template <> float Map::findValue<float, float>(float value) const {
  if (!impl)
    return 0.0f;
  ReadLockGuard guard(&impl->lock);

  int index = impl->values.find<float>(value);
  if (index < 0)
    return 0.0f;

  return impl->keys.at<float>(index);
}

template <> String Map::findValue<String, float>(float value) const {
  if (!impl)
    return String();
  ReadLockGuard guard(&impl->lock);

  int index = impl->values.find<float>(value);
  if (index < 0)
    return String();

  return impl->keys.at<String>(index);
}

template <> bool Map::findValue<bool, String>(String value) const {
  if (!impl)
    return false;
  ReadLockGuard guard(&impl->lock);

  int index = impl->values.find<String>(value);
  if (index < 0)
    return false;

  return impl->keys.at<bool>(index);
}

template <> int Map::findValue<int, String>(String value) const {
  if (!impl)
    return 0;
  ReadLockGuard guard(&impl->lock);

  int index = impl->values.find<String>(value);
  if (index < 0)
    return 0;

  return impl->keys.at<int>(index);
}



template <> String Map::findValue<String, String>(String value) const {
  if (!impl)
    return String();
  ReadLockGuard guard(&impl->lock);

  int index = impl->values.find<String>(value);
  if (index < 0)
    return String();

  return impl->keys.at<String>(index);
}

// Remove implementations
void Map::remove_impl(bool key) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<bool>(key);
  if (index >= 0) {
    impl->keys.remove(index);
    impl->values.remove(index);
  }
}

void Map::remove_impl(int key) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<int>(key);
  if (index >= 0) {
    impl->keys.remove(index);
    impl->values.remove(index);
  }
}

void Map::remove_impl(float key) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<float>(key);
  if (index >= 0) {
    impl->keys.remove(index);
    impl->values.remove(index);
  }
}

void Map::remove_impl(const char *key) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<const char *>(key);
  if (index >= 0) {
    impl->keys.remove(index);
    impl->values.remove(index);
  }
}

void Map::remove_impl(const String &key) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<String>(key);
  if (index >= 0) {
    impl->keys.remove(index);
    impl->values.remove(index);
  }
}

bool Map::compare(const Map &other) const {
  if (!impl && !other.impl)
    return true;
  if (!impl || !other.impl)
    return false;

  ReadLockGuard guard1(&impl->lock);
  ReadLockGuard guard2(&other.impl->lock);

  if (impl->keys.length() != other.impl->keys.length())
    return false;

  return impl->keys.compare(other.impl->keys) &&
         impl->values.compare(other.impl->values);
}

bool Map::operator==(const Map &other) const { return compare(other); }

bool Map::operator!=(const Map &other) const { return !compare(other); }

} // namespace attoboy
