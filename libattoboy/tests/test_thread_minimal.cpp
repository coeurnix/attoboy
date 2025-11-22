#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

void *simpleFunc(void *arg) {
  return (void *)42;
}

void atto_main() {
  Log(ATTO_TEXT("Starting minimal thread test..."));

  Log(ATTO_TEXT("Creating thread..."));
  Thread t(simpleFunc, nullptr);

  Log(ATTO_TEXT("Awaiting thread..."));
  void *result = t.await();

  Log(ATTO_TEXT("Thread completed, result: "), (int)(long long)result);

  if (result == (void *)42) {
    Log(ATTO_TEXT("Test passed!"));
    Exit(0);
  } else {
    Log(ATTO_TEXT("Test failed!"));
    Exit(1);
  }
}
