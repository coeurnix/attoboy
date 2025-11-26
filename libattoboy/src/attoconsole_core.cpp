#include "attoconsole_internal.h"

namespace attoboy {

Console::Console() {
  impl = (ConsoleImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                  sizeof(ConsoleImpl));
  InitializeSRWLock(&impl->lock);

  impl->hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
  impl->hStdIn = GetStdHandle(STD_INPUT_HANDLE);

  GetConsoleMode(impl->hStdOut, &impl->originalOutputMode);
  GetConsoleMode(impl->hStdIn, &impl->originalInputMode);

  GetConsoleScreenBufferInfo(impl->hStdOut, &impl->originalBufferInfo);
  impl->originalAttributes = impl->originalBufferInfo.wAttributes;

  DWORD outputMode = impl->originalOutputMode | ENABLE_PROCESSED_OUTPUT |
                     ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  SetConsoleMode(impl->hStdOut, outputMode);

  DWORD inputMode =
      ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS;
  SetConsoleMode(impl->hStdIn, inputMode);

  SetConsoleOutputCP(CP_UTF8);

  UpdateConsoleSize(impl);

  impl->progressVisible = false;
  impl->progressPercent = 0.0f;
  impl->progressAnimFrame = 0;
  impl->hotkeyCallbacks = nullptr;
  impl->hotkeyThread = nullptr;
  impl->hotkeyThreadRunning = false;
}

Console::~Console() {
  if (!impl)
    return;

  if (impl->progressVisible) {
    progressHide();
  }

  AcquireSRWLockExclusive(&impl->lock);

  if (impl->hotkeyThreadRunning) {
    impl->hotkeyThreadRunning = false;
    if (impl->hotkeyThread) {
      WaitForSingleObject(impl->hotkeyThread, 1000);
      CloseHandle(impl->hotkeyThread);
    }
  }

  if (impl->hotkeyCallbacks) {
    delete impl->hotkeyCallbacks;
  }

  SetConsoleMode(impl->hStdOut, impl->originalOutputMode);
  SetConsoleMode(impl->hStdIn, impl->originalInputMode);

  SetConsoleTextAttribute(impl->hStdOut, impl->originalAttributes);

  ReleaseSRWLockExclusive(&impl->lock);

  HeapFree(GetProcessHeap(), 0, impl);
  impl = nullptr;
}

int Console::width() const {
  AcquireSRWLockShared(&impl->lock);
  UpdateConsoleSize(impl);
  int w = impl->cachedWidth;
  ReleaseSRWLockShared(&impl->lock);
  return w;
}

int Console::height() const {
  AcquireSRWLockShared(&impl->lock);
  UpdateConsoleSize(impl);
  int h = impl->cachedHeight;
  ReleaseSRWLockShared(&impl->lock);
  return h;
}

void UpdateConsoleSize(ConsoleImpl *impl) {
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  if (GetConsoleScreenBufferInfo(impl->hStdOut, &csbi)) {
    impl->cachedWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    impl->cachedHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
  } else {
    if (impl->cachedWidth == 0)
      impl->cachedWidth = 80;
    if (impl->cachedHeight == 0)
      impl->cachedHeight = 25;
  }
}

} // namespace attoboy
