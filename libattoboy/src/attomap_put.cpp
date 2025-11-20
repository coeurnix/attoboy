#include "attomap_internal.h"

namespace attoboy {

void Map::put_impl(bool key, bool value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<bool>(key);
  if (index >= 0) {
    impl->values.set<bool>(index, value);
  } else {
    impl->keys.append<bool>(key);
    impl->values.append<bool>(value);
  }
}

void Map::put_impl(bool key, int value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<bool>(key);
  if (index >= 0) {
    impl->values.set<int>(index, value);
  } else {
    impl->keys.append<bool>(key);
    impl->values.append<int>(value);
  }
}

void Map::put_impl(bool key, long long value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<bool>(key);
  if (index >= 0) {
    impl->values.set<long long>(index, value);
  } else {
    impl->keys.append<bool>(key);
    impl->values.append<long long>(value);
  }
}

void Map::put_impl(bool key, double value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<bool>(key);
  if (index >= 0) {
    impl->values.set<double>(index, value);
  } else {
    impl->keys.append<bool>(key);
    impl->values.append<double>(value);
  }
}

void Map::put_impl(bool key, const char *value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<bool>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append<bool>(key);
    impl->values.append(value);
  }
}

void Map::put_impl(bool key, const wchar_t *value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<bool>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append<bool>(key);
    impl->values.append(value);
  }
}

void Map::put_impl(bool key, const String &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<bool>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append<bool>(key);
    impl->values.append(value);
  }
}

void Map::put_impl(bool key, const List &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<bool>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append<bool>(key);
    impl->values.append(value);
  }
}

void Map::put_impl(bool key, const Map &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<bool>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append<bool>(key);
    impl->values.append(value);
  }
}

void Map::put_impl(bool key, const Set &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<bool>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append<bool>(key);
    impl->values.append(value);
  }
}

void Map::put_impl(int key, bool value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<int>(key);
  if (index >= 0) {
    impl->values.set<bool>(index, value);
  } else {
    impl->keys.append<int>(key);
    impl->values.append<bool>(value);
  }
}

void Map::put_impl(int key, int value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<int>(key);
  if (index >= 0) {
    impl->values.set<int>(index, value);
  } else {
    impl->keys.append<int>(key);
    impl->values.append<int>(value);
  }
}

void Map::put_impl(int key, long long value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<int>(key);
  if (index >= 0) {
    impl->values.set<long long>(index, value);
  } else {
    impl->keys.append<int>(key);
    impl->values.append<long long>(value);
  }
}

void Map::put_impl(int key, double value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<int>(key);
  if (index >= 0) {
    impl->values.set<double>(index, value);
  } else {
    impl->keys.append<int>(key);
    impl->values.append<double>(value);
  }
}

void Map::put_impl(int key, const char *value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<int>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append<int>(key);
    impl->values.append(value);
  }
}

void Map::put_impl(int key, const wchar_t *value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<int>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append<int>(key);
    impl->values.append(value);
  }
}

void Map::put_impl(int key, const String &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<int>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append<int>(key);
    impl->values.append(value);
  }
}

void Map::put_impl(int key, const List &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<int>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append<int>(key);
    impl->values.append(value);
  }
}

void Map::put_impl(int key, const Map &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<int>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append<int>(key);
    impl->values.append(value);
  }
}

void Map::put_impl(int key, const Set &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<int>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append<int>(key);
    impl->values.append(value);
  }
}

void Map::put_impl(long long key, bool value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<long long>(key);
  if (index >= 0) {
    impl->values.set<bool>(index, value);
  } else {
    impl->keys.append<long long>(key);
    impl->values.append<bool>(value);
  }
}

void Map::put_impl(long long key, int value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<long long>(key);
  if (index >= 0) {
    impl->values.set<int>(index, value);
  } else {
    impl->keys.append<long long>(key);
    impl->values.append<int>(value);
  }
}

void Map::put_impl(long long key, long long value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<long long>(key);
  if (index >= 0) {
    impl->values.set<long long>(index, value);
  } else {
    impl->keys.append<long long>(key);
    impl->values.append<long long>(value);
  }
}

void Map::put_impl(long long key, double value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<long long>(key);
  if (index >= 0) {
    impl->values.set<double>(index, value);
  } else {
    impl->keys.append<long long>(key);
    impl->values.append<double>(value);
  }
}

void Map::put_impl(long long key, const char *value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<long long>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append<long long>(key);
    impl->values.append(value);
  }
}

void Map::put_impl(long long key, const wchar_t *value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<long long>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append<long long>(key);
    impl->values.append(value);
  }
}

void Map::put_impl(long long key, const String &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<long long>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append<long long>(key);
    impl->values.append(value);
  }
}

void Map::put_impl(long long key, const List &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<long long>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append<long long>(key);
    impl->values.append(value);
  }
}

void Map::put_impl(long long key, const Map &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<long long>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append<long long>(key);
    impl->values.append(value);
  }
}

void Map::put_impl(long long key, const Set &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<long long>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append<long long>(key);
    impl->values.append(value);
  }
}

