#include "attoboy/attoboy.h"

using namespace attoboy;

void atto_main() {
  Log(ATTO_TEXT("=== Buffer Crypto Example ==="));

  String message(ATTO_TEXT("Hello, World! This is a secret message."));
  Buffer buf(message);

  Log(ATTO_TEXT("Original: "), message);

  String key(ATTO_TEXT("12345678901234567890123456789012"));
  String nonce(ATTO_TEXT("123456789012"));

  Buffer encrypted = buf.crypt(key, nonce);
  Log(ATTO_TEXT("Encrypted (base64): "), encrypted.toBase64());

  Buffer decrypted = encrypted.crypt(key, nonce);
  Log(ATTO_TEXT("Decrypted: "), decrypted.toString());

  String base64 = buf.toBase64();
  Log(ATTO_TEXT("Base64: "), base64);

  Buffer decoded = Buffer::fromBase64(base64);
  Log(ATTO_TEXT("Decoded: "), decoded.toString());

  Log(ATTO_TEXT("All crypto operations completed successfully!"));
  Exit(0);
}
