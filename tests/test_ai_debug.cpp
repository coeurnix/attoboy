#include "test_framework.h"

void atto_main() {
  EnableLoggingToConsole();
  Log("=== AI Debug Test ===");

  String apiKey = GetEnv(String("OPENAI_API_KEY"));
  if (apiKey.isEmpty()) {
    LogError("OPENAI_API_KEY not set");
    Exit(1);
  }

  Log("API Key length:", apiKey.length());

  String baseUrl = String("https://api.openai.com/v1/");
  String model = String("gpt-5-mini-2025-08-07");

  Log("Creating AI instance...");
  AI ai(baseUrl, apiKey, model);

  Log("AI created. Model:", ai.getModel());
  Log("Base URL:", ai.getBaseUrl());
  Log("API Key (first 10 chars):", ai.getAPIKey().substring(0, 10));

  Log("Sending test request...");
  String response = ai.ask(String("Say 'hello' and nothing else."));

  if (!response.isEmpty()) {
    Log("SUCCESS! Response:", response);
    Log("Finish reason:", ai.getFinishReason());
    Log("Tokens - Prompt:", ai.getPromptTokensUsed(),
        "Response:", ai.getResponseTokensUsed());
  } else {
    LogError("FAILED - No response received");

    // Let's try to debug by making a direct WebRequest
    Log("Attempting direct WebRequest for debugging...");

    String url = String(baseUrl, "chat/completions");
    Log("URL:", url);

    String authHeader = String("Bearer ", apiKey);
    Map headers;
    headers.put("Authorization", authHeader);
    headers.put("Content-Type", "application/json");

    Map userMsg;
    userMsg.put("role", "user");
    userMsg.put("content", "Say hello");

    List messages;
    messages.append(userMsg);

    Map requestBody;
    requestBody.put("model", model);
    requestBody.put("messages", messages);

    String requestJson = requestBody.toJSONString();
    Log("Request JSON:", requestJson);

    {
      WebRequest req(url, nullptr, &headers);
      WebResponse resp = req.doPost(requestBody, -1);

      Log("WebResponse received");
      Log("Status code:", resp.getStatusCode());
      Log("Status reason:", resp.getStatusReason());
      Log("Succeeded:", resp.succeeded() ? "true" : "false");

      String respBody = resp.asString();
      Log("Response body:", respBody);

      const Map *jsonResp = resp.asJson();
      if (jsonResp) {
        Log("JSON parsed successfully");
        Log("JSON keys:", jsonResp->keys().length());
      } else {
        LogError("JSON parsing failed");
      }
    }
  } // WebResponse destroyed here

  Exit(0); // Explicit exit after cleanup
}
