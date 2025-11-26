#include "attoregistry_internal.h"
#include "attostring_internal.h"
#include "attolist_internal.h"

namespace attoboy {

static bool EnsureKeyOpen(RegistryImpl *impl) {
  if (!impl || !impl->hRootKey || !impl->subKeyPath)
    return false;

  if (impl->hKey)
    return true;

  LONG result = RegOpenKeyEx(impl->hRootKey, impl->subKeyPath, 0,
                             KEY_READ | KEY_WRITE, &impl->hKey);
  if (result != ERROR_SUCCESS) {
    result = RegOpenKeyEx(impl->hRootKey, impl->subKeyPath, 0, KEY_READ,
                          &impl->hKey);
  }

  return result == ERROR_SUCCESS;
}

List Registry::listValueNames() const {
  List result;

  if (!impl)
    return result;

  ReadLockGuard guard(&impl->lock);

  RegistryImpl *mutableImpl = const_cast<RegistryImpl *>(impl);
  if (!EnsureKeyOpen(mutableImpl))
    return result;

  DWORD valueCount = 0;
  DWORD maxValueNameLen = 0;

  LONG queryResult = RegQueryInfoKey(impl->hKey, nullptr, nullptr, nullptr, nullptr,
                                     nullptr, nullptr, &valueCount, &maxValueNameLen,
                                     nullptr, nullptr, nullptr);

  if (queryResult != ERROR_SUCCESS || valueCount == 0)
    return result;

  ATTO_LPSTR valueName = AllocRegistryString(maxValueNameLen + 1);
  if (!valueName)
    return result;

  for (DWORD i = 0; i < valueCount; i++) {
    DWORD valueNameLen = maxValueNameLen + 1;
    LONG enumResult = RegEnumValue(impl->hKey, i, valueName, &valueNameLen,
                                    nullptr, nullptr, nullptr, nullptr);

    if (enumResult == ERROR_SUCCESS) {
      result.append(String(valueName));
    }
  }

  FreeRegistryString(valueName);

  return result;
}

List Registry::listSubkeys() const {
  List result;

  if (!impl)
    return result;

  ReadLockGuard guard(&impl->lock);

  RegistryImpl *mutableImpl = const_cast<RegistryImpl *>(impl);
  if (!EnsureKeyOpen(mutableImpl))
    return result;

  DWORD subkeyCount = 0;
  DWORD maxSubkeyLen = 0;

  LONG queryResult = RegQueryInfoKey(impl->hKey, nullptr, nullptr, nullptr,
                                     &subkeyCount, &maxSubkeyLen, nullptr, nullptr,
                                     nullptr, nullptr, nullptr, nullptr);

  if (queryResult != ERROR_SUCCESS || subkeyCount == 0)
    return result;

  ATTO_LPSTR subkeyName = AllocRegistryString(maxSubkeyLen + 1);
  if (!subkeyName)
    return result;

  for (DWORD i = 0; i < subkeyCount; i++) {
    DWORD subkeyNameLen = maxSubkeyLen + 1;
    LONG enumResult = RegEnumKeyEx(impl->hKey, i, subkeyName, &subkeyNameLen,
                                    nullptr, nullptr, nullptr, nullptr);

    if (enumResult == ERROR_SUCCESS) {
      result.append(String(subkeyName));
    }
  }

  FreeRegistryString(subkeyName);

  return result;
}

} // namespace attoboy
