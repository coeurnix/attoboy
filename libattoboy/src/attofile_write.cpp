#include "attofile_internal.h"

namespace attoboy {

int File::write(const Buffer &buf) {
  if (!impl || !impl->isOpen || !impl->isValid)
    return -1;

  int len = 0;
  const unsigned char *data = buf.c_ptr(&len);
  if (!data || len == 0)
    return 0;

  WriteLockGuard lock(&impl->lock);

  if (impl->type == FILE_TYPE_SOCKET) {
    int bytesSent = send(impl->sock, (const char *)data, len, 0);
    if (bytesSent == SOCKET_ERROR)
      return -1;
    return bytesSent;
  } else {
    DWORD bytesWritten = 0;
    if (!WriteFile(impl->handle, data, len, &bytesWritten, nullptr))
      return -1;
    FlushFileBuffers(impl->handle);
    return (int)bytesWritten;
  }
}

int File::write(const String &str) {
  if (!impl || !impl->isOpen || !impl->isValid)
    return -1;

  const ATTO_CHAR *data = str.c_str();
  if (!data)
    return 0;

  int len = str.byteLength();
  if (len == 0)
    return 0;

  int byteLen = len * sizeof(ATTO_CHAR);

  WriteLockGuard lock(&impl->lock);

  if (impl->type == FILE_TYPE_SOCKET) {
    int bytesSent = send(impl->sock, (const char *)data, byteLen, 0);
    if (bytesSent == SOCKET_ERROR)
      return -1;
    return bytesSent;
  } else {
    DWORD bytesWritten = 0;
    if (!WriteFile(impl->handle, data, byteLen, &bytesWritten, nullptr))
      return -1;
    FlushFileBuffers(impl->handle);
    return (int)bytesWritten;
  }
}

int File::writeUpTo(const Buffer &buf, int count) {
  if (!impl || !impl->isOpen || !impl->isValid)
    return -1;

  int len = 0;
  const unsigned char *data = buf.c_ptr(&len);
  if (!data || len == 0)
    return 0;

  if (count == -1)
    count = len;
  else if (count > len)
    count = len;

  if (count <= 0)
    return 0;

  WriteLockGuard lock(&impl->lock);

  if (impl->type == FILE_TYPE_SOCKET) {
    int bytesSent = send(impl->sock, (const char *)data, count, 0);
    if (bytesSent == SOCKET_ERROR)
      return -1;
    return bytesSent;
  } else {
    DWORD bytesWritten = 0;
    if (!WriteFile(impl->handle, data, count, &bytesWritten, nullptr))
      return -1;
    FlushFileBuffers(impl->handle);
    return (int)bytesWritten;
  }
}

int File::writeUpTo(const String &str, int count) {
  if (!impl || !impl->isOpen || !impl->isValid)
    return -1;

  const ATTO_CHAR *data = str.c_str();
  if (!data)
    return 0;

  int len = str.byteLength();
  if (len == 0)
    return 0;

  int byteLen = len * sizeof(ATTO_CHAR);

  if (count == -1)
    count = byteLen;
  else if (count > byteLen)
    count = byteLen;

  if (count <= 0)
    return 0;

  WriteLockGuard lock(&impl->lock);

  if (impl->type == FILE_TYPE_SOCKET) {
    int bytesSent = send(impl->sock, (const char *)data, count, 0);
    if (bytesSent == SOCKET_ERROR)
      return -1;
    return bytesSent;
  } else {
    DWORD bytesWritten = 0;
    if (!WriteFile(impl->handle, data, count, &bytesWritten, nullptr))
      return -1;
    FlushFileBuffers(impl->handle);
    return (int)bytesWritten;
  }
}

bool File::flush() {
  if (!impl || !impl->isOpen || !impl->isValid)
    return false;

  WriteLockGuard lock(&impl->lock);

  if (impl->type == FILE_TYPE_SOCKET) {
    return true;
  } else {
    return FlushFileBuffers(impl->handle) != 0;
  }
}

} // namespace attoboy
