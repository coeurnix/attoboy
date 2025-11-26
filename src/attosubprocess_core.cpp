#include "attosubprocess_internal.h"
#include "attopath_internal.h"
#include "attostring_internal.h"
#include "attolist_internal.h"

namespace attoboy {

static bool NeedsQuoting(const String &str) {
  const char *s = str.c_str();
  if (!s || *s == 0)
    return true;

  while (*s) {
    if (*s == ' ' || *s == '\t')
      return true;
    s++;
  }
  return false;
}

static String BuildCommandLine(const char *exePath, const List &arguments) {
  String cmdLine = String("\"") + String(exePath) + String("\"");

  for (int i = 0; i < arguments.length(); i++) {
    String arg = arguments.at<String>(i);
    if (!arg.isEmpty()) {
      cmdLine = cmdLine + String(" ");
      if (NeedsQuoting(arg)) {
        cmdLine = cmdLine + String("\"") + arg + String("\"");
      } else {
        cmdLine = cmdLine + arg;
      }
    }
  }

  return cmdLine;
}

SubprocessImpl *CreateSubprocessImpl(const Path &executable,
                                      const List &arguments,
                                      bool captureOutput, bool streaming) {
  SubprocessImpl *impl = (SubprocessImpl *)HeapAlloc(
      GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(SubprocessImpl));
  if (!impl)
    return nullptr;

  new (impl) SubprocessImpl();
  impl->streaming = streaming;

  HANDLE hStdInRead = nullptr;
  HANDLE hStdOutWrite = nullptr;
  HANDLE hStdErrWrite = nullptr;

  SECURITY_ATTRIBUTES sa;
  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.bInheritHandle = TRUE;
  sa.lpSecurityDescriptor = nullptr;

  if (captureOutput || streaming) {
    if (!CreatePipe(&hStdInRead, &impl->hStdInWrite, &sa, 0)) {
      HeapFree(GetProcessHeap(), 0, impl);
      return nullptr;
    }
    SetHandleInformation(impl->hStdInWrite, HANDLE_FLAG_INHERIT, 0);

    if (!CreatePipe(&impl->hStdOutRead, &hStdOutWrite, &sa, 0)) {
      CloseHandle(hStdInRead);
      CloseHandle(impl->hStdInWrite);
      HeapFree(GetProcessHeap(), 0, impl);
      return nullptr;
    }
    SetHandleInformation(impl->hStdOutRead, HANDLE_FLAG_INHERIT, 0);

    hStdErrWrite = hStdOutWrite;
  }

  STARTUPINFOW si;
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);

  if (captureOutput || streaming) {
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = hStdInRead;
    si.hStdOutput = hStdOutWrite;
    si.hStdError = hStdErrWrite;
  }

  PROCESS_INFORMATION pi;
  ZeroMemory(&pi, sizeof(pi));

  const char *exePath = GetPathString(executable.impl);
  String cmdLine = BuildCommandLine(exePath, arguments);
  const char* cmdLineCStr = cmdLine.c_str();

  WCHAR* cmdLineWide = Utf8ToWide(cmdLineCStr);
  if (!cmdLineWide) {
    if (hStdInRead)
      CloseHandle(hStdInRead);
    if (impl->hStdInWrite)
      CloseHandle(impl->hStdInWrite);
    if (impl->hStdOutRead)
      CloseHandle(impl->hStdOutRead);
    if (hStdOutWrite)
      CloseHandle(hStdOutWrite);
    HeapFree(GetProcessHeap(), 0, impl);
    return nullptr;
  }

  BOOL result = CreateProcessW(nullptr, cmdLineWide, nullptr, nullptr, TRUE, 0,
                                nullptr, nullptr, &si, &pi);

  FreeConvertedString(cmdLineWide);

  if (hStdInRead)
    CloseHandle(hStdInRead);
  if (hStdOutWrite)
    CloseHandle(hStdOutWrite);

  if (!result) {
    if (impl->hStdInWrite)
      CloseHandle(impl->hStdInWrite);
    if (impl->hStdOutRead)
      CloseHandle(impl->hStdOutRead);
    HeapFree(GetProcessHeap(), 0, impl);
    return nullptr;
  }

  impl->hProcess = pi.hProcess;
  impl->hThread = pi.hThread;
  impl->processId = pi.dwProcessId;
  impl->valid = true;

  return impl;
}

Subprocess::Subprocess(const Path &executable, const List &arguments) {
  impl = CreateSubprocessImpl(executable, arguments, true, true);
}

Subprocess::Subprocess(const Subprocess &other) {
  impl = other.impl;
  if (impl) {
    WriteLockGuard guard(&impl->lock);
    impl->refCount++;
  }
}

Subprocess::~Subprocess() {
  if (!impl)
    return;

  bool shouldDelete = false;
  {
    WriteLockGuard guard(&impl->lock);
    impl->refCount--;
    shouldDelete = (impl->refCount == 0);
  }

  if (shouldDelete) {
    if (impl->hStdInWrite)
      CloseHandle(impl->hStdInWrite);
    if (impl->hStdOutRead)
      CloseHandle(impl->hStdOutRead);
    if (impl->hThread)
      CloseHandle(impl->hThread);
    if (impl->hProcess)
      CloseHandle(impl->hProcess);
    HeapFree(GetProcessHeap(), 0, impl);
  }
}

Subprocess &Subprocess::operator=(const Subprocess &other) {
  if (this == &other)
    return *this;

  this->~Subprocess();

  impl = other.impl;
  if (impl) {
    WriteLockGuard guard(&impl->lock);
    impl->refCount++;
  }

  return *this;
}

bool Subprocess::isValid() const {
  if (!impl)
    return false;
  ReadLockGuard guard(&impl->lock);
  return impl->valid;
}

bool Subprocess::isRunning() const {
  if (!impl || !impl->valid)
    return false;

  ReadLockGuard guard(&impl->lock);
  if (!impl->hProcess)
    return false;

  DWORD exitCode;
  if (!GetExitCodeProcess(impl->hProcess, &exitCode))
    return false;

  return exitCode == STILL_ACTIVE;
}

int Subprocess::wait() {
  if (!impl || !impl->valid)
    return -1;

  ReadLockGuard guard(&impl->lock);
  if (!impl->hProcess)
    return -1;

  WaitForSingleObject(impl->hProcess, INFINITE);

  DWORD exitCode;
  if (!GetExitCodeProcess(impl->hProcess, &exitCode))
    return -1;

  return (int)exitCode;
}

bool Subprocess::terminate() {
  if (!impl || !impl->valid)
    return false;

  ReadLockGuard guard(&impl->lock);
  if (!impl->hProcess)
    return false;

  return TerminateProcess(impl->hProcess, 1) != 0;
}

int Subprocess::getProcessId() const {
  if (!impl || !impl->valid)
    return -1;

  ReadLockGuard guard(&impl->lock);
  return (int)impl->processId;
}

} // namespace attoboy
