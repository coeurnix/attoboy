#include "attoboy/attoboy.h"

using namespace attoboy;

void atto_main() {
  Log("=== Buffer Crypto Example ===");

  String message("Hello, World! This is a secret message.");
  Buffer buf(message);

  Log("Original: ", message);

  String key("12345678901234567890123456789012");
  String nonce("123456789012");

  Buffer encrypted = buf.crypt(key, nonce);
  Log("Encrypted (base64): ", encrypted.toBase64());

  Buffer decrypted = encrypted.crypt(key, nonce);
  Log("Decrypted: ", decrypted.toString());

  String base64 = buf.toBase64();
  Log("Base64: ", base64);

  Buffer decoded = Buffer::fromBase64(base64);
  Log("Decoded: ", decoded.toString());

  Log("All crypto operations completed successfully!");
  Exit(0);
}
