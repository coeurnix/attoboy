#include "attoboy/attoboy.h"

using namespace attoboy;

void atto_main() {
  Log(ATTO_TEXT("Testing LZ4 compression..."));

  // Test 1: Very simple string
  String original(ATTO_TEXT("Hello World"));
  Buffer buf(original);

  int len = 0;
  const unsigned char *data = buf.c_ptr(&len);
  Log(ATTO_TEXT("Original length: "), len);

  Buffer compressed = buf.compress();
  int compLen = 0;
  const unsigned char *compData = compressed.c_ptr(&compLen);
  Log(ATTO_TEXT("Compressed length: "), compLen);

  if (compressed.isEmpty()) {
    LogError(ATTO_TEXT("Compression failed - returned empty buffer"));
    Exit(1);
  }

  Buffer decompressed = compressed.decompress();
  int decompLen = 0;
  const unsigned char *decompData = decompressed.c_ptr(&decompLen);
  Log(ATTO_TEXT("Decompressed length: "), decompLen);

  if (decompressed.isEmpty()) {
    LogError(ATTO_TEXT("Decompression failed - returned empty buffer"));
    Exit(1);
  }

  String result = decompressed.toString();
  Log(ATTO_TEXT("Original: "), original);
  Log(ATTO_TEXT("Result: "), result);

  if (result == original) {
    Log(ATTO_TEXT("SUCCESS: Round-trip works!"));
    Exit(0);
  } else {
    LogError(ATTO_TEXT("FAIL: Result doesn't match original"));
    Exit(1);
  }
}
