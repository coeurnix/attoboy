#include "attomap_internal.h"
#include "attostring_internal.h"

namespace attoboy {

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

template <> float Map::get<bool, float>(bool key, float defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<bool>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<float>(index);
}

template <> String Map::get<bool, String>(bool key, String defaultValue) const {
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

template <> float Map::get<int, float>(int key, float defaultValue) const {
  if (!impl)
    return defaultValue;
  ReadLockGuard guard(&impl->lock);

  int index = impl->keys.find<int>(key);
  if (index < 0)
    return defaultValue;

  return impl->values.at<float>(index);
}

template <> String Map::get<int, String>(int key, String defaultValue) const {
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

template <> bool Map::get<float, bool>(float key, bool defaultValue) const {
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

template <> float Map::get<float, float>(float key, float defaultValue) const {
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

template <> bool Map::get<String, bool>(String key, bool defaultValue) const {
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

template <> bool Map::operator[]<bool, bool>(bool key) const {
  return get<bool, bool>(key);
}

template <> int Map::operator[]<bool, int>(bool key) const {
  return get<bool, int>(key);
}

template <> float Map::operator[]<bool, float>(bool key) const {
  return get<bool, float>(key);
}

template <> String Map::operator[]<bool, String>(bool key) const {
  return get<bool, String>(key);
}

template <> List Map::operator[]<bool, List>(bool key) const {
  return get<bool, List>(key);
}

template <> Map Map::operator[]<bool, Map>(bool key) const {
  return get<bool, Map>(key);
}

template <> Set Map::operator[]<bool, Set>(bool key) const {
  return get<bool, Set>(key);
}

template <> bool Map::operator[]<int, bool>(int key) const {
  return get<int, bool>(key);
}

template <> int Map::operator[]<int, int>(int key) const {
  return get<int, int>(key);
}

template <> float Map::operator[]<int, float>(int key) const {
  return get<int, float>(key);
}

template <> String Map::operator[]<int, String>(int key) const {
  return get<int, String>(key);
}

template <> List Map::operator[]<int, List>(int key) const {
  return get<int, List>(key);
}

template <> Map Map::operator[]<int, Map>(int key) const {
  return get<int, Map>(key);
}

template <> Set Map::operator[]<int, Set>(int key) const {
  return get<int, Set>(key);
}

template <> bool Map::operator[]<float, bool>(float key) const {
  return get<float, bool>(key);
}

template <> int Map::operator[]<float, int>(float key) const {
  return get<float, int>(key);
}

template <> float Map::operator[]<float, float>(float key) const {
  return get<float, float>(key);
}

template <> String Map::operator[]<float, String>(float key) const {
  return get<float, String>(key);
}

template <> List Map::operator[]<float, List>(float key) const {
  return get<float, List>(key);
}

template <> Map Map::operator[]<float, Map>(float key) const {
  return get<float, Map>(key);
}

template <> Set Map::operator[]<float, Set>(float key) const {
  return get<float, Set>(key);
}

template <> bool Map::operator[]<String, bool>(String key) const {
  return get<String, bool>(key);
}

template <> int Map::operator[]<String, int>(String key) const {
  return get<String, int>(key);
}

template <> float Map::operator[]<String, float>(String key) const {
  return get<String, float>(key);
}

template <> String Map::operator[]<String, String>(String key) const {
  return get<String, String>(key);
}

template <> List Map::operator[]<String, List>(String key) const {
  return get<String, List>(key);
}

template <> Map Map::operator[]<String, Map>(String key) const {
  return get<String, Map>(key);
}

template <> Set Map::operator[]<String, Set>(String key) const {
  return get<String, Set>(key);
}

template <>
bool Map::get<const char *, bool>(const char *key, bool defaultValue) const {
  return get<String, bool>(String(key), defaultValue);
}

template <>
int Map::get<const char *, int>(const char *key, int defaultValue) const {
  return get<String, int>(String(key), defaultValue);
}

template <>
float Map::get<const char *, float>(const char *key, float defaultValue) const {
  return get<String, float>(String(key), defaultValue);
}

template <>
String Map::get<const char *, String>(const char *key,
                                      String defaultValue) const {
  return get<String, String>(String(key), defaultValue);
}

template <>
List Map::get<const char *, List>(const char *key, List defaultValue) const {
  return get<String, List>(String(key), defaultValue);
}

template <>
Map Map::get<const char *, Map>(const char *key, Map defaultValue) const {
  return get<String, Map>(String(key), defaultValue);
}

template <>
Set Map::get<const char *, Set>(const char *key, Set defaultValue) const {
  return get<String, Set>(String(key), defaultValue);
}

template <> bool Map::operator[]<const char *, bool>(const char *key) const {
  return operator[]<String, bool>(String(key));
}

template <> int Map::operator[]<const char *, int>(const char *key) const {
  return operator[]<String, int>(String(key));
}

template <> float Map::operator[]<const char *, float>(const char *key) const {
  return operator[]<String, float>(String(key));
}

template <>
String Map::operator[]<const char *, String>(const char *key) const {
  return operator[]<String, String>(String(key));
}

template <> List Map::operator[]<const char *, List>(const char *key) const {
  return operator[]<String, List>(String(key));
}

template <> Map Map::operator[]<const char *, Map>(const char *key) const {
  return operator[]<String, Map>(String(key));
}

template <> Set Map::operator[]<const char *, Set>(const char *key) const {
  return operator[]<String, Set>(String(key));
}

template <>
String Map::findValue<String, const char *>(const char *value) const {
  return findValue<String, String>(String(value));
}

bool DefaultValue::asBool() const {
  if (type == TYPE_BOOL)
    return boolVal;
  return false;
}

int DefaultValue::asInt() const {
  if (type == TYPE_INT)
    return intVal;
  return 0;
}

float DefaultValue::asFloat() const {
  if (type == TYPE_FLOAT)
    return floatVal;
  return 0.0f;
}

String DefaultValue::asString() const {
  if (type == TYPE_STRING)
    return stringVal;
  return String();
}

List DefaultValue::asList() const {
  if (type == TYPE_LIST)
    return listVal;
  return List();
}

Map DefaultValue::asMap() const {
  if (type == TYPE_MAP)
    return mapVal;
  return Map();
}

Set DefaultValue::asSet() const {
  if (type == TYPE_SET)
    return setVal;
  return Set();
}

MapValueView::MapValueView()
    : map(nullptr), keyType(KEY_INVALID), boolKey(false), intKey(0),
      floatKey(0.0f), stringKey(), hasDefault(false), defaultValue() {}

MapValueView::MapValueView(const Map *mapPtr, bool key)
    : map(mapPtr), keyType(KEY_BOOL), boolKey(key), intKey(0), floatKey(0.0f),
      stringKey(), hasDefault(false), defaultValue() {}

MapValueView::MapValueView(const Map *mapPtr, int key)
    : map(mapPtr), keyType(KEY_INT), boolKey(false), intKey(key),
      floatKey(0.0f), stringKey(), hasDefault(false), defaultValue() {}

MapValueView::MapValueView(const Map *mapPtr, float key)
    : map(mapPtr), keyType(KEY_FLOAT), boolKey(false), intKey(0), floatKey(key),
      stringKey(), hasDefault(false), defaultValue() {}

MapValueView::MapValueView(const Map *mapPtr, const char *key)
    : map(mapPtr), keyType(KEY_STRING), boolKey(false), intKey(0),
      floatKey(0.0f), stringKey(key), hasDefault(false), defaultValue() {}

MapValueView::MapValueView(const Map *mapPtr, const String &key)
    : map(mapPtr), keyType(KEY_STRING), boolKey(false), intKey(0),
      floatKey(0.0f), stringKey(key), hasDefault(false), defaultValue() {}

MapValueView::MapValueView(const Map *mapPtr, bool key,
                           const DefaultValue &def)
    : map(mapPtr), keyType(KEY_BOOL), boolKey(key), intKey(0), floatKey(0.0f),
      stringKey(), hasDefault(true), defaultValue(def) {}

MapValueView::MapValueView(const Map *mapPtr, int key,
                           const DefaultValue &def)
    : map(mapPtr), keyType(KEY_INT), boolKey(false), intKey(key),
      floatKey(0.0f), stringKey(), hasDefault(true), defaultValue(def) {}

MapValueView::MapValueView(const Map *mapPtr, float key,
                           const DefaultValue &def)
    : map(mapPtr), keyType(KEY_FLOAT), boolKey(false), intKey(0), floatKey(key),
      stringKey(), hasDefault(true), defaultValue(def) {}

MapValueView::MapValueView(const Map *mapPtr, const char *key,
                           const DefaultValue &def)
    : map(mapPtr), keyType(KEY_STRING), boolKey(false), intKey(0),
      floatKey(0.0f), stringKey(key), hasDefault(true), defaultValue(def) {}

MapValueView::MapValueView(const Map *mapPtr, const String &key,
                           const DefaultValue &def)
    : map(mapPtr), keyType(KEY_STRING), boolKey(false), intKey(0),
      floatKey(0.0f), stringKey(key), hasDefault(true), defaultValue(def) {}

MapValueView::operator bool() const {
  if (!map) {
    if (hasDefault)
      return defaultValue.asBool();
    return false;
  }
  switch (keyType) {
  case KEY_BOOL:
    return map->get<bool, bool>(boolKey,
                                hasDefault ? defaultValue.asBool() : false);
  case KEY_INT:
    return map->get<int, bool>(intKey,
                               hasDefault ? defaultValue.asBool() : false);
  case KEY_FLOAT:
    return map->get<float, bool>(floatKey,
                                 hasDefault ? defaultValue.asBool() : false);
  case KEY_STRING:
    return map->get<String, bool>(stringKey,
                                  hasDefault ? defaultValue.asBool() : false);
  default:
    return hasDefault ? defaultValue.asBool() : false;
  }
}

MapValueView::operator int() const {
  if (!map) {
    if (hasDefault)
      return defaultValue.asInt();
    return 0;
  }
  switch (keyType) {
  case KEY_BOOL:
    return map->get<bool, int>(boolKey,
                               hasDefault ? defaultValue.asInt() : 0);
  case KEY_INT:
    return map->get<int, int>(intKey, hasDefault ? defaultValue.asInt() : 0);
  case KEY_FLOAT:
    return map->get<float, int>(floatKey,
                                hasDefault ? defaultValue.asInt() : 0);
  case KEY_STRING:
    return map->get<String, int>(stringKey,
                                 hasDefault ? defaultValue.asInt() : 0);
  default:
    return hasDefault ? defaultValue.asInt() : 0;
  }
}

MapValueView::operator float() const {
  if (!map) {
    if (hasDefault)
      return defaultValue.asFloat();
    return 0.0f;
  }
  switch (keyType) {
  case KEY_BOOL:
    return map->get<bool, float>(boolKey,
                                 hasDefault ? defaultValue.asFloat() : 0.0f);
  case KEY_INT:
    return map->get<int, float>(intKey,
                                hasDefault ? defaultValue.asFloat() : 0.0f);
  case KEY_FLOAT:
    return map->get<float, float>(floatKey,
                                  hasDefault ? defaultValue.asFloat() : 0.0f);
  case KEY_STRING:
    return map->get<String, float>(stringKey,
                                   hasDefault ? defaultValue.asFloat() : 0.0f);
  default:
    return hasDefault ? defaultValue.asFloat() : 0.0f;
  }
}

MapValueView::operator String() const {
  if (!map) {
    if (hasDefault)
      return defaultValue.asString();
    return String();
  }
  switch (keyType) {
  case KEY_BOOL:
    return map->get<bool, String>(boolKey,
                                  hasDefault ? defaultValue.asString()
                                             : String());
  case KEY_INT:
    return map->get<int, String>(intKey,
                                 hasDefault ? defaultValue.asString()
                                            : String());
  case KEY_FLOAT:
    return map->get<float, String>(floatKey,
                                   hasDefault ? defaultValue.asString()
                                              : String());
  case KEY_STRING:
    return map->get<String, String>(stringKey,
                                    hasDefault ? defaultValue.asString()
                                               : String());
  default:
    return hasDefault ? defaultValue.asString() : String();
  }
}

MapValueView::operator List() const {
  if (!map) {
    if (hasDefault)
      return defaultValue.asList();
    return List();
  }
  switch (keyType) {
  case KEY_BOOL:
    return map->get<bool, List>(boolKey,
                                hasDefault ? defaultValue.asList() : List());
  case KEY_INT:
    return map->get<int, List>(intKey,
                               hasDefault ? defaultValue.asList() : List());
  case KEY_FLOAT:
    return map->get<float, List>(floatKey,
                                 hasDefault ? defaultValue.asList() : List());
  case KEY_STRING:
    return map->get<String, List>(stringKey,
                                  hasDefault ? defaultValue.asList() : List());
  default:
    return hasDefault ? defaultValue.asList() : List();
  }
}

MapValueView::operator Map() const {
  if (!map) {
    if (hasDefault)
      return defaultValue.asMap();
    return Map();
  }
  switch (keyType) {
  case KEY_BOOL:
    return map->get<bool, Map>(boolKey,
                               hasDefault ? defaultValue.asMap() : Map());
  case KEY_INT:
    return map->get<int, Map>(intKey,
                              hasDefault ? defaultValue.asMap() : Map());
  case KEY_FLOAT:
    return map->get<float, Map>(floatKey,
                                hasDefault ? defaultValue.asMap() : Map());
  case KEY_STRING:
    return map->get<String, Map>(stringKey,
                                 hasDefault ? defaultValue.asMap() : Map());
  default:
    return hasDefault ? defaultValue.asMap() : Map();
  }
}

MapValueView::operator Set() const {
  if (!map) {
    if (hasDefault)
      return defaultValue.asSet();
    return Set();
  }
  switch (keyType) {
  case KEY_BOOL:
    return map->get<bool, Set>(boolKey,
                               hasDefault ? defaultValue.asSet() : Set());
  case KEY_INT:
    return map->get<int, Set>(intKey,
                              hasDefault ? defaultValue.asSet() : Set());
  case KEY_FLOAT:
    return map->get<float, Set>(floatKey,
                                hasDefault ? defaultValue.asSet() : Set());
  case KEY_STRING:
    return map->get<String, Set>(stringKey,
                                 hasDefault ? defaultValue.asSet() : Set());
  default:
    return hasDefault ? defaultValue.asSet() : Set();
  }
}

MapValueView Map::get(bool key) const { return MapValueView(this, key); }

MapValueView Map::get(int key) const { return MapValueView(this, key); }

MapValueView Map::get(float key) const { return MapValueView(this, key); }

MapValueView Map::get(const char *key) const {
  return MapValueView(this, key);
}

MapValueView Map::get(const String &key) const {
  return MapValueView(this, key);
}

MapValueView Map::get(bool key, const DefaultValue &defaultValue) const {
  return MapValueView(this, key, defaultValue);
}

MapValueView Map::get(int key, const DefaultValue &defaultValue) const {
  return MapValueView(this, key, defaultValue);
}

MapValueView Map::get(float key, const DefaultValue &defaultValue) const {
  return MapValueView(this, key, defaultValue);
}

MapValueView Map::get(const char *key, const DefaultValue &defaultValue) const {
  return MapValueView(this, key, defaultValue);
}

MapValueView Map::get(const String &key, const DefaultValue &defaultValue) const {
  return MapValueView(this, key, defaultValue);
}

MapValueView Map::get(const char *key, const char *defaultValue) const {
  return MapValueView(this, key, DefaultValue(defaultValue));
}

MapValueView Map::operator[](bool key) const {
  return MapValueView(this, key);
}

MapValueView Map::operator[](int key) const {
  return MapValueView(this, key);
}

MapValueView Map::operator[](float key) const {
  return MapValueView(this, key);
}

MapValueView Map::operator[](const char *key) const {
  return MapValueView(this, key);
}

MapValueView Map::operator[](const String &key) const {
  return MapValueView(this, key);
}

} // namespace attoboy
