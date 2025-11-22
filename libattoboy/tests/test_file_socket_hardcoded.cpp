#include "attoboy/attoboy.h"
#include <windows.h>

using namespace attoboy;

void atto_main() {
  // Write directly to console
  HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
  const char *msg1 = "Test starting (hardcoded port 12356)...\r\n";
  DWORD written;
  WriteFile(hStdout, msg1, lstrlenA(msg1), &written, nullptr);

  const int port = 12356;

  WriteFile(hStdout, "Creating File socket...\r\n", 25, &written, nullptr);

  File socketFile(String(ATTO_TEXT("127.0.0.1")), port);

  WriteFile(hStdout, "File socket constructor returned\r\n", 35, &written, nullptr);

  if (!socketFile.isValid()) {
    WriteFile(hStdout, "ERROR: Socket not valid\r\n", 25, &written, nullptr);
    Exit(1);
  }

  WriteFile(hStdout, "Socket is valid\r\n", 17, &written, nullptr);

  if (!socketFile.isOpen()) {
    WriteFile(hStdout, "ERROR: Socket not open\r\n", 24, &written, nullptr);
    Exit(1);
  }

  WriteFile(hStdout, "Socket is open\r\n", 16, &written, nullptr);

  String msg(ATTO_TEXT("Test message"));
  int wrote = socketFile.write(msg);

  char buf[64];
  int len = wsprintfA(buf, "Wrote %d bytes\r\n", wrote);
  WriteFile(hStdout, buf, len, &written, nullptr);

  if (wrote <= 0) {
    WriteFile(hStdout, "ERROR: Write failed\r\n", 21, &written, nullptr);
    Exit(1);
  }

  WriteFile(hStdout, "SUCCESS: All tests passed!\r\n", 28, &written, nullptr);
  Exit(0);
}
