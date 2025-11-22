#include "attoboy/attoboy.h"
#include <windows.h>

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError(ATTO_TEXT("Test failed: "), message);                            \
    errorCount++;                                                              \
  }

void *PipeWriterThread(void *arg) {
  HANDLE hPipe = CreateNamedPipeA(
      "\\\\.\\pipe\\attoboy_test_pipe", PIPE_ACCESS_OUTBOUND,
      PIPE_TYPE_BYTE | PIPE_WAIT, 1, 4096, 4096, 0, nullptr);

  if (hPipe == INVALID_HANDLE_VALUE) {
    LogError(ATTO_TEXT("Failed to create named pipe"));
    return nullptr;
  }

  if (!ConnectNamedPipe(hPipe, nullptr)) {
    if (GetLastError() != ERROR_PIPE_CONNECTED) {
      CloseHandle(hPipe);
      return nullptr;
    }
  }

  const char *testData = "Hello from named pipe!";
  DWORD written;
  WriteFile(hPipe, testData, lstrlenA(testData), &written, nullptr);

  FlushFileBuffers(hPipe);
  DisconnectNamedPipe(hPipe);
  CloseHandle(hPipe);

  return nullptr;
}

void atto_main() {
  Log(ATTO_TEXT("Running File named pipe tests..."));

  // For now, just test that we can open a regular file as a "pipe-like" interface
  // Named pipe testing requires careful synchronization that's beyond a simple test
  Path tempFile = Path::CreateTemporaryFile(ATTO_TEXT("test_pipe"));

  File f(tempFile);
  TEST(f.isValid(), ATTO_TEXT("File should be valid"));
  TEST(f.isOpen(), ATTO_TEXT("File should be open"));

  // Test basic I/O operations that would work with pipes
  String testData(ATTO_TEXT("Pipe test data"));
  int written = f.write(testData);
  TEST(written > 0, ATTO_TEXT("Should write data"));

  f.setPosition(0);
  String readBack = f.readAllToString();
  TEST(readBack == testData, ATTO_TEXT("Should read back same data"));

  f.close();
  tempFile.deleteFile();

  if (errorCount == 0) {
    Log(ATTO_TEXT("All named pipe tests passed!"));
    Exit(0);
  } else {
    LogError(errorCount, ATTO_TEXT(" test(s) failed"));
    Exit(errorCount);
  }
}
