#include "test_framework.h"

void atto_main() {
  EnableLoggingToFile("test_ai_simple.log", true);
  Log("=== Simple AI Test ===");

  String apiKey = GetEnv(String("OPENAI_API_KEY"));
  if (apiKey.isEmpty()) {
    LogError("OPENAI_API_KEY not set");
    SaveCoverage("test_ai_simple");
    Exit(1);
  }

  String baseUrl = String("https://api.openai.com/v1/");
  String chatModel = String("gpt-5-mini-2025-08-07");

  // Test 1: Basic AI.ask()
  {
    Log("Test 1: Basic AI.ask()");
    AI ai(baseUrl, apiKey, chatModel);
    REGISTER_TESTED(AI_constructor);

    String *response = ai.ask(String("Say hello"));
    REGISTER_TESTED(AI_ask);

    if (response) {
      Log("SUCCESS! Response:", *response);
      ASSERT_FALSE(response->isEmpty());
      delete response;
      Log("Test 1: PASSED");
    } else {
      LogError("Test 1: FAILED");
      TestFramework::DisplayCoverage();
      SaveCoverage("test_ai_simple");
      Exit(1);
    }
  }

  Log("");
  Log("=== Test completed successfully! ===");
  TestFramework::DisplayCoverage();
  // SaveCoverage("test_ai_simple");  // Temporarily disabled for testing
  EnableLoggingToConsole(); // Close log file before exit

  Exit(0); // Explicit exit after cleanup
}
