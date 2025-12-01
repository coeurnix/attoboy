// example_compare_files.cpp
//
// This program compares two files semantically using AI embeddings.
// It reads up to 20,000 bytes from each file, creates vector embeddings,
// and computes how similar their content is.
//
// Usage: example_compare_files.exe <file1> <file2>
//
// Requires: OPENAI_API_KEY environment variable to be set
//

#include <attoboy/attoboy.h>
using namespace attoboy;

void atto_main() {
  // Set up command-line argument parsing
  // We need exactly two file paths as positional arguments
  Arguments args;
  args.setHelp("Compare two files semantically using AI embeddings");
  args.addPositionalParameter("file1", "First file to compare");
  args.addPositionalParameter("file2", "Second file to compare");
  args.requireArgument("file1");
  args.requireArgument("file2");

  // Parse the arguments - returns empty map if help was shown or args missing
  Map parsed = args.parseArguments();
  if (parsed.isEmpty()) {
    Exit(1);
  }

  // Get the file paths from parsed arguments
  String file1Path = parsed.get("file1");
  String file2Path = parsed.get("file2");

  // Create Path objects to work with the files
  Path path1(file1Path);
  Path path2(file2Path);

  // Verify both files exist before proceeding
  if (!path1.exists()) {
    LogError("File not found: ", file1Path);
    Exit(1);
  }
  if (!path2.exists()) {
    LogError("File not found: ", file2Path);
    Exit(1);
  }

  // Read the contents of both files
  String content1 = path1.readToString();
  String content2 = path2.readToString();

  // Limit each file to 20,000 bytes as specified
  // byteSubstring works with raw bytes, not UTF-8 characters
  int maxBytes = 20000;
  if (content1.byteLength() > maxBytes) {
    content1 = content1.byteSubstring(0, maxBytes);
    Log("Note: Just using first 20,000 bytes of first file");
  }
  if (content2.byteLength() > maxBytes) {
    content2 = content2.byteSubstring(0, maxBytes);
    Log("Note: Just using first 20,000 bytes of second file");
  }

  // Get the OpenAI API key from environment variables
  String apiKey = GetEnv("OPENAI_API_KEY");
  if (apiKey.isEmpty()) {
    LogError("OPENAI_API_KEY environment variable is not set");
    Exit(1);
  }

  // Create an AI client configured for OpenAI embeddings
  // Using the text-embedding-3-small model
  AI ai("https://api.openai.com/v1", apiKey, "text-embedding-3-small");

  Log("Creating embedding for first file...");

  // Create embedding for the first file with 1536 dimensions
  Embedding embedding1 = ai.createEmbedding(content1, 1536);

  // Check if embedding creation succeeded (dimensions will be 0 on error)
  if (embedding1.getDimensions() == 0) {
    LogError("Failed to create embedding for: ", file1Path);
    Exit(1);
  }

  Log("Creating embedding for second file...");

  // Create embedding for the second file with 1536 dimensions
  Embedding embedding2 = ai.createEmbedding(content2, 1536);

  if (embedding2.getDimensions() == 0) {
    LogError("Failed to create embedding for: ", file2Path);
    Exit(1);
  }

  // Compare the two embeddings using cosine similarity
  // Returns a value from -1.0 to 1.0
  float similarity = embedding1.compare(embedding2);

  // Display the results
  Log("");
  Log("=== Comparison Results ===");
  Log("File 1: ", file1Path);
  Log("File 2: ", file2Path);
  Log("");
  Log("Similarity score (-1 to 1): ", similarity);
  Log("");
  Log("Interpretation:");
  Log("   1.0 = Nearly identical content");
  Log("   0.5+ = Related content");
  Log("   0.0 = Unrelated content");
  Log("  -1.0 = Opposite meaning");

  Exit();
}