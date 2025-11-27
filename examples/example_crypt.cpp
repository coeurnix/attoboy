//==============================================================================
// example_crypt.cpp - ChaCha20 Encrypt/Decrypt Utility
//==============================================================================
// Encrypts or decrypts files using the ChaCha20 stream cipher.
// ChaCha20 is symmetric, so the same operation encrypts AND decrypts!
//
// Usage:
//   example_crypt -k <key> <input_file> <output_file>
//   example_crypt --key <key> --nonce <nonce> <input_file> <output_file>
//
// Example:
//   example_crypt -k "my-32-character-secret-key-here!" secret.txt secret.enc
//   example_crypt -k "my-32-character-secret-key-here!" secret.enc secret.txt
//
// Note: Key must be at least 32 characters. Nonce defaults to 12 zeros.
//==============================================================================

#include "attoboy/attoboy.h"

using namespace attoboy;

extern "C" void atto_main() {
  // Set up command-line arguments
  Arguments args;
  args.addParameter("k", "Encryption key (minimum 32 characters)", "", "key")
      .addParameter("n", "Nonce/IV (minimum 12 characters)", "000000000000",
                    "nonce")
      .addPositionalParameter("input", "Input file to encrypt/decrypt")
      .addPositionalParameter("output", "Output file")
      .requireArgument("k")
      .requireArgument("input")
      .requireArgument("output")
      .setHelp("example_crypt - ChaCha20 Encrypt/Decrypt Utility\n\n"
               "Encrypts or decrypts files using ChaCha20.\n"
               "The same key+nonce encrypts AND decrypts (symmetric).\n\n"
               "Usage: example_crypt -k <key> <input> <output>\n\n"
               "Options:\n"
               "  -k, --key    Encryption key (32+ chars, required)\n"
               "  -n, --nonce  Nonce/IV (12+ chars, default: 000000000000)\n\n"
               "Example:\n"
               "  example_crypt -k \"my-secret-key-at-least-32-chars!\" "
               "file.txt file.enc\n"
               "  example_crypt -k \"my-secret-key-at-least-32-chars!\" "
               "file.enc file.txt");

  // Parse arguments
  Map parsed = args.parseArguments();
  if (parsed.isEmpty()) {
    return;
  }

  // Get parameter values
  String key = parsed.get<String, String>("k");
  String nonce = parsed.get<String, String>("n");
  String inputPath = parsed.get<String, String>("input");
  String outputPath = parsed.get<String, String>("output");

  // Validate key length (ChaCha20 requires 32 bytes)
  if (key.byteLength() < 32) {
    LogError("Key must be at least 32 characters.");
    LogError("Your key is only ", key.byteLength(), " characters.");
    Exit(1);
    return;
  }

  // Validate nonce length (ChaCha20 requires 12 bytes)
  if (nonce.byteLength() < 12) {
    LogError("Nonce must be at least 12 characters.");
    Exit(1);
    return;
  }

  // Check input file exists
  Path inputFile(inputPath);
  if (!inputFile.exists()) {
    LogError("Input file not found: ", inputPath);
    Exit(1);
    return;
  }

  if (!inputFile.isRegularFile()) {
    LogError("Input is not a regular file: ", inputPath);
    Exit(1);
    return;
  }

  // Read input file into a buffer
  Buffer inputData = inputFile.readToBuffer();
  if (inputData.isEmpty()) {
    LogError("Failed to read input file (or file is empty)");
    Exit(1);
    return;
  }

  Log("Processing ", inputData.length(), " bytes...");

  // Encrypt or decrypt using ChaCha20 (symmetric operation)
  Buffer outputData = inputData.crypt(key, nonce);

  // Write to output file
  Path outputFile(outputPath);
  if (outputFile.writeFromBuffer(outputData)) {
    Log("Success: ", inputPath, " -> ", outputPath);
    Log("Output size: ", outputData.length(), " bytes");
  } else {
    LogError("Failed to write output file: ", outputPath);
    Exit(1);
  }
}