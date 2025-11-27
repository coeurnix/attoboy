#include "attoconsole_internal.h"

namespace attoboy {

String GetKeyName(KEY_EVENT_RECORD keyEvent) {
  bool ctrl = (keyEvent.dwControlKeyState &
               (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) != 0;
  bool alt = (keyEvent.dwControlKeyState &
              (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED)) != 0;
  bool shift = (keyEvent.dwControlKeyState & SHIFT_PRESSED) != 0;

  String modifiers = "";
  if (ctrl)
    modifiers = modifiers + "Ctrl+";
  if (alt)
    modifiers = modifiers + "Alt+";
  if (shift &&
      (keyEvent.wVirtualKeyCode < 'A' || keyEvent.wVirtualKeyCode > 'Z'))
    modifiers = modifiers + "Shift+";

  WORD vk = keyEvent.wVirtualKeyCode;
  char c = keyEvent.uChar.AsciiChar;

  if (vk >= 0x70 && vk <= 0x87) {
    return modifiers + "F" + String((int)(vk - 0x70 + 1));
  }

  switch (vk) {
  case VK_RETURN:
    return modifiers + "Enter";
  case VK_ESCAPE:
    return modifiers + "Escape";
  case VK_BACK:
    return modifiers + "Backspace";
  case VK_TAB:
    return modifiers + "Tab";
  case VK_SPACE:
    return modifiers + "Space";
  case VK_DELETE:
    return modifiers + "Delete";
  case VK_INSERT:
    return modifiers + "Insert";
  case VK_HOME:
    return modifiers + "Home";
  case VK_END:
    return modifiers + "End";
  case VK_PRIOR:
    return modifiers + "PageUp";
  case VK_NEXT:
    return modifiers + "PageDown";
  case VK_UP:
    return modifiers + "Up";
  case VK_DOWN:
    return modifiers + "Down";
  case VK_LEFT:
    return modifiers + "Left";
  case VK_RIGHT:
    return modifiers + "Right";
  default:
    if (c >= 32 && c <= 126) {
      return modifiers + String(c);
    }
    return modifiers + "Unknown";
  }
}

String Console::readEvent() {
  AcquireSRWLockExclusive(&impl->lock);

  DWORD originalMode;
  GetConsoleMode(impl->hStdIn, &originalMode);

  DWORD readMode =
      ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS;
  SetConsoleMode(impl->hStdIn, readMode);

  INPUT_RECORD inputRecord;
  DWORD eventsRead;

  while (true) {
    ReadConsoleInputA(impl->hStdIn, &inputRecord, 1, &eventsRead);

    if (inputRecord.EventType == KEY_EVENT &&
        inputRecord.Event.KeyEvent.bKeyDown) {
      String keyName = GetKeyName(inputRecord.Event.KeyEvent);

      SetConsoleMode(impl->hStdIn, originalMode);
      ReleaseSRWLockExclusive(&impl->lock);

      return keyName;
    } else if (inputRecord.EventType == MOUSE_EVENT) {
      MOUSE_EVENT_RECORD mouseEvent = inputRecord.Event.MouseEvent;

      if (mouseEvent.dwEventFlags == 0 &&
          (mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED)) {
        int x = mouseEvent.dwMousePosition.X;
        int y = mouseEvent.dwMousePosition.Y;

        bool ctrl = (mouseEvent.dwControlKeyState &
                     (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) != 0;
        String result;
        if (ctrl) {
          result = String("MouseRight@") + String(x) + String(",") + String(y);
        } else {
          result = String("MouseLeft@") + String(x) + String(",") + String(y);
        }

        SetConsoleMode(impl->hStdIn, originalMode);
        ReleaseSRWLockExclusive(&impl->lock);
        return result;
      } else if (mouseEvent.dwEventFlags == MOUSE_MOVED) {
        int x = mouseEvent.dwMousePosition.X;
        int y = mouseEvent.dwMousePosition.Y;
        String result =
            String("MouseMove@") + String(x) + String(",") + String(y);

        SetConsoleMode(impl->hStdIn, originalMode);
        ReleaseSRWLockExclusive(&impl->lock);
        return result;
      }
    }
  }

  SetConsoleMode(impl->hStdIn, originalMode);
  ReleaseSRWLockExclusive(&impl->lock);

  return "";
}

} // namespace attoboy
