#include "attoboy/attoboy.h"

using namespace attoboy;

void atto_main() {
  List list;
  list.append(ATTO_TEXT("Hi"));
  Log(list.contains(ATTO_TEXT("Hi")));
  Exit(0);
}
