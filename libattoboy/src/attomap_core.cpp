#include "attomap_internal.h"

namespace attoboy {

Map::Map() {
  impl = (MapImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                              sizeof(MapImpl));
  if (impl) {
    new (impl) MapImpl();
  }
}

Map::Map(int capacity) {
  impl = (MapImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                              sizeof(MapImpl));
  if (impl) {
    new (impl) MapImpl();
    impl->keys = List(capacity);
    impl->values = List(capacity);
  }
}

Map::Map(const Map &other) {
  impl = (MapImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                              sizeof(MapImpl));
  if (!impl)
    return;

  new (impl) MapImpl();

  if (other.impl) {
    ReadLockGuard guard(&other.impl->lock);
    impl->keys = other.impl->keys.duplicate();
    impl->values = other.impl->values.duplicate();
  }
}

Map::~Map() {
  if (impl) {
    impl->~MapImpl();
    HeapFree(GetProcessHeap(), 0, impl);
  }
}

Map &Map::operator=(const Map &other) {
  if (this != &other) {
    Map temp(other);
    MapImpl *oldImpl = impl;
    impl = temp.impl;
    temp.impl = oldImpl;
  }
  return *this;
}

int Map::length() const {
  if (!impl)
    return 0;
  ReadLockGuard guard(&impl->lock);
  return impl->keys.length();
}

bool Map::isEmpty() const {
  if (!impl)
    return true;
  ReadLockGuard guard(&impl->lock);
  return impl->keys.isEmpty();
}

Map &Map::clear() {
  if (!impl)
    return *this;
  WriteLockGuard guard(&impl->lock);
  impl->keys.clear();
  impl->values.clear();
  return *this;
}

Map Map::duplicate() const {
  return Map(*this);
}

List Map::keys() const {
  if (!impl)
    return List();
  ReadLockGuard guard(&impl->lock);
  return impl->keys.duplicate();
}

List Map::values() const {
  if (!impl)
    return List();
  ReadLockGuard guard(&impl->lock);
  return impl->values.duplicate();
}

Map &Map::merge(const Map &other) {
  if (!impl || !other.impl)
    return *this;

  if (this == &other)
    return *this;

  List otherKeys = other.keys();
  List otherValues = other.values();

  int otherLen = otherKeys.length();
  for (int i = 0; i < otherLen; i++) {
    ValueType keyType = otherKeys.typeAt(i);
    ValueType valueType = otherValues.typeAt(i);

    switch (keyType) {
    case TYPE_BOOL: {
      bool key = otherKeys.at<bool>(i);
      switch (valueType) {
      case TYPE_BOOL:
        put_impl(key, otherValues.at<bool>(i));
        break;
      case TYPE_INT:
        put_impl(key, otherValues.at<int>(i));
        break;
      case TYPE_LONG_LONG:
        put_impl(key, otherValues.at<long long>(i));
        break;
      case TYPE_DOUBLE:
        put_impl(key, otherValues.at<double>(i));
        break;
      case TYPE_STRING:
        put_impl(key, otherValues.at<String>(i));
        break;
      case TYPE_LIST:
        put_impl(key, otherValues.at<List>(i));
        break;
      case TYPE_MAP:
        put_impl(key, otherValues.at<Map>(i));
        break;
      case TYPE_SET:
        put_impl(key, otherValues.at<Set>(i));
        break;
      default:
        break;
      }
      break;
    }
    case TYPE_INT: {
      int key = otherKeys.at<int>(i);
      switch (valueType) {
      case TYPE_BOOL:
        put_impl(key, otherValues.at<bool>(i));
        break;
      case TYPE_INT:
        put_impl(key, otherValues.at<int>(i));
        break;
      case TYPE_LONG_LONG:
        put_impl(key, otherValues.at<long long>(i));
        break;
      case TYPE_DOUBLE:
        put_impl(key, otherValues.at<double>(i));
        break;
      case TYPE_STRING:
        put_impl(key, otherValues.at<String>(i));
        break;
      case TYPE_LIST:
        put_impl(key, otherValues.at<List>(i));
        break;
      case TYPE_MAP:
        put_impl(key, otherValues.at<Map>(i));
        break;
      case TYPE_SET:
        put_impl(key, otherValues.at<Set>(i));
        break;
      default:
        break;
      }
      break;
    }
    case TYPE_LONG_LONG: {
      long long key = otherKeys.at<long long>(i);
      switch (valueType) {
      case TYPE_BOOL:
        put_impl(key, otherValues.at<bool>(i));
        break;
      case TYPE_INT:
        put_impl(key, otherValues.at<int>(i));
        break;
      case TYPE_LONG_LONG:
        put_impl(key, otherValues.at<long long>(i));
        break;
      case TYPE_DOUBLE:
        put_impl(key, otherValues.at<double>(i));
        break;
      case TYPE_STRING:
        put_impl(key, otherValues.at<String>(i));
        break;
      case TYPE_LIST:
        put_impl(key, otherValues.at<List>(i));
        break;
      case TYPE_MAP:
        put_impl(key, otherValues.at<Map>(i));
        break;
      case TYPE_SET:
        put_impl(key, otherValues.at<Set>(i));
        break;
      default:
        break;
      }
      break;
    }
    case TYPE_DOUBLE: {
      double key = otherKeys.at<double>(i);
      switch (valueType) {
      case TYPE_BOOL:
        put_impl(key, otherValues.at<bool>(i));
        break;
      case TYPE_INT:
        put_impl(key, otherValues.at<int>(i));
        break;
      case TYPE_LONG_LONG:
        put_impl(key, otherValues.at<long long>(i));
        break;
      case TYPE_DOUBLE:
        put_impl(key, otherValues.at<double>(i));
        break;
      case TYPE_STRING:
        put_impl(key, otherValues.at<String>(i));
        break;
      case TYPE_LIST:
        put_impl(key, otherValues.at<List>(i));
        break;
      case TYPE_MAP:
        put_impl(key, otherValues.at<Map>(i));
        break;
      case TYPE_SET:
        put_impl(key, otherValues.at<Set>(i));
        break;
      default:
        break;
      }
      break;
    }
    case TYPE_STRING: {
      String key = otherKeys.at<String>(i);
      switch (valueType) {
      case TYPE_BOOL:
        put_impl(key, otherValues.at<bool>(i));
        break;
      case TYPE_INT:
        put_impl(key, otherValues.at<int>(i));
        break;
      case TYPE_LONG_LONG:
        put_impl(key, otherValues.at<long long>(i));
        break;
      case TYPE_DOUBLE:
        put_impl(key, otherValues.at<double>(i));
        break;
      case TYPE_STRING:
        put_impl(key, otherValues.at<String>(i));
        break;
      case TYPE_LIST:
        put_impl(key, otherValues.at<List>(i));
        break;
      case TYPE_MAP:
        put_impl(key, otherValues.at<Map>(i));
        break;
      case TYPE_SET:
        put_impl(key, otherValues.at<Set>(i));
        break;
      default:
        break;
      }
      break;
    }
    default:
      break;
    }
  }

  return *this;
}

} // namespace attoboy
