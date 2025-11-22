#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError(ATTO_TEXT("Test failed: "), message);                            \
    errorCount++;                                                              \
  }

void atto_main() {
  Log(ATTO_TEXT("Running File simple tests..."));

  Path tempFile = Path::CreateTemporaryFile(ATTO_TEXT("test_file"));
  TEST(tempFile.exists(), ATTO_TEXT("Temporary file should exist"));

  {
    File f(tempFile);
    TEST(f.isValid(), ATTO_TEXT("File should be valid"));
    TEST(f.isOpen(), ATTO_TEXT("File should be open"));
    TEST(f.isRegularFile(), ATTO_TEXT("File should be regular file"));
    TEST(!f.isSocket(), ATTO_TEXT("File should not be socket"));
    TEST(!f.isNamedPipe(), ATTO_TEXT("File should not be named pipe"));

    const ATTO_CHAR *path = f.getPath();
    TEST(path != nullptr, ATTO_TEXT("File path should not be null"));

    TEST(f.getHost() == nullptr, ATTO_TEXT("File host should be null for regular file"));
    TEST(f.getPort() == -1, ATTO_TEXT("File port should be -1 for regular file"));

    String testData(ATTO_TEXT("Hello, World!"));
    int written = f.write(testData);
    TEST(written > 0, ATTO_TEXT("Should write data successfully"));

    TEST(f.flush(), ATTO_TEXT("Should flush successfully"));

    TEST(f.setPosition(0), ATTO_TEXT("Should set position to 0"));
    TEST(f.getPosition() == 0, ATTO_TEXT("Position should be 0"));

    TEST(f.hasAvailable(), ATTO_TEXT("Should have data available"));
    int available = f.getAvailableCount();
    TEST(available > 0, ATTO_TEXT("Available count should be > 0"));

    String readData = f.readAllToString();
    TEST(!readData.isEmpty(), ATTO_TEXT("Should read data successfully"));
    TEST(readData == testData, ATTO_TEXT("Read data should match written data"));

    TEST(f.isAtEnd(), ATTO_TEXT("Should be at end after reading all"));

    long long endPos = f.getEndPosition();
    TEST(endPos > 0, ATTO_TEXT("End position should be > 0"));

    f.close();
    TEST(!f.isOpen(), ATTO_TEXT("File should be closed"));
  }

  {
    File f(tempFile);
    TEST(f.setPosition(0), ATTO_TEXT("Should set position to 0 for second test"));

    Buffer testBuf(String(ATTO_TEXT("Binary data test")));
    int written = f.write(testBuf);
    TEST(written > 0, ATTO_TEXT("Should write buffer successfully"));

    f.setPosition(0);
    Buffer readBuf = f.readAllToBuffer();
    TEST(!readBuf.isEmpty(), ATTO_TEXT("Should read buffer successfully"));
    TEST(readBuf == testBuf, ATTO_TEXT("Read buffer should match written buffer"));

    f.close();
  }

  {
    File f(tempFile);
    f.setPosition(0);
    String partial1(ATTO_TEXT("ABCDEFGHIJ"));
    f.write(partial1);

    f.setPosition(0);
    String readPartial = f.readToString(5 * sizeof(ATTO_CHAR));
    TEST(readPartial.length() == 5, ATTO_TEXT("Should read partial data (5 chars)"));

    long long pos = f.getPosition();
    TEST(pos == 5 * sizeof(ATTO_CHAR), ATTO_TEXT("Position should be at 5 chars"));

    f.close();
  }

  tempFile.deleteFile();
  TEST(!tempFile.exists(), ATTO_TEXT("Temporary file should be deleted"));

  if (errorCount == 0) {
    Log(ATTO_TEXT("All simple file tests passed!"));
    Exit(0);
  } else {
    LogError(errorCount, ATTO_TEXT(" test(s) failed"));
    Exit(errorCount);
  }
}
