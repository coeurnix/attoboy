#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError(ATTO_TEXT("Test failed: "), message);                             \
    errorCount++;                                                              \
  }

void atto_main() {
  Log(ATTO_TEXT("Running Arguments simple tests..."));

  Arguments args;

  args.addFlag(ATTO_TEXT("v"), ATTO_TEXT("Verbose mode"), false,
               ATTO_TEXT("verbose"));
  args.addParameter(ATTO_TEXT("o"), ATTO_TEXT("Output file"),
                    ATTO_TEXT("default.txt"), ATTO_TEXT("output"));
  args.addPositionalParameter(ATTO_TEXT("input"), ATTO_TEXT("Input file path"));

  TEST(!args.getArgument(ATTO_TEXT("v")).isEmpty(),
       ATTO_TEXT("Flag should have default value"));
  TEST(args.getArgument(ATTO_TEXT("v")) == ATTO_TEXT("false"),
       ATTO_TEXT("Flag default should be 'false'"));
  TEST(args.getArgument(ATTO_TEXT("o")) == ATTO_TEXT("default.txt"),
       ATTO_TEXT("Parameter should have default value"));

  TEST(args.hasArgument(ATTO_TEXT("v")),
       ATTO_TEXT("hasArgument should return true for flag with default"));
  TEST(args.hasArgument(ATTO_TEXT("o")),
       ATTO_TEXT("hasArgument should return true for parameter with default"));
  TEST(!args.hasArgument(ATTO_TEXT("input")),
       ATTO_TEXT("hasArgument should return false for positional without "
                 "default"));

  Map parsed = args.parseArguments(true);

  TEST(!parsed.isEmpty(), ATTO_TEXT("parseArguments should return a map"));
  TEST(parsed.hasKey(ATTO_TEXT("v")),
       ATTO_TEXT("Parsed map should contain flag"));
  TEST(parsed.hasKey(ATTO_TEXT("o")),
       ATTO_TEXT("Parsed map should contain parameter"));

  Arguments args2;
  args2.addFlag(ATTO_TEXT("h"), ATTO_TEXT("Show help"));
  args2.setHelp(ATTO_TEXT("Test program help text"));

  String help = args2.getArgument(ATTO_TEXT("help"));
  TEST(help.isEmpty() || help == ATTO_TEXT(""),
       ATTO_TEXT("Help flag should not be in arguments"));

  Arguments args3;
  args3.addParameter(ATTO_TEXT("name"), ATTO_TEXT("User name"));
  args3.requireArgument(ATTO_TEXT("name"));

  Map parsed3 = args3.parseArguments(true);
  TEST(parsed3.isEmpty(),
       ATTO_TEXT("parseArguments should return empty map when required "
                 "argument is missing"));

  Arguments args4(args);
  TEST(args4.getArgument(ATTO_TEXT("o")) == ATTO_TEXT("default.txt"),
       ATTO_TEXT("Copy constructor should preserve arguments"));

  Arguments args5;
  args5 = args;
  TEST(args5.getArgument(ATTO_TEXT("o")) == ATTO_TEXT("default.txt"),
       ATTO_TEXT("Assignment operator should preserve arguments"));

  if (errorCount == 0) {
    Log(ATTO_TEXT("All Arguments simple tests passed!"));
    Exit(0);
  } else {
    LogError(errorCount, ATTO_TEXT(" test(s) failed"));
    Exit(errorCount);
  }
}
