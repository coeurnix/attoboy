#pragma once
#include "attoboy/attoboy.h"
#include "atto_internal_common.h"
#include <windows.h>

namespace attoboy {

struct HotkeyCallback {
  void (*callback)(void*);
  void* arg;
  WORD modifiers;
  WORD vk;
  int id;
};

struct ConsoleImpl {
  HANDLE hStdOut;
  HANDLE hStdIn;
  DWORD originalOutputMode;
  DWORD originalInputMode;
  WORD originalAttributes;
  CONSOLE_SCREEN_BUFFER_INFO originalBufferInfo;
  mutable SRWLOCK lock;

  int cachedWidth;
  int cachedHeight;

  bool progressVisible;
  float progressPercent;
  String progressLabel;
  int progressAnimFrame;

  List* hotkeyCallbacks;
  HANDLE hotkeyThread;
  bool hotkeyThreadRunning;
};

void UpdateConsoleSize(ConsoleImpl* impl);
void SetConsoleColor(HANDLE hOut, ConsoleColor fg, ConsoleColor bg);
WORD ConsoleColorToWord(ConsoleColor color);
String GetKeyName(KEY_EVENT_RECORD keyEvent);
int UTF8CharWidth(const char* str);
int UTF8StringWidth(const char* str);
List SplitIntoWords(const String& text);

} // namespace attoboy
