#include "attoregistry_internal.h"
#include "attostring_internal.h"

namespace attoboy {

Registry::Registry(const String &key) {
  impl = (RegistryImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                   sizeof(RegistryImpl));
  InitializeSRWLock(&impl->lock);

  const ATTO_CHAR *keyStr = key.c_str();
  impl->keyPathLen = ATTO_LSTRLEN(keyStr);
  impl->keyPath = AllocRegistryString(impl->keyPathLen);
  if (impl->keyPath) {
    ATTO_LSTRCPY(impl->keyPath, keyStr);
  }

  int prefixLen = 0;
  impl->hRootKey = ParseRootKey(impl->keyPath, &prefixLen);

  if (impl->hRootKey && prefixLen < impl->keyPathLen) {
    const ATTO_CHAR *subKey = impl->keyPath + prefixLen;
    while (*subKey == '\\' || *subKey == '/')
      subKey++;

    impl->subKeyPathLen = ATTO_LSTRLEN(subKey);
    if (impl->subKeyPathLen > 0) {
      impl->subKeyPath = AllocRegistryString(impl->subKeyPathLen);
      if (impl->subKeyPath) {
        ATTO_LSTRCPY(impl->subKeyPath, subKey);
      }
    }
  }
}

Registry::Registry(const Registry &other) {
  impl = (RegistryImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                   sizeof(RegistryImpl));
  InitializeSRWLock(&impl->lock);

  if (other.impl) {
    ReadLockGuard guard(&other.impl->lock);

    if (other.impl->keyPath) {
      impl->keyPathLen = other.impl->keyPathLen;
      impl->keyPath = AllocRegistryString(impl->keyPathLen);
      if (impl->keyPath) {
        ATTO_LSTRCPY(impl->keyPath, other.impl->keyPath);
      }
    }

    impl->hRootKey = other.impl->hRootKey;

    if (other.impl->subKeyPath) {
      impl->subKeyPathLen = other.impl->subKeyPathLen;
      impl->subKeyPath = AllocRegistryString(impl->subKeyPathLen);
      if (impl->subKeyPath) {
        ATTO_LSTRCPY(impl->subKeyPath, other.impl->subKeyPath);
      }
    }
  }
}

Registry::~Registry() {
  if (impl) {
    if (impl->hKey) {
      RegCloseKey(impl->hKey);
    }
    FreeRegistryString(impl->keyPath);
    FreeRegistryString(impl->subKeyPath);
    HeapFree(GetProcessHeap(), 0, impl);
  }
}

Registry &Registry::operator=(const Registry &other) {
  if (this == &other)
    return *this;

  if (impl) {
    WriteLockGuard guard(&impl->lock);

    if (impl->hKey) {
      RegCloseKey(impl->hKey);
      impl->hKey = nullptr;
    }

    FreeRegistryString(impl->keyPath);
    FreeRegistryString(impl->subKeyPath);
    impl->keyPath = nullptr;
    impl->subKeyPath = nullptr;
    impl->keyPathLen = 0;
    impl->subKeyPathLen = 0;

    if (other.impl) {
      ReadLockGuard otherGuard(&other.impl->lock);

      if (other.impl->keyPath) {
        impl->keyPathLen = other.impl->keyPathLen;
        impl->keyPath = AllocRegistryString(impl->keyPathLen);
        if (impl->keyPath) {
          ATTO_LSTRCPY(impl->keyPath, other.impl->keyPath);
        }
      }

      impl->hRootKey = other.impl->hRootKey;

      if (other.impl->subKeyPath) {
        impl->subKeyPathLen = other.impl->subKeyPathLen;
        impl->subKeyPath = AllocRegistryString(impl->subKeyPathLen);
        if (impl->subKeyPath) {
          ATTO_LSTRCPY(impl->subKeyPath, other.impl->subKeyPath);
        }
      }
    }
  }

  return *this;
}

bool Registry::exists() const {
  if (!impl || !impl->hRootKey || !impl->subKeyPath)
    return false;

  ReadLockGuard guard(&impl->lock);

  HKEY hKey;
  LONG result = RegOpenKeyEx(impl->hRootKey, impl->subKeyPath, 0, KEY_READ, &hKey);
  if (result == ERROR_SUCCESS) {
    RegCloseKey(hKey);
    return true;
  }

  return false;
}

bool Registry::create() {
  if (!impl || !impl->hRootKey || !impl->subKeyPath)
    return false;

  WriteLockGuard guard(&impl->lock);

  HKEY hKey;
  DWORD disposition;
  LONG result = RegCreateKeyEx(impl->hRootKey, impl->subKeyPath, 0, nullptr,
                                REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr,
                                &hKey, &disposition);

  if (result == ERROR_SUCCESS) {
    if (impl->hKey) {
      RegCloseKey(impl->hKey);
    }
    impl->hKey = hKey;
    return true;
  }

  return false;
}

bool Registry::remove(bool isRecursive) {
  if (!impl || !impl->hRootKey || !impl->subKeyPath)
    return false;

  WriteLockGuard guard(&impl->lock);

  if (impl->hKey) {
    RegCloseKey(impl->hKey);
    impl->hKey = nullptr;
  }

  LONG result;
  if (isRecursive) {
    result = RegDeleteTree(impl->hRootKey, impl->subKeyPath);
  } else {
    result = RegDeleteKey(impl->hRootKey, impl->subKeyPath);
  }

  return result == ERROR_SUCCESS;
}

} // namespace attoboy
