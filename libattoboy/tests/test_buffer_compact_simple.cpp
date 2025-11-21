#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError("Test failed: ", message);                                        \
    errorCount++;                                                              \
  }

void atto_main() {
  Log("Running Buffer trim simple tests...");

  // Test 1: trim() on buffer with excess capacity
  {
    Buffer buf(1024);
    String small("tiny");
    buf.append(small);

    int lengthBefore = buf.length();
    Log("Length before trim: ", lengthBefore);

    buf.trim();
    int lengthAfter = buf.length();
    Log("Length after trim: ", lengthAfter);

    TEST(lengthBefore == lengthAfter, "trim() should not change buffer length");
    TEST(buf.toString() == small, "trim() should preserve buffer contents");
  }

  // Test 2: trim() on empty buffer
  {
    Buffer buf(2048);
    buf.trim();
    TEST(buf.isEmpty(), "Trimming empty buffer should keep it empty");
  }

  // Report results
  if (errorCount == 0) {
    Log("All trim tests passed!");
    Exit(0);
  } else {
    LogError(errorCount, " test(s) failed");
    Exit(errorCount);
  }
}
