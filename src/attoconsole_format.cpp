#include "attoconsole_internal.h"

namespace attoboy {

Console &Console::printAligned(const String &text, int width,
                               ConsoleAlign align, ConsoleColor fg,
                               ConsoleColor bg) {
  int w = width;
  if (w == -1) {
    w = this->width();
  }

  String aligned = Console::Align(text, w, align);
  return print(aligned, fg, bg);
}

Console &Console::printWrapped(const String &text, int width, ConsoleColor fg,
                               ConsoleColor bg) {
  int w = width;
  if (w == -1) {
    w = this->width();
  }

  String wrapped = Console::Wrap(text, w);
  return print(wrapped, fg, bg);
}

} // namespace attoboy
