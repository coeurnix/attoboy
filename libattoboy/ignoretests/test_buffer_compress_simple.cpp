#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError(ATTO_TEXT("Test failed: "), message);                                        \
    errorCount++;                                                              \
  }

void atto_main() {
  Log(ATTO_TEXT("Running simple Buffer compression test..."));

  // Test 1: Basic compress and decompress
  {
    Log(ATTO_TEXT("Test 1: Starting basic compress/decompress test"));
    String original(ATTO_TEXT("Hello World"));
    Log(ATTO_TEXT("Created string: "), original);

    Buffer buf(original);
    Log(ATTO_TEXT("Created buffer from string"));

    Log(ATTO_TEXT("About to compress..."));
    Buffer compressed = buf.compress();
    Log(ATTO_TEXT("Compression complete"));

    if (compressed.isEmpty()) {
      LogError(ATTO_TEXT("Compression returned empty buffer"));
      errorCount++;
    } else {
      Log(ATTO_TEXT("Compressed buffer is not empty - good"));

      Log(ATTO_TEXT("About to decompress..."));
      Buffer decompressed = compressed.decompress();
      Log(ATTO_TEXT("Decompression complete"));

      String result = decompressed.toString();
      Log(ATTO_TEXT("Result: "), result);

      TEST(result == original, ATTO_TEXT("Decompressed should match original"));
    }
  }

  // Report results
  if (errorCount == 0) {
    Log(ATTO_TEXT("All tests passed!"));
    Exit(0);
  } else {
    LogError(errorCount, ATTO_TEXT(" test(s) failed"));
    Exit(errorCount);
  }
}
