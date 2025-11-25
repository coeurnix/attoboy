#include "test_framework.h"

void atto_main() {
  EnableLoggingToFile("test_conversation_only.log", true);
  Log("=== Conversation Test ===");

  String baseUrl = String("http://localhost:8080/v1/");
  String apiKey = String("mock-api-key-12345");
  String chatModel = String("gpt-5-mini-mock");

  Log("Creating AI instance...");
  AI ai(baseUrl, apiKey, chatModel);
  Log("AI instance created");

  Log("Creating conversation...");
  Conversation *conv = ai.createConversation();

  if (!conv) {
    LogError("FAILED: Could not create conversation");
    EnableLoggingToConsole();
    ExitProcess(1);
  }

  Log("Conversation created");

  Log("Asking conversation a question...");
  String *response = conv->ask(String("First message"));

  if (!response) {
    LogError("FAILED: No conversation response");
    delete conv;
    EnableLoggingToConsole();
    ExitProcess(1);
  }

  Log("Conversation response:", *response);

  List messages = conv->getConversationList();
  Log("Message count:", messages.length());

  if (messages.length() != 2) {
    LogError("FAILED: Expected 2 messages, got", messages.length());
    delete response;
    delete conv;
    EnableLoggingToConsole();
    ExitProcess(1);
  }

  delete response;
  delete conv;

  Log("Test PASSED");
  EnableLoggingToConsole();
  ExitProcess(0);
}
