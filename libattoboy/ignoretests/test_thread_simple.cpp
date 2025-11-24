#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError(ATTO_TEXT("Test failed: "), message);                             \
    errorCount++;                                                              \
  }

void atto_main() {
  Log(ATTO_TEXT("Test 1: Sleep function"));
  Sleep(100);
  Log(ATTO_TEXT("Sleep completed successfully"));

  Log(ATTO_TEXT("Test 2: Thread equals and comparison operators"));
  int val = 0;
  Thread t1([](void* arg) -> void* { return arg; }, &val);
  Thread t2 = t1;
  Thread t3([](void* arg) -> void* { return arg; }, &val);

  TEST(t1.equals(t2), ATTO_TEXT("copied threads should be equal"));
  TEST(t1 == t2, ATTO_TEXT("operator== should work for equal threads"));
  TEST(!(t1 == t3), ATTO_TEXT("different threads should not be equal"));
  TEST(t1 != t3, ATTO_TEXT("operator!= should work for different threads"));

  t1.await();
  t2.await();
  t3.await();

  if (errorCount == 0) {
    Log(ATTO_TEXT("All tests passed!"));
    Exit(0);
  } else {
    LogError(errorCount, ATTO_TEXT(" test(s) failed"));
    Exit(errorCount);
  }
}
