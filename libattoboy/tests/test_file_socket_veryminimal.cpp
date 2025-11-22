#include "attoboy/attoboy.h"
#include <windows.h>
#include <stdio.h>

using namespace attoboy;

void atto_main() {
  // Write directly to console to confirm we're running
  HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
  const char *msg = "Test starting...\r\n";
  DWORD written;
  WriteFile(hStdout, msg, lstrlenA(msg), &written, nullptr);

  Arguments args;
  args.addPositionalParameter(ATTO_TEXT("port"), ATTO_TEXT("Server port"));
  args.parseArguments(true);

  WriteFile(hStdout, "Args parsed\r\n", 13, &written, nullptr);

  String portStr = args.getArgument(ATTO_TEXT("port"));
  if (portStr.isEmpty()) {
    WriteFile(hStdout, "ERROR: No port\r\n", 16, &written, nullptr);
    Exit(1);
  }

  WriteFile(hStdout, "Port arg OK\r\n", 13, &written, nullptr);

  int port = portStr.toInteger();

  char buf[128];
  int len = wsprintfA(buf, "Creating socket to port %d...\r\n", port);
  WriteFile(hStdout, buf, len, &written, nullptr);

  File socketFile(String(ATTO_TEXT("127.0.0.1")), port);

  WriteFile(hStdout, "Socket constructor returned\r\n", 29, &written, nullptr);

  if (!socketFile.isValid()) {
    WriteFile(hStdout, "ERROR: Socket not valid\r\n", 25, &written, nullptr);
    Exit(1);
  }

  WriteFile(hStdout, "SUCCESS: All tests passed!\r\n", 28, &written, nullptr);
  Exit(0);
}
