#include "attoboy/attoboy.h"

using namespace attoboy;

void atto_main() {
  Arguments args;

  args.addFlag(ATTO_TEXT("v"), ATTO_TEXT("Enable verbose mode"), false,
               ATTO_TEXT("verbose"));
  args.addFlag(ATTO_TEXT("d"), ATTO_TEXT("Enable debug mode"), false,
               ATTO_TEXT("debug"));
  args.addParameter(ATTO_TEXT("o"), ATTO_TEXT("Output file path"),
                    ATTO_TEXT("default.txt"), ATTO_TEXT("output"));
  args.addParameter(ATTO_TEXT("r"), ATTO_TEXT("Required parameter"));
  args.addPositionalParameter(ATTO_TEXT("input"),
                               ATTO_TEXT("Input file path"));
  args.addPositionalParameter(ATTO_TEXT("extra"),
                               ATTO_TEXT("Extra parameter"));

  args.requireArgument(ATTO_TEXT("r"));

  Map parsed = args.parseArguments();

  if (parsed.isEmpty()) {
    Exit(1);
  }

  Exit(0);
}
