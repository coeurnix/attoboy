#include "test_framework.h"

void atto_main() {
  EnableLoggingToConsole();
  EnableLoggingToFile("test_ai_verbose.log", true);
  Log("=== VERBOSE AI DEBUGGING TEST ===");

  String apiKey = GetEnv(String("OPENAI_API_KEY"));
  Log("1. Got API key, length:", apiKey.length());
  if (apiKey.isEmpty()) {
    LogError("OPENAI_API_KEY not set");
    Exit(1);
  }

  String baseUrl = String("https://api.openai.com/v1/");
  String chatModel = String("gpt-5-mini-2025-08-07");
  Log("2. Creating AI instance...");

  AI ai(baseUrl, apiKey, chatModel);
  Log("3. AI instance created");

  Log("4. Getting model:", ai.getModel());
  Log("5. Getting base URL:", ai.getBaseUrl());

  String prompt = String("Say hello");
  Log("6. About to call ai.ask() with prompt:", prompt);

  // Let's manually do what AI::ask does with full logging
  Log("7. Building request manually for debugging...");

  String url = baseUrl.append(String("chat/completions"));
  Log("8. URL:", url);

  String authHeader = String("Bearer ").append(apiKey);
  Log("9. Auth header length:", authHeader.length());

  Map headers;
  headers.put("Authorization", authHeader);
  headers.put("Content-Type", "application/json");
  Log("10. Headers created");

  Map userMsg;
  userMsg.put("role", "user");
  userMsg.put("content", prompt);
  Log("11. User message created");

  List messages;
  messages.append(userMsg);
  Log("12. Messages list created, length:", messages.length());

  Map requestBody;
  requestBody.put("model", chatModel);
  requestBody.put("messages", messages);
  Log("13. Request body created");

  String requestJson = requestBody.toJSONString();
  Log("14. Request JSON:");
  Log(requestJson);

  Log("15. Creating WebRequest...");
  {
    WebRequest request(url, nullptr, &headers);
    Log("16. WebRequest created");

    Log("17. Calling doPost (timeout 10 seconds)...");
    WebResponse response = request.doPost(requestBody, 10000);
    Log("18. doPost Exit(1)ed");
    Log("19. Response is NOT nullptr");

    int statusCode = response.getStatusCode();
    Log("20. Status code:", statusCode);

    String statusReason = response.getStatusReason();
    Log("21. Status reason:", statusReason);

    bool succeeded = response.succeeded();
    Log("22. Succeeded:", succeeded ? "true" : "false");

    String bodyStr = response.asString();
    Log("23. Response body length:", bodyStr.length());
    Log("24. Response body:");
    Log(bodyStr);

    if (!succeeded) {
      LogError("25. Request did not succeed");
      Exit(1);
    }

    Log("25. Getting JSON...");
    const Map *jsonResponse = response.asJson();

    if (!jsonResponse) {
      LogError("26. JSON is nullptr!");
      Exit(1);
    }
    Log("26. JSON is NOT nullptr");

    List keys = jsonResponse->keys();
    Log("27. JSON has", keys.length(), "keys");
    for (int i = 0; i < keys.length(); i++) {
      String key = keys.at<String>(i);
      Log("    Key", i, ":", key);
    }

    if (jsonResponse->hasKey("error")) {
      Log("28. JSON has 'error' key!");
      Map errorObj = jsonResponse->get<String, Map>("error");
      if (errorObj.hasKey("message")) {
        String errMsg = errorObj.get<String, String>("message");
        LogError("Error message:", errMsg);
      }
      Exit(1);
    }

    if (!jsonResponse->hasKey("choices")) {
      LogError("28. JSON does not have 'choices' key!");
      Exit(1);
    }
    Log("28. JSON has 'choices' key");

    List choices = jsonResponse->get<String, List>("choices");
    Log("29. Choices list length:", choices.length());

    if (choices.isEmpty()) {
      LogError("30. Choices list is empty!");
      Exit(1);
    }

    Map firstChoice = choices.at<Map>(0);
    Log("30. Got first choice");

    if (!firstChoice.hasKey("message")) {
      LogError("31. First choice does not have 'message' key!");
      Exit(1);
    }

    Map message = firstChoice.get<String, Map>("message");
    Log("31. Got message from first choice");

    if (!message.hasKey("content")) {
      LogError("32. Message does not have 'content' key!");
      Exit(1);
    }

    String content = message.get<String, String>("content");
    Log("32. Got content:", content);

    Log("");
    Log("=== SUCCESS! ===");
    Log("Response:", content);
  } // WebResponse destroyed here

  EnableLoggingToConsole(); // Close log file before exit
  Exit(0);                  // Explicit exit after cleanup
}
