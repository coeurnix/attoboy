#include "attoboy/attoboy.h"

using namespace attoboy;

void atto_main() {
  Thread t(nullptr, nullptr);
  Sleep(20);
  bool running = t.isRunning();

  if (!running) {
    Exit(0);
  } else {
    Exit(1);
  }
}
