#include "test_framework.h"

void atto_main() {
  EnableLoggingToFile("test_ai_live.log", true); // Truncate log file
  Log("=== Live AI Test (Quick API Verification) ===");

  String apiKey = GetEnv(String("OPENAI_API_KEY"));
  if (apiKey.isEmpty()) {
    LogError("OPENAI_API_KEY not set - skipping live test");
    EnableLoggingToConsole();
    Exit(0); // Success - just skip if no key
  }

  String baseUrl = String("https://api.openai.com/v1/");
  String chatModel = String("gpt-5-mini-2025-08-07");

  Log("Testing live API with one simple call...");
  AI ai(baseUrl, apiKey, chatModel);

  String *response = ai.ask(String("Respond with exactly: hello"));

  if (!response) {
    LogError("FAILED: No response from live API");
    EnableLoggingToConsole();
    Exit(1);
  }

  Log("Response:", *response);

  if (response->lower().contains(String("hello"))) {
    Log("SUCCESS: Live API responded correctly");
    delete response;
    EnableLoggingToConsole();
    Exit(0);
  } else {
    LogError("FAILED: Response didn't contain 'hello'");
    delete response;
    EnableLoggingToConsole();
    Exit(1);
  }
}
