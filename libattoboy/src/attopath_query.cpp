#include "attopath_internal.h"
#include "attostring_internal.h"
#include "attolist_internal.h"

namespace attoboy {

static void FindChildrenRecursive(const ATTO_CHAR *path, List &result);

String Path::getName() const {
  if (!impl || !impl->pathStr || impl->len == 0)
    return String();

  ReadLockGuard guard(&impl->lock);

  int lastSep = -1;
  for (int i = impl->len - 1; i >= 0; i--) {
    ATTO_CHAR c = impl->pathStr[i];
    if (c == '\\' || c == '/') {
      lastSep = i;
      break;
    }
  }

  if (lastSep == -1) {
    return String(impl->pathStr);
  }

  if (lastSep == impl->len - 1) {
    return String();
  }

  return String(impl->pathStr + lastSep + 1);
}

bool Path::equals(const Path &other) const {
  if (!impl || !other.impl)
    return false;

  ReadLockGuard guard(&impl->lock);
  ReadLockGuard otherGuard(&other.impl->lock);

  if (impl->len != other.impl->len)
    return false;

  return ATTO_LSTRCMPI(impl->pathStr, other.impl->pathStr) == 0;
}

bool Path::operator==(const Path &other) const {
  return equals(other);
}

bool Path::operator!=(const Path &other) const {
  return !equals(other);
}

bool Path::isWithin(const Path &path) const {
  if (!impl || !impl->pathStr || !path.impl || !path.impl->pathStr)
    return false;

  ReadLockGuard guard(&impl->lock);
  ReadLockGuard otherGuard(&path.impl->lock);

  if (impl->len <= path.impl->len)
    return false;

  for (int i = 0; i < path.impl->len; i++) {
    ATTO_CHAR c1 = impl->pathStr[i];
    ATTO_CHAR c2 = path.impl->pathStr[i];

    if (c1 >= 'A' && c1 <= 'Z')
      c1 = c1 - 'A' + 'a';
    if (c2 >= 'A' && c2 <= 'Z')
      c2 = c2 - 'A' + 'a';

    if (c1 != c2)
      return false;
  }

  ATTO_CHAR nextChar = impl->pathStr[path.impl->len];
  return (nextChar == '\\' || nextChar == '/');
}

List Path::listChildren(bool recursive) const {
  List result;

  if (!impl || !impl->pathStr)
    return result;

  ReadLockGuard guard(&impl->lock);

  if (!recursive) {
    ATTO_CHAR searchPath[MAX_PATH];
    int pathLen = impl->len;
    if (pathLen + 3 >= MAX_PATH)
      return result;

    for (int i = 0; i < pathLen; i++) {
      searchPath[i] = impl->pathStr[i];
    }

    if (pathLen > 0 && searchPath[pathLen - 1] != '\\' &&
        searchPath[pathLen - 1] != '/') {
      searchPath[pathLen++] = '\\';
    }
    searchPath[pathLen] = '*';
    searchPath[pathLen + 1] = 0;

    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(searchPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE)
      return result;

    do {
      const ATTO_CHAR *name = findData.cFileName;
      if (name[0] == '.' &&
          (name[1] == 0 || (name[1] == '.' && name[2] == 0))) {
        continue;
      }

      ATTO_CHAR fullPath[MAX_PATH];
      int offset = 0;
      for (int i = 0; i < impl->len; i++) {
        fullPath[offset++] = impl->pathStr[i];
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

      result.append(String(fullPath));
    } while (FindNextFile(hFind, &findData));

    FindClose(hFind);
  } else {
    FindChildrenRecursive(impl->pathStr, result);
  }

  return result;
}

Path Path::getParentDirectory() const {
  if (!impl || !impl->pathStr || impl->len == 0)
    return Path(String());

  ReadLockGuard guard(&impl->lock);

  int lastSep = -1;
  for (int i = impl->len - 1; i >= 0; i--) {
    ATTO_CHAR c = impl->pathStr[i];
    if (c == '\\' || c == '/') {
      if (i == impl->len - 1) {
        continue;
      }
      lastSep = i;
      break;
    }
  }

  if (lastSep <= 0)
    return Path(String());

  ATTO_CHAR parentPath[MAX_PATH];
  if (lastSep >= MAX_PATH)
    return Path(String());

  for (int i = 0; i < lastSep; i++) {
    parentPath[i] = impl->pathStr[i];
  }
  parentPath[lastSep] = 0;

  return Path(String(parentPath));
}

String Path::getExtension() const {
  if (!impl || !impl->pathStr || impl->len == 0)
    return String();

  ReadLockGuard guard(&impl->lock);

  int lastDot = -1;
  int lastSep = -1;

  for (int i = impl->len - 1; i >= 0; i--) {
    ATTO_CHAR c = impl->pathStr[i];
    if (c == '.' && lastDot == -1) {
      lastDot = i;
    }
    if (c == '\\' || c == '/') {
      lastSep = i;
      break;
    }
  }

  if (lastDot == -1 || lastDot <= lastSep || lastDot == impl->len - 1)
    return String();

  return String(impl->pathStr + lastDot + 1);
}

bool Path::hasExtension(const String &ext) const {
  String currentExt = getExtension();
  if (currentExt.isEmpty())
    return ext.isEmpty();

  return ATTO_LSTRCMPI(currentExt.c_str(), ext.c_str()) == 0;
}

String Path::getStem() const {
  if (!impl || !impl->pathStr || impl->len == 0)
    return String();

  ReadLockGuard guard(&impl->lock);

  int lastDot = -1;
  int lastSep = -1;

  for (int i = impl->len - 1; i >= 0; i--) {
    ATTO_CHAR c = impl->pathStr[i];
    if (c == '.' && lastDot == -1) {
      lastDot = i;
    }
    if (c == '\\' || c == '/') {
      lastSep = i;
      break;
    }
  }

  int start = lastSep + 1;
  int end = (lastDot > lastSep) ? lastDot : impl->len;

  if (start >= end)
    return String();

  ATTO_CHAR stem[MAX_PATH];
  int stemLen = end - start;
  if (stemLen >= MAX_PATH)
    stemLen = MAX_PATH - 1;

  for (int i = 0; i < stemLen; i++) {
    stem[i] = impl->pathStr[start + i];
  }
  stem[stemLen] = 0;

  return String(stem);
}

static void FindChildrenRecursive(const ATTO_CHAR *path, List &result) {
  ATTO_CHAR searchPath[MAX_PATH];
  int pathLen = ATTO_LSTRLEN(path);
  if (pathLen + 3 >= MAX_PATH)
    return;

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
  if (hFind == INVALID_HANDLE_VALUE)
    return;

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

    result.append(String(fullPath));

    if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      FindChildrenRecursive(fullPath, result);
    }
  } while (FindNextFile(hFind, &findData));

  FindClose(hFind);
}

} // namespace attoboy
