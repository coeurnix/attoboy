#include "attofile_internal.h"

namespace attoboy {

Buffer File::readAllToBuffer() {
  if (!impl || !impl->isOpen || !impl->isValid)
    return Buffer();

  ReadLockGuard lock(&impl->lock);

  if (impl->type == FILE_TYPE_SOCKET) {
    unsigned char *tempBuf =
        (unsigned char *)HeapAlloc(GetProcessHeap(), 0, 8192);
    if (!tempBuf)
      return Buffer();

    Buffer result;
    int totalRead = 0;

    u_long available = 0;
    if (ioctlsocket(impl->sock, FIONREAD, &available) == SOCKET_ERROR) {
      HeapFree(GetProcessHeap(), 0, tempBuf);
      return Buffer();
    }

    if (available == 0) {
      HeapFree(GetProcessHeap(), 0, tempBuf);
      return Buffer();
    }

    while (totalRead < (int)available) {
      int toRead = (available - totalRead > 8192) ? 8192 : (available - totalRead);
      int bytesRead = recv(impl->sock, (char *)tempBuf, toRead, 0);
      if (bytesRead <= 0)
        break;
      result.append(tempBuf, bytesRead);
      totalRead += bytesRead;
    }

    HeapFree(GetProcessHeap(), 0, tempBuf);
    return result;
  } else {
    DWORD fileSize = 0;
    if (impl->type == FILE_TYPE_REGULAR) {
      fileSize = GetFileSize(impl->handle, nullptr);
      if (fileSize == INVALID_FILE_SIZE)
        return Buffer();

      LARGE_INTEGER currentPos;
      currentPos.QuadPart = 0;
      if (!SetFilePointerEx(impl->handle, currentPos, &currentPos, FILE_CURRENT))
        return Buffer();

      if (currentPos.QuadPart >= fileSize)
        return Buffer();

      fileSize -= (DWORD)currentPos.QuadPart;
    } else {
      DWORD available = 0;
      if (!PeekNamedPipe(impl->handle, nullptr, 0, nullptr, &available, nullptr))
        return Buffer();
      fileSize = available;
    }

    if (fileSize == 0)
      return Buffer();

    unsigned char *buffer =
        (unsigned char *)HeapAlloc(GetProcessHeap(), 0, fileSize);
    if (!buffer)
      return Buffer();

    DWORD bytesRead = 0;
    if (!ReadFile(impl->handle, buffer, fileSize, &bytesRead, nullptr)) {
      HeapFree(GetProcessHeap(), 0, buffer);
      return Buffer();
    }

    Buffer result(buffer, bytesRead);
    HeapFree(GetProcessHeap(), 0, buffer);
    return result;
  }
}

Buffer File::readToBuffer(int count) {
  if (!impl || !impl->isOpen || !impl->isValid || count <= 0)
    return Buffer();

  ReadLockGuard lock(&impl->lock);

  unsigned char *buffer =
      (unsigned char *)HeapAlloc(GetProcessHeap(), 0, count);
  if (!buffer)
    return Buffer();

  int bytesRead = 0;

  if (impl->type == FILE_TYPE_SOCKET) {
    bytesRead = recv(impl->sock, (char *)buffer, count, 0);
    if (bytesRead <= 0) {
      HeapFree(GetProcessHeap(), 0, buffer);
      return Buffer();
    }
  } else {
    DWORD dwBytesRead = 0;
    if (!ReadFile(impl->handle, buffer, count, &dwBytesRead, nullptr)) {
      HeapFree(GetProcessHeap(), 0, buffer);
      return Buffer();
    }
    bytesRead = (int)dwBytesRead;
  }

  Buffer result(buffer, bytesRead);
  HeapFree(GetProcessHeap(), 0, buffer);
  return result;
}

String File::readAllToString() {
  Buffer buf = readAllToBuffer();
  if (buf.isEmpty())
    return String();
  return buf.toString();
}

String File::readToString(int count) {
  Buffer buf = readToBuffer(count);
  if (buf.isEmpty())
    return String();
  return buf.toString();
}

bool File::hasAvailable() const {
  if (!impl || !impl->isOpen || !impl->isValid)
    return false;

  ReadLockGuard lock(&impl->lock);

  if (impl->type == FILE_TYPE_SOCKET) {
    u_long available = 0;
    if (ioctlsocket(impl->sock, FIONREAD, &available) == SOCKET_ERROR)
      return false;
    return available > 0;
  } else if (impl->type == FILE_TYPE_REGULAR) {
    DWORD fileSize = GetFileSize(impl->handle, nullptr);
    if (fileSize == INVALID_FILE_SIZE)
      return false;

    LARGE_INTEGER currentPos;
    currentPos.QuadPart = 0;
    if (!SetFilePointerEx(impl->handle, currentPos, &currentPos, FILE_CURRENT))
      return false;

    return currentPos.QuadPart < fileSize;
  } else {
    DWORD available = 0;
    if (!PeekNamedPipe(impl->handle, nullptr, 0, nullptr, &available, nullptr))
      return false;
    return available > 0;
  }
}

int File::getAvailableCount() const {
  if (!impl || !impl->isOpen || !impl->isValid)
    return 0;

  ReadLockGuard lock(&impl->lock);

  if (impl->type == FILE_TYPE_SOCKET) {
    u_long available = 0;
    if (ioctlsocket(impl->sock, FIONREAD, &available) == SOCKET_ERROR)
      return 0;
    return (int)available;
  } else if (impl->type == FILE_TYPE_REGULAR) {
    DWORD fileSize = GetFileSize(impl->handle, nullptr);
    if (fileSize == INVALID_FILE_SIZE)
      return 0;

    LARGE_INTEGER currentPos;
    currentPos.QuadPart = 0;
    if (!SetFilePointerEx(impl->handle, currentPos, &currentPos, FILE_CURRENT))
      return 0;

    if (currentPos.QuadPart >= fileSize)
      return 0;

    return (int)(fileSize - currentPos.QuadPart);
  } else {
    DWORD available = 0;
    if (!PeekNamedPipe(impl->handle, nullptr, 0, nullptr, &available, nullptr))
      return 0;
    return (int)available;
  }
}

} // namespace attoboy
