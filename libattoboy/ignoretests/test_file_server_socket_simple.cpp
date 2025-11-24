#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError(ATTO_TEXT("Test failed: "), message);                             \
    errorCount++;                                                              \
  }

void atto_main() {
  Log(ATTO_TEXT("Running simple File server socket test..."));

  // Test 1: Create server socket
  File server(8766);
  TEST(server.isValid(), ATTO_TEXT("Server socket should be valid"));
  TEST(server.isServerSocket(), ATTO_TEXT("Should be identified as server socket"));

  // Test 2: Create client and connect
  File client(String(ATTO_TEXT("127.0.0.1")), 8766);
  TEST(client.isValid(), ATTO_TEXT("Client socket should be valid"));
  TEST(client.isSocket(), ATTO_TEXT("Should be identified as client socket"));

  // Test 3: Accept connection
  File accepted = server.accept();
  TEST(accepted.isValid(), ATTO_TEXT("Accepted client should be valid"));
  TEST(accepted.isSocket(), ATTO_TEXT("Accepted client should be a socket"));

  // Test 4: Send and receive
  String testMsg(ATTO_TEXT("Hello"));
  int written = client.write(testMsg);
  TEST(written > 0, ATTO_TEXT("Client write should succeed"));

  Sleep(100);

  String received = accepted.readAllToString();
  TEST(received.equals(testMsg), ATTO_TEXT("Received data should match sent data"));

  // Cleanup
  client.close();
  accepted.close();
  server.close();

  // Report results
  if (errorCount == 0) {
    Log(ATTO_TEXT("All simple server socket tests passed!"));
    Exit(0);
  } else {
    LogError(errorCount, ATTO_TEXT(" test(s) failed"));
    Exit(errorCount);
  }
}
