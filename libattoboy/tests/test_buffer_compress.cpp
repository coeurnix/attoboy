#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError(ATTO_TEXT("Test failed: "), message);                                        \
    errorCount++;                                                              \
  }

void atto_main() {
  Log(ATTO_TEXT("Running Buffer compression tests..."));

  // Test 1: Basic compress and decompress round-trip
  Log(ATTO_TEXT("Step 1"));
  {
    String original(ATTO_TEXT("Hello, World! This is a test string."));
    Buffer buf(original);
    Log(ATTO_TEXT("Step 1.a"));
    Buffer compressed = buf.compress();
    Log(ATTO_TEXT("Step 1.b"));
    TEST(!compressed.isEmpty(), ATTO_TEXT("Compressed buffer should not be empty"));
    Log(ATTO_TEXT("Step 1.c"));
    Buffer decompressed = compressed.decompress();
    Log(ATTO_TEXT("Step 1.d"));
    String result = decompressed.toString();
    Log(ATTO_TEXT("Step 1.e"));
    TEST(result == original, ATTO_TEXT("Decompressed data should match original"));
  }

  // Test 2: Compress larger text with repetition
  Log(ATTO_TEXT("Step 2"));
  {
    String repeated(ATTO_TEXT("The quick brown fox jumps over the lazy dog. "));
    String original = repeated.repeat(100);
    Buffer buf(original);

    int originalLength = buf.length();
    Buffer compressed = buf.compress();
    int compressedLength = compressed.length();

    TEST(compressedLength < originalLength,
         ATTO_TEXT("Compressed data should be smaller than original for repetitive text"));

    Buffer decompressed = compressed.decompress();
    TEST(decompressed.toString() == original,
         ATTO_TEXT("Decompressed repetitive text should match original"));
  }

  // Test 3: Compress empty buffer
  Log(ATTO_TEXT("Step 3"));
  {
    Buffer empty;
    Buffer compressed = empty.compress();
    TEST(compressed.isEmpty(),
         ATTO_TEXT("Compressing empty buffer should return empty buffer"));
  }

  // Test 4: Decompress empty buffer
  Log(ATTO_TEXT("Step 4"));
  {
    Buffer empty;
    Buffer decompressed = empty.decompress();
    TEST(decompressed.isEmpty(),
         ATTO_TEXT("Decompressing empty buffer should return empty buffer"));
  }

  // Test 5: Multiple compress/decompress cycles
  Log(ATTO_TEXT("Step 5"));
  {
    String original(ATTO_TEXT("Testing multiple compression cycles with some data."));
    Buffer buf(original);

    // First cycle
    Buffer compressed1 = buf.compress();
    Buffer decompressed1 = compressed1.decompress();
    TEST(decompressed1.toString() == original,
         ATTO_TEXT("First cycle should preserve data"));

    // Second cycle on the decompressed data
    Buffer compressed2 = decompressed1.compress();
    Buffer decompressed2 = compressed2.decompress();
    TEST(decompressed2.toString() == original,
         ATTO_TEXT("Second cycle should preserve data"));
  }

  // Test 6: Compress small buffer
  Log(ATTO_TEXT("Step 6"));
  {
    String small(ATTO_TEXT("Hi"));
    Buffer buf(small);

    Buffer compressed = buf.compress();
    Buffer decompressed = compressed.decompress();

    TEST(decompressed.toString() == small,
         ATTO_TEXT("Small text should compress and decompress correctly"));
  }

  // Test 7: Compress buffer with various characters
  Log(ATTO_TEXT("Step 7"));
  {
    String varied(ATTO_TEXT("ABCabc123!@#$%^&*()_+-=[]{}|;:',.<>?/~`"));
    Buffer buf(varied);

    Buffer compressed = buf.compress();
    Buffer decompressed = compressed.decompress();

    TEST(decompressed.toString() == varied,
         ATTO_TEXT("Varied characters should compress and decompress correctly"));
  }

  // Test 8: Verify compression actually reduces size for suitable data
  Log(ATTO_TEXT("Step 8"));
  {
    String pattern(ATTO_TEXT("A"));
    String original = pattern.repeat(1000);
    Buffer buf(original);

    int originalLength = buf.length();
    Buffer compressed = buf.compress();
    int compressedLength = compressed.length();

    TEST(compressedLength < originalLength,
         ATTO_TEXT("Highly repetitive data should compress significantly"));

    Buffer decompressed = compressed.decompress();
    TEST(decompressed.toString() == original,
         ATTO_TEXT("Decompressed highly repetitive data should match"));
  }

  // Report results
  if (errorCount == 0) {
    Log(ATTO_TEXT("All buffer compression tests passed!"));
    Exit(0);
  } else {
    LogError(errorCount, ATTO_TEXT(" test(s) failed"));
    Exit(errorCount);
  }
}
