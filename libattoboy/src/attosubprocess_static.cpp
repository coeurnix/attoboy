#include "attosubprocess_internal.h"
#include "attopath_internal.h"
#include "attostring_internal.h"
#include "attolist_internal.h"
#include "attobuffer_internal.h"

namespace attoboy {

void Subprocess::Start_impl(const Path &executable, const List &arguments) {
  SubprocessImpl *impl =
      CreateSubprocessImpl(executable, arguments, false, false);
  if (impl) {
    if (impl->hThread)
      CloseHandle(impl->hThread);
    if (impl->hProcess)
      CloseHandle(impl->hProcess);
    HeapFree(GetProcessHeap(), 0, impl);
  }
}

int Subprocess::Run_impl(const Path &executable, const List &arguments) {
  SubprocessImpl *impl =
      CreateSubprocessImpl(executable, arguments, false, false);
  if (!impl)
    return -1;

  WaitForSingleObject(impl->hProcess, INFINITE);

  DWORD exitCode;
  if (!GetExitCodeProcess(impl->hProcess, &exitCode))
    exitCode = -1;

  if (impl->hThread)
    CloseHandle(impl->hThread);
  if (impl->hProcess)
    CloseHandle(impl->hProcess);
  HeapFree(GetProcessHeap(), 0, impl);

  return (int)exitCode;
}

String Subprocess::Capture_impl(const Path &executable, const List &arguments) {
  SubprocessImpl *impl =
      CreateSubprocessImpl(executable, arguments, true, false);
  if (!impl)
    return String();

  Buffer outputBuf;

  const int BUFFER_SIZE = 4096;
  unsigned char *readBuf = (unsigned char *)HeapAlloc(GetProcessHeap(), 0, BUFFER_SIZE);
  if (!readBuf) {
    if (impl->hStdInWrite)
      CloseHandle(impl->hStdInWrite);
    if (impl->hStdOutRead)
      CloseHandle(impl->hStdOutRead);
    if (impl->hThread)
      CloseHandle(impl->hThread);
    if (impl->hProcess)
      CloseHandle(impl->hProcess);
    HeapFree(GetProcessHeap(), 0, impl);
    return String();
  }

  while (true) {
    DWORD bytesAvailable = 0;
    if (!PeekNamedPipe(impl->hStdOutRead, nullptr, 0, nullptr, &bytesAvailable,
                       nullptr)) {
      break;
    }

    if (bytesAvailable > 0) {
      DWORD bytesRead = 0;
      DWORD toRead = bytesAvailable > BUFFER_SIZE ? BUFFER_SIZE : bytesAvailable;
      if (ReadFile(impl->hStdOutRead, readBuf, toRead, &bytesRead, nullptr) &&
          bytesRead > 0) {
        outputBuf.append(readBuf, bytesRead);
      }
    }

    DWORD exitCode;
    if (GetExitCodeProcess(impl->hProcess, &exitCode) &&
        exitCode != STILL_ACTIVE) {
      break;
    }

    Sleep(10);
  }

  WaitForSingleObject(impl->hProcess, INFINITE);

#ifdef UNICODE
  // Convert ANSI subprocess output to wide characters
  int bufLen = 0;
  const unsigned char *bufData = outputBuf.c_ptr(&bufLen);

  if (bufLen == 0) {
    HeapFree(GetProcessHeap(), 0, readBuf);
    if (impl->hStdInWrite)
      CloseHandle(impl->hStdInWrite);
    if (impl->hStdOutRead)
      CloseHandle(impl->hStdOutRead);
    if (impl->hThread)
      CloseHandle(impl->hThread);
    if (impl->hProcess)
      CloseHandle(impl->hProcess);
    HeapFree(GetProcessHeap(), 0, impl);
    return String();
  }

  int wideLen = MultiByteToWideChar(CP_ACP, 0, (const char *)bufData, bufLen, nullptr, 0);
  if (wideLen == 0) {
    HeapFree(GetProcessHeap(), 0, readBuf);
    if (impl->hStdInWrite)
      CloseHandle(impl->hStdInWrite);
    if (impl->hStdOutRead)
      CloseHandle(impl->hStdOutRead);
    if (impl->hThread)
      CloseHandle(impl->hThread);
    if (impl->hProcess)
      CloseHandle(impl->hProcess);
    HeapFree(GetProcessHeap(), 0, impl);
    return String();
  }

  wchar_t *wideBuf = (wchar_t *)HeapAlloc(GetProcessHeap(), 0, (wideLen + 1) * sizeof(wchar_t));
  if (!wideBuf) {
    HeapFree(GetProcessHeap(), 0, readBuf);
    if (impl->hStdInWrite)
      CloseHandle(impl->hStdInWrite);
    if (impl->hStdOutRead)
      CloseHandle(impl->hStdOutRead);
    if (impl->hThread)
      CloseHandle(impl->hThread);
    if (impl->hProcess)
      CloseHandle(impl->hProcess);
    HeapFree(GetProcessHeap(), 0, impl);
    return String();
  }

  MultiByteToWideChar(CP_ACP, 0, (const char *)bufData, bufLen, wideBuf, wideLen);
  wideBuf[wideLen] = 0;

  String result(wideBuf);
  HeapFree(GetProcessHeap(), 0, wideBuf);
#else
  String result = outputBuf.toString();
#endif

  HeapFree(GetProcessHeap(), 0, readBuf);

  if (impl->hStdInWrite)
    CloseHandle(impl->hStdInWrite);
  if (impl->hStdOutRead)
    CloseHandle(impl->hStdOutRead);
  if (impl->hThread)
    CloseHandle(impl->hThread);
  if (impl->hProcess)
    CloseHandle(impl->hProcess);
  HeapFree(GetProcessHeap(), 0, impl);

  return result;
}

} // namespace attoboy
