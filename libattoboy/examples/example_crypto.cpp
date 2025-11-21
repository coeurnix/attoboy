#include "attoboy/attoboy.h"

using namespace attoboy;

void atto_main() {
  Log(ATTO_TEXT("=== Buffer Crypto Example ==="));

  String message(ATTO_TEXT("Hello, World!"));
  Buffer buf(message);

  Log(ATTO_TEXT("Original: "), message);

  Buffer encrypted = buf.encrypt();
  Log(ATTO_TEXT("Encrypted (base64): "), encrypted.toString());

  Buffer decrypted = encrypted.decrypt();
  Log(ATTO_TEXT("Decrypted: "), decrypted.toString());

  String base64 = buf.toBase64();
  Log(ATTO_TEXT("Base64: "), base64);

  Buffer decoded = Buffer::fromBase64(base64);
  Log(ATTO_TEXT("Decoded: "), decoded.toString());

  Log(ATTO_TEXT("All crypto operations completed successfully!"));
  Exit(0);
}
