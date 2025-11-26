#include "attoconsole_internal.h"

namespace attoboy {

Console &Console::progress(float percent, const String &label) {
  AcquireSRWLockExclusive(&impl->lock);

  impl->progressVisible = true;
  impl->progressPercent = percent;
  impl->progressLabel = label;

  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(impl->hStdOut, &csbi);

  int consWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
  int consHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

  COORD savedPos = csbi.dwCursorPosition;

  COORD barPos = {0, (SHORT)(consHeight - 1)};
  SetConsoleCursorPosition(impl->hStdOut, barPos);

  String bar = "";

  if (percent < 0) {
    const char *spinner[] = {"|", "/", "-", "\\"};
    impl->progressAnimFrame = (impl->progressAnimFrame + 1) % 4;
    bar = String(spinner[impl->progressAnimFrame]) + " " + label;
  } else {
    float clampedPercent = percent;
    if (clampedPercent < 0.0f)
      clampedPercent = 0.0f;
    if (clampedPercent > 1.0f)
      clampedPercent = 1.0f;

    int percentNum = (int)(clampedPercent * 100);
    String percentStr = String(percentNum) + "%";

    int labelLen = UTF8StringWidth(label.c_str());
    int percentLen = UTF8StringWidth(percentStr.c_str());
    int barWidth = consWidth - labelLen - percentLen - 4;

    if (barWidth > 0) {
      int filled = (int)(clampedPercent * barWidth);
      String fillBar = String("=").repeat(filled > 0 ? filled : 0);
      String emptyBar = String(" ").repeat(barWidth - filled);
      bar = String("[") + fillBar + emptyBar + "] " + percentStr;
      if (!label.isEmpty()) {
        bar = bar + " " + label;
      }
    } else {
      bar = percentStr + " " + label;
    }
  }

  int barLen = UTF8StringWidth(bar.c_str());
  if (barLen < consWidth) {
    bar = bar + String(" ").repeat(consWidth - barLen);
  } else if (barLen > consWidth) {
    bar = bar.substring(0, consWidth);
  }

  DWORD written;
  WriteConsoleA(impl->hStdOut, bar.c_str(), bar.byteLength(), &written,
                nullptr);

  SetConsoleCursorPosition(impl->hStdOut, savedPos);

  ReleaseSRWLockExclusive(&impl->lock);
  return *this;
}

Console &Console::progressHide() {
  AcquireSRWLockExclusive(&impl->lock);

  if (!impl->progressVisible) {
    ReleaseSRWLockExclusive(&impl->lock);
    return *this;
  }

  impl->progressVisible = false;

  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(impl->hStdOut, &csbi);

  int consWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
  int consHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

  COORD savedPos = csbi.dwCursorPosition;

  COORD barPos = {0, (SHORT)(consHeight - 1)};
  SetConsoleCursorPosition(impl->hStdOut, barPos);

  String spaces = String(" ").repeat(consWidth);
  DWORD written;
  WriteConsoleA(impl->hStdOut, spaces.c_str(), spaces.byteLength(), &written,
                nullptr);

  SetConsoleCursorPosition(impl->hStdOut, savedPos);

  ReleaseSRWLockExclusive(&impl->lock);
  return *this;
}

} // namespace attoboy
