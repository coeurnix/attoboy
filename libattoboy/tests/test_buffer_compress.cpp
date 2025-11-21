#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError("Test failed: ", message);                                        \
    errorCount++;                                                              \
  }

void atto_main() {
  Log("Running Buffer compression tests...");

  // Test 1: Basic compress and decompress round-trip
  Log("Step 1");
  {
    String original("Hello, World! This is a test string.");
    Buffer buf(original);
    Log("Step 1.a");
    Buffer compressed = buf.compress();
    Log("Step 1.b");
    TEST(!compressed.isEmpty(), "Compressed buffer should not be empty");
    Log("Step 1.c");
    Buffer decompressed = compressed.decompress();
    Log("Step 1.d");
    String result = decompressed.toString();
    Log("Step 1.e");
    TEST(result == original, "Decompressed data should match original");
  }

  // Test 2: Compress larger text with repetition
  Log("Step 2");
  {
    String repeated("The quick brown fox jumps over the lazy dog. ");
    String original = repeated.repeat(100);
    Buffer buf(original);

    int originalLength = buf.length();
    Buffer compressed = buf.compress();
    int compressedLength = compressed.length();

    TEST(compressedLength < originalLength,
         "Compressed data should be smaller than original for repetitive text");

    Buffer decompressed = compressed.decompress();
    TEST(decompressed.toString() == original,
         "Decompressed repetitive text should match original");
  }

  // Test 3: Compress empty buffer
  Log("Step 3");
  {
    Buffer empty;
    Buffer compressed = empty.compress();
    TEST(compressed.isEmpty(),
         "Compressing empty buffer should return empty buffer");
  }

  // Test 4: Decompress empty buffer
  Log("Step 4");
  {
    Buffer empty;
    Buffer decompressed = empty.decompress();
    TEST(decompressed.isEmpty(),
         "Decompressing empty buffer should return empty buffer");
  }

  // Test 5: Multiple compress/decompress cycles
  Log("Step 5");
  {
    String original("Testing multiple compression cycles with some data.");
    Buffer buf(original);

    // First cycle
    Buffer compressed1 = buf.compress();
    Buffer decompressed1 = compressed1.decompress();
    TEST(decompressed1.toString() == original,
         "First cycle should preserve data");

    // Second cycle on the decompressed data
    Buffer compressed2 = decompressed1.compress();
    Buffer decompressed2 = compressed2.decompress();
    TEST(decompressed2.toString() == original,
         "Second cycle should preserve data");
  }

  // Test 6: Compress small buffer
  Log("Step 6");
  {
    String small("Hi");
    Buffer buf(small);

    Buffer compressed = buf.compress();
    Buffer decompressed = compressed.decompress();

    TEST(decompressed.toString() == small,
         "Small text should compress and decompress correctly");
  }

  // Test 7: Compress buffer with various characters
  Log("Step 7");
  {
    String varied("ABCabc123!@#$%^&*()_+-=[]{}|;:',.<>?/~`");
    Buffer buf(varied);

    Buffer compressed = buf.compress();
    Buffer decompressed = compressed.decompress();

    TEST(decompressed.toString() == varied,
         "Varied characters should compress and decompress correctly");
  }

  // Test 8: Verify compression actually reduces size for suitable data
  Log("Step 8");
  {
    String pattern("A");
    String original = pattern.repeat(1000);
    Buffer buf(original);

    int originalLength = buf.length();
    Buffer compressed = buf.compress();
    int compressedLength = compressed.length();

    TEST(compressedLength < originalLength,
         "Highly repetitive data should compress significantly");

    Buffer decompressed = compressed.decompress();
    TEST(decompressed.toString() == original,
         "Decompressed highly repetitive data should match");
  }

  // Report results
  if (errorCount == 0) {
    Log("All buffer compression tests passed!");
    Exit(0);
  } else {
    LogError(errorCount, " test(s) failed");
    Exit(errorCount);
  }
}