void Map::put_impl(double key, bool value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<double>(key);
  if (index >= 0) {
    impl->values.set<bool>(index, value);
  } else {
    impl->keys.append<double>(key);
    impl->values.append<bool>(value);
  }
}

void Map::put_impl(double key, int value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<double>(key);
  if (index >= 0) {
    impl->values.set<int>(index, value);
  } else {
    impl->keys.append<double>(key);
    impl->values.append<int>(value);
  }
}

void Map::put_impl(double key, long long value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<double>(key);
  if (index >= 0) {
    impl->values.set<long long>(index, value);
  } else {
    impl->keys.append<double>(key);
    impl->values.append<long long>(value);
  }
}

void Map::put_impl(double key, double value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<double>(key);
  if (index >= 0) {
    impl->values.set<double>(index, value);
  } else {
    impl->keys.append<double>(key);
    impl->values.append<double>(value);
  }
}

void Map::put_impl(double key, const char *value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<double>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append<double>(key);
    impl->values.append(value);
  }
}

void Map::put_impl(double key, const wchar_t *value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<double>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append<double>(key);
    impl->values.append(value);
  }
}

void Map::put_impl(double key, const String &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<double>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append<double>(key);
    impl->values.append(value);
  }
}

void Map::put_impl(double key, const List &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<double>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append<double>(key);
    impl->values.append(value);
  }
}

void Map::put_impl(double key, const Map &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<double>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append<double>(key);
    impl->values.append(value);
  }
}

void Map::put_impl(double key, const Set &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<double>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append<double>(key);
    impl->values.append(value);
  }
}

void Map::put_impl(const char *key, bool value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<const char *>(key);
  if (index >= 0) {
    impl->values.set<bool>(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append<bool>(value);
  }
}

void Map::put_impl(const char *key, int value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<const char *>(key);
  if (index >= 0) {
    impl->values.set<int>(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append<int>(value);
  }
}

void Map::put_impl(const char *key, long long value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<const char *>(key);
  if (index >= 0) {
    impl->values.set<long long>(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append<long long>(value);
  }
}

void Map::put_impl(const char *key, double value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<const char *>(key);
  if (index >= 0) {
    impl->values.set<double>(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append<double>(value);
  }
}

void Map::put_impl(const char *key, const char *value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<const char *>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append(value);
  }
}

void Map::put_impl(const char *key, const wchar_t *value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<const char *>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append(value);
  }
}

void Map::put_impl(const char *key, const String &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<const char *>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append(value);
  }
}

void Map::put_impl(const char *key, const List &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<const char *>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append(value);
  }
}

void Map::put_impl(const char *key, const Map &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<const char *>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append(value);
  }
}

void Map::put_impl(const char *key, const Set &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<const char *>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append(value);
  }
}

void Map::put_impl(const wchar_t *key, bool value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<const wchar_t *>(key);
  if (index >= 0) {
    impl->values.set<bool>(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append<bool>(value);
  }
}

void Map::put_impl(const wchar_t *key, int value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<const wchar_t *>(key);
  if (index >= 0) {
    impl->values.set<int>(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append<int>(value);
  }
}

void Map::put_impl(const wchar_t *key, long long value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<const wchar_t *>(key);
  if (index >= 0) {
    impl->values.set<long long>(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append<long long>(value);
  }
}

void Map::put_impl(const wchar_t *key, double value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<const wchar_t *>(key);
  if (index >= 0) {
    impl->values.set<double>(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append<double>(value);
  }
}

void Map::put_impl(const wchar_t *key, const char *value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<const wchar_t *>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append(value);
  }
}

void Map::put_impl(const wchar_t *key, const wchar_t *value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<const wchar_t *>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append(value);
  }
}

void Map::put_impl(const wchar_t *key, const String &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<const wchar_t *>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append(value);
  }
}

void Map::put_impl(const wchar_t *key, const List &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<const wchar_t *>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append(value);
  }
}

void Map::put_impl(const wchar_t *key, const Map &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<const wchar_t *>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append(value);
  }
}

void Map::put_impl(const wchar_t *key, const Set &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<const wchar_t *>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append(value);
  }
}

void Map::put_impl(const String &key, bool value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<String>(key);
  if (index >= 0) {
    impl->values.set<bool>(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append<bool>(value);
  }
}

void Map::put_impl(const String &key, int value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<String>(key);
  if (index >= 0) {
    impl->values.set<int>(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append<int>(value);
  }
}

void Map::put_impl(const String &key, long long value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<String>(key);
  if (index >= 0) {
    impl->values.set<long long>(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append<long long>(value);
  }
}

void Map::put_impl(const String &key, double value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<String>(key);
  if (index >= 0) {
    impl->values.set<double>(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append<double>(value);
  }
}

void Map::put_impl(const String &key, const char *value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<String>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append(value);
  }
}

void Map::put_impl(const String &key, const wchar_t *value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<String>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append(value);
  }
}

void Map::put_impl(const String &key, const String &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<String>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append(value);
  }
}

void Map::put_impl(const String &key, const List &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<String>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append(value);
  }
}

void Map::put_impl(const String &key, const Map &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<String>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append(value);
  }
}

void Map::put_impl(const String &key, const Set &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<String>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append(value);
  }
}

} // namespace attoboy
