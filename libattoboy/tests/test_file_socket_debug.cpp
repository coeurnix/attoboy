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
  Log(ATTO_TEXT("Running File socket tests (debug version)..."));

  Arguments args;
  args.addPositionalParameter(ATTO_TEXT("port"), ATTO_TEXT("Server port"));
  Map argMap = args.parseArguments(true);

  String portStr = args.getArgument(ATTO_TEXT("port"));
  if (portStr.isEmpty()) {
    LogError(ATTO_TEXT("Port argument required"));
    Exit(1);
  }

  int port = portStr.toInteger();
  if (port <= 0) {
    LogError(ATTO_TEXT("Invalid port number"));
    Exit(1);
  }

  Log(ATTO_TEXT("Connecting to 127.0.0.1:"), port);
  File socketFile(String(ATTO_TEXT("127.0.0.1")), port);

  TEST(socketFile.isValid(), ATTO_TEXT("Socket should be valid"));
  TEST(socketFile.isOpen(), ATTO_TEXT("Socket should be open"));
  TEST(socketFile.isSocket(), ATTO_TEXT("Should be identified as socket"));
  TEST(!socketFile.isRegularFile(), ATTO_TEXT("Should not be regular file"));
  TEST(!socketFile.isNamedPipe(), ATTO_TEXT("Should not be named pipe"));

  const ATTO_CHAR *host = socketFile.getHost();
  TEST(host != nullptr, ATTO_TEXT("Socket should have host"));
  if (host) {
    Log(ATTO_TEXT("Host: "), host);
  }

  int retrievedPort = socketFile.getPort();
  TEST(retrievedPort == port, ATTO_TEXT("Socket should return correct port"));
  Log(ATTO_TEXT("Port: "), retrievedPort);

  TEST(socketFile.getPath() == nullptr,
       ATTO_TEXT("Socket should not have path"));

  String testMessage(ATTO_TEXT("Hello, Server!"));
  Log(ATTO_TEXT("Writing message: "), testMessage);
  int written = socketFile.write(testMessage);
  Log(ATTO_TEXT("Bytes written: "), written);
  TEST(written > 0, ATTO_TEXT("Should write data to socket"));

  Log(ATTO_TEXT("Waiting 200ms for response..."));
  Sleep(200);

  Log(ATTO_TEXT("Checking for available data..."));
  bool hasData = socketFile.hasAvailable();
  Log(ATTO_TEXT("Has available: "), hasData ? ATTO_TEXT("yes") : ATTO_TEXT("no"));

  if (hasData) {
    int available = socketFile.getAvailableCount();
    Log(ATTO_TEXT("Available bytes: "), available);

    String response = socketFile.readAllToString();
    Log(ATTO_TEXT("Response: "), response);
    TEST(!response.isEmpty(), ATTO_TEXT("Should receive response from server"));
    TEST(response.contains(ATTO_TEXT("received")) ||
             response.contains(ATTO_TEXT("Server")),
         ATTO_TEXT("Response should contain expected text"));
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
