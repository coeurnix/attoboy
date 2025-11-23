#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError(ATTO_TEXT("Test failed: "), message);                             \
    errorCount++;                                                              \
  }

void atto_main() {
  Log(ATTO_TEXT("Running Path directory tests..."));

  Path home = Path::GetHomeDirectory();
  TEST(home.exists(), ATTO_TEXT("Home directory should exist"));
  TEST(home.isDirectory(), ATTO_TEXT("Home directory should be a directory"));

  Path docs = Path::GetDocumentsDirectory();
  TEST(docs.exists(), ATTO_TEXT("Documents directory should exist"));
  TEST(docs.isDirectory(),
       ATTO_TEXT("Documents directory should be a directory"));

  Path roaming = Path::GetRoamingAppDirectory();
  TEST(roaming.exists(), ATTO_TEXT("Roaming app directory should exist"));
  TEST(roaming.isDirectory(),
       ATTO_TEXT("Roaming app directory should be a directory"));

  Path local = Path::GetLocalAppDirectory();
  TEST(local.exists(), ATTO_TEXT("Local app directory should exist"));
  TEST(local.isDirectory(),
       ATTO_TEXT("Local app directory should be a directory"));

  if (errorCount == 0) {
    Log(ATTO_TEXT("All Path directory tests passed!"));
    Exit(0);
  } else {
    LogError(errorCount, ATTO_TEXT(" test(s) failed"));
    Exit(errorCount);
  }
}
