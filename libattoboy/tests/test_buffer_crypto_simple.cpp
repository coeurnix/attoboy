#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError(ATTO_TEXT("Test failed: "), message);                             \
    errorCount++;                                                              \
  }

void atto_main() {
  Log(ATTO_TEXT("Running simple Buffer crypto tests..."));

  {
    String original(ATTO_TEXT("Hello, World!"));
    Buffer buf(original);
    Buffer encrypted = buf.encrypt();
    TEST(!encrypted.isEmpty(),
         ATTO_TEXT("Encrypted buffer should not be empty"));

    encrypted.decrypt();
    TEST(encrypted.length() == buf.length(),
         ATTO_TEXT("Decrypted buffer size should match original"));
    TEST(buf.compare(encrypted),
         ATTO_TEXT("Decrypted buffer should match original"));
  }

  {
    String original(ATTO_TEXT("Test"));
    Buffer buf(original);
    String base64 = buf.toBase64();
    TEST(!base64.isEmpty(), ATTO_TEXT("Base64 string should not be empty"));

    Buffer decoded = Buffer::fromBase64(base64);
    String result = decoded.toString();
    TEST(result == original,
         ATTO_TEXT("Base64 decoded data should match original"));
  }

  {
    Buffer empty;
    Buffer encrypted = empty.encrypt();
    TEST(encrypted.isEmpty(),
         ATTO_TEXT("Encrypting empty buffer should return empty buffer"));
  }

  {
    Buffer empty;
    String base64 = empty.toBase64();
    TEST(base64.isEmpty(),
         ATTO_TEXT("Base64 of empty buffer should be empty string"));
  }

  {
    Buffer binary;
    for (int i = 0; i < 256; i++) {
      unsigned char byte = (unsigned char)i;
      binary.append(&byte, 1);
    }

    String base64 = binary.toBase64();
    TEST(!base64.isEmpty(),
         ATTO_TEXT("Binary data Base64 should not be empty"));

    Buffer decoded = Buffer::fromBase64(base64);

    bool match = true;
    if (decoded.length() == binary.length()) {
      int len;
      const unsigned char *orig = binary.c_ptr(&len);
      const unsigned char *decd = decoded.c_ptr(&len);
      for (int i = 0; i < len; i++) {
        if (orig[i] != decd[i]) {
          match = false;
          break;
        }
      }
    } else {
      match = false;
    }

    TEST(match, ATTO_TEXT("Binary data should Base64 encode/decode correctly"));
  }

  if (errorCount == 0) {
    Log(ATTO_TEXT("All crypto tests passed!"));
    Exit(0);
  } else {
    LogError(errorCount, ATTO_TEXT(" test(s) failed"));
    Exit(errorCount);
  }
}
