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
    String key(ATTO_TEXT("12345678901234567890123456789012"));
    String nonce(ATTO_TEXT("123456789012"));

    Buffer encrypted = buf.crypt(key, nonce);
    TEST(!encrypted.isEmpty(),
         ATTO_TEXT("Encrypted buffer should not be empty"));
    TEST(encrypted.length() == buf.length(),
         ATTO_TEXT("Encrypted buffer size should match original"));

    Buffer decrypted = encrypted.crypt(key, nonce);
    TEST(decrypted.length() == buf.length(),
         ATTO_TEXT("Decrypted buffer size should match original"));
    TEST(buf.compare(decrypted),
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
    String key(ATTO_TEXT("12345678901234567890123456789012"));
    String nonce(ATTO_TEXT("123456789012"));
    Buffer encrypted = empty.crypt(key, nonce);
    TEST(encrypted.isEmpty(),
         ATTO_TEXT("Encrypting empty buffer should return empty buffer"));
  }

  {
    String original(ATTO_TEXT("Test message"));
    Buffer buf(original);
    String shortKey(ATTO_TEXT("short"));
    String nonce(ATTO_TEXT("123456789012"));
    Buffer encrypted = buf.crypt(shortKey, nonce);
    TEST(encrypted.isEmpty(),
         ATTO_TEXT("Encryption with short key should fail"));
  }

  {
    String original(ATTO_TEXT("Test message"));
    Buffer buf(original);
    String key(ATTO_TEXT("12345678901234567890123456789012"));
    String shortNonce(ATTO_TEXT("short"));
    Buffer encrypted = buf.crypt(key, shortNonce);
    TEST(encrypted.isEmpty(),
         ATTO_TEXT("Encryption with short nonce should fail"));
  }

  {
    String original(ATTO_TEXT("ChaCha20 test vector"));
    Buffer buf(original);

    unsigned char keyBytes[32];
    for (int i = 0; i < 32; i++) {
      keyBytes[i] = (unsigned char)i;
    }
    Buffer keyBuf(keyBytes, 32);

    unsigned char nonceBytes[12];
    for (int i = 0; i < 12; i++) {
      nonceBytes[i] = (unsigned char)(i * 2);
    }
    Buffer nonceBuf(nonceBytes, 12);

    Buffer encrypted = buf.crypt(keyBuf, nonceBuf);
    TEST(!encrypted.isEmpty(),
         ATTO_TEXT("Buffer-to-buffer encryption should work"));

    Buffer decrypted = encrypted.crypt(keyBuf, nonceBuf);
    TEST(buf.compare(decrypted),
         ATTO_TEXT("Buffer-to-buffer decryption should match original"));
  }

  {
    String msg1(ATTO_TEXT("Message one"));
    String msg2(ATTO_TEXT("Message one"));
    Buffer buf1(msg1);
    Buffer buf2(msg2);
    String key(ATTO_TEXT("12345678901234567890123456789012"));
    String nonce(ATTO_TEXT("123456789012"));

    Buffer enc1 = buf1.crypt(key, nonce);
    Buffer enc2 = buf2.crypt(key, nonce);

    TEST(enc1.compare(enc2),
         ATTO_TEXT("Same message with same key/nonce produces same ciphertext"));
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
