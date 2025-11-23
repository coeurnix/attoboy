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

  const char *httpRequestStr = "HEAD / HTTP/1.0\r\nHost: example.com\r\n\r\n";
  int httpLen = 0;
  while (httpRequestStr[httpLen]) httpLen++;
  Buffer httpRequest((const unsigned char *)httpRequestStr, httpLen);
  int written = socketFile.write(httpRequest);
  TEST(written > 0, ATTO_TEXT("Should write HTTP request to socket"));

  Sleep(500);

  if (socketFile.hasAvailable()) {
    int available = socketFile.getAvailableCount();
    Log(ATTO_TEXT("Available bytes: "), available);

    Buffer responseBuffer = socketFile.readAllToBuffer();
    TEST(!responseBuffer.isEmpty(), ATTO_TEXT("Should receive HTTP response"));

    int len = 0;
    const unsigned char *data = responseBuffer.c_ptr(&len);
    bool hasHTTP = false;
    bool has200 = false;
    for (int i = 0; i < len - 3; i++) {
      if (data[i] == 'H' && data[i+1] == 'T' && data[i+2] == 'T' && data[i+3] == 'P') hasHTTP = true;
      if (data[i] == '2' && data[i+1] == '0' && data[i+2] == '0') has200 = true;
    }
    TEST(hasHTTP || has200,
         ATTO_TEXT("Response should contain HTTP status"));
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
