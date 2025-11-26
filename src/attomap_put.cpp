#include "attomap_internal.h"
#include "attostring_internal.h"

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

void Map::put_impl(bool key, float value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<bool>(key);
  if (index >= 0) {
    impl->values.set<float>(index, value);
  } else {
    impl->keys.append<bool>(key);
    impl->values.append<float>(value);
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

void Map::put_impl(int key, float value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<int>(key);
  if (index >= 0) {
    impl->values.set<float>(index, value);
  } else {
    impl->keys.append<int>(key);
    impl->values.append<float>(value);
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

void Map::put_impl(float key, bool value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<float>(key);
  if (index >= 0) {
    impl->values.set<bool>(index, value);
  } else {
    impl->keys.append<float>(key);
    impl->values.append<bool>(value);
  }
}

void Map::put_impl(float key, int value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<float>(key);
  if (index >= 0) {
    impl->values.set<int>(index, value);
  } else {
    impl->keys.append<float>(key);
    impl->values.append<int>(value);
  }
}

void Map::put_impl(float key, float value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<float>(key);
  if (index >= 0) {
    impl->values.set<float>(index, value);
  } else {
    impl->keys.append<float>(key);
    impl->values.append<float>(value);
  }
}

void Map::put_impl(float key, const char *value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<float>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append<float>(key);
    impl->values.append(value);
  }
}

void Map::put_impl(float key, const String &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<float>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append<float>(key);
    impl->values.append(value);
  }
}

void Map::put_impl(float key, const List &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<float>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append<float>(key);
    impl->values.append(value);
  }
}

void Map::put_impl(float key, const Map &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<float>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append<float>(key);
    impl->values.append(value);
  }
}

void Map::put_impl(float key, const Set &value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<float>(key);
  if (index >= 0) {
    impl->values.set(index, value);
  } else {
    impl->keys.append<float>(key);
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

void Map::put_impl(const char *key, float value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<const char *>(key);
  if (index >= 0) {
    impl->values.set<float>(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append<float>(value);
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

void Map::put_impl(const String &key, float value) {
  if (!impl)
    return;
  WriteLockGuard guard(&impl->lock);

  int index = impl->keys.find<String>(key);
  if (index >= 0) {
    impl->values.set<float>(index, value);
  } else {
    impl->keys.append(key);
    impl->values.append<float>(value);
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
