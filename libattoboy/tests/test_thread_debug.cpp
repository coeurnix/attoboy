#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;
static volatile int counter = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    errorCount++;                                                              \
  }

void *simpleFunc(void *arg) {
  counter++;
  return (void *)42;
}

void *sleepFunc(void *arg) {
  Sleep(50);
  counter++;
  return nullptr;
}

void atto_main() {
  // Test 1: Basic creation and await
  {
    counter = 0;
    Thread t(simpleFunc, nullptr);
    void *result = t.await();
    TEST(result == (void *)42, ATTO_TEXT("T1: Return value"));
    TEST(counter == 1, ATTO_TEXT("T1: Counter"));
  }

  // Test 2: isRunning
  {
    counter = 0;
    Thread t(sleepFunc, nullptr);
    TEST(t.isRunning(), ATTO_TEXT("T2: isRunning initially"));
    t.await();
    TEST(!t.isRunning(), ATTO_TEXT("T2: not running after await"));
  }

  // Test 3: Sleep function
  {
    DateTime before;
    Sleep(100);
    DateTime after;
    long long diff = after.diff(before);
    TEST(diff >= 90 && diff <= 200, ATTO_TEXT("T3: Sleep duration"));
  }

  Exit(errorCount);
}
