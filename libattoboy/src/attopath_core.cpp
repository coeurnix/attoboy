#include "attopath_internal.h"
#include "attostring_internal.h"

namespace attoboy {

Path::Path(const String &pathStr) {
  impl = (PathImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                               sizeof(PathImpl));
  InitializeSRWLock(&impl->lock);

  const char *str = pathStr.c_str();
  impl->len = ATTO_LSTRLEN(str);
  impl->pathStr = AllocPath(impl->len);
  if (impl->pathStr) {
    ATTO_LSTRCPY(impl->pathStr, str);
  } else {
    impl->len = 0;
  }
}

Path::Path(const Path &other) {
  impl = (PathImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                               sizeof(PathImpl));
  InitializeSRWLock(&impl->lock);

  if (other.impl) {
    ReadLockGuard guard(&other.impl->lock);
    if (other.impl->pathStr) {
      impl->len = other.impl->len;
      impl->pathStr = AllocPath(impl->len);
      if (impl->pathStr) {
        ATTO_LSTRCPY(impl->pathStr, other.impl->pathStr);
      } else {
        impl->len = 0;
      }
    }
  }
}

Path::~Path() {
  if (impl) {
    FreePath(impl->pathStr);
    HeapFree(GetProcessHeap(), 0, impl);
  }
}

Path &Path::operator=(const Path &other) {
  if (this == &other) {
    return *this;
  }

  if (impl) {
    WriteLockGuard guard(&impl->lock);
    FreePath(impl->pathStr);

    if (other.impl) {
      ReadLockGuard otherGuard(&other.impl->lock);
      impl->len = other.impl->len;
      impl->pathStr = AllocPath(impl->len);
      if (impl->pathStr) {
        ATTO_LSTRCPY(impl->pathStr, other.impl->pathStr);
      } else {
        impl->pathStr = nullptr;
        impl->len = 0;
      }
    } else {
      impl->pathStr = nullptr;
      impl->len = 0;
    }
  }

  return *this;
}

bool Path::exists() const {
  if (!impl || !impl->pathStr)
    return false;

  ReadLockGuard guard(&impl->lock);
  DWORD attrs = GetFileAttributes(impl->pathStr);
  return attrs != INVALID_FILE_ATTRIBUTES;
}

bool Path::isRegularFile() const {
  if (!impl || !impl->pathStr)
    return false;

  ReadLockGuard guard(&impl->lock);
  DWORD attrs = GetFileAttributes(impl->pathStr);
  if (attrs == INVALID_FILE_ATTRIBUTES)
    return false;

  if (attrs & FILE_ATTRIBUTE_DIRECTORY)
    return false;

  if (attrs & FILE_ATTRIBUTE_REPARSE_POINT) {
    HANDLE hFile = CreateFileA(impl->pathStr, 0, FILE_SHARE_READ, nullptr,
                              OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
      return false;
    BY_HANDLE_FILE_INFORMATION info;
    bool result = GetFileInformationByHandle(hFile, &info);
    CloseHandle(hFile);
    if (!result)
      return false;
    if (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      return false;
  }

  return true;
}

bool Path::isDirectory() const {
  if (!impl || !impl->pathStr)
    return false;

  ReadLockGuard guard(&impl->lock);
  DWORD attrs = GetFileAttributes(impl->pathStr);
  if (attrs == INVALID_FILE_ATTRIBUTES)
    return false;

  return (attrs & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool Path::isNamedPipe() const {
  if (!impl || !impl->pathStr)
    return false;

  ReadLockGuard guard(&impl->lock);

  if (impl->len < 9)
    return false;

  const char *prefix = "\\\\.\\pipe\\";
  for (int i = 0; i < 9; i++) {
    char c1 = impl->pathStr[i];
    char c2 = prefix[i];
    if (c1 >= 'A' && c1 <= 'Z')
      c1 = c1 - 'A' + 'a';
    if (c2 >= 'A' && c2 <= 'Z')
      c2 = c2 - 'A' + 'a';
    if (c1 != c2)
      return false;
  }

  return true;
}

bool Path::isSymbolicLink() const {
  if (!impl || !impl->pathStr)
    return false;

  ReadLockGuard guard(&impl->lock);
  DWORD attrs = GetFileAttributes(impl->pathStr);
  if (attrs == INVALID_FILE_ATTRIBUTES)
    return false;

  if (!(attrs & FILE_ATTRIBUTE_REPARSE_POINT))
    return false;

  HANDLE hFile =
      CreateFileA(impl->pathStr, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                 OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
                 nullptr);
  if (hFile == INVALID_HANDLE_VALUE)
    return false;

  BYTE *buffer = (BYTE *)HeapAlloc(GetProcessHeap(), 0, MAXIMUM_REPARSE_DATA_BUFFER_SIZE);
  if (!buffer) {
    CloseHandle(hFile);
    return false;
  }

  DWORD bytesReturned;
  bool isReparsePoint = DeviceIoControl(
      hFile, FSCTL_GET_REPARSE_POINT, nullptr, 0, buffer, MAXIMUM_REPARSE_DATA_BUFFER_SIZE,
      &bytesReturned, nullptr);

  bool result = false;
  if (isReparsePoint) {
    REPARSE_DATA_BUFFER *reparseData = (REPARSE_DATA_BUFFER *)buffer;
    result = reparseData->ReparseTag == IO_REPARSE_TAG_SYMLINK;
  }

  HeapFree(GetProcessHeap(), 0, buffer);
  CloseHandle(hFile);
  return result;
}

bool Path::isOther() const {
  if (!impl || !impl->pathStr)
    return false;

  ReadLockGuard guard(&impl->lock);

  if (isNamedPipe())
    return true;

  if (impl->len >= 4) {
    const char *prefix = "\\\\.\\";
    bool hasPrefix = true;
    for (int i = 0; i < 4; i++) {
      if (impl->pathStr[i] != prefix[i]) {
        hasPrefix = false;
        break;
      }
    }
    if (hasPrefix && !isNamedPipe())
      return true;
  }

  return false;
}

} // namespace attoboy
