#include "attoai_internal.h"
#include "attostring_internal.h"

namespace attoboy {

String AI::ask(const String &prompt, int timeout) {
  if (!impl)
    return String();

  String url;
  String authHeader;
  String model;
  char *systemPrompt = nullptr;
  int maxTokens;
  bool jsonMode;

  {
    ReadLockGuard lock(&impl->lock);
    url = String(impl->baseUrl).append(String("chat/completions"));
    authHeader = String("Bearer ").append(String(impl->apiKey));
    model = String(impl->model);
    if (impl->systemPrompt) {
      systemPrompt = DuplicateString(impl->systemPrompt);
    }
    maxTokens = impl->maxTokens;
    jsonMode = impl->jsonMode;
  }

  Map headers;
  headers.put("Authorization", authHeader);
  headers.put("Content-Type", "application/json");

  List messages;
  if (systemPrompt) {
    Map systemMsg;
    systemMsg.put("role", "system");
    systemMsg.put("content", String(systemPrompt));
    messages.append(systemMsg);
    FreeAIString(systemPrompt);
  }

  Map userMsg;
  userMsg.put("role", "user");
  userMsg.put("content", prompt);
  messages.append(userMsg);

  Map requestBody;
  requestBody.put("model", model);
  requestBody.put("messages", messages);

  if (maxTokens > 0) {
    requestBody.put("max_completion_tokens", maxTokens);
  }

  if (jsonMode) {
    Map responseFormat;
    responseFormat.put("type", "json_object");
    requestBody.put("response_format", responseFormat);
  }

  WebRequest request(url, nullptr, &headers);
  WebResponse response = request.doPost(requestBody, timeout);

  if (!response.succeeded()) {
    return String();
  }

  const Map *jsonResponse = response.asJson();
  if (!jsonResponse) {
    return String();
  }

  if (jsonResponse->hasKey("usage")) {
    Map usage = jsonResponse->get<String, Map>("usage");
    WriteLockGuard wlock(&impl->lock);
    if (usage.hasKey("prompt_tokens")) {
      int promptTokens = usage.get<String, int>("prompt_tokens");
      impl->promptTokensUsed += promptTokens;
    }
    if (usage.hasKey("completion_tokens")) {
      int completionTokens = usage.get<String, int>("completion_tokens");
      impl->responseTokensUsed += completionTokens;
    }
  }

  if (!jsonResponse->hasKey("choices")) {
    return String();
  }

  List choices = jsonResponse->get<String, List>("choices");
  if (choices.isEmpty()) {
    return String();
  }

  Map firstChoice = choices.at<Map>(0);

  if (firstChoice.hasKey("finish_reason")) {
    String finishReason = firstChoice.get<String, String>("finish_reason");
    WriteLockGuard wlock(&impl->lock);
    FreeAIString(impl->finishReason);
    impl->finishReason = DuplicateString(finishReason.c_str());
  }

  if (!firstChoice.hasKey("message")) {
    return String();
  }

  Map message = firstChoice.get<String, Map>("message");
  if (!message.hasKey("content")) {
    return String();
  }

  String content = message.get<String, String>("content");
  return content;
}

} // namespace attoboy
