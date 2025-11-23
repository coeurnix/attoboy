#include "attoboy/attoboy.h"

using namespace attoboy;

void atto_main() {
  EnableLoggingToFile(ATTO_TEXT("test_server.log"));

  Log(ATTO_TEXT("Test 1: Create server socket"));
  File server(8767);
  Log(ATTO_TEXT("Server created, valid="), server.isValid());
  Log(ATTO_TEXT("Is server socket="), server.isServerSocket());

  if (!server.isValid()) {
    LogError(ATTO_TEXT("FAIL: Server not valid"));
    Exit(1);
  }

  Log(ATTO_TEXT("Test 2: Create client"));
  File client(String(ATTO_TEXT("127.0.0.1")), 8767);
  Log(ATTO_TEXT("Client created, valid="), client.isValid());

  if (!client.isValid()) {
    LogError(ATTO_TEXT("FAIL: Client not valid"));
    Exit(1);
  }

  Log(ATTO_TEXT("Test 3: Accept connection"));
  File accepted = server.accept();
  Log(ATTO_TEXT("Accept returned, valid="), accepted.isValid());

  if (!accepted.isValid()) {
    LogError(ATTO_TEXT("FAIL: Accept failed"));
    Exit(1);
  }

  Log(ATTO_TEXT("Test 4: Send message"));
  String msg(ATTO_TEXT("Test"));
  int written = client.write(msg);
  Log(ATTO_TEXT("Wrote "), written, ATTO_TEXT(" bytes"));

  Log(ATTO_TEXT("Test 5: Receive message"));
  Sleep(200);

  if (!accepted.hasAvailable()) {
    Log(ATTO_TEXT("WARN: No data available"));
  }

  String received = accepted.readAllToString();
  Log(ATTO_TEXT("Received: '"), received, ATTO_TEXT("' (length="), received.length(), ATTO_TEXT(")"));
  Log(ATTO_TEXT("Expected: '"), msg, ATTO_TEXT("' (length="), msg.length(), ATTO_TEXT(")"));
  Log(ATTO_TEXT("Match: "), received.equals(msg));

  if (received.equals(msg)) {
    Log(ATTO_TEXT("SUCCESS!"));
    Exit(0);
  } else {
    LogError(ATTO_TEXT("FAIL: Mismatch"));
    Exit(1);
  }
}
