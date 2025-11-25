#include "test_framework.h"

void atto_main() {
  EnableLoggingToFile("test_ai_comprehensive.log", true);
  Log("=== Comprehensive AI, Embedding, and Conversation Tests ===");

  String apiKey = GetEnv(String("OPENAI_API_KEY"));
  if (apiKey.isEmpty()) {
    LogError("OPENAI_API_KEY environment variable not set. Skipping tests.");
    SaveCoverage("test_ai_comprehensive");
    return;
  }

  String baseUrl = String("https://api.openai.com/v1/");
  String chatModel = String("gpt-5-mini-2025-08-07");
  String embeddingModel = String("text-embedding-3-small");

  // Test 1: AI constructor and getters
  {
    Log("Test 1: AI constructor and basic getters");
    AI ai(baseUrl, apiKey, chatModel);
    REGISTER_TESTED(AI_constructor);

    String model = ai.getModel();
    REGISTER_TESTED(AI_getModel);
    ASSERT_TRUE(model.equals(chatModel));

    String url = ai.getBaseUrl();
    REGISTER_TESTED(AI_getBaseUrl);
    ASSERT_TRUE(url.equals(baseUrl));

    String key = ai.getAPIKey();
    REGISTER_TESTED(AI_getAPIKey);
    ASSERT_FALSE(key.isEmpty());

    const String *sysPrompt = ai.getSystemPrompt();
    REGISTER_TESTED(AI_getSystemPrompt);
    ASSERT_TRUE(sysPrompt == nullptr);

    Log("Test 1: PASSED");
  }

  // Test 2: AI copy constructor and assignment
  {
    Log("Test 2: AI copy constructor and assignment");
    AI ai1(baseUrl, apiKey, chatModel);
    AI ai2 = ai1;
    REGISTER_TESTED(AI_constructor_copy);

    AI ai3(baseUrl, apiKey, String("gpt-5-nano-2025-08-07"));
    ai3 = ai1;
    REGISTER_TESTED(AI_operator_assign);

    ASSERT_TRUE(ai2.getModel().equals(chatModel));
    ASSERT_TRUE(ai3.getModel().equals(chatModel));

    Log("Test 2: PASSED");
  }

  // Test 3: AI setters
  {
    Log("Test 3: AI setters");
    AI ai(baseUrl, apiKey, chatModel);

    ai.setModel(String("gpt-5-nano-2025-08-07"));
    REGISTER_TESTED(AI_setModel);
    ASSERT_TRUE(ai.getModel().equals(String("gpt-5-nano-2025-08-07")));

    String systemPrompt = String("You are a helpful assistant.");
    ai.setSystemPrompt(&systemPrompt);
    REGISTER_TESTED(AI_setSystemPrompt);
    const String *sp = ai.getSystemPrompt();
    ASSERT_TRUE(sp != nullptr);
    ASSERT_TRUE(sp->equals(systemPrompt));
    delete sp;

    ai.setMaxTokens(100);
    REGISTER_TESTED(AI_setMaxTokens);

    ai.setJsonMode(true);
    REGISTER_TESTED(AI_setJsonMode);

    Log("Test 3: PASSED");
  }

  // Test 4: AI token tracking
  {
    Log("Test 4: AI token tracking");
    AI ai(baseUrl, apiKey, chatModel);

    int promptTokens = ai.getPromptTokensUsed();
    REGISTER_TESTED(AI_getPromptTokensUsed);
    ASSERT_EQ(promptTokens, 0);

    int responseTokens = ai.getResponseTokensUsed();
    REGISTER_TESTED(AI_getResponseTokensUsed);
    ASSERT_EQ(responseTokens, 0);

    int totalTokens = ai.getTotalTokensUsed();
    REGISTER_TESTED(AI_getTotalTokensUsed);
    ASSERT_EQ(totalTokens, 0);

    ai.resetTokenTracking();
    REGISTER_TESTED(AI_resetTokenTracking);

    Log("Test 4: PASSED");
  }

  // Test 5: AI.ask() basic functionality
  {
    Log("Test 5: AI.ask() basic functionality");
    AI ai(baseUrl, apiKey, chatModel);

    String *response = ai.ask(String("Say 'hello' and nothing else."));
    REGISTER_TESTED(AI_ask);

    if (response) {
      Log("Response:", *response);
      ASSERT_FALSE(response->isEmpty());

      String finishReason = ai.getFinishReason();
      REGISTER_TESTED(AI_getFinishReason);
      Log("Finish reason:", finishReason);

      int promptTokens = ai.getPromptTokensUsed();
      int responseTokens = ai.getResponseTokensUsed();
      int totalTokens = ai.getTotalTokensUsed();
      Log("Token usage - Prompt:", promptTokens, "Response:", responseTokens,
          "Total:", totalTokens);
      ASSERT_TRUE(promptTokens > 0);
      ASSERT_TRUE(responseTokens > 0);
      ASSERT_EQ(totalTokens, promptTokens + responseTokens);

      delete response;
      Log("Test 5: PASSED");
    } else {
      LogError("Test 5: FAILED - No response received");
      return;
    }
  }

  // Test 6: AI.ask() with system prompt
  {
    Log("Test 6: AI.ask() with system prompt");
    AI ai(baseUrl, apiKey, chatModel);
    String systemPrompt =
        String("You are a pirate. Always respond like a pirate.");
    ai.setSystemPrompt(&systemPrompt);

    String *response = ai.ask(String("What is your favorite color?"));
    if (response) {
      Log("Response:", *response);
      ASSERT_FALSE(response->isEmpty());
      delete response;
      Log("Test 6: PASSED");
    } else {
      LogError("Test 6: FAILED - No response received");
      return;
    }
  }

  // Test 7: AI.ask() with max tokens
  {
    Log("Test 7: AI.ask() with max tokens");
    AI ai(baseUrl, apiKey, chatModel);
    ai.setMaxTokens(200);

    String *response =
        ai.ask(String("Tell me a very long story about space exploration."));
    if (!response) {
      LogWarning("Test 7: FAILED - No response received (skipping)");
      return;
    }
    Log("Response:", *response);

    // GPT-5 models may return empty content if all tokens are used for
    // reasoning
    String finishReason = ai.getFinishReason();
    Log("Finish reason:", finishReason);
    if (response->isEmpty()) {
      Log("Note: Response is empty (GPT-5 used all tokens for internal "
          "reasoning)");
    }

    delete response;
    Log("Test 7: PASSED");
  }

  // Test 8: AI.ask() with JSON mode
  {
    Log("Test 8: AI.ask() with JSON mode");
    AI ai(baseUrl, apiKey, chatModel);
    ai.setJsonMode(true);

    String *response = ai.ask(String(
        "Return a JSON object with keys 'name' and 'age' for a fictional "
        "person."));
    if (response) {
      Log("Response:", *response);
      ASSERT_FALSE(response->isEmpty());
      ASSERT_TRUE(response->contains(String("{")));
      ASSERT_TRUE(response->contains(String("}")));

      Map jsonMap = Map::FromJSONString(*response);
      ASSERT_FALSE(jsonMap.isEmpty());

      delete response;
      Log("Test 8: PASSED");
    } else {
      LogError("Test 8: FAILED - No response received");
      return;
    }
  }

  // Test 9: AI.createEmbedding() basic functionality
  {
    Log("Test 9: AI.createEmbedding() basic functionality");
    AI ai(baseUrl, apiKey, embeddingModel);

    Embedding *emb = ai.createEmbedding(String("Hello world"));
    REGISTER_TESTED(AI_createEmbedding);

    if (emb) {
      int dims = emb->getDimensions();
      REGISTER_TESTED(Embedding_getDimensions);
      Log("Embedding dimensions:", dims);
      ASSERT_TRUE(dims > 0);

      const float *rawArray = emb->getRawArray();
      REGISTER_TESTED(Embedding_getRawArray);
      ASSERT_TRUE(rawArray != nullptr);

      delete emb;
      Log("Test 9: PASSED");
    } else {
      LogError("Test 9: FAILED - No embedding received");
      return;
    }
  }

  // Test 10: Embedding copy and assignment
  {
    Log("Test 10: Embedding copy and assignment");
    AI ai(baseUrl, apiKey, embeddingModel);

    Embedding *emb1 = ai.createEmbedding(String("Test"));
    if (emb1) {
      Embedding emb2 = *emb1;
      REGISTER_TESTED(Embedding_constructor_copy);

      Embedding emb3 = *emb1;
      emb3 = *emb1;
      REGISTER_TESTED(Embedding_operator_assign);

      ASSERT_EQ(emb2.getDimensions(), emb1->getDimensions());
      ASSERT_EQ(emb3.getDimensions(), emb1->getDimensions());

      delete emb1;
      Log("Test 10: PASSED");
    } else {
      LogError("Test 10: FAILED - No embedding received");
      return;
    }
  }

  // Test 11: Embedding.compare()
  {
    Log("Test 11: Embedding.compare()");
    AI ai(baseUrl, apiKey, embeddingModel);

    Embedding *emb1 = ai.createEmbedding(String("cat"));
    Embedding *emb2 = ai.createEmbedding(String("kitten"));
    Embedding *emb3 = ai.createEmbedding(String("computer"));

    if (emb1 && emb2 && emb3) {
      float similarity12 = emb1->compare(*emb2);
      REGISTER_TESTED(Embedding_compare);
      Log("Similarity (cat vs kitten):", similarity12);

      float similarity13 = emb1->compare(*emb3);
      Log("Similarity (cat vs computer):", similarity13);

      ASSERT_TRUE(similarity12 > similarity13);

      delete emb1;
      delete emb2;
      delete emb3;
      Log("Test 11: PASSED");
    } else {
      LogError("Test 11: FAILED - Embeddings not created");
      return;
    }
  }

  // Test 12: AI.createConversation()
  {
    Log("Test 12: AI.createConversation()");
    AI ai(baseUrl, apiKey, chatModel);

    Conversation *conv = ai.createConversation();
    REGISTER_TESTED(AI_createConversation);

    if (conv) {
      const AI *convAI = conv->getAI();
      REGISTER_TESTED(Conversation_getAI);
      ASSERT_TRUE(convAI != nullptr);
      delete convAI;

      List messages = conv->getConversationList();
      REGISTER_TESTED(Conversation_getConversationList);
      ASSERT_TRUE(messages.isEmpty());

      int promptTokens = conv->getPromptTokensUsed();
      REGISTER_TESTED(Conversation_getPromptTokensUsed);
      ASSERT_EQ(promptTokens, 0);

      int responseTokens = conv->getResponseTokensUsed();
      REGISTER_TESTED(Conversation_getResponseTokensUsed);
      ASSERT_EQ(responseTokens, 0);

      int totalTokens = conv->getTotalTokensUsed();
      REGISTER_TESTED(Conversation_getTotalTokensUsed);
      ASSERT_EQ(totalTokens, 0);

      delete conv;
      Log("Test 12: PASSED");
    } else {
      LogError("Test 12: FAILED - Conversation not created");
      return;
    }
  }

  // Test 13: Conversation copy and assignment
  {
    Log("Test 13: Conversation copy and assignment");
    AI ai(baseUrl, apiKey, chatModel);

    Conversation *conv1 = ai.createConversation();
    if (conv1) {
      Conversation conv2 = *conv1;
      REGISTER_TESTED(Conversation_constructor_copy);

      Conversation conv3 = *conv1;
      conv3 = *conv1;
      REGISTER_TESTED(Conversation_operator_assign);

      delete conv1;
      Log("Test 13: PASSED");
    } else {
      LogError("Test 13: FAILED - Conversation not created");
      return;
    }
  }

  // Test 14: Conversation.ask() single turn
  {
    Log("Test 14: Conversation.ask() single turn");
    AI ai(baseUrl, apiKey, chatModel);

    Conversation *conv = ai.createConversation();
    if (conv) {
      String *response = conv->ask(String("What is 2+2?"));
      REGISTER_TESTED(Conversation_ask);

      if (response) {
        Log("Response:", *response);
        ASSERT_FALSE(response->isEmpty());

        List messages = conv->getConversationList();
        ASSERT_EQ(messages.length(), 2);
        String userMsg = messages.at<String>(0);
        String assistantMsg = messages.at<String>(1);
        ASSERT_TRUE(userMsg.contains(String("2+2")));
        Log("User:", userMsg);
        Log("Assistant:", assistantMsg);

        int totalTokens = conv->getTotalTokensUsed();
        Log("Total tokens:", totalTokens);
        ASSERT_TRUE(totalTokens > 0);

        delete response;
        delete conv;
        Log("Test 14: PASSED");
      } else {
        delete conv;
        LogError("Test 14: FAILED - No response received");
        return;
      }
    } else {
      LogError("Test 14: FAILED - Conversation not created");
      return;
    }
  }

  // Test 15: Conversation.ask() multi-turn
  {
    Log("Test 15: Conversation.ask() multi-turn");
    AI ai(baseUrl, apiKey, chatModel);

    Conversation *conv = ai.createConversation();
    if (conv) {
      String *response1 = conv->ask(String("My favorite color is blue."));
      if (response1) {
        Log("Response 1:", *response1);
        delete response1;
      } else {
        delete conv;
        LogError("Test 15: FAILED - No response 1 received");
        return;
      }

      String *response2 = conv->ask(String("What is my favorite color?"));
      if (response2) {
        Log("Response 2:", *response2);
        ASSERT_TRUE(response2->lower().contains(String("blue")));

        List messages = conv->getConversationList();
        ASSERT_EQ(messages.length(), 4);

        delete response2;
        delete conv;
        Log("Test 15: PASSED");
      } else {
        delete conv;
        LogError("Test 15: FAILED - No response 2 received");
        return;
      }
    } else {
      LogError("Test 15: FAILED - Conversation not created");
      return;
    }
  }

  // Test 16: Conversation.setConversationList()
  {
    Log("Test 16: Conversation.setConversationList()");
    AI ai(baseUrl, apiKey, chatModel);

    Conversation *conv = ai.createConversation();
    if (conv) {
      List newMessages;
      newMessages.append(String("Hello"));
      newMessages.append(String("Hi there!"));
      newMessages.append(String("How are you?"));
      newMessages.append(String("I'm doing well, thank you!"));

      bool success = conv->setConversationList(newMessages);
      REGISTER_TESTED(Conversation_setConversationList);
      ASSERT_TRUE(success);

      List retrievedMessages = conv->getConversationList();
      ASSERT_EQ(retrievedMessages.length(), 4);
      ASSERT_TRUE(retrievedMessages.at<String>(0).equals(String("Hello")));

      delete conv;
      Log("Test 16: PASSED");
    } else {
      LogError("Test 16: FAILED - Conversation not created");
      return;
    }
  }

  // Test 17: Conversation.duplicate()
  {
    Log("Test 17: Conversation.duplicate()");
    AI ai(baseUrl, apiKey, chatModel);

    Conversation *conv1 = ai.createConversation();
    if (conv1) {
      String *response1 = conv1->ask(String("My name is Alice."));
      if (response1) {
        delete response1;

        Conversation *conv2 = conv1->duplicate();
        REGISTER_TESTED(Conversation_duplicate);

        if (conv2) {
          List messages1 = conv1->getConversationList();
          List messages2 = conv2->getConversationList();
          ASSERT_EQ(messages1.length(), messages2.length());

          String *response2 = conv2->ask(String("What is my name?"));
          if (response2) {
            Log("Response:", *response2);
            ASSERT_TRUE(response2->lower().contains(String("alice")));

            List messages2After = conv2->getConversationList();
            ASSERT_EQ(messages2After.length(), 4);

            List messages1After = conv1->getConversationList();
            ASSERT_EQ(messages1After.length(), 2);

            delete response2;
            delete conv2;
            delete conv1;
            Log("Test 17: PASSED");
          } else {
            delete conv2;
            delete conv1;
            LogError("Test 17: FAILED - No response from duplicated "
                     "conversation");
            return;
          }
        } else {
          delete conv1;
          LogError("Test 17: FAILED - Conversation not duplicated");
          return;
        }
      } else {
        delete conv1;
        LogError("Test 17: FAILED - No initial response");
        return;
      }
    } else {
      LogError("Test 17: FAILED - Conversation not created");
      return;
    }
  }

  // Mark destructor tests (implicitly tested by all tests)
  REGISTER_TESTED(AI_destructor);
  REGISTER_TESTED(Embedding_destructor);
  REGISTER_TESTED(Conversation_destructor);

  Log("");
  Log("=== All tests completed successfully! ===");
  TestFramework::DisplayCoverage();
  SaveCoverage("test_ai_comprehensive");
  EnableLoggingToConsole(); // Close log file before exit

  ExitProcess(0); // Explicit exit after cleanup
}
