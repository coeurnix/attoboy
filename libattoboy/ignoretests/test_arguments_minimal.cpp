#include "attoboy/attoboy.h"

using namespace attoboy;

void atto_main() {
  Arguments args;

  args.addFlag(ATTO_TEXT("v"), ATTO_TEXT("Verbose"), false);
  args.addParameter(ATTO_TEXT("o"), ATTO_TEXT("Output"), ATTO_TEXT("default.txt"));

  String v = args.getArgument(ATTO_TEXT("v"));
  String o = args.getArgument(ATTO_TEXT("o"));

  if (v.isEmpty() || o != ATTO_TEXT("default.txt")) {
    Exit(1);
  }

  Map parsed = args.parseArguments(true);

  if (parsed.isEmpty() || !parsed.hasKey(ATTO_TEXT("v")) ||
      !parsed.hasKey(ATTO_TEXT("o"))) {
    Exit(2);
  }

  Exit(0);
}
