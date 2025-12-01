// example_summarize.cpp
//
// This program summarizes the contents of a file using AI.
// It reads up to 150,000 bytes and asks the AI for a one-paragraph summary.
// Works with both regular text and programming source code.
//
// Usage: example_summarize.exe <file>
//
// Requires: OPENAI_API_KEY environment variable to be set
//

#include <attoboy/attoboy.h>
using namespace attoboy;

void atto_main() {
  // Set up command-line argument parsing
  // We need exactly one file path as a positional argument
  Arguments args;
  args.setHelp("Summarize the contents of a file using AI");
  args.addPositionalParameter("file", "File to summarize");
  args.requireArgument("file");

  // Parse the arguments - returns empty map if help was shown or args missing
  Map parsed = args.parseArguments();
  if (parsed.isEmpty()) {
    Exit(1);
  }

  // Get the file path from parsed arguments
  String filePath = parsed.get("file");

  // Create a Path object to work with the file
  Path path(filePath);

  // Verify the file exists before proceeding
  if (!path.exists()) {
    LogError("File not found: ", filePath);
    Exit(1);
  }

  // Check that it's actually a file (not a directory)
  if (!path.isRegularFile()) {
    LogError("Not a regular file: ", filePath);
    Exit(1);
  }

  Log("Reading file: ", filePath);

  // Read the contents of the file
  String content = path.readToString();

  // Check if the file was empty
  if (content.isEmpty()) {
    LogError("File is empty");
    Exit(1);
  }

  // Limit the content to 150,000 bytes as specified
  int maxBytes = 150000;
  if (content.byteLength() > maxBytes) {
    content = content.byteSubstring(0, maxBytes);
    Log("Note: Only using first 150,000 bytes of file");
  }

  Log("File size: ", content.byteLength(), " bytes");

  // Get the OpenAI API key from environment variables
  String apiKey = GetEnv("OPENAI_API_KEY");
  if (apiKey.isEmpty()) {
    LogError("OPENAI_API_KEY environment variable is not set");
    Exit(1);
  }

  // Create an AI client configured for OpenAI chat completions
  // Using the gpt-5-nano-2025-08-07 model
  AI ai("https://api.openai.com/v1", apiKey, "gpt-5-nano-2025-08-07");

  // Build the prompt for summarization
  // The prompt is designed to work with both source code and regular text
  String prompt = String(
      "Please provide a concise summary of the following content. ",
      "Write the summary as approximately one paragraph. ",
      "Use the same language as the original content. ",
      "The content may be programming source code, technical documentation, ",
      "or ordinary written text - adjust your summary style accordingly.\n\n",
      "## CONTENT IS BELOW THIS LINE ##\n", content);

  Log("");
  Log("Requesting summary from AI...");

  // Send the prompt to the AI and get the summary
  String summary = ai.ask(prompt);

  // Check if the AI call succeeded (returns empty string on error)
  if (summary.isEmpty()) {
    LogError("Failed to get summary from AI");
    LogError("Finish reason: ", ai.getFinishReason());
    Exit(1);
  }

  // Display the summary
  Log("");
  Log("=== Summary ===");
  Log("");
  Log(summary);
  Log("");
  Log("=== End of Summary ===");
  Log("");
  Log("Tokens used: ", ai.getTotalTokensUsed());

  Exit();
}