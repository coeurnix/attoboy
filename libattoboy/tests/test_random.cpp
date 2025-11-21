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

  // Test random() produces different values
  int r1 = Math::random();
  int r2 = Math::random();
  int r3 = Math::random();
  TEST(r1 != r2 || r2 != r3, "random() should produce different values");
  Log("Random values: ", r1, ", ", r2, ", ", r3);

  // Test randomFloat() produces values in range [0, 1)
  bool allInRange = true;
  for (int i = 0; i < 100; i++) {
    float val = Math::randomFloat();
    if (val < 0.0f || val >= 1.0f) {
      allInRange = false;
      LogError("randomFloat() produced out-of-range value: ", val);
      break;
    }
  }
  TEST(allInRange, "randomFloat() should produce values in [0, 1)");

  // Test randomFloat() produces different values
  float d1 = Math::randomFloat();
  float d2 = Math::randomFloat();
  float d3 = Math::randomFloat();
  TEST(d1 != d2 || d2 != d3, "randomFloat() should produce different values");
  Log("randomFloat values: ", d1, ", ", d2, ", ", d3);

  // Test randomRange() with valid range
  bool allInRangeInt = true;
  for (int i = 0; i < 100; i++) {
    int val = Math::randomRange(10, 20);
    if (val < 10 || val >= 20) {
      allInRangeInt = false;
      LogError("randomRange(10, 20) produced out-of-range value: ", val);
      break;
    }
  }
  TEST(allInRangeInt, "randomRange(10, 20) should produce values in [10, 20)");

  // Test randomRange() with start >= end
  int sameVal = Math::randomRange(10, 10);
  TEST(sameVal == 10, "randomRange(10, 10) should return 10");
  int startVal = Math::randomRange(20, 10);
  TEST(startVal == 20, "randomRange(20, 10) should return 20");

  // Test randomRange() produces different values
  int rr1 = Math::randomRange(0, 1000);
  int rr2 = Math::randomRange(0, 1000);
  int rr3 = Math::randomRange(0, 1000);
  TEST(rr1 != rr2 || rr2 != rr3,
       "randomRange() should produce different values");
  Log("randomRange values: ", rr1, ", ", rr2, ", ", rr3);

  // Test randomBool()
  int trueCount = 0;
  int falseCount = 0;
  for (int i = 0; i < 100; i++) {
    if (Math::randomBool()) {
      trueCount++;
    } else {
      falseCount++;
    }
  }
  TEST(trueCount > 0 && falseCount > 0,
       "randomBool() should produce both true and false");
  Log("randomBool distribution: ", trueCount, " true, ", falseCount, " false");

  // Test randomChoice() with non-empty list
  List numbers;
  numbers.append(10).append(20).append(30).append(40).append(50);
  bool allValid = true;
  for (int i = 0; i < 50; i++) {
    int choice = Math::randomChoice<int>(numbers);
    if (choice != 10 && choice != 20 && choice != 30 && choice != 40 &&
        choice != 50) {
      allValid = false;
      LogError("randomChoice() returned invalid value: ", choice);
      break;
    }
  }
  TEST(allValid, "randomChoice() should return values from the list");

  // Test randomChoice() with empty list
  List empty;
  int emptyChoice = Math::randomChoice<int>(empty);
  TEST(emptyChoice == 0,
       "randomChoice() on empty list should return default value");

  // Test randomChoice() with strings
  List strings;
  strings.append("apple").append("banana").append("cherry");
  bool allValidStrings = true;
  for (int i = 0; i < 30; i++) {
    String choice = Math::randomChoice<String>(strings);
    if (choice != "apple" && choice != "banana" && choice != "cherry") {
      allValidStrings = false;
      LogError("randomChoice() returned invalid string: ", choice);
      break;
    }
  }
  TEST(allValidStrings,
       "randomChoice() should return string values from the list");

  // Report results
  if (errorCount == 0) {
    Log("All random tests passed!");
    Exit(0);
  } else {
    LogError(errorCount, " test(s) failed");
    Exit(errorCount);
  }
}
