#include "attoboy/attoboy.h"

using namespace attoboy;

void atto_main() {
  List list;
  list.append("Hi");
  Log(list.contains("Hi"));
  Exit(0);
}
