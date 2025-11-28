//==============================================================================
// example_simple_chat.cpp - Simple AI Chat Client
//==============================================================================
// Interactive chat with an OpenAI-compatible LLM using the Console class.
// Requires OPENAI_API_KEY environment variable to be set.
//
// Usage:
//   set OPENAI_API_KEY=your-key-here
//   example_simple_chat
//
// Type messages and press Enter. Type "quit" or "exit" to leave.
//==============================================================================

#include "attoboy/attoboy.h"

using namespace attoboy;

extern "C" void atto_main() {
  // Get API key from environment
  String apiKey = GetEnv("OPENAI_API_KEY");
  if (apiKey.isEmpty()) {
    LogError("OPENAI_API_KEY environment variable is not set.");
    LogError("Please set it with: set OPENAI_API_KEY=your-key-here");
    Exit(1);
    return;
  }

  // Create AI client with OpenAI settings
  AI ai("https://api.openai.com/v1", apiKey, "gpt-5-nano-2025-08-07");

  // Create a conversation to maintain chat history
  Conversation conversation = ai.createConversation();

  // Initialize the console for interactive input
  Console console;

  // Set up input options with command history
  List history;
  ConsoleInput inputOptions;
  inputOptions.history = history;
  inputOptions.multiline = true;

  // Welcome message
  console.println("=== AI Chat ===", CON_GREEN);
  console.println("Type your message and press Enter.", CON_GRAY);
  console.println("Type 'quit' or 'exit' to leave.", CON_GRAY);
  console.println();

  // Main chat loop
  while (true) {
    // Get user input with a prompt
    String userMessage = console.input("You: ", inputOptions);

    // Check for empty input (Ctrl+C) or quit commands
    if (userMessage.isEmpty()) {
      console.println();
      console.println("Goodbye!", CON_YELLOW);
      break;
    }

    String lowerMsg = userMessage.lower().trim();
    if (lowerMsg == "quit" || lowerMsg == "exit") {
      console.println("Goodbye!", CON_YELLOW);
      break;
    }

    // Show that we're waiting for a response
    console.print("AI: ", CON_CYAN);

    // Send message to the AI and get response
    String response = conversation.ask(userMessage);

    if (response.isEmpty()) {
      console.println("(Error: No response received)", CON_RED);
    } else {
      // Print the AI's response
      console.println(response);
    }

    console.println(); // Blank line between exchanges
  }

  // Show token usage summary
  console.println();
  console.print("Session tokens used: ", CON_DARK_GRAY);
  console.println(String(ai.getTotalTokensUsed()), CON_DARK_GRAY);
}