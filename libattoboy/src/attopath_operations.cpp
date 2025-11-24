#include "attopath_internal.h"
#include "attostring_internal.h"

namespace attoboy {

static bool RemoveDirectoryRecursive(const ATTO_CHAR *path);

bool Path::moveTo(const Path &path) const {
  if (!impl || !impl->pathStr || !path.impl || !path.impl->pathStr)
    return false;

  ReadLockGuard guard(&impl->lock);
  ReadLockGuard otherGuard(&path.impl->lock);

  return MoveFile(impl->pathStr, path.impl->pathStr) != 0;
}

bool Path::copyTo(const Path &path) const {
  if (!impl || !impl->pathStr || !path.impl || !path.impl->pathStr)
    return false;

  ReadLockGuard guard(&impl->lock);
  ReadLockGuard otherGuard(&path.impl->lock);

  return CopyFile(impl->pathStr, path.impl->pathStr, FALSE) != 0;
}

bool Path::deleteFile() const {
  if (!impl || !impl->pathStr)
    return false;

  ReadLockGuard guard(&impl->lock);
  return DeleteFile(impl->pathStr) != 0;
}

bool Path::removeDirectory(bool deleteIfNotEmpty) const {
  if (!impl || !impl->pathStr)
    return false;

  ReadLockGuard guard(&impl->lock);

  if (!deleteIfNotEmpty) {
    return RemoveDirectory(impl->pathStr) != 0;
  }

  return RemoveDirectoryRecursive(impl->pathStr);
}

bool Path::makeDirectory(bool createParents) const {
  if (!impl || !impl->pathStr)
    return false;

  ReadLockGuard guard(&impl->lock);

  if (!createParents) {
    return CreateDirectory(impl->pathStr, nullptr) != 0;
  }

  ATTO_CHAR tempPath[MAX_PATH];
  int len = impl->len;
  if (len >= MAX_PATH)
    len = MAX_PATH - 1;

  for (int i = 0; i <= len; i++) {
    tempPath[i] = impl->pathStr[i];
  }
  tempPath[len] = 0;

  for (int i = 0; i < len; i++) {
    ATTO_CHAR c = tempPath[i];
    if ((c == '\\' || c == '/') && i > 0) {
      ATTO_CHAR saved = tempPath[i];
      tempPath[i] = 0;

      DWORD attrs = GetFileAttributes(tempPath);
      if (attrs == INVALID_FILE_ATTRIBUTES) {
        if (!CreateDirectory(tempPath, nullptr)) {
          DWORD err = GetLastError();
          if (err != ERROR_ALREADY_EXISTS) {
            return false;
          }
        }
      }

      tempPath[i] = saved;
    }
  }

  DWORD attrs = GetFileAttributes(tempPath);
  if (attrs == INVALID_FILE_ATTRIBUTES) {
    return CreateDirectory(tempPath, nullptr) != 0;
  }

  return true;
}

Path Path::getSymbolicLinkTarget() const {
  if (!impl || !impl->pathStr)
    return Path(String());

  ReadLockGuard guard(&impl->lock);

  HANDLE hFile = CreateFile(impl->pathStr, 0, FILE_SHARE_READ | FILE_SHARE_WRITE,
                            nullptr, OPEN_EXISTING,
                            FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
                            nullptr);
  if (hFile == INVALID_HANDLE_VALUE)
    return Path(String());

  BYTE *buffer = (BYTE *)HeapAlloc(GetProcessHeap(), 0, MAXIMUM_REPARSE_DATA_BUFFER_SIZE);
  if (!buffer) {
    CloseHandle(hFile);
    return Path(String());
  }

  DWORD bytesReturned;
  bool success = DeviceIoControl(hFile, FSCTL_GET_REPARSE_POINT, nullptr, 0,
                                 buffer, MAXIMUM_REPARSE_DATA_BUFFER_SIZE, &bytesReturned, nullptr);

  if (!success) {
    HeapFree(GetProcessHeap(), 0, buffer);
    CloseHandle(hFile);
    return Path(String());
  }

  REPARSE_DATA_BUFFER *reparseData = (REPARSE_DATA_BUFFER *)buffer;
  if (reparseData->ReparseTag != IO_REPARSE_TAG_SYMLINK) {
    HeapFree(GetProcessHeap(), 0, buffer);
    CloseHandle(hFile);
    return Path(String());
  }

  WCHAR *targetPathW = reparseData->SymbolicLinkReparseBuffer.PathBuffer +
                       (reparseData->SymbolicLinkReparseBuffer.SubstituteNameOffset /
                        sizeof(WCHAR));
  int targetLenW = reparseData->SymbolicLinkReparseBuffer.SubstituteNameLength /
                   sizeof(WCHAR);

  char* targetPathUtf8 = WideToUtf8(targetPathW);
  if (!targetPathUtf8) {
    HeapFree(GetProcessHeap(), 0, buffer);
    CloseHandle(hFile);
    return Path(String());
  }

  Path result = Path(String(targetPathUtf8));
  FreeConvertedString(targetPathUtf8);
  HeapFree(GetProcessHeap(), 0, buffer);
  CloseHandle(hFile);
  return result;
}

bool Path::setSymbolicLinkTarget(const Path &target) const {
  if (!impl || !impl->pathStr || !target.impl || !target.impl->pathStr)
    return false;

  ReadLockGuard guard(&impl->lock);
  ReadLockGuard targetGuard(&target.impl->lock);

  DWORD flags = 0;
  DWORD targetAttrs = GetFileAttributes(target.impl->pathStr);
  if (targetAttrs != INVALID_FILE_ATTRIBUTES &&
      (targetAttrs & FILE_ATTRIBUTE_DIRECTORY)) {
    flags = SYMBOLIC_LINK_FLAG_DIRECTORY;
  }

  return CreateSymbolicLink(impl->pathStr, target.impl->pathStr, flags) != 0;
}

static bool RemoveDirectoryRecursive(const ATTO_CHAR *path) {
  ATTO_CHAR searchPath[MAX_PATH];
  int pathLen = ATTO_LSTRLEN(path);
  if (pathLen + 3 >= MAX_PATH)
    return false;

  for (int i = 0; i < pathLen; i++) {
    searchPath[i] = path[i];
  }

  int searchLen = pathLen;
  if (searchLen > 0 && searchPath[searchLen - 1] != '\\' &&
      searchPath[searchLen - 1] != '/') {
    searchPath[searchLen++] = '\\';
  }
  searchPath[searchLen] = '*';
  searchPath[searchLen + 1] = 0;

  WIN32_FIND_DATA findData;
  HANDLE hFind = FindFirstFile(searchPath, &findData);
  if (hFind == INVALID_HANDLE_VALUE) {
    return RemoveDirectory(path) != 0;
  }

  do {
    const ATTO_CHAR *name = findData.cFileName;
    if (name[0] == '.' && (name[1] == 0 || (name[1] == '.' && name[2] == 0))) {
      continue;
    }

    ATTO_CHAR fullPath[MAX_PATH];
    int offset = 0;
    for (int i = 0; i < pathLen; i++) {
      fullPath[offset++] = path[i];
    }
    if (offset > 0 && fullPath[offset - 1] != '\\' &&
        fullPath[offset - 1] != '/') {
      fullPath[offset++] = '\\';
    }

    int nameLen = ATTO_LSTRLEN(name);
    if (offset + nameLen >= MAX_PATH) {
      continue;
    }

    for (int i = 0; i < nameLen; i++) {
      fullPath[offset++] = name[i];
    }
    fullPath[offset] = 0;

    if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      RemoveDirectoryRecursive(fullPath);
    } else {
      DWORD attrs = GetFileAttributes(fullPath);
      if (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_READONLY)) {
        SetFileAttributes(fullPath, attrs & ~FILE_ATTRIBUTE_READONLY);
      }
      DeleteFile(fullPath);
    }
  } while (FindNextFile(hFind, &findData));

  FindClose(hFind);
  return RemoveDirectory(path) != 0;
}

} // namespace attoboy
