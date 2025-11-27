#include "test_framework.h"

void atto_main() {
  EnableLoggingToFile("test_embedding_similarity.log", true);
  Log("=== Embedding Similarity Test ===");

  String baseUrl = String("http://localhost:8469/v1/");
  String apiKey = String("mock-api-key-12345");
  String embeddingModel = String("text-embedding-mock");

  AI ai(baseUrl, apiKey, embeddingModel);

  Log("Creating embedding 1...");
  Embedding emb1 = ai.createEmbedding(String("cat"));
  if (emb1.getDimensions() == 0) {
    LogError("Failed to create embedding 1");
    EnableLoggingToConsole();
    ExitProcess(1);
  }
  Log("Embedding 1 created");

  Log("Creating embedding 2...");
  Embedding emb2 = ai.createEmbedding(String("kitten"));
  if (emb2.getDimensions() == 0) {
    LogError("Failed to create embedding 2");
    EnableLoggingToConsole();
    ExitProcess(1);
  }
  Log("Embedding 2 created");

  float similarity = emb1.compare(emb2);
  Log("Similarity between 'cat' and 'kitten':", similarity);

  // Check if it's in range
  if (similarity > 0.99f && similarity <= 1.0f) {
    Log("Similarity is in expected range (>0.99 and <=1.0)");
  } else {
    LogError("Similarity is OUT OF RANGE! Expected >0.99 and <=1.0");
  }

  EnableLoggingToConsole();
  ExitProcess(0);
}
