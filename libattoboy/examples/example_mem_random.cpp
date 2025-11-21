#include "attoboy/attoboy.h"

using namespace attoboy;

void atto_main() {
  Log(ATTO_TEXT("=== Memory Allocation Example ==="));

  int *numbers = static_cast<int *>(Alloc(5 * sizeof(int)));
  if (numbers != nullptr) {
    for (int i = 0; i < 5; i++) {
      numbers[i] = Math::randomRange(0, 100);
    }
    Log(ATTO_TEXT("Allocated array: "), numbers[0], ATTO_TEXT(", "), numbers[1], ATTO_TEXT(", "), numbers[2],
        ATTO_TEXT(", "), numbers[3], ATTO_TEXT(", "), numbers[4]);

    numbers = static_cast<int *>(Realloc(numbers, 10 * sizeof(int)));
    if (numbers != nullptr) {
      for (int i = 5; i < 10; i++) {
        numbers[i] = Math::randomRange(0, 100);
      }
      Log(ATTO_TEXT("Reallocated array: "), numbers[0], ATTO_TEXT(", "), numbers[1], ATTO_TEXT(", "), numbers[2],
          ATTO_TEXT(", "), numbers[3], ATTO_TEXT(", "), numbers[4], ATTO_TEXT(", "), numbers[5], ATTO_TEXT(", "),
          numbers[6], ATTO_TEXT(", "), numbers[7], ATTO_TEXT(", "), numbers[8], ATTO_TEXT(", "), numbers[9]);
      Free(numbers);
    }
  }

  Log(ATTO_TEXT(""));
  Log(ATTO_TEXT("=== Random Number Generation Example ==="));

  Log(ATTO_TEXT("Random int: "), Math::random());
  Log(ATTO_TEXT("Random float [0, 1): "), Math::randomFloat());
  Log(ATTO_TEXT("Random range [10, 20): "), Math::randomRange(10, 20));
  Log(ATTO_TEXT("Random bool: "), Math::randomBool() ? ATTO_TEXT("true") : ATTO_TEXT("false"));

  List colors;
  colors.append(ATTO_TEXT("Red")).append(ATTO_TEXT("Green")).append(ATTO_TEXT("Blue")).append(ATTO_TEXT("Yellow"));
  Log(ATTO_TEXT("Random color: "), Math::randomChoice<String>(colors));

  List dice;
  for (int i = 1; i <= 6; i++) {
    dice.append(i);
  }
  Log(ATTO_TEXT("Random dice roll: "), Math::randomChoice<int>(dice));

  Log(ATTO_TEXT(""));
  Log(ATTO_TEXT("=== Demonstration Complete ==="));
  Exit(0);
}
