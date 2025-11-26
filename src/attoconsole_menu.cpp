#include "attoconsole_internal.h"

namespace attoboy {

int Console::menu(const List &options, const String &title) {
  if (options.isEmpty()) {
    return -1;
  }

  AcquireSRWLockExclusive(&impl->lock);

  DWORD originalMode;
  GetConsoleMode(impl->hStdIn, &originalMode);
  SetConsoleMode(impl->hStdIn, ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT);

  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(impl->hStdOut, &csbi);
  COORD startPos = csbi.dwCursorPosition;

  if (!title.isEmpty()) {
    DWORD written;
    String titleLine = title + "\n";
    WriteConsoleA(impl->hStdOut, titleLine.c_str(), titleLine.byteLength(),
                  &written, nullptr);
  }

  int selectedIndex = 0;

  auto redrawMenu = [&]() {
    CONSOLE_SCREEN_BUFFER_INFO currentCsbi;
    GetConsoleScreenBufferInfo(impl->hStdOut, &currentCsbi);

    COORD pos = startPos;
    if (!title.isEmpty()) {
      pos.Y++;
    }
    SetConsoleCursorPosition(impl->hStdOut, pos);

    for (int i = 0; i < options.length(); i++) {
      String option = options.at<String>(i);
      String prefix = (i == selectedIndex) ? "> " : "  ";
      String line = "";
      if (i < 9) {
        line = String(i + 1) + ". " + prefix + option;
      } else {
        line = prefix + option;
      }

      WORD color = (i == selectedIndex) ? (ConsoleColorToWord(CON_BLACK) << 4) |
                                              ConsoleColorToWord(CON_WHITE)
                                        : impl->originalAttributes;
      SetConsoleTextAttribute(impl->hStdOut, color);

      DWORD written;
      WriteConsoleA(impl->hStdOut, line.c_str(), line.byteLength(), &written,
                    nullptr);

      int consWidth = currentCsbi.srWindow.Right - currentCsbi.srWindow.Left + 1;
      int lineLen = UTF8StringWidth(line.c_str());
      if (lineLen < consWidth) {
        String spaces = String(" ").repeat(consWidth - lineLen);
        WriteConsoleA(impl->hStdOut, spaces.c_str(), spaces.byteLength(),
                      &written, nullptr);
      }

      if (i < options.length() - 1) {
        WriteConsoleA(impl->hStdOut, "\n", 1, &written, nullptr);
      }
    }

    SetConsoleTextAttribute(impl->hStdOut, impl->originalAttributes);
  };

  redrawMenu();

  INPUT_RECORD inputRecord;
  DWORD eventsRead;
  int result = -1;

  while (true) {
    ReadConsoleInputA(impl->hStdIn, &inputRecord, 1, &eventsRead);

    if (inputRecord.EventType == KEY_EVENT &&
        inputRecord.Event.KeyEvent.bKeyDown) {
      WORD vk = inputRecord.Event.KeyEvent.wVirtualKeyCode;
      char c = inputRecord.Event.KeyEvent.uChar.AsciiChar;

      if (vk == VK_ESCAPE) {
        result = -1;
        break;
      } else if (vk == VK_RETURN) {
        result = selectedIndex;
        break;
      } else if (vk == VK_UP) {
        selectedIndex =
            (selectedIndex - 1 + options.length()) % options.length();
        redrawMenu();
      } else if (vk == VK_DOWN) {
        selectedIndex = (selectedIndex + 1) % options.length();
        redrawMenu();
      } else if (c >= '1' && c <= '9') {
        int num = c - '1';
        if (num < options.length()) {
          result = num;
          break;
        }
      } else if (c >= 'a' && c <= 'z') {
        for (int i = 0; i < options.length(); i++) {
          String option = options.at<String>(i);
          if (!option.isEmpty() && (option.c_str()[0] == c ||
                                    option.c_str()[0] == (c - 'a' + 'A'))) {
            selectedIndex = i;
            redrawMenu();
            break;
          }
        }
      } else if (c >= 'A' && c <= 'Z') {
        for (int i = 0; i < options.length(); i++) {
          String option = options.at<String>(i);
          if (!option.isEmpty() && option.c_str()[0] == c) {
            selectedIndex = i;
            redrawMenu();
            break;
          }
        }
      }
    } else if (inputRecord.EventType == MOUSE_EVENT) {
      MOUSE_EVENT_RECORD mouseEvent = inputRecord.Event.MouseEvent;
      if (mouseEvent.dwEventFlags == 0 &&
          mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
        int clickY = mouseEvent.dwMousePosition.Y;
        int menuStartY = startPos.Y;
        if (!title.isEmpty()) {
          menuStartY++;
        }
        int index = clickY - menuStartY;
        if (index >= 0 && index < options.length()) {
          result = index;
          break;
        }
      } else if (mouseEvent.dwEventFlags == MOUSE_MOVED) {
        int hoverY = mouseEvent.dwMousePosition.Y;
        int menuStartY = startPos.Y;
        if (!title.isEmpty()) {
          menuStartY++;
        }
        int index = hoverY - menuStartY;
        if (index >= 0 && index < options.length() && index != selectedIndex) {
          selectedIndex = index;
          redrawMenu();
        }
      }
    }
  }

  COORD endPos = startPos;
  endPos.Y += options.length();
  if (!title.isEmpty()) {
    endPos.Y++;
  }
  SetConsoleCursorPosition(impl->hStdOut, endPos);
  DWORD written;
  WriteConsoleA(impl->hStdOut, "\n", 1, &written, nullptr);

  SetConsoleMode(impl->hStdIn, originalMode);
  ReleaseSRWLockExclusive(&impl->lock);

  return result;
}

} // namespace attoboy
