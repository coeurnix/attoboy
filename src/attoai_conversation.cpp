#include "attoai_internal.h"
#include "attostring_internal.h"

namespace attoboy {

Conversation AI::createConversation() {
  if (!impl)
    return Conversation();

  ConversationImpl *convImpl =
      (ConversationImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                    sizeof(ConversationImpl));
  if (!convImpl)
    return Conversation();

  InitializeSRWLock(&convImpl->lock);
  convImpl->refCount = 1;
  convImpl->messages = new List();
  convImpl->ai = new AI(*this);
  convImpl->promptTokensUsed = 0;
  convImpl->responseTokensUsed = 0;

  Conversation conv;
  conv.impl = convImpl;
  return conv;
}

Conversation::Conversation() { impl = nullptr; }

Conversation::Conversation(const Conversation &other) {
  impl = other.impl;
  if (impl) {
    InterlockedIncrement(&impl->refCount);
  }
}

Conversation::~Conversation() {
  if (impl) {
    if (InterlockedDecrement(&impl->refCount) == 0) {
      if (impl->messages)
        delete impl->messages;
      if (impl->ai)
        delete impl->ai;
      HeapFree(GetProcessHeap(), 0, impl);
    }
  }
}

Conversation &Conversation::operator=(const Conversation &other) {
  if (this != &other) {
    if (impl && InterlockedDecrement(&impl->refCount) == 0) {
      if (impl->messages)
        delete impl->messages;
      if (impl->ai)
        delete impl->ai;
      HeapFree(GetProcessHeap(), 0, impl);
    }
    impl = other.impl;
    if (impl) {
      InterlockedIncrement(&impl->refCount);
    }
  }
  return *this;
}

String Conversation::ask(const String &prompt, int timeout) {
  if (!impl || !impl->ai)
    return String();

  String url;
  String authHeader;
  String model;
  List apiMessages;
  int maxTokens = -1;
  bool jsonMode = false;
  AIImpl *aiImpl = nullptr;

  {
    ReadLockGuard lock(&impl->lock);

    url = impl->ai->getBaseUrl().append(String("chat/completions"));
    authHeader = String("Bearer ").append(impl->ai->getAPIKey());
    model = impl->ai->getModel();

    String systemPrompt = impl->ai->getSystemPrompt();
    if (!systemPrompt.isEmpty()) {
      Map systemMsg;
      systemMsg.put("role", "system");
      systemMsg.put("content", systemPrompt);
      apiMessages.append(systemMsg);
    }

    for (int i = 0; i < impl->messages->length(); i++) {
      String messageText = impl->messages->at<String>(i);
      Map msg;
      msg.put("role", (i % 2 == 0) ? "user" : "assistant");
      msg.put("content", messageText);
      apiMessages.append(msg);
    }

    aiImpl = impl->ai->impl;
  }

  Map userMsg;
  userMsg.put("role", "user");
  userMsg.put("content", prompt);
  apiMessages.append(userMsg);

  Map requestBody;
  requestBody.put("model", model);
  requestBody.put("messages", apiMessages);

  if (aiImpl) {
    ReadLockGuard aiLock(&aiImpl->lock);
    maxTokens = aiImpl->maxTokens;
    jsonMode = aiImpl->jsonMode;
  }

  if (maxTokens > 0) {
    requestBody.put("max_completion_tokens", maxTokens);
  }

  if (jsonMode) {
    Map responseFormat;
    responseFormat.put("type", "json_object");
    requestBody.put("response_format", responseFormat);
  }

  Map headers;
  headers.put("Authorization", authHeader);
  headers.put("Content-Type", "application/json");

  WebRequest request(url, Map(), headers);
  WebResponse response = request.doPost(requestBody, timeout);

  if (!response.succeeded()) {
    return String();
  }

  Map jsonResponse = response.asJson();
  if (jsonResponse.isEmpty()) {
    return String();
  }

  if (jsonResponse.hasKey("usage")) {
    Map usage = jsonResponse.get<String, Map>("usage");
    int promptTokens = 0;
    int completionTokens = 0;

    if (usage.hasKey("prompt_tokens")) {
      promptTokens = usage.get<String, int>("prompt_tokens");
    }
    if (usage.hasKey("completion_tokens")) {
      completionTokens = usage.get<String, int>("completion_tokens");
    }

    {
      WriteLockGuard wlock(&impl->lock);
      impl->promptTokensUsed += promptTokens;
      impl->responseTokensUsed += completionTokens;
    }

    if (aiImpl) {
      WriteLockGuard aiWlock(&aiImpl->lock);
      aiImpl->promptTokensUsed += promptTokens;
      aiImpl->responseTokensUsed += completionTokens;
    }
  }

  if (!jsonResponse.hasKey("choices")) {
    return String();
  }

  List choices = jsonResponse.get<String, List>("choices");
  if (choices.isEmpty()) {
    return String();
  }

  Map firstChoice = choices.at<Map>(0);

  if (firstChoice.hasKey("finish_reason") && aiImpl) {
    String finishReason = firstChoice.get<String, String>("finish_reason");
    WriteLockGuard aiWlock(&aiImpl->lock);
    FreeAIString(aiImpl->finishReason);
    aiImpl->finishReason = DuplicateString(finishReason.c_str());
  }

  if (!firstChoice.hasKey("message")) {
    return String();
  }

  Map message = firstChoice.get<String, Map>("message");
  if (!message.hasKey("content")) {
    return String();
  }

  String content = message.get<String, String>("content");

  {
    WriteLockGuard wlock(&impl->lock);
    impl->messages->append(prompt);
    impl->messages->append(content);
  }

  return content;
}

List Conversation::getConversationList() const {
  if (!impl)
    return List();
  ReadLockGuard lock(&impl->lock);
  if (!impl->messages)
    return List();
  return impl->messages->duplicate();
}

bool Conversation::setConversationList(const List &list) {
  if (!impl)
    return false;
  WriteLockGuard lock(&impl->lock);
  if (!impl->messages)
    impl->messages = new List();
  *impl->messages = list.duplicate();
  impl->promptTokensUsed = 0;
  impl->responseTokensUsed = 0;
  return true;
}

Conversation Conversation::duplicate() const {
  if (!impl)
    return Conversation();

  ReadLockGuard lock(&impl->lock);

  ConversationImpl *newImpl =
      (ConversationImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                    sizeof(ConversationImpl));
  if (!newImpl)
    return Conversation();

  InitializeSRWLock(&newImpl->lock);
  newImpl->refCount = 1;
  newImpl->messages = new List(impl->messages->duplicate());
  newImpl->ai = new AI(*impl->ai);
  newImpl->promptTokensUsed = impl->promptTokensUsed;
  newImpl->responseTokensUsed = impl->responseTokensUsed;

  Conversation newConv;
  newConv.impl = newImpl;
  return newConv;
}

AI Conversation::getAI() const {
  if (!impl)
    return AI("", "", "");
  ReadLockGuard lock(&impl->lock);
  if (!impl->ai)
    return AI("", "", "");
  return AI(*impl->ai);
}

int Conversation::getPromptTokensUsed() const {
  if (!impl)
    return 0;
  ReadLockGuard lock(&impl->lock);
  return impl->promptTokensUsed;
}

int Conversation::getResponseTokensUsed() const {
  if (!impl)
    return 0;
  ReadLockGuard lock(&impl->lock);
  return impl->responseTokensUsed;
}

int Conversation::getTotalTokensUsed() const {
  if (!impl)
    return 0;
  ReadLockGuard lock(&impl->lock);
  return impl->promptTokensUsed + impl->responseTokensUsed;
}

} // namespace attoboy
