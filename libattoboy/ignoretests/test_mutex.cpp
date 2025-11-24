#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;
static int sharedCounter = 0;
static Mutex* counterMutex = nullptr;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError(ATTO_TEXT("Test failed: "), message);                             \
    errorCount++;                                                              \
  }

void* incrementCounter(void* arg) {
  int iterations = *(int*)arg;
  for (int i = 0; i < iterations; i++) {
    counterMutex->lock();
    sharedCounter++;
    counterMutex->unlock();
  }
  return nullptr;
}

void atto_main() {
  Log(ATTO_TEXT("Running Mutex tests..."));

  Mutex m1;
  m1.lock();
  TEST(m1.tryLock() == false, ATTO_TEXT("tryLock should fail when already locked"));
  m1.unlock();
  TEST(m1.tryLock() == true, ATTO_TEXT("tryLock should succeed when unlocked"));
  m1.unlock();

  Mutex m2 = m1;
  m2.lock();
  TEST(m1.tryLock() == false, ATTO_TEXT("copied mutex should share same lock"));
  m2.unlock();

  sharedCounter = 0;
  Mutex tempMutex;
  counterMutex = &tempMutex;

  int iterations = 1000;
  Thread t1(incrementCounter, &iterations);
  Thread t2(incrementCounter, &iterations);
  Thread t3(incrementCounter, &iterations);

  t1.await();
  t2.await();
  t3.await();

  TEST(sharedCounter == 3000, ATTO_TEXT("mutex should protect shared counter"));

  counterMutex = nullptr;

  if (errorCount == 0) {
    Log(ATTO_TEXT("All Mutex tests passed!"));
    Exit(0);
  } else {
    LogError(errorCount, ATTO_TEXT(" test(s) failed"));
    Exit(errorCount);
  }
}
