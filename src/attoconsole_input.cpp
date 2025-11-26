#include "attoconsole_internal.h"

namespace attoboy {

String Console::input(const String &prompt, const ConsoleInput *options) {
  AcquireSRWLockExclusive(&impl->lock);

  if (!prompt.isEmpty()) {
    DWORD written;
    WriteConsoleA(impl->hStdOut, prompt.c_str(), prompt.byteLength(), &written,
                  nullptr);
  }

  DWORD originalMode;
  GetConsoleMode(impl->hStdIn, &originalMode);
  SetConsoleMode(impl->hStdIn, ENABLE_PROCESSED_INPUT | ENABLE_LINE_INPUT |
                                   ENABLE_ECHO_INPUT);

  bool password = options && options->password;
  bool multiline = options && options->multiline;
  List *history = options ? options->history : nullptr;
  const List *completions = options ? options->completions : nullptr;

  String buffer = "";
  int historyIndex = history ? history->length() : 0;
  String savedInput = "";

  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(impl->hStdOut, &csbi);
  int startX = csbi.dwCursorPosition.X;
  int startY = csbi.dwCursorPosition.Y;

  INPUT_RECORD inputRecord;
  DWORD eventsRead;

  SetConsoleMode(impl->hStdIn, 0);

  while (true) {
    ReadConsoleInputA(impl->hStdIn, &inputRecord, 1, &eventsRead);

    if (inputRecord.EventType == KEY_EVENT &&
        inputRecord.Event.KeyEvent.bKeyDown) {
      WORD vk = inputRecord.Event.KeyEvent.wVirtualKeyCode;
      char c = inputRecord.Event.KeyEvent.uChar.AsciiChar;
      bool ctrl = (inputRecord.Event.KeyEvent.dwControlKeyState &
                   (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) != 0;
      bool shift =
          (inputRecord.Event.KeyEvent.dwControlKeyState & SHIFT_PRESSED) != 0;

      if (ctrl && vk == 'C') {
        SetConsoleMode(impl->hStdIn, originalMode);
        ReleaseSRWLockExclusive(&impl->lock);
        return "";
      }

      if (vk == VK_RETURN) {
        if (multiline && (shift || ctrl)) {
          buffer = buffer + "\n";
          WriteConsoleA(impl->hStdOut, "\n", 1, &eventsRead, nullptr);
          if (options && !options->continuation.isEmpty()) {
            const char *cont = options->continuation.c_str();
            WriteConsoleA(impl->hStdOut, cont,
                          options->continuation.byteLength(), &eventsRead,
                          nullptr);
          }
        } else {
          break;
        }
      } else if (vk == VK_BACK) {
        if (buffer.length() > 0) {
          buffer = buffer.substring(0, buffer.length() - 1);
          COORD pos = {(SHORT)startX, (SHORT)startY};
          SetConsoleCursorPosition(impl->hStdOut, pos);
          String spaces = String(" ").repeat(buffer.length() + 1);
          WriteConsoleA(impl->hStdOut, spaces.c_str(), spaces.byteLength(),
                        &eventsRead, nullptr);
          SetConsoleCursorPosition(impl->hStdOut, pos);
          if (password) {
            String masked = String("*").repeat(buffer.length());
            WriteConsoleA(impl->hStdOut, masked.c_str(), masked.byteLength(),
                          &eventsRead, nullptr);
          } else {
            WriteConsoleA(impl->hStdOut, buffer.c_str(), buffer.byteLength(),
                          &eventsRead, nullptr);
          }
        }
      } else if (vk == VK_UP) {
        if (history && history->length() > 0 && historyIndex > 0) {
          if (historyIndex == history->length()) {
            savedInput = buffer;
          }
          historyIndex--;
          buffer = history->at<String>(historyIndex);
          COORD pos = {(SHORT)startX, (SHORT)startY};
          SetConsoleCursorPosition(impl->hStdOut, pos);
          String spaces = String(" ").repeat(savedInput.length() + 10);
          WriteConsoleA(impl->hStdOut, spaces.c_str(), spaces.byteLength(),
                        &eventsRead, nullptr);
          SetConsoleCursorPosition(impl->hStdOut, pos);
          if (password) {
            String masked = String("*").repeat(buffer.length());
            WriteConsoleA(impl->hStdOut, masked.c_str(), masked.byteLength(),
                          &eventsRead, nullptr);
          } else {
            WriteConsoleA(impl->hStdOut, buffer.c_str(), buffer.byteLength(),
                          &eventsRead, nullptr);
          }
        }
      } else if (vk == VK_DOWN) {
        if (history && historyIndex < history->length()) {
          historyIndex++;
          if (historyIndex >= history->length()) {
            buffer = savedInput;
          } else {
            buffer = history->at<String>(historyIndex);
          }
          COORD pos = {(SHORT)startX, (SHORT)startY};
          SetConsoleCursorPosition(impl->hStdOut, pos);
          String spaces = String(" ").repeat(savedInput.length() + 10);
          WriteConsoleA(impl->hStdOut, spaces.c_str(), spaces.byteLength(),
                        &eventsRead, nullptr);
          SetConsoleCursorPosition(impl->hStdOut, pos);
          if (password) {
            String masked = String("*").repeat(buffer.length());
            WriteConsoleA(impl->hStdOut, masked.c_str(), masked.byteLength(),
                          &eventsRead, nullptr);
          } else {
            WriteConsoleA(impl->hStdOut, buffer.c_str(), buffer.byteLength(),
                          &eventsRead, nullptr);
          }
        }
      } else if (vk == VK_TAB) {
        if (completions && completions->length() > 0) {
          for (int i = 0; i < completions->length(); i++) {
            String comp = completions->at<String>(i);
            if (comp.startsWith(buffer) && comp != buffer) {
              buffer = comp;
              COORD pos = {(SHORT)startX, (SHORT)startY};
              SetConsoleCursorPosition(impl->hStdOut, pos);
              String spaces = String(" ").repeat(comp.length() + 10);
              WriteConsoleA(impl->hStdOut, spaces.c_str(), spaces.byteLength(),
                            &eventsRead, nullptr);
              SetConsoleCursorPosition(impl->hStdOut, pos);
              if (password) {
                String masked = String("*").repeat(buffer.length());
                WriteConsoleA(impl->hStdOut, masked.c_str(),
                              masked.byteLength(), &eventsRead, nullptr);
              } else {
                WriteConsoleA(impl->hStdOut, buffer.c_str(),
                              buffer.byteLength(), &eventsRead, nullptr);
              }
              break;
            }
          }
        }
      } else if (c >= 32 && c <= 126) {
        char temp[2] = {c, 0};
        buffer = buffer + String(temp);
        if (password) {
          WriteConsoleA(impl->hStdOut, "*", 1, &eventsRead, nullptr);
        } else {
          WriteConsoleA(impl->hStdOut, &c, 1, &eventsRead, nullptr);
        }
      }
    }
  }

  WriteConsoleA(impl->hStdOut, "\n", 1, &eventsRead, nullptr);

  if (history && !buffer.isEmpty()) {
    history->append(buffer);
  }

  SetConsoleMode(impl->hStdIn, originalMode);
  ReleaseSRWLockExclusive(&impl->lock);

  return buffer;
}

} // namespace attoboy
