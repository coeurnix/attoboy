#include "attopath_internal.h"
#include "attostring_internal.h"
#include "attolist_internal.h"
#include <shlobj.h>

namespace attoboy {

bool Path::ChangeCurrentDirectory(const Path &path) {
  if (!path.impl || !path.impl->pathStr)
    return false;

  ReadLockGuard guard(&path.impl->lock);
  return SetCurrentDirectory(path.impl->pathStr) != 0;
}

List Path::ListVolumes() {
  List result;

  DWORD drives = GetLogicalDrives();
  for (int i = 0; i < 26; i++) {
    if (drives & (1 << i)) {
      ATTO_CHAR driveLetter[4];
      driveLetter[0] = ATTO_TEXT('A') + i;
      driveLetter[1] = ATTO_TEXT(':');
      driveLetter[2] = ATTO_TEXT('\\');
      driveLetter[3] = 0;

      result.append(String(driveLetter));
    }
  }

  return result;
}

Path Path::CreateTemporaryFile(const String &prefix) {
  ATTO_CHAR tempPath[MAX_PATH];
  ATTO_CHAR tempFile[MAX_PATH];

  DWORD pathLen = GetTempPath(MAX_PATH, tempPath);
  if (pathLen == 0 || pathLen >= MAX_PATH)
    return Path(String());

  const ATTO_CHAR *prefixStr = prefix.isEmpty() ? ATTO_TEXT("tmp") : prefix.c_str();

  if (GetTempFileName(tempPath, prefixStr, 0, tempFile) == 0)
    return Path(String());

  return Path(String(tempFile));
}

Path Path::CreateTemporaryDirectory(const String &prefix) {
  ATTO_CHAR tempPath[MAX_PATH];
  DWORD pathLen = GetTempPath(MAX_PATH, tempPath);
  if (pathLen == 0 || pathLen >= MAX_PATH)
    return Path(String());

  const ATTO_CHAR *prefixStr = prefix.isEmpty() ? ATTO_TEXT("tmp") : prefix.c_str();
  int prefixLen = ATTO_LSTRLEN(prefixStr);

  ATTO_CHAR dirPath[MAX_PATH];
  int offset = 0;

  for (DWORD i = 0; i < pathLen && offset < MAX_PATH - 1; i++) {
    dirPath[offset++] = tempPath[i];
  }

  if (offset > 0 && dirPath[offset - 1] != '\\' && dirPath[offset - 1] != '/') {
    if (offset < MAX_PATH - 1) {
      dirPath[offset++] = '\\';
    }
  }

  for (int i = 0; i < prefixLen && offset < MAX_PATH - 1; i++) {
    dirPath[offset++] = prefixStr[i];
  }

  for (int attempt = 0; attempt < 1000; attempt++) {
    int numOffset = offset;

    LARGE_INTEGER perfCounter;
    QueryPerformanceCounter(&perfCounter);
    unsigned int randomNum = (unsigned int)((perfCounter.QuadPart ^ (perfCounter.QuadPart >> 32)) & 0xFFFFFF) + attempt * 1231;
    randomNum = randomNum % 1000000;

    ATTO_CHAR numStr[16];
    int numLen = 0;
    unsigned int temp = randomNum;
    do {
      numStr[numLen++] = ATTO_TEXT('0') + (temp % 10);
      temp /= 10;
    } while (temp > 0 && numLen < 15);

    for (int i = numLen - 1; i >= 0 && numOffset < MAX_PATH - 1; i--) {
      dirPath[numOffset++] = numStr[i];
    }

    dirPath[numOffset] = 0;

    if (CreateDirectory(dirPath, nullptr)) {
      return Path(String(dirPath));
    }

    DWORD err = GetLastError();
    if (err != ERROR_ALREADY_EXISTS) {
      break;
    }
  }

  return Path(String());
}

Path Path::GetHomeDirectory() {
  ATTO_CHAR path[MAX_PATH];

#ifdef UNICODE
  if (SHGetFolderPathW(nullptr, CSIDL_PROFILE, nullptr, 0, path) != S_OK)
    return Path(String());
#else
  if (SHGetFolderPathA(nullptr, CSIDL_PROFILE, nullptr, 0, path) != S_OK)
    return Path(String());
#endif

  return Path(String(path));
}

Path Path::GetDocumentsDirectory() {
  ATTO_CHAR path[MAX_PATH];

#ifdef UNICODE
  if (SHGetFolderPathW(nullptr, CSIDL_MYDOCUMENTS, nullptr, 0, path) != S_OK)
    return Path(String());
#else
  if (SHGetFolderPathA(nullptr, CSIDL_MYDOCUMENTS, nullptr, 0, path) != S_OK)
    return Path(String());
#endif

  return Path(String(path));
}

Path Path::GetRoamingAppDirectory() {
  ATTO_CHAR path[MAX_PATH];

#ifdef UNICODE
  if (SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, 0, path) != S_OK)
    return Path(String());
#else
  if (SHGetFolderPathA(nullptr, CSIDL_APPDATA, nullptr, 0, path) != S_OK)
    return Path(String());
#endif

  return Path(String(path));
}

Path Path::GetLocalAppDirectory() {
  ATTO_CHAR path[MAX_PATH];

#ifdef UNICODE
  if (SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, path) != S_OK)
    return Path(String());
#else
  if (SHGetFolderPathA(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, path) != S_OK)
    return Path(String());
#endif

  return Path(String(path));
}

} // namespace attoboy
