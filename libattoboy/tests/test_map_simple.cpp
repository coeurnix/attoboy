#include "attoboy/attoboy.h"
#include <windows.h>

using namespace attoboy;

void atto_main() {
  Log("Test 1: Map in Map (without List test first)");
  
  Map innerMap1;
  innerMap1.put(1, 100);
  Map m1;
  m1.put(String("map"), innerMap1);
  Log("Test 1 PASSED");

  Log("Test 2: List in Map");
  List innerList;
  innerList.append(1).append(2).append(3);
  Map m2;
  m2.put(String("list"), innerList);
  Log("Test 2 PASSED");

  Log("Test 3: Map in Map (after List test)");
  Map innerMap2;
  innerMap2.put(1, 100);
  Map m3;
  m3.put(String("map"), innerMap2);
  Log("Test 3 PASSED");

  Exit(0);
}
