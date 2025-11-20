#include "attomap_internal.h"

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
long long Map::get<bool, long long>(bool key, long long defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<bool>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<long long>(index);
}

template <>
double Map::get<bool, double>(bool key, double defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<bool>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<double>(index);
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
long long Map::get<int, long long>(int key, long long defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<int>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<long long>(index);
}

template <> double Map::get<int, double>(int key, double defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<int>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<double>(index);
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
bool Map::get<long long, bool>(long long key, bool defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<long long>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<bool>(index);
}

template <>
int Map::get<long long, int>(long long key, int defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<long long>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<int>(index);
}

template <>
long long Map::get<long long, long long>(long long key,
                                         long long defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<long long>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<long long>(index);
}

template <>
double Map::get<long long, double>(long long key, double defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<long long>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<double>(index);
}

template <>
String Map::get<long long, String>(long long key, String defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<long long>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<String>(index);
}

template <>
List Map::get<long long, List>(long long key, List defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<long long>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<List>(index);
}

template <>
Map Map::get<long long, Map>(long long key, Map defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<long long>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<Map>(index);
}

template <>
Set Map::get<long long, Set>(long long key, Set defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<long long>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<Set>(index);
}

template <>
bool Map::get<double, bool>(double key, bool defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<double>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<bool>(index);
}

template <> int Map::get<double, int>(double key, int defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<double>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<int>(index);
}

template <>
long long Map::get<double, long long>(double key, long long defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<double>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<long long>(index);
}

template <>
double Map::get<double, double>(double key, double defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<double>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<double>(index);
}

template <>
String Map::get<double, String>(double key, String defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<double>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<String>(index);
}

template <> List Map::get<double, List>(double key, List defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<double>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<List>(index);
}

template <> Map Map::get<double, Map>(double key, Map defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<double>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<Map>(index);
}

template <> Set Map::get<double, Set>(double key, Set defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<double>(key);
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
long long Map::get<String, long long>(String key, long long defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<String>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<long long>(index);
}

template <>
double Map::get<String, double>(String key, double defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<String>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<double>(index);
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

template <> bool Map::hasKey<long long>(long long key) const {
  if (!impl)
    return false;
  ReadLockGuard guard(&impl->lock);
  return impl->keys.find<long long>(key) >= 0;
}

template <> bool Map::hasKey<double>(double key) const {
  if (!impl)
    return false;
  ReadLockGuard guard(&impl->lock);
  return impl->keys.find<double>(key) >= 0;
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

template <> bool Map::hasKey<const wchar_t *>(const wchar_t *key) const {
  if (!impl)
    return false;
  ReadLockGuard guard(&impl->lock);
  return impl->keys.find<const wchar_t *>(key) >= 0;
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

template <> ValueType Map::typeAt<long long>(long long key) const {
  if (!impl)
    return TYPE_INVALID;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<long long>(key);
  if (index < 0)
    return TYPE_INVALID;

  return impl->values.typeAt(index);
}

template <> ValueType Map::typeAt<double>(double key) const {
  if (!impl)
    return TYPE_INVALID;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<double>(key);
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

template <> ValueType Map::typeAt<const wchar_t *>(const wchar_t *key) const {
  if (!impl)
    return TYPE_INVALID;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<const wchar_t *>(key);
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

template <> long long Map::findValue<long long, bool>(bool value) const {
  if (!impl)
    return 0LL;
  ReadLockGuard guard(&impl->lock);

  int index = impl->values.find<bool>(value);
  if (index < 0)
    return 0LL;

  return impl->keys.at<long long>(index);
}

template <> double Map::findValue<double, bool>(bool value) const {
  if (!impl)
    return 0.0;
  ReadLockGuard guard(&impl->lock);

  int index = impl->values.find<bool>(value);
  if (index < 0)
    return 0.0;

  return impl->keys.at<double>(index);
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

template <> long long Map::findValue<long long, int>(int value) const {
  if (!impl)
    return 0LL;
  ReadLockGuard guard(&impl->lock);

  int index = impl->values.find<int>(value);
  if (index < 0)
    return 0LL;

  return impl->keys.at<long long>(index);
}

template <> double Map::findValue<double, int>(int value) const {
  if (!impl)
    return 0.0;
  ReadLockGuard guard(&impl->lock);

  int index = impl->values.find<int>(value);
  if (index < 0)
    return 0.0;

  return impl->keys.at<double>(index);
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

template <> bool Map::findValue<bool, long long>(long long value) const {
  if (!impl)
    return false;
  ReadLockGuard guard(&impl->lock);

  int index = impl->values.find<long long>(value);
  if (index < 0)
    return false;

  return impl->keys.at<bool>(index);
}

template <> int Map::findValue<int, long long>(long long value) const {
  if (!impl)
    return 0;
  ReadLockGuard guard(&impl->lock);

  int index = impl->values.find<long long>(value);
  if (index < 0)
    return 0;

  return impl->keys.at<int>(index);
}

template <>
long long Map::findValue<long long, long long>(long long value) const {
  if (!impl)
    return 0LL;
  ReadLockGuard guard(&impl->lock);

  int index = impl->values.find<long long>(value);
  if (index < 0)
    return 0LL;

  return impl->keys.at<long long>(index);
}

template <> double Map::findValue<double, long long>(long long value) const {
  if (!impl)
    return 0.0;
  ReadLockGuard guard(&impl->lock);

  int index = impl->values.find<long long>(value);
  if (index < 0)
    return 0.0;

  return impl->keys.at<double>(index);
}

template <> String Map::findValue<String, long long>(long long value) const {
  if (!impl)
    return String();
  ReadLockGuard guard(&impl->lock);

  int index = impl->values.find<long long>(value);
  if (index < 0)
    return String();

  return impl->keys.at<String>(index);
}

template <> bool Map::findValue<bool, double>(double value) const {
  if (!impl)
    return false;
  ReadLockGuard guard(&impl->lock);

  int index = impl->values.find<double>(value);
  if (index < 0)
    return false;

  return impl->keys.at<bool>(index);
}

template <> int Map::findValue<int, double>(double value) const {
  if (!impl)
    return 0;
  ReadLockGuard guard(&impl->lock);

  int index = impl->values.find<double>(value);
  if (index < 0)
    return 0;

  return impl->keys.at<int>(index);
}

template <> long long Map::findValue<long long, double>(double value) const {
  if (!impl)
    return 0LL;
  ReadLockGuard guard(&impl->lock);

  int index = impl->values.find<double>(value);
  if (index < 0)
    return 0LL;

  return impl->keys.at<long long>(index);
}

template <> double Map::findValue<double, double>(double value) const {
  if (!impl)
    return 0.0;
  ReadLockGuard guard(&impl->lock);

  int index = impl->values.find<double>(value);
  if (index < 0)
    return 0.0;

  return impl->keys.at<double>(index);
}

template <> String Map::findValue<String, double>(double value) const {
  if (!impl)
    return String();
  ReadLockGuard guard(&impl->lock);

  int index = impl->values.find<double>(value);
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

template <> long long Map::findValue<long long, String>(String value) const {
  if (!impl)
    return 0LL;
  ReadLockGuard guard(&impl->lock);

  int index = impl->values.find<String>(value);
  if (index < 0)
    return 0LL;

  return impl->keys.at<long long>(index);
}

template <> double Map::findValue<double, String>(String value) const {
  if (!impl)
    return 0.0;
  ReadLockGuard guard(&impl->lock);

  int index = impl->values.find<String>(value);
  if (index < 0)
    return 0.0;

  return impl->keys.at<double>(index);
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

void Map::remove_impl(long long key) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<long long>(key);
  if (index >= 0) {
    impl->keys.remove(index);
    impl->values.remove(index);
  }
}

void Map::remove_impl(double key) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<double>(key);
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

void Map::remove_impl(const wchar_t *key) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<const wchar_t *>(key);
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
