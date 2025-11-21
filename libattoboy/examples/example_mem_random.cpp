#include "attoboy/attoboy.h"

using namespace attoboy;

void atto_main() {
  Log("=== Memory Allocation Example ===");

  int *numbers = static_cast<int *>(Alloc(5 * sizeof(int)));
  if (numbers != nullptr) {
    for (int i = 0; i < 5; i++) {
      numbers[i] = static_cast<int>(RandomRange(0, 100));
    }
    Log("Allocated array: ", numbers[0], ", ", numbers[1], ", ", numbers[2],
        ", ", numbers[3], ", ", numbers[4]);

    numbers = static_cast<int *>(Realloc(numbers, 10 * sizeof(int)));
    if (numbers != nullptr) {
      for (int i = 5; i < 10; i++) {
        numbers[i] = static_cast<int>(RandomRange(0, 100));
      }
      Log("Reallocated array: ", numbers[0], ", ", numbers[1], ", ", numbers[2],
          ", ", numbers[3], ", ", numbers[4], ", ", numbers[5], ", ",
          numbers[6], ", ", numbers[7], ", ", numbers[8], ", ", numbers[9]);
      Free(numbers);
    }
  }

  Log("");
  Log("=== Random Number Generation Example ===");

  Log("Random int: ", static_cast<int>(Random()));
  Log("Random float [0, 1): ", RandomFloat());
  Log("Random range [10, 20): ", RandomRange(10, 20));
  Log("Random bool: ", RandomBool() ? "true" : "false");

  List colors;
  colors.append("Red").append("Green").append("Blue").append("Yellow");
  Log("Random color: ", RandomChoice<String>(colors));

  List dice;
  for (int i = 1; i <= 6; i++) {
    dice.append(i);
  }
  Log("Random dice roll: ", RandomChoice<int>(dice));

  Log("");
  Log("=== Demonstration Complete ===");
  Exit(0);
}
