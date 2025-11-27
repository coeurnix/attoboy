#include "test_framework.h"

void atto_main() {
  EnableLoggingToFile("test_ai_mock_debug.log", true);
  Log("=== AI Mock Server Test Debug ===");

  String baseUrl = String("http://localhost:8469/v1/");
  String apiKey = String("mock-api-key-12345");
  String chatModel = String("gpt-5-mini-mock");

  Log("Creating AI instance...");
  AI ai(baseUrl, apiKey, chatModel);
  Log("AI instance created");

  Log("Calling AI.ask()...");
  String response = ai.ask(String("Hello, test!"));

  if (response.isEmpty()) {
    LogError("Test FAILED: No response received");
    EnableLoggingToConsole(); // Close log file
    Exit(1);
  }

  Log("Response received:", response);

  Log("Test completed successfully");
  EnableLoggingToConsole(); // Close log file
  Exit(0);
}
