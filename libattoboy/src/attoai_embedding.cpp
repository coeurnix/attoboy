#include "attoai_internal.h"
#include "attostring_internal.h"

namespace attoboy {

Embedding *AI::createEmbedding(const String &str, int dimensions, int timeout) {
  if (!impl)
    return nullptr;

  String url;
  String authHeader;
  String model;

  {
    ReadLockGuard lock(&impl->lock);
    url = String(impl->baseUrl).append(String("embeddings"));
    authHeader = String("Bearer ").append(String(impl->apiKey));
    model = String(impl->model);
  }

  Map headers;
  headers.put("Authorization", authHeader);
  headers.put("Content-Type", "application/json");

  Map requestBody;
  requestBody.put("model", model);
  requestBody.put("input", str);

  if (dimensions > 0) {
    requestBody.put("dimensions", dimensions);
  }

  WebRequest request(url, nullptr, &headers);
  WebResponse response = request.doPost(requestBody, timeout);

  if (!response.succeeded()) {
    return nullptr;
  }

  const Map *jsonResponse = response.asJson();
  if (!jsonResponse) {
    return nullptr;
  }

  if (jsonResponse->hasKey("usage")) {
    Map usage = jsonResponse->get<String, Map>("usage");
    if (usage.hasKey("prompt_tokens")) {
      int promptTokens = usage.get<String, int>("prompt_tokens");
      WriteLockGuard wlock(&impl->lock);
      impl->promptTokensUsed += promptTokens;
    }
  }

  if (!jsonResponse->hasKey("data")) {
    return nullptr;
  }

  List data = jsonResponse->get<String, List>("data");
  if (data.isEmpty()) {
    return nullptr;
  }

  Map firstEmbedding = data.at<Map>(0);
  if (!firstEmbedding.hasKey("embedding")) {
    return nullptr;
  }

  List embeddingList = firstEmbedding.get<String, List>("embedding");
  if (embeddingList.isEmpty()) {
    return nullptr;
  }

  int embDim = embeddingList.length();

  EmbeddingImpl *embImpl =
      (EmbeddingImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                 sizeof(EmbeddingImpl));
  if (!embImpl) {
    return nullptr;
  }

  InitializeSRWLock(&embImpl->lock);
  embImpl->refCount = 1;
  embImpl->dimensions = embDim;
  embImpl->normComputed = false;
  embImpl->cachedNorm = 0.0f;

  embImpl->data = (float *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                     embDim * sizeof(float));
  if (!embImpl->data) {
    HeapFree(GetProcessHeap(), 0, embImpl);
    return nullptr;
  }

  for (int i = 0; i < embDim; i++) {
    embImpl->data[i] = embeddingList.at<float>(i);
  }

  Embedding *emb = new Embedding();
  emb->impl = embImpl;
  return emb;
}

Embedding::Embedding() { impl = nullptr; }

Embedding::Embedding(const Embedding &other) {
  impl = other.impl;
  if (impl) {
    InterlockedIncrement(&impl->refCount);
  }
}

Embedding::~Embedding() {
  if (impl) {
    if (InterlockedDecrement(&impl->refCount) == 0) {
      if (impl->data)
        HeapFree(GetProcessHeap(), 0, impl->data);
      HeapFree(GetProcessHeap(), 0, impl);
    }
  }
}

Embedding &Embedding::operator=(const Embedding &other) {
  if (this != &other) {
    if (impl && InterlockedDecrement(&impl->refCount) == 0) {
      if (impl->data)
        HeapFree(GetProcessHeap(), 0, impl->data);
      HeapFree(GetProcessHeap(), 0, impl);
    }
    impl = other.impl;
    if (impl) {
      InterlockedIncrement(&impl->refCount);
    }
  }
  return *this;
}

float Embedding::compare(const Embedding &other) const {
  if (!impl || !other.impl)
    return 0.0f;
  if (impl->dimensions != other.impl->dimensions)
    return 0.0f;

  ReadLockGuard lock(&impl->lock);
  ReadLockGuard lock2(&other.impl->lock);

  float dotProduct = 0.0f;
  for (int i = 0; i < impl->dimensions; i++) {
    dotProduct += impl->data[i] * other.impl->data[i];
  }

  if (!impl->normComputed) {
    float sumSq = 0.0f;
    for (int i = 0; i < impl->dimensions; i++) {
      sumSq += impl->data[i] * impl->data[i];
    }
    impl->cachedNorm = sumSq > 0.0f ? 1.0f / Math::sqrt(sumSq) : 0.0f;
    impl->normComputed = true;
  }

  if (!other.impl->normComputed) {
    float sumSq = 0.0f;
    for (int i = 0; i < other.impl->dimensions; i++) {
      sumSq += other.impl->data[i] * other.impl->data[i];
    }
    other.impl->cachedNorm =
        sumSq > 0.0f ? 1.0f / Math::sqrt(sumSq) : 0.0f;
    other.impl->normComputed = true;
  }

  return dotProduct * impl->cachedNorm * other.impl->cachedNorm;
}

const float *Embedding::getRawArray() const {
  if (!impl)
    return nullptr;
  ReadLockGuard lock(&impl->lock);
  return impl->data;
}

int Embedding::getDimensions() const {
  if (!impl)
    return 0;
  ReadLockGuard lock(&impl->lock);
  return impl->dimensions;
}

} // namespace attoboy
