#pragma once
#include "atto_internal_common.h"
#include "attoboy/attoboy.h"
#include <windows.h>
#include <winternl.h>

#ifndef FSCTL_GET_REPARSE_POINT
#define FSCTL_GET_REPARSE_POINT CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 42, METHOD_BUFFERED, FILE_ANY_ACCESS)
#endif

#ifndef IO_REPARSE_TAG_SYMLINK
#define IO_REPARSE_TAG_SYMLINK 0xA000000C
#endif

#ifndef REPARSE_DATA_BUFFER_HEADER_SIZE
typedef struct _REPARSE_DATA_BUFFER {
  ULONG ReparseTag;
  USHORT ReparseDataLength;
  USHORT Reserved;
  union {
    struct {
      USHORT SubstituteNameOffset;
      USHORT SubstituteNameLength;
      USHORT PrintNameOffset;
      USHORT PrintNameLength;
      ULONG Flags;
      WCHAR PathBuffer[1];
    } SymbolicLinkReparseBuffer;
    struct {
      USHORT SubstituteNameOffset;
      USHORT SubstituteNameLength;
      USHORT PrintNameOffset;
      USHORT PrintNameLength;
      WCHAR PathBuffer[1];
    } MountPointReparseBuffer;
    struct {
      UCHAR DataBuffer[1];
    } GenericReparseBuffer;
  };
} REPARSE_DATA_BUFFER, *PREPARSE_DATA_BUFFER;
#endif

namespace attoboy {

#ifdef UNICODE
using ATTO_LPSTR = LPWSTR;
using ATTO_WCHAR = WCHAR;
#define ATTO_LSTRCPY lstrcpyW
#define ATTO_LSTRLEN lstrlenW
#define ATTO_LSTRCMPI lstrcmpiW
#else
using ATTO_LPSTR = LPSTR;
using ATTO_WCHAR = CHAR;
#define ATTO_LSTRCPY lstrcpyA
#define ATTO_LSTRLEN lstrlenA
#define ATTO_LSTRCMPI lstrcmpiA
#endif

struct PathImpl {
  ATTO_LPSTR pathStr;
  int len;
  SRWLOCK lock;

  PathImpl() : pathStr(nullptr), len(0) { InitializeSRWLock(&lock); }
};

static inline ATTO_LPSTR AllocPath(int len) {
  return (ATTO_LPSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                               (len + 1) * sizeof(ATTO_WCHAR));
}

static inline void FreePath(ATTO_LPSTR str) {
  if (str)
    HeapFree(GetProcessHeap(), 0, str);
}

static inline const ATTO_CHAR *GetPathString(const PathImpl *impl) {
  return impl ? impl->pathStr : nullptr;
}

static inline int GetPathLength(const PathImpl *impl) {
  return impl ? impl->len : 0;
}

} // namespace attoboy
