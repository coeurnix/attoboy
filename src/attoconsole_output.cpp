#include "attoconsole_internal.h"

namespace attoboy {

WORD ConsoleColorToWord(ConsoleColor color) {
  switch (color) {
  case CON_BLACK:
    return 0;
  case CON_DARK_BLUE:
    return FOREGROUND_BLUE;
  case CON_DARK_GREEN:
    return FOREGROUND_GREEN;
  case CON_DARK_CYAN:
    return FOREGROUND_GREEN | FOREGROUND_BLUE;
  case CON_DARK_RED:
    return FOREGROUND_RED;
  case CON_DARK_MAGENTA:
    return FOREGROUND_RED | FOREGROUND_BLUE;
  case CON_DARK_YELLOW:
    return FOREGROUND_RED | FOREGROUND_GREEN;
  case CON_GRAY:
    return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
  case CON_DARK_GRAY:
    return FOREGROUND_INTENSITY;
  case CON_BLUE:
    return FOREGROUND_BLUE | FOREGROUND_INTENSITY;
  case CON_GREEN:
    return FOREGROUND_GREEN | FOREGROUND_INTENSITY;
  case CON_CYAN:
    return FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
  case CON_RED:
    return FOREGROUND_RED | FOREGROUND_INTENSITY;
  case CON_MAGENTA:
    return FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
  case CON_YELLOW:
    return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
  case CON_WHITE:
    return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE |
           FOREGROUND_INTENSITY;
  default:
    return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
  }
}

void SetConsoleColor(HANDLE hOut, ConsoleColor fg, ConsoleColor bg) {
  WORD fgWord = ConsoleColorToWord(fg);
  WORD bgWord = ConsoleColorToWord(bg) << 4;
  SetConsoleTextAttribute(hOut, fgWord | bgWord);
}

Console &Console::print(const String &text, ConsoleColor fg, ConsoleColor bg) {
  AcquireSRWLockExclusive(&impl->lock);

  SetConsoleColor(impl->hStdOut, fg, bg);

  DWORD written;
  WriteConsoleA(impl->hStdOut, text.c_str(), text.byteLength(), &written,
                nullptr);

  ReleaseSRWLockExclusive(&impl->lock);
  return *this;
}

Console &Console::println(const String &text, ConsoleColor fg,
                          ConsoleColor bg) {
  print(text, fg, bg);
  print("\n", fg, bg);
  return *this;
}

Console &Console::clear() {
  AcquireSRWLockExclusive(&impl->lock);

  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(impl->hStdOut, &csbi);

  COORD topLeft = {0, 0};
  DWORD cellCount = csbi.dwSize.X * csbi.dwSize.Y;
  DWORD written;

  FillConsoleOutputCharacterA(impl->hStdOut, ' ', cellCount, topLeft, &written);
  FillConsoleOutputAttribute(impl->hStdOut, impl->originalAttributes, cellCount,
                             topLeft, &written);

  SetConsoleCursorPosition(impl->hStdOut, topLeft);

  ReleaseSRWLockExclusive(&impl->lock);
  return *this;
}

} // namespace attoboy
