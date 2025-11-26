#include "attofile_internal.h"
#include "attopath_internal.h"

namespace attoboy {

static bool InitWinsock() {
  static bool initialized = false;
  if (!initialized) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
      return false;
    }
    initialized = true;
  }
  return true;
}

File::File(const Path &path) {
  impl = (FileImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                               sizeof(FileImpl));
  if (!impl)
    return;
  InitializeSRWLock(&impl->lock);

  impl->type = FILE_TYPE_INVALID;
  impl->handle = INVALID_HANDLE_VALUE;
  impl->sock = INVALID_SOCKET;
  impl->port = -1;
  impl->refCount = 1;

  const char *pathCStr = nullptr;
  int pathLen = 0;

  if (path.impl) {
    ReadLockGuard lock(&path.impl->lock);
    pathCStr = path.impl->pathStr;
    pathLen = path.impl->len;
  }

  if (!pathCStr || pathLen == 0) {
    impl->isValid = false;
    return;
  }

  impl->pathStr = AllocFileStr(pathLen);
  if (!impl->pathStr) {
    impl->isValid = false;
    return;
  }
  ATTO_LSTRCPY(impl->pathStr, pathCStr);

  WCHAR* pathWide = Utf8ToWide(pathCStr);
  if (!pathWide) {
    impl->isValid = false;
    return;
  }

  impl->handle = CreateFileW(pathWide, GENERIC_READ | GENERIC_WRITE,
                             FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                             OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

  FreeConvertedString(pathWide);

  if (impl->handle != INVALID_HANDLE_VALUE) {
    DWORD fileType = GetFileType(impl->handle);
    if (fileType == FILE_TYPE_DISK) {
      impl->type = FILE_TYPE_REGULAR;
    } else if (fileType == FILE_TYPE_PIPE) {
      impl->type = FILE_TYPE_NAMED_PIPE;
    } else {
      impl->type = FILE_TYPE_INVALID;
      CloseHandle(impl->handle);
      impl->handle = INVALID_HANDLE_VALUE;
      impl->isValid = false;
      return;
    }
    impl->isOpen = true;
    impl->isValid = true;
  } else {
    impl->isValid = false;
  }
}

File::File(const String &host, int port) {
  impl = (FileImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                               sizeof(FileImpl));
  if (!impl)
    return;
  InitializeSRWLock(&impl->lock);

  impl->type = FILE_TYPE_INVALID;
  impl->handle = INVALID_HANDLE_VALUE;
  impl->sock = INVALID_SOCKET;
  impl->port = -1;
  impl->refCount = 1;

  if (!InitWinsock()) {
    impl->isValid = false;
    return;
  }

  const char *hostCStr = host.c_str();
  if (!hostCStr) {
    impl->isValid = false;
    return;
  }

  int hostLen = ATTO_LSTRLEN(hostCStr);
  impl->hostStr = AllocFileStr(hostLen);
  if (!impl->hostStr) {
    impl->isValid = false;
    return;
  }
  ATTO_LSTRCPY(impl->hostStr, hostCStr);
  impl->port = port;

  impl->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (impl->sock == INVALID_SOCKET) {
    impl->isValid = false;
    return;
  }

  char hostA[256];
  lstrcpyA(hostA, hostCStr);

  char portStr[16];
  int portVal = port;
  int idx = 0;
  if (portVal == 0) {
    portStr[idx++] = '0';
  } else {
    char temp[16];
    int tempIdx = 0;
    while (portVal > 0) {
      temp[tempIdx++] = '0' + (portVal % 10);
      portVal /= 10;
    }
    for (int i = tempIdx - 1; i >= 0; i--) {
      portStr[idx++] = temp[i];
    }
  }
  portStr[idx] = '\0';

  struct addrinfo hints, *result = nullptr;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  if (getaddrinfo(hostA, portStr, &hints, &result) != 0) {
    closesocket(impl->sock);
    impl->sock = INVALID_SOCKET;
    impl->isValid = false;
    return;
  }

  if (connect(impl->sock, result->ai_addr, (int)result->ai_addrlen) ==
      SOCKET_ERROR) {
    freeaddrinfo(result);
    closesocket(impl->sock);
    impl->sock = INVALID_SOCKET;
    impl->isValid = false;
    return;
  }

  freeaddrinfo(result);
  impl->type = FILE_TYPE_SOCKET;
  impl->isOpen = true;
  impl->isValid = true;
}

File::File(int port) {
  impl = (FileImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                               sizeof(FileImpl));
  if (!impl)
    return;
  InitializeSRWLock(&impl->lock);

  impl->type = FILE_TYPE_INVALID;
  impl->handle = INVALID_HANDLE_VALUE;
  impl->sock = INVALID_SOCKET;
  impl->port = -1;
  impl->refCount = 1;

  if (!InitWinsock()) {
    impl->isValid = false;
    return;
  }

  impl->port = port;

  impl->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (impl->sock == INVALID_SOCKET) {
    impl->isValid = false;
    return;
  }

  int optval = 1;
  setsockopt(impl->sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval,
             sizeof(optval));

  struct sockaddr_in addr;
  ZeroMemory(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons((u_short)port);

  if (bind(impl->sock, (struct sockaddr *)&addr, sizeof(addr)) ==
      SOCKET_ERROR) {
    closesocket(impl->sock);
    impl->sock = INVALID_SOCKET;
    impl->isValid = false;
    return;
  }

  if (listen(impl->sock, SOMAXCONN) == SOCKET_ERROR) {
    closesocket(impl->sock);
    impl->sock = INVALID_SOCKET;
    impl->isValid = false;
    return;
  }

  impl->type = FILE_TYPE_SERVER_SOCKET;
  impl->isOpen = true;
  impl->isValid = true;
}

File::File(const File &other) {
  impl = other.impl;
  if (impl) {
    InterlockedIncrement(&impl->refCount);
  }
}

File::~File() {
  if (impl) {
    if (InterlockedDecrement(&impl->refCount) == 0) {
      close();
      FreeFileStr(impl->pathStr);
      FreeFileStr(impl->hostStr);
      HeapFree(GetProcessHeap(), 0, impl);
    }
  }
}

File &File::operator=(const File &other) {
  if (this != &other) {
    if (impl && InterlockedDecrement(&impl->refCount) == 0) {
      close();
      FreeFileStr(impl->pathStr);
      FreeFileStr(impl->hostStr);
      HeapFree(GetProcessHeap(), 0, impl);
    }
    impl = other.impl;
    if (impl) {
      InterlockedIncrement(&impl->refCount);
    }
  }
  return *this;
}

const char *File::getPath() const {
  if (!impl)
    return nullptr;
  ReadLockGuard lock(&impl->lock);
  return impl->pathStr;
}

const char *File::getHost() const {
  if (!impl)
    return nullptr;
  ReadLockGuard lock(&impl->lock);
  return impl->hostStr;
}

int File::getPort() const {
  if (!impl)
    return -1;
  ReadLockGuard lock(&impl->lock);
  return impl->port;
}

bool File::isValid() const {
  if (!impl)
    return false;
  ReadLockGuard lock(&impl->lock);
  return impl->isValid;
}

bool File::isOpen() const {
  if (!impl)
    return false;
  ReadLockGuard lock(&impl->lock);
  return impl->isOpen;
}

void File::close() {
  if (!impl)
    return;
  WriteLockGuard lock(&impl->lock);
  if (!impl->isOpen)
    return;

  if (impl->type == FILE_TYPE_SOCKET || impl->type == FILE_TYPE_SERVER_SOCKET) {
    if (impl->sock != INVALID_SOCKET) {
      closesocket(impl->sock);
      impl->sock = INVALID_SOCKET;
    }
  } else {
    if (impl->handle != INVALID_HANDLE_VALUE) {
      CloseHandle(impl->handle);
      impl->handle = INVALID_HANDLE_VALUE;
    }
  }
  impl->isOpen = false;
}

bool File::isRegularFile() const {
  if (!impl)
    return false;
  ReadLockGuard lock(&impl->lock);
  return impl->type == FILE_TYPE_REGULAR;
}

bool File::isSocket() const {
  if (!impl)
    return false;
  ReadLockGuard lock(&impl->lock);
  return impl->type == FILE_TYPE_SOCKET;
}

bool File::isNamedPipe() const {
  if (!impl)
    return false;
  ReadLockGuard lock(&impl->lock);
  return impl->type == FILE_TYPE_NAMED_PIPE;
}

bool File::isServerSocket() const {
  if (!impl)
    return false;
  ReadLockGuard lock(&impl->lock);
  return impl->type == FILE_TYPE_SERVER_SOCKET;
}

} // namespace attoboy
