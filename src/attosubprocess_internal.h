#pragma once
#include "atto_internal_common.h"
#include "attoboy/attoboy.h"
#include <windows.h>

namespace attoboy {

struct SubprocessImpl {
  HANDLE hProcess;
  HANDLE hThread;
  HANDLE hStdInWrite;
  HANDLE hStdOutRead;
  DWORD processId;
  int refCount;
  SRWLOCK lock;
  bool valid;
  bool streaming;

  SubprocessImpl()
      : hProcess(nullptr), hThread(nullptr), hStdInWrite(nullptr),
        hStdOutRead(nullptr), processId(0), refCount(1), valid(false),
        streaming(false) {
    InitializeSRWLock(&lock);
  }
};

SubprocessImpl *CreateSubprocessImpl(const Path &executable,
                                      const List &arguments,
                                      bool captureOutput, bool streaming);

} // namespace attoboy
