#include "test_framework.h"

void atto_main() {
  EnableLoggingToFile("test_conversation_only.log", true);
  Log("=== Conversation Test ===");

  String baseUrl = String("http://localhost:8469/v1/");
  String apiKey = String("mock-api-key-12345");
  String chatModel = String("gpt-5-mini-mock");

  Log("Creating AI instance...");
  AI ai(baseUrl, apiKey, chatModel);
  Log("AI instance created");

  Log("Creating conversation...");
  Conversation conv = ai.createConversation();

  if (conv.getTotalTokensUsed() < 0) {
    LogError("FAILED: Could not create conversation");
    EnableLoggingToConsole();
    ExitProcess(1);
  }

  Log("Conversation created");

  Log("Asking conversation a question...");
  String response = conv.ask(String("First message"));

  if (response.isEmpty()) {
    LogError("FAILED: No conversation response");
    EnableLoggingToConsole();
    ExitProcess(1);
  }

  Log("Conversation response:", response);

  List messages = conv.getConversationList();
  Log("Message count:", messages.length());

  if (messages.length() != 2) {
    LogError("FAILED: Expected 2 messages, got", messages.length());
    EnableLoggingToConsole();
    ExitProcess(1);
  }

  Log("Test PASSED");
  EnableLoggingToConsole();
  ExitProcess(0);
}
