#include "attosubprocess_internal.h"
#include "attostring_internal.h"
#include "attobuffer_internal.h"

namespace attoboy {

bool Subprocess::hasAvailable() const {
  if (!impl || !impl->valid || !impl->streaming)
    return false;

  ReadLockGuard guard(&impl->lock);
  if (!impl->hStdOutRead)
    return false;

  DWORD bytesAvailable = 0;
  if (!PeekNamedPipe(impl->hStdOutRead, nullptr, 0, nullptr, &bytesAvailable,
                     nullptr))
    return false;

  return bytesAvailable > 0;
}

Buffer Subprocess::readToBuffer() {
  if (!impl || !impl->valid || !impl->streaming)
    return Buffer();

  ReadLockGuard guard(&impl->lock);
  if (!impl->hStdOutRead)
    return Buffer();

  DWORD bytesAvailable = 0;
  if (!PeekNamedPipe(impl->hStdOutRead, nullptr, 0, nullptr, &bytesAvailable,
                     nullptr))
    return Buffer();

  if (bytesAvailable == 0)
    return Buffer();

  unsigned char *data =
      (unsigned char *)HeapAlloc(GetProcessHeap(), 0, bytesAvailable);
  if (!data)
    return Buffer();

  DWORD bytesRead = 0;
  if (!ReadFile(impl->hStdOutRead, data, bytesAvailable, &bytesRead, nullptr) ||
      bytesRead == 0) {
    HeapFree(GetProcessHeap(), 0, data);
    return Buffer();
  }

  Buffer result(data, bytesRead);
  HeapFree(GetProcessHeap(), 0, data);
  return result;
}

String Subprocess::readToString() {
  Buffer buf = readToBuffer();
  return buf.toString();
}

int Subprocess::write(const Buffer &buf) {
  if (!impl || !impl->valid || !impl->streaming)
    return -1;

  ReadLockGuard guard(&impl->lock);
  if (!impl->hStdInWrite)
    return -1;

  int len = 0;
  const unsigned char *data = buf.c_ptr(&len);
  if (!data || len == 0)
    return 0;

  DWORD bytesWritten = 0;
  if (!WriteFile(impl->hStdInWrite, data, len, &bytesWritten, nullptr))
    return -1;

  return (int)bytesWritten;
}

int Subprocess::write(const String &str) {
  if (str.isEmpty())
    return 0;

  Buffer buf(str);
  return write(buf);
}

} // namespace attoboy
