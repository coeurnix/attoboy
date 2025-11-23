#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include "atto_internal_common.h"
#include "attoboy/attoboy.h"
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

namespace attoboy {

#ifdef UNICODE
using ATTO_LPSTR = LPWSTR;
using ATTO_WCHAR = WCHAR;
#define ATTO_LSTRCPY lstrcpyW
#define ATTO_LSTRLEN lstrlenW
#else
using ATTO_LPSTR = LPSTR;
using ATTO_WCHAR = CHAR;
#define ATTO_LSTRCPY lstrcpyA
#define ATTO_LSTRLEN lstrlenA
#endif

enum FileType {
  FILE_TYPE_INVALID = 0,
  FILE_TYPE_REGULAR,
  FILE_TYPE_NAMED_PIPE,
  FILE_TYPE_SOCKET,
  FILE_TYPE_SERVER_SOCKET
};

struct FileImpl {
  FileType type;
  HANDLE handle;
  SOCKET sock;
  ATTO_LPSTR pathStr;
  ATTO_LPSTR hostStr;
  int port;
  bool isOpen;
  bool isValid;
  SRWLOCK lock;
};

static inline ATTO_LPSTR AllocFileStr(int len) {
  return (ATTO_LPSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                               (len + 1) * sizeof(ATTO_WCHAR));
}

static inline void FreeFileStr(ATTO_LPSTR str) {
  if (str)
    HeapFree(GetProcessHeap(), 0, str);
}

} // namespace attoboy
