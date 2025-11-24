#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError(ATTO_TEXT("Test failed: "), message);                             \
    errorCount++;                                                              \
  }

void atto_main() {
  Log(ATTO_TEXT("Running File server socket tests..."));

  // Test 1: Create server socket
  File server(8765);
  TEST(server.isValid(), ATTO_TEXT("Server socket should be valid"));
  TEST(server.isOpen(), ATTO_TEXT("Server socket should be open"));
  TEST(server.isServerSocket(), ATTO_TEXT("Should be identified as server socket"));
  TEST(!server.isSocket(), ATTO_TEXT("Server socket should not be a client socket"));
  TEST(!server.isRegularFile(), ATTO_TEXT("Server socket should not be a file"));
  TEST(!server.isNamedPipe(), ATTO_TEXT("Server socket should not be a pipe"));
  TEST(server.getPort() == 8765, ATTO_TEXT("Server should report correct port"));

  // Test 2: Create client connection to server
  File client(String(ATTO_TEXT("127.0.0.1")), 8765);
  TEST(client.isValid(), ATTO_TEXT("Client socket should be valid"));
  TEST(client.isOpen(), ATTO_TEXT("Client socket should be open"));
  TEST(client.isSocket(), ATTO_TEXT("Should be identified as client socket"));
  TEST(!client.isServerSocket(), ATTO_TEXT("Client should not be server socket"));

  // Test 3: Accept client connection
  File accepted = server.accept();
  TEST(accepted.isValid(), ATTO_TEXT("Accepted client should be valid"));
  TEST(accepted.isOpen(), ATTO_TEXT("Accepted client should be open"));
  TEST(accepted.isSocket(), ATTO_TEXT("Accepted client should be a socket"));
  TEST(!accepted.isServerSocket(), ATTO_TEXT("Accepted client should not be server socket"));

  // Test 4: Send data from client to accepted connection
  String testMsg(ATTO_TEXT("Hello Server"));
  int written = client.write(testMsg);
  TEST(written > 0, ATTO_TEXT("Client write should succeed"));

  Sleep(100);

  TEST(accepted.hasAvailable(), ATTO_TEXT("Accepted socket should have data available"));
  String received = accepted.readAllToString();
  TEST(received.equals(testMsg), ATTO_TEXT("Received data should match sent data"));

  // Test 5: Send data from accepted connection to client
  String response(ATTO_TEXT("Hello Client"));
  int responseWritten = accepted.write(response);
  TEST(responseWritten > 0, ATTO_TEXT("Server response write should succeed"));

  Sleep(100);

  TEST(client.hasAvailable(), ATTO_TEXT("Client should have response data available"));
  String clientReceived = client.readAllToString();
  TEST(clientReceived.equals(response), ATTO_TEXT("Client should receive server response"));

  // Test 6: Binary data transfer
  unsigned char binData[] = {0x01, 0x02, 0x03, 0xFF, 0xFE};
  Buffer binBuffer(binData, 5);
  int binWritten = client.write(binBuffer);
  TEST(binWritten == 5, ATTO_TEXT("Binary write should succeed"));

  Sleep(100);

  Buffer binReceived = accepted.readAllToBuffer();
  int binLen = 0;
  const unsigned char *binPtr = binReceived.c_ptr(&binLen);
  TEST(binLen == 5, ATTO_TEXT("Received binary size should match"));
  TEST(binPtr && binPtr[0] == 0x01, ATTO_TEXT("Binary data byte 0 should match"));
  TEST(binPtr && binPtr[4] == 0xFE, ATTO_TEXT("Binary data byte 4 should match"));

  // Test 7: Close and cleanup
  client.close();
  TEST(!client.isOpen(), ATTO_TEXT("Client should be closed"));

  accepted.close();
  TEST(!accepted.isOpen(), ATTO_TEXT("Accepted client should be closed"));

  server.close();
  TEST(!server.isOpen(), ATTO_TEXT("Server should be closed"));

  // Test 8: Accept on closed server should fail
  File invalidAccept = server.accept();
  TEST(!invalidAccept.isValid(), ATTO_TEXT("Accept on closed server should fail"));

  // Test 9: Multiple sequential connections
  File server2(8766);
  TEST(server2.isValid(), ATTO_TEXT("Second server socket should be valid"));

  for (int i = 0; i < 3; i++) {
    File tempClient(String(ATTO_TEXT("127.0.0.1")), 8766);
    TEST(tempClient.isValid(), ATTO_TEXT("Sequential client should be valid"));

    File tempAccepted = server2.accept();
    TEST(tempAccepted.isValid(), ATTO_TEXT("Sequential accept should be valid"));

    String msg(ATTO_TEXT("Test"));
    tempClient.write(msg);
    Sleep(50);

    String recv = tempAccepted.readAllToString();
    TEST(recv.equals(msg), ATTO_TEXT("Sequential message should match"));

    tempClient.close();
    tempAccepted.close();
  }

  server2.close();

  // Report results
  if (errorCount == 0) {
    Log(ATTO_TEXT("All server socket tests passed!"));
    Exit(0);
  } else {
    LogError(errorCount, ATTO_TEXT(" test(s) failed"));
    Exit(errorCount);
  }
}
