#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError("Test failed: ", message);                                        \
    errorCount++;                                                              \
  }

void atto_main() {
  Log("Running random number generation tests...");

  // Test Random() produces different values
  int r1 = Random();
  int r2 = Random();
  int r3 = Random();
  TEST(r1 != r2 || r2 != r3, "Random() should produce different values");
  Log("Random values: ", r1, ", ", r2, ", ", r3);

  // Test RandomFloat() produces values in range [0, 1)
  bool allInRange = true;
  for (int i = 0; i < 100; i++) {
    float val = RandomFloat();
    if (val < 0.0f || val >= 1.0f) {
      allInRange = false;
      LogError("RandomFloat() produced out-of-range value: ", val);
      break;
    }
  }
  TEST(allInRange, "RandomFloat() should produce values in [0, 1)");

  // Test RandomFloat() produces different values
  float d1 = RandomFloat();
  float d2 = RandomFloat();
  float d3 = RandomFloat();
  TEST(d1 != d2 || d2 != d3, "RandomFloat() should produce different values");
  Log("RandomFloat values: ", d1, ", ", d2, ", ", d3);

  // Test RandomRange() with valid range
  bool allInRangeInt = true;
  for (int i = 0; i < 100; i++) {
    int val = RandomRange(10, 20);
    if (val < 10 || val >= 20) {
      allInRangeInt = false;
      LogError("RandomRange(10, 20) produced out-of-range value: ", val);
      break;
    }
  }
  TEST(allInRangeInt, "RandomRange(10, 20) should produce values in [10, 20)");

  // Test RandomRange() with start >= end
  int sameVal = RandomRange(10, 10);
  TEST(sameVal == 10, "RandomRange(10, 10) should return 10");
  int startVal = RandomRange(20, 10);
  TEST(startVal == 20, "RandomRange(20, 10) should return 20");

  // Test RandomRange() produces different values
  int rr1 = RandomRange(0, 1000);
  int rr2 = RandomRange(0, 1000);
  int rr3 = RandomRange(0, 1000);
  TEST(rr1 != rr2 || rr2 != rr3,
       "RandomRange() should produce different values");
  Log("RandomRange values: ", rr1, ", ", rr2, ", ", rr3);

  // Test RandomBool()
  int trueCount = 0;
  int falseCount = 0;
  for (int i = 0; i < 100; i++) {
    if (RandomBool()) {
      trueCount++;
    } else {
      falseCount++;
    }
  }
  TEST(trueCount > 0 && falseCount > 0,
       "RandomBool() should produce both true and false");
  Log("RandomBool distribution: ", trueCount, " true, ", falseCount, " false");

  // Test RandomChoice() with non-empty list
  List numbers;
  numbers.append(10).append(20).append(30).append(40).append(50);
  bool allValid = true;
  for (int i = 0; i < 50; i++) {
    int choice = RandomChoice<int>(numbers);
    if (choice != 10 && choice != 20 && choice != 30 && choice != 40 &&
        choice != 50) {
      allValid = false;
      LogError("RandomChoice() returned invalid value: ", choice);
      break;
    }
  }
  TEST(allValid, "RandomChoice() should return values from the list");

  // Test RandomChoice() with empty list
  List empty;
  int emptyChoice = RandomChoice<int>(empty);
  TEST(emptyChoice == 0,
       "RandomChoice() on empty list should return default value");

  // Test RandomChoice() with strings
  List strings;
  strings.append("apple").append("banana").append("cherry");
  bool allValidStrings = true;
  for (int i = 0; i < 30; i++) {
    String choice = RandomChoice<String>(strings);
    if (choice != "apple" && choice != "banana" && choice != "cherry") {
      allValidStrings = false;
      LogError("RandomChoice() returned invalid string: ", choice);
      break;
    }
  }
  TEST(allValidStrings,
       "RandomChoice() should return string values from the list");

  // Report results
  if (errorCount == 0) {
    Log("All random tests passed!");
    Exit(0);
  } else {
    LogError(errorCount, " test(s) failed");
    Exit(errorCount);
  }
}
