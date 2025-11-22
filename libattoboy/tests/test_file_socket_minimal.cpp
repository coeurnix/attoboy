#include "attoboy/attoboy.h"

using namespace attoboy;

void atto_main() {
  Arguments args;
  args.addPositionalParameter(ATTO_TEXT("port"), ATTO_TEXT("Server port"));
  args.parseArguments(true);

  String portStr = args.getArgument(ATTO_TEXT("port"));
  if (portStr.isEmpty()) {
    LogError(ATTO_TEXT("Port argument required"));
    Exit(1);
  }

  int port = portStr.toInteger();

  Log(ATTO_TEXT("Attempting to connect to 127.0.0.1:"), port);

  File socketFile(String(ATTO_TEXT("127.0.0.1")), port);

  if (!socketFile.isValid()) {
    LogError(ATTO_TEXT("Socket creation failed - not valid"));
    Exit(1);
  }

  Log(ATTO_TEXT("Socket created successfully"));

  if (!socketFile.isOpen()) {
    LogError(ATTO_TEXT("Socket not open"));
    Exit(1);
  }

  Log(ATTO_TEXT("Socket is open"));

  String msg(ATTO_TEXT("Test"));
  int written = socketFile.write(msg);

  Log(ATTO_TEXT("Wrote "), written, ATTO_TEXT(" bytes"));

  if (written <= 0) {
    LogError(ATTO_TEXT("Write failed"));
    Exit(1);
  }

  Log(ATTO_TEXT("All tests passed!"));
  Exit(0);
}
