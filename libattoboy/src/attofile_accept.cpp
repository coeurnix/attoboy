#include "attofile_internal.h"

namespace attoboy {

File File::accept() {
  if (!impl) {
    File invalid(Path(""));
    return invalid;
  }

  ReadLockGuard lock(&impl->lock);

  if (impl->type != FILE_TYPE_SERVER_SOCKET || !impl->isValid ||
      !impl->isOpen) {
    File invalid(Path(""));
    return invalid;
  }

  SOCKET clientSock = ::accept(impl->sock, nullptr, nullptr);
  if (clientSock == INVALID_SOCKET) {
    File invalid(Path(""));
    return invalid;
  }

  FileImpl *clientImpl =
      (FileImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                            sizeof(FileImpl));
  if (!clientImpl) {
    closesocket(clientSock);
    File invalid(Path(""));
    return invalid;
  }

  InitializeSRWLock(&clientImpl->lock);
  clientImpl->type = FILE_TYPE_SOCKET;
  clientImpl->handle = INVALID_HANDLE_VALUE;
  clientImpl->sock = clientSock;
  clientImpl->pathStr = nullptr;
  clientImpl->hostStr = nullptr;
  clientImpl->port = -1;
  clientImpl->isOpen = true;
  clientImpl->isValid = true;
  clientImpl->refCount = 1;

  File result(Path(""));
  if (result.impl && InterlockedDecrement(&result.impl->refCount) == 0) {
    FreeFileStr(result.impl->pathStr);
    result.impl->pathStr = nullptr;
    HeapFree(GetProcessHeap(), 0, result.impl);
  }
  result.impl = clientImpl;

  return result;
}

} // namespace attoboy
