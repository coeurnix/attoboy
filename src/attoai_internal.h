#pragma once
#include "atto_internal_common.h"
#include "attoboy/attoboy.h"
#include <windows.h>

namespace attoboy {

struct EmbeddingImpl {
  float *data;
  int dimensions;
  float cachedNorm;
  bool normComputed;
  mutable SRWLOCK lock;
  volatile LONG refCount;
};

struct ConversationImpl {
  List *messages;
  class AI *ai;
  int promptTokensUsed;
  int responseTokensUsed;
  mutable SRWLOCK lock;
  volatile LONG refCount;
};

struct AIImpl {
  char *baseUrl;
  char *apiKey;
  char *model;
  char *systemPrompt;
  int maxTokens;
  bool jsonMode;
  int promptTokensUsed;
  int responseTokensUsed;
  char *finishReason;
  mutable SRWLOCK lock;
  volatile LONG refCount;
};

static inline char *AllocAIString(int len) {
  return (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                           (len + 1) * sizeof(char));
}

static inline void FreeAIString(char *str) {
  if (str)
    HeapFree(GetProcessHeap(), 0, str);
}

static inline char *DuplicateString(const char *str) {
  if (!str)
    return nullptr;
  int len = lstrlenA(str);
  char *copy = AllocAIString(len);
  if (copy)
    lstrcpyA(copy, str);
  return copy;
}

} // namespace attoboy
