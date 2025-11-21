#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError("Test failed: ", message);                                        \
    errorCount++;                                                              \
  }

void atto_main() {
  Log("Running simple Buffer compression test...");

  // Test 1: Basic compress and decompress
  {
    Log("Test 1: Starting basic compress/decompress test");
    String original("Hello World");
    Log("Created string: ", original);

    Buffer buf(original);
    Log("Created buffer from string");

    Log("About to compress...");
    Buffer compressed = buf.compress();
    Log("Compression complete");

    if (compressed.isEmpty()) {
      LogError("Compression returned empty buffer");
      errorCount++;
    } else {
      Log("Compressed buffer is not empty - good");

      Log("About to decompress...");
      Buffer decompressed = compressed.decompress();
      Log("Decompression complete");

      String result = decompressed.toString();
      Log("Result: ", result);

      TEST(result == original, "Decompressed should match original");
    }
  }

  // Report results
  if (errorCount == 0) {
    Log("All tests passed!");
    Exit(0);
  } else {
    LogError(errorCount, " test(s) failed");
    Exit(errorCount);
  }
}
