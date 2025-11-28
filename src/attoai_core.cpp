#include "attoai_internal.h"
#include "attostring_internal.h"

namespace attoboy {

static String NormalizeBaseUrl(const String &url) {
  String normalized = url;
  if (normalized.isEmpty())
    return String("https://api.openai.com/v1/");

  if (!normalized.endsWith(String("/")))
    normalized = normalized.append(String("/"));

  return normalized;
}

AI::AI(const String &baseUrl, const String &apiKey, const String &model) {
  impl = (AIImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                             sizeof(AIImpl));
  if (!impl)
    return;
  InitializeSRWLock(&impl->lock);
  impl->refCount = 1;

  String normalizedUrl = NormalizeBaseUrl(baseUrl);
  impl->baseUrl = DuplicateString(normalizedUrl.c_str());
  impl->apiKey = DuplicateString(apiKey.c_str());
  impl->model = DuplicateString(model.c_str());
  impl->systemPrompt = nullptr;
  impl->maxTokens = -1;
  impl->jsonMode = false;
  impl->promptTokensUsed = 0;
  impl->responseTokensUsed = 0;
  impl->finishReason = nullptr;
}

AI::AI(const AI &other) {
  impl = other.impl;
  if (impl) {
    InterlockedIncrement(&impl->refCount);
  }
}

AI::~AI() {
  if (impl) {
    if (InterlockedDecrement(&impl->refCount) == 0) {
      FreeAIString(impl->baseUrl);
      FreeAIString(impl->apiKey);
      FreeAIString(impl->model);
      FreeAIString(impl->systemPrompt);
      FreeAIString(impl->finishReason);
      HeapFree(GetProcessHeap(), 0, impl);
    }
  }
}

AI &AI::operator=(const AI &other) {
  if (this != &other) {
    if (impl && InterlockedDecrement(&impl->refCount) == 0) {
      FreeAIString(impl->baseUrl);
      FreeAIString(impl->apiKey);
      FreeAIString(impl->model);
      FreeAIString(impl->systemPrompt);
      FreeAIString(impl->finishReason);
      HeapFree(GetProcessHeap(), 0, impl);
    }
    impl = other.impl;
    if (impl) {
      InterlockedIncrement(&impl->refCount);
    }
  }
  return *this;
}

AI &AI::setModel(const String &model) {
  if (!impl)
    return *this;
  WriteLockGuard lock(&impl->lock);
  FreeAIString(impl->model);
  impl->model = DuplicateString(model.c_str());
  return *this;
}

AI &AI::setSystemPrompt(const String &prompt) {
  if (!impl)
    return *this;
  WriteLockGuard lock(&impl->lock);
  FreeAIString(impl->systemPrompt);
  impl->systemPrompt = prompt.isEmpty() ? nullptr : DuplicateString(prompt.c_str());
  return *this;
}

AI &AI::setMaxTokens(int max) {
  if (!impl)
    return *this;
  WriteLockGuard lock(&impl->lock);
  impl->maxTokens = max;
  return *this;
}

AI &AI::setJsonMode(bool isJsonMode) {
  if (!impl)
    return *this;
  WriteLockGuard lock(&impl->lock);
  impl->jsonMode = isJsonMode;
  return *this;
}

String AI::getModel() const {
  if (!impl)
    return String();
  ReadLockGuard lock(&impl->lock);
  return String(impl->model);
}

String AI::getSystemPrompt() const {
  if (!impl)
    return String();
  ReadLockGuard lock(&impl->lock);
  if (!impl->systemPrompt)
    return String();
  return String(impl->systemPrompt);
}

String AI::getBaseUrl() const {
  if (!impl)
    return String();
  ReadLockGuard lock(&impl->lock);
  return String(impl->baseUrl);
}

String AI::getAPIKey() const {
  if (!impl)
    return String();
  ReadLockGuard lock(&impl->lock);
  return String(impl->apiKey);
}

int AI::getPromptTokensUsed() const {
  if (!impl)
    return 0;
  ReadLockGuard lock(&impl->lock);
  return impl->promptTokensUsed;
}

int AI::getResponseTokensUsed() const {
  if (!impl)
    return 0;
  ReadLockGuard lock(&impl->lock);
  return impl->responseTokensUsed;
}

int AI::getTotalTokensUsed() const {
  if (!impl)
    return 0;
  ReadLockGuard lock(&impl->lock);
  return impl->promptTokensUsed + impl->responseTokensUsed;
}

void AI::resetTokenTracking() {
  if (!impl)
    return;
  WriteLockGuard lock(&impl->lock);
  impl->promptTokensUsed = 0;
  impl->responseTokensUsed = 0;
}

String AI::getFinishReason() const {
  if (!impl)
    return String();
  ReadLockGuard lock(&impl->lock);
  if (!impl->finishReason)
    return String();
  return String(impl->finishReason);
}

} // namespace attoboy
