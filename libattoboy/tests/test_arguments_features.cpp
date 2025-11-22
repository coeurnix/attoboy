#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError(ATTO_TEXT("Test failed: "), message);                             \
    errorCount++;                                                              \
  }

void atto_main() {
  Log(ATTO_TEXT("Running Arguments feature tests..."));

  Arguments args;
  args.addFlag(ATTO_TEXT("v"), ATTO_TEXT("First verbose flag"), false);
  args.addFlag(ATTO_TEXT("v"), ATTO_TEXT("Second verbose flag (should clobber)"),
               true);

  String v = args.getArgument(ATTO_TEXT("v"));
  TEST(v == ATTO_TEXT("true"),
       ATTO_TEXT("Clobbered flag should have new default value"));

  Arguments args2;
  args2.addParameter(ATTO_TEXT("file"), ATTO_TEXT("Input file"));
  args2.requireArgument(ATTO_TEXT("file"));

  Map parsed = args2.parseArguments(true);
  TEST(parsed.isEmpty(), ATTO_TEXT("Required argument missing should return "
                                    "empty map"));

  Arguments args3;
  args3.addFlag(ATTO_TEXT("h"), ATTO_TEXT("Help flag"));
  args3.setHelp(ATTO_TEXT("Test help text"));

  Map parsed3 = args3.parseArguments(true);
  TEST(!parsed3.isEmpty(), ATTO_TEXT("Help flag 'h' can be used as regular flag"));

  Arguments args4;
  args4.addParameter(ATTO_TEXT("name"), ATTO_TEXT("User name"),
                     ATTO_TEXT("default"));
  TEST(args4.hasArgument(ATTO_TEXT("name")),
       ATTO_TEXT("hasArgument should return true for param with default"));
  TEST(args4.getArgument(ATTO_TEXT("name")) == ATTO_TEXT("default"),
       ATTO_TEXT("getArgument should return default value"));

  if (errorCount == 0) {
    Log(ATTO_TEXT("All Arguments feature tests passed!"));
    Exit(0);
  } else {
    LogError(errorCount, ATTO_TEXT(" test(s) failed"));
    Exit(errorCount);
  }
}
