#include "attopath_internal.h"
#include "attostring_internal.h"
#include "attobuffer_internal.h"

namespace attoboy {

String Path::readToString() const {
  if (!impl || !impl->pathStr)
    return String();

  ReadLockGuard guard(&impl->lock);

  HANDLE hFile = CreateFile(impl->pathStr, GENERIC_READ, FILE_SHARE_READ, nullptr,
                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (hFile == INVALID_HANDLE_VALUE)
    return String();

  DWORD fileSize = GetFileSize(hFile, nullptr);
  if (fileSize == INVALID_FILE_SIZE) {
    CloseHandle(hFile);
    return String();
  }

  if (fileSize == 0) {
    CloseHandle(hFile);
    return String();
  }

  unsigned char *rawBuffer = (unsigned char *)HeapAlloc(GetProcessHeap(), 0, fileSize + sizeof(ATTO_CHAR));
  if (!rawBuffer) {
    CloseHandle(hFile);
    return String();
  }

  DWORD bytesRead = 0;
  bool success = ReadFile(hFile, rawBuffer, fileSize, &bytesRead, nullptr);
  CloseHandle(hFile);

  if (!success || bytesRead == 0) {
    HeapFree(GetProcessHeap(), 0, rawBuffer);
    return String();
  }

  for (int i = 0; i < (int)sizeof(ATTO_CHAR); i++) {
    rawBuffer[bytesRead + i] = 0;
  }

  String result((const ATTO_CHAR *)rawBuffer);
  HeapFree(GetProcessHeap(), 0, rawBuffer);
  return result;
}

Buffer Path::readToBuffer() const {
  if (!impl || !impl->pathStr)
    return Buffer();

  ReadLockGuard guard(&impl->lock);

  HANDLE hFile = CreateFile(impl->pathStr, GENERIC_READ, FILE_SHARE_READ, nullptr,
                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (hFile == INVALID_HANDLE_VALUE)
    return Buffer();

  DWORD fileSize = GetFileSize(hFile, nullptr);
  if (fileSize == INVALID_FILE_SIZE || fileSize == 0) {
    CloseHandle(hFile);
    return Buffer();
  }

  unsigned char *buffer =
      (unsigned char *)HeapAlloc(GetProcessHeap(), 0, fileSize);
  if (!buffer) {
    CloseHandle(hFile);
    return Buffer();
  }

  DWORD bytesRead = 0;
  bool success = ReadFile(hFile, buffer, fileSize, &bytesRead, nullptr);
  CloseHandle(hFile);

  if (!success || bytesRead == 0) {
    HeapFree(GetProcessHeap(), 0, buffer);
    return Buffer();
  }

  Buffer result(buffer, bytesRead);
  HeapFree(GetProcessHeap(), 0, buffer);
  return result;
}

bool Path::writeFromString(const String &str) const {
  if (!impl || !impl->pathStr)
    return false;

  ReadLockGuard guard(&impl->lock);

  HANDLE hFile = CreateFile(impl->pathStr, GENERIC_WRITE, 0, nullptr,
                            CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (hFile == INVALID_HANDLE_VALUE)
    return false;

  const ATTO_CHAR *data = str.c_str();
  int len = str.length();
  DWORD bytesToWrite = len * sizeof(ATTO_CHAR);
  DWORD bytesWritten = 0;

  bool success = WriteFile(hFile, data, bytesToWrite, &bytesWritten, nullptr);
  CloseHandle(hFile);

  return success && (bytesWritten == bytesToWrite);
}

bool Path::writeFromBuffer(const Buffer &buf) const {
  if (!impl || !impl->pathStr)
    return false;

  ReadLockGuard guard(&impl->lock);

  HANDLE hFile = CreateFile(impl->pathStr, GENERIC_WRITE, 0, nullptr,
                            CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (hFile == INVALID_HANDLE_VALUE)
    return false;

  int len = 0;
  const unsigned char *data = buf.c_ptr(&len);
  if (!data || len == 0) {
    CloseHandle(hFile);
    return true;
  }

  DWORD bytesWritten = 0;
  bool success = WriteFile(hFile, data, len, &bytesWritten, nullptr);
  CloseHandle(hFile);

  return success && (bytesWritten == (DWORD)len);
}

bool Path::appendFromString(const String &str) const {
  if (!impl || !impl->pathStr)
    return false;

  ReadLockGuard guard(&impl->lock);

  HANDLE hFile = CreateFile(impl->pathStr, FILE_APPEND_DATA, 0, nullptr,
                            OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (hFile == INVALID_HANDLE_VALUE)
    return false;

  const ATTO_CHAR *data = str.c_str();
  int len = str.length();
  DWORD bytesToWrite = len * sizeof(ATTO_CHAR);
  DWORD bytesWritten = 0;

  bool success = WriteFile(hFile, data, bytesToWrite, &bytesWritten, nullptr);
  CloseHandle(hFile);

  return success && (bytesWritten == bytesToWrite);
}

bool Path::appendFromBuffer(const Buffer &buf) const {
  if (!impl || !impl->pathStr)
    return false;

  ReadLockGuard guard(&impl->lock);

  HANDLE hFile = CreateFile(impl->pathStr, FILE_APPEND_DATA, 0, nullptr,
                            OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (hFile == INVALID_HANDLE_VALUE)
    return false;

  int len = 0;
  const unsigned char *data = buf.c_ptr(&len);
  if (!data || len == 0) {
    CloseHandle(hFile);
    return true;
  }

  DWORD bytesWritten = 0;
  bool success = WriteFile(hFile, data, len, &bytesWritten, nullptr);
  CloseHandle(hFile);

  return success && (bytesWritten == (DWORD)len);
}

} // namespace attoboy
