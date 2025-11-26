#include "attoconsole_internal.h"

namespace attoboy {

Console &Console::setCursor(int x, int y) {
  AcquireSRWLockExclusive(&impl->lock);

  COORD pos = {(SHORT)x, (SHORT)y};
  SetConsoleCursorPosition(impl->hStdOut, pos);

  ReleaseSRWLockExclusive(&impl->lock);
  return *this;
}

Console &Console::showCursor() {
  AcquireSRWLockExclusive(&impl->lock);

  CONSOLE_CURSOR_INFO cursorInfo;
  GetConsoleCursorInfo(impl->hStdOut, &cursorInfo);
  cursorInfo.bVisible = TRUE;
  SetConsoleCursorInfo(impl->hStdOut, &cursorInfo);

  ReleaseSRWLockExclusive(&impl->lock);
  return *this;
}

Console &Console::hideCursor() {
  AcquireSRWLockExclusive(&impl->lock);

  CONSOLE_CURSOR_INFO cursorInfo;
  GetConsoleCursorInfo(impl->hStdOut, &cursorInfo);
  cursorInfo.bVisible = FALSE;
  SetConsoleCursorInfo(impl->hStdOut, &cursorInfo);

  ReleaseSRWLockExclusive(&impl->lock);
  return *this;
}

} // namespace attoboy
