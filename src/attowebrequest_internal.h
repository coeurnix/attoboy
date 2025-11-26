#pragma once
#include "atto_internal_common.h"
#include "attoboy/attoboy.h"
#include <windows.h>
#include <winhttp.h>

#pragma comment(lib, "winhttp.lib")

namespace attoboy {

struct WebResponseImpl {
  int statusCode;
  WCHAR *statusReason;
  WCHAR *finalUrl;
  unsigned char *body;
  int bodySize;
  Map *headers;
  Map *jsonCache;
  mutable SRWLOCK lock;
  volatile LONG refCount;
};

struct WebRequestImpl {
  WCHAR *url;
  Map *params;
  Map *headers;
  bool hasCompleted;
  WebResponseImpl *response;
  mutable SRWLOCK lock;
  volatile LONG refCount;
};

static inline WCHAR *AllocWebString(int len) {
  return (WCHAR *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                            (len + 1) * sizeof(WCHAR));
}

static inline void FreeWebString(WCHAR *str) {
  if (str)
    HeapFree(GetProcessHeap(), 0, str);
}

} // namespace attoboy
