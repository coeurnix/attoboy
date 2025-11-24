#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError(ATTO_TEXT("Test failed: "), message);                                        \
    errorCount++;                                                              \
  }

void atto_main() {
  Log(ATTO_TEXT("Running Buffer trim simple tests..."));

  // Test 1: trim() on buffer with excess capacity
  {
    Buffer buf(1024);
    String small(ATTO_TEXT("tiny"));
    buf.append(small);

    int lengthBefore = buf.length();
    Log(ATTO_TEXT("Length before trim: "), lengthBefore);

    buf.trim();
    int lengthAfter = buf.length();
    Log(ATTO_TEXT("Length after trim: "), lengthAfter);

    TEST(lengthBefore == lengthAfter, ATTO_TEXT("trim() should not change buffer length"));
    TEST(buf.toString() == small, ATTO_TEXT("trim() should preserve buffer contents"));
  }

  // Test 2: trim() on empty buffer
  {
    Buffer buf(2048);
    buf.trim();
    TEST(buf.isEmpty(), ATTO_TEXT("Trimming empty buffer should keep it empty"));
  }

  // Report results
  if (errorCount == 0) {
    Log(ATTO_TEXT("All trim tests passed!"));
    Exit(0);
  } else {
    LogError(errorCount, ATTO_TEXT(" test(s) failed"));
    Exit(errorCount);
  }
}
