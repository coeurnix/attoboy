#pragma once
#include "attoboy/attoboy.h"
#include "atto_internal_common.h"
#include <windows.h>

namespace attoboy {

enum ArgumentType { ARG_FLAG = 0, ARG_PARAMETER, ARG_POSITIONAL };

struct ArgumentDef {
  String name;
  String description;
  String longName;
  String defaultValue;
  ArgumentType type;
  bool required;
};

struct ArgumentsImpl {
  List *argDefs;
  List *cmdLineArgs;
  String *helpText;
  mutable SRWLOCK lock;
};

} // namespace attoboy
