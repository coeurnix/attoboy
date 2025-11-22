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
  Log(ATTO_TEXT("Running File socket tests (connecting to example.com:80)..."));

  File socketFile(String(ATTO_TEXT("example.com")), 80);

  TEST(socketFile.isValid(), ATTO_TEXT("Socket should be valid"));
  TEST(socketFile.isOpen(), ATTO_TEXT("Socket should be open"));
  TEST(socketFile.isSocket(), ATTO_TEXT("Should be identified as socket"));
  TEST(!socketFile.isRegularFile(), ATTO_TEXT("Should not be regular file"));
  TEST(!socketFile.isNamedPipe(), ATTO_TEXT("Should not be named pipe"));

  const ATTO_CHAR *host = socketFile.getHost();
  TEST(host != nullptr, ATTO_TEXT("Socket should have host"));

  int retrievedPort = socketFile.getPort();
  TEST(retrievedPort == 80, ATTO_TEXT("Socket should return correct port"));

  TEST(socketFile.getPath() == nullptr,
       ATTO_TEXT("Socket should not have path"));

  String httpRequest(ATTO_TEXT("HEAD / HTTP/1.0\r\nHost: example.com\r\n\r\n"));
  int written = socketFile.write(httpRequest);
  TEST(written > 0, ATTO_TEXT("Should write HTTP request to socket"));

  Sleep(500);

  if (socketFile.hasAvailable()) {
    int available = socketFile.getAvailableCount();
    Log(ATTO_TEXT("Available bytes: "), available);

    String response = socketFile.readAllToString();
    TEST(!response.isEmpty(), ATTO_TEXT("Should receive HTTP response"));
    TEST(response.contains(ATTO_TEXT("HTTP")) ||
             response.contains(ATTO_TEXT("200")),
         ATTO_TEXT("Response should contain HTTP status"));
    Log(ATTO_TEXT("Received response starting with: "),
        response.substring(0, 50));
  } else {
    LogError(ATTO_TEXT("No data available from server"));
    errorCount++;
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
