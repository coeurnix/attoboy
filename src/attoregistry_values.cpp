#include "attoregistry_internal.h"
#include "attostring_internal.h"
#include "attobuffer_internal.h"

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

bool Registry::valueExists(const String &name) const {
  if (!impl)
    return false;

  ReadLockGuard guard(&impl->lock);

  RegistryImpl *mutableImpl = const_cast<RegistryImpl *>(impl);
  if (!EnsureKeyOpen(mutableImpl))
    return false;

  const char *valueName = name.isEmpty() ? nullptr : name.c_str();
  DWORD type;
  LONG result = RegQueryValueEx(impl->hKey, valueName, nullptr, &type, nullptr, nullptr);

  return result == ERROR_SUCCESS;
}

String Registry::getStringValue(const String &name) const {
  if (!impl)
    return String();

  ReadLockGuard guard(&impl->lock);

  RegistryImpl *mutableImpl = const_cast<RegistryImpl *>(impl);
  if (!EnsureKeyOpen(mutableImpl))
    return String();

  const char *valueName = name.isEmpty() ? nullptr : name.c_str();
  DWORD type;
  DWORD size = 0;

  LONG result = RegQueryValueEx(impl->hKey, valueName, nullptr, &type, nullptr, &size);
  if (result != ERROR_SUCCESS || (type != REG_SZ && type != REG_EXPAND_SZ))
    return String();

  if (size == 0)
    return String("");

  ATTO_LPSTR buffer = AllocRegistryString(size / sizeof(ATTO_WCHAR) + 1);
  if (!buffer)
    return String();

  result = RegQueryValueEx(impl->hKey, valueName, nullptr, &type,
                           (LPBYTE)buffer, &size);
  if (result != ERROR_SUCCESS) {
    FreeRegistryString(buffer);
    return String();
  }

  String resultStr(buffer);
  FreeRegistryString(buffer);

  return resultStr;
}

Buffer Registry::getBinaryValue(const String &name) const {
  if (!impl)
    return Buffer();

  ReadLockGuard guard(&impl->lock);

  RegistryImpl *mutableImpl = const_cast<RegistryImpl *>(impl);
  if (!EnsureKeyOpen(mutableImpl))
    return Buffer();

  const char *valueName = name.isEmpty() ? nullptr : name.c_str();
  DWORD type;
  DWORD size = 0;

  LONG result = RegQueryValueEx(impl->hKey, valueName, nullptr, &type, nullptr, &size);
  if (result != ERROR_SUCCESS || type != REG_BINARY)
    return Buffer();

  if (size == 0)
    return Buffer();

  unsigned char *buffer = (unsigned char *)HeapAlloc(GetProcessHeap(), 0, size);
  if (!buffer)
    return Buffer();

  result = RegQueryValueEx(impl->hKey, valueName, nullptr, &type, buffer, &size);
  if (result != ERROR_SUCCESS) {
    HeapFree(GetProcessHeap(), 0, buffer);
    return Buffer();
  }

  Buffer resultBuf(buffer, size);
  HeapFree(GetProcessHeap(), 0, buffer);

  return resultBuf;
}

unsigned int Registry::getIntegerValue(const String &name) const {
  if (!impl)
    return 0;

  ReadLockGuard guard(&impl->lock);

  RegistryImpl *mutableImpl = const_cast<RegistryImpl *>(impl);
  if (!EnsureKeyOpen(mutableImpl))
    return 0;

  const char *valueName = name.isEmpty() ? nullptr : name.c_str();
  DWORD type;
  DWORD value = 0;
  DWORD size = sizeof(DWORD);

  LONG result = RegQueryValueEx(impl->hKey, valueName, nullptr, &type,
                                 (LPBYTE)&value, &size);
  if (result != ERROR_SUCCESS || type != REG_DWORD)
    return 0;

  return value;
}

bool Registry::setStringValue(const String &name, const String &str) {
  if (!impl)
    return false;

  WriteLockGuard guard(&impl->lock);

  if (!EnsureKeyOpen(impl))
    return false;

  const char *valueName = name.isEmpty() ? nullptr : name.c_str();
  const char *valueData = str.c_str();
  DWORD size = (ATTO_LSTRLEN(valueData) + 1) * sizeof(ATTO_WCHAR);

  LONG result = RegSetValueEx(impl->hKey, valueName, 0, REG_SZ,
                               (const BYTE *)valueData, size);

  return result == ERROR_SUCCESS;
}

bool Registry::setBinaryValue(const String &name, const Buffer &buf) {
  if (!impl)
    return false;

  WriteLockGuard guard(&impl->lock);

  if (!EnsureKeyOpen(impl))
    return false;

  const char *valueName = name.isEmpty() ? nullptr : name.c_str();

  int len = 0;
  const unsigned char *data = buf.c_ptr(&len);

  if (!data || len == 0)
    return false;

  LONG result = RegSetValueEx(impl->hKey, valueName, 0, REG_BINARY, data, len);

  return result == ERROR_SUCCESS;
}

bool Registry::setIntegerValue(const String &name, unsigned int num) {
  if (!impl)
    return false;

  WriteLockGuard guard(&impl->lock);

  if (!EnsureKeyOpen(impl))
    return false;

  const char *valueName = name.isEmpty() ? nullptr : name.c_str();
  DWORD value = num;

  LONG result = RegSetValueEx(impl->hKey, valueName, 0, REG_DWORD,
                               (const BYTE *)&value, sizeof(DWORD));

  return result == ERROR_SUCCESS;
}

bool Registry::deleteValue(const String &name) {
  if (!impl)
    return false;

  WriteLockGuard guard(&impl->lock);

  if (!EnsureKeyOpen(impl))
    return false;

  const char *valueName = name.isEmpty() ? nullptr : name.c_str();

  LONG result = RegDeleteValue(impl->hKey, valueName);

  return result == ERROR_SUCCESS;
}

} // namespace attoboy
