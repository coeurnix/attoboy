#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  do {                                                                         \
    if (!(condition)) {                                                        \
      LogError(ATTO_TEXT("FAILED: "), message);                               \
      errorCount++;                                                            \
    } else {                                                                   \
      Log(ATTO_TEXT("PASSED: "), message);                                    \
    }                                                                          \
  } while (0)

void atto_main() {
  Log(ATTO_TEXT("Running complete File socket tests (port 12357)..."));

  const int port = 12357;

  File socketFile(String(ATTO_TEXT("127.0.0.1")), port);

  TEST(socketFile.isValid(), ATTO_TEXT("Socket should be valid"));
  TEST(socketFile.isOpen(), ATTO_TEXT("Socket should be open"));
  TEST(socketFile.isSocket(), ATTO_TEXT("Should be identified as socket"));
  TEST(!socketFile.isRegularFile(), ATTO_TEXT("Should not be regular file"));
  TEST(!socketFile.isNamedPipe(), ATTO_TEXT("Should not be named pipe"));

  const ATTO_CHAR *host = socketFile.getHost();
  TEST(host != nullptr, ATTO_TEXT("Socket should have host"));

  int retrievedPort = socketFile.getPort();
  TEST(retrievedPort == port, ATTO_TEXT("Socket should return correct port"));

  TEST(socketFile.getPath() == nullptr,
       ATTO_TEXT("Socket should not have path"));

  String testMessage(ATTO_TEXT("Hello, Server!"));
  int written = socketFile.write(testMessage);
  TEST(written > 0, ATTO_TEXT("Should write data to socket"));

  Sleep(200);

  if (socketFile.hasAvailable()) {
    int available = socketFile.getAvailableCount();
    Log(ATTO_TEXT("Available bytes: "), available);

    String response = socketFile.readAllToString();
    TEST(!response.isEmpty(), ATTO_TEXT("Should receive response from server"));
    TEST(response.contains(ATTO_TEXT("received")) ||
             response.contains(ATTO_TEXT("Server")),
         ATTO_TEXT("Response should contain expected text"));
    Log(ATTO_TEXT("Received: "), response);
  } else {
    LogError(ATTO_TEXT("No data available from server"));
  }

  socketFile.close();
  TEST(!socketFile.isOpen(), ATTO_TEXT("Socket should be closed"));

  if (errorCount == 0) {
    Log(ATTO_TEXT("All socket tests passed!"));
    Exit(0);
  } else {
    LogError(errorCount, ATTO_TEXT(" test(s) failed"));
    Exit(errorCount);
  }
}
