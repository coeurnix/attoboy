#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    errorCount++;                                                              \
  }

// Global variables for testing
static volatile int counter = 0;
static volatile bool threadExecuted = false;

// Simple thread function that increments a counter
void *simpleThreadFunc(void *arg) {
  int *value = (int *)arg;
  if (value) {
    *value = *value + 1;
  }
  counter++;
  threadExecuted = true;
  return (void *)42;
}

// Thread function that sleeps
void *sleepThreadFunc(void *arg) {
  int ms = (int)(long long)arg;
  Sleep(ms);
  counter++;
  return nullptr;
}

// Thread function for testing multiple threads
void *multiThreadFunc(void *arg) {
  int id = (int)(long long)arg;
  Sleep(10);
  counter += id;
  return (void *)(long long)(id * 2);
}

void atto_main() {
  // Test 1: Basic thread creation and execution
  {
    counter = 0;
    threadExecuted = false;
    int value = 10;
    Thread t(simpleThreadFunc, &value);
    Sleep(50);
    TEST(threadExecuted, ATTO_TEXT("Thread should execute"));
    TEST(value == 11, ATTO_TEXT("Thread should modify argument"));
    TEST(counter == 1, ATTO_TEXT("Counter should be incremented"));
  }

  // Test 2: Thread await and return value
  {
    counter = 0;
    threadExecuted = false;
    Thread t(simpleThreadFunc, nullptr);
    void *result = t.await();
    TEST(result == (void *)42, ATTO_TEXT("Thread should return correct value"));
    TEST(threadExecuted, ATTO_TEXT("Thread should complete before await returns"));
  }

  // Test 3: Thread isRunning
  {
    counter = 0;
    Thread t(sleepThreadFunc, (void *)100);
    TEST(t.isRunning(), ATTO_TEXT("Thread should be running initially"));
    t.await();
    Sleep(10);
    TEST(!t.isRunning(), ATTO_TEXT("Thread should not be running after await"));
  }

  // Test 4: Multiple threads
  {
    counter = 0;
    Thread t1(multiThreadFunc, (void *)1);
    Thread t2(multiThreadFunc, (void *)2);
    Thread t3(multiThreadFunc, (void *)3);

    void *r1 = t1.await();
    void *r2 = t2.await();
    void *r3 = t3.await();

    TEST(r1 == (void *)2, ATTO_TEXT("Thread 1 should return 2"));
    TEST(r2 == (void *)4, ATTO_TEXT("Thread 2 should return 4"));
    TEST(r3 == (void *)6, ATTO_TEXT("Thread 3 should return 6"));
    TEST(counter == 6, ATTO_TEXT("Counter should be sum of all thread increments"));
  }

  // Test 5: Thread copy constructor
  {
    counter = 0;
    Thread t1(sleepThreadFunc, (void *)50);
    Thread t2(t1);
    TEST(t2.isRunning(), ATTO_TEXT("Copied thread should share running state"));
    t1.await();
    TEST(!t2.isRunning(), ATTO_TEXT("Both threads should share completion state"));
  }

  // Test 6: Thread assignment operator
  {
    counter = 0;
    Thread t1(sleepThreadFunc, (void *)50);
    Thread t2(simpleThreadFunc, nullptr);
    t2.await();  // Wait for t2's original thread to complete first
    t2 = t1;
    TEST(t2.isRunning(), ATTO_TEXT("Assigned thread should share running state"));
    t1.await();
    TEST(!t2.isRunning(), ATTO_TEXT("Both threads should share completion state"));
  }

  // Test 7: Sleep function
  {
    DateTime before;
    Sleep(100);
    DateTime after;
    long long diff = after.diff(before);
    TEST(diff >= 80 && diff <= 500, ATTO_TEXT("Sleep should pause for approximately correct duration"));
  }

  // Test 8: Thread with null function pointer
  {
    Thread t(nullptr, nullptr);
    Sleep(10);
    TEST(!t.isRunning(), ATTO_TEXT("Thread with null function should not be running"));
  }

  // Test 9: Multiple await calls
  {
    counter = 0;
    Thread t(simpleThreadFunc, nullptr);
    void *r1 = t.await();
    void *r2 = t.await();
    TEST(r1 == (void *)42, ATTO_TEXT("First await should return correct value"));
    TEST(r2 == (void *)42, ATTO_TEXT("Second await should return same value"));
  }

  // Report results
  if (errorCount == 0) {
    Exit(0);
  } else {
    Exit(errorCount);
  }
}
