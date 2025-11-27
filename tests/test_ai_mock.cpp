#include "test_framework.h"

void atto_main() {
  EnableLoggingToConsole();
  Log("=== AI Mock Server Test ===");

  String baseUrl = String("http://localhost:8469/v1/");
  String apiKey = String("mock-api-key-12345");
  String chatModel = String("gpt-5-mini-mock");
  String embeddingModel = String("text-embedding-mock");

  // Test 1: Basic AI.ask()
  {
    Log("Test 1: AI.ask() basic functionality");
    AI ai(baseUrl, apiKey, chatModel);
    REGISTER_TESTED(AI_constructor);

    String response = ai.ask(String("Hello, test!"));
    REGISTER_TESTED(AI_ask);

    if (response.isEmpty()) {
      LogError("Test 1 FAILED: No response received");
      Exit(1);
    }

    Log("Response:", response);
    ASSERT_FALSE(response.isEmpty());
    ASSERT_TRUE(response.contains(String("Mock response")));

    // Check token tracking
    int promptTokens = ai.getPromptTokensUsed();
    int responseTokens = ai.getResponseTokensUsed();
    int totalTokens = ai.getTotalTokensUsed();

    REGISTER_TESTED(AI_getPromptTokensUsed);
    REGISTER_TESTED(AI_getResponseTokensUsed);
    REGISTER_TESTED(AI_getTotalTokensUsed);

    Log("Tokens - Prompt:", promptTokens, "Response:", responseTokens,
        "Total:", totalTokens);
    ASSERT_TRUE(promptTokens > 0);
    ASSERT_TRUE(responseTokens > 0);
    ASSERT_EQ(totalTokens, promptTokens + responseTokens);

    Log("Test 1: PASSED");
  }

  // Test 2: AI.createEmbedding()
  {
    Log("Test 2: AI.createEmbedding()");
    AI ai(baseUrl, apiKey, embeddingModel);

    Embedding emb = ai.createEmbedding(String("test embedding"));
    REGISTER_TESTED(AI_createEmbedding);

    if (emb.getDimensions() == 0) {
      LogError("Test 2 FAILED: No embedding received");
      Exit(1);
    }

    int dims = emb.getDimensions();
    REGISTER_TESTED(Embedding_getDimensions);
    Log("Embedding dimensions:", dims);
    ASSERT_EQ(dims, 1536);

    const float *rawData = emb.getRawArray();
    REGISTER_TESTED(Embedding_getRawArray);
    ASSERT_TRUE(rawData != nullptr);

    Log("Test 2: PASSED");
  }

  // Test 3: Embedding comparison
  {
    Log("Test 3: Embedding.compare()");
    AI ai(baseUrl, apiKey, embeddingModel);

    Embedding emb1 = ai.createEmbedding(String("cat"));
    Embedding emb2 = ai.createEmbedding(String("kitten"));

    if (emb1.getDimensions() == 0 || emb2.getDimensions() == 0) {
      LogError("Test 3 FAILED: Could not create embeddings");
      Exit(1);
    }

    float similarity = emb1.compare(emb2);
    REGISTER_TESTED(Embedding_compare);
    Log("Similarity:", similarity);

    // Mock embeddings are all 0.1, so similarity should be ~1.0 (identical)
    // Allow small floating point tolerance
    ASSERT_TRUE(similarity > 0.99f && similarity < 1.01f);

    Log("Test 3: PASSED");
  }

  // Test 4: AI.createConversation()
  {
    Log("Test 4: Conversation basic functionality");
    AI ai(baseUrl, apiKey, chatModel);

    Conversation conv = ai.createConversation();
    REGISTER_TESTED(AI_createConversation);

    if (conv.getTotalTokensUsed() < 0) {
      LogError("Test 4 FAILED: Could not create conversation");
      Exit(1);
    }

    String response = conv.ask(String("First message"));
    REGISTER_TESTED(Conversation_ask);

    if (response.isEmpty()) {
      LogError("Test 4 FAILED: No conversation response");
      Exit(1);
    }

    Log("Conversation response:", response);
    ASSERT_FALSE(response.isEmpty());

    List messages = conv.getConversationList();
    REGISTER_TESTED(Conversation_getConversationList);
    ASSERT_EQ(messages.length(), 2); // User message + assistant response

    Log("Test 4: PASSED");
  }

  // Mark destructors as tested (implicitly called)
  REGISTER_TESTED(AI_destructor);
  REGISTER_TESTED(Embedding_destructor);
  REGISTER_TESTED(Conversation_destructor);

  Log("");
  Log("=== All AI tests passed! ===");
  TestFramework::DisplayCoverage();

  // Clean exit
  Exit(0);
}
