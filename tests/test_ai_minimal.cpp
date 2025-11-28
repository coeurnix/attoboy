#include "test_framework.h"

void atto_main() {
  EnableLoggingToConsole();
  Log("=== Minimal AI/WebRequest Test ===");

  String apiKey = GetEnv(String("OPENAI_API_KEY"));
  if (apiKey.isEmpty()) {
    LogError("OPENAI_API_KEY not set");
    Exit(1);
  }

  Log("Testing direct WebRequest to OpenAI API...");

  String url = String("https://api.openai.com/v1/chat/completions");
  String authHeader = String("Bearer ", apiKey);

  Map headers;
  headers.put("Authorization", authHeader);
  headers.put("Content-Type", "application/json");

  Log("Headers set");

  Map userMsg;
  userMsg.put("role", "user");
  userMsg.put("content", "Say hello");

  List messages;
  messages.append(userMsg);

  Map requestBody;
  requestBody.put("model", "gpt-5-mini-2025-08-07");
  requestBody.put("messages", messages);

  Log("Request body:");
  Log(requestBody.toJSONString());

  Log("Creating WebRequest...");
  {
    WebRequest req(url, Map(), headers);

    Log("Calling doPost (timeout 10 seconds)...");
    WebResponse resp = req.doPost(requestBody, 10000);

    Log("Response received!");
    Log("Status code:", resp.getStatusCode());
    Log("Succeeded:", resp.succeeded() ? "true" : "false");

    if (resp.succeeded()) {
      String body = resp.asString();
      Log("Body length:", body.length());
      Log("Body:", body);
    } else {
      Log("Request failed");
      String body = resp.asString();
      Log("Error body:", body);
    }
  } // WebResponse destroyed here

  Exit(0); // Explicit exit after cleanup
}
