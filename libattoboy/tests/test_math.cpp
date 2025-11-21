#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError("Test failed: ", message);                                        \
    errorCount++;                                                              \
  }

#define TEST_APPROX(a, b, epsilon, message)                                    \
  if (Math::abs((a) - (b)) > (epsilon)) {                                      \
    LogError("Test failed: ", message, " (got ", a, ", expected ", b, ")");    \
    errorCount++;                                                              \
  }

void atto_main() {
  Log("Running Math class tests...");

  // Basic math functions
  TEST(Math::abs(-5) == 5, "abs(-5) should be 5");
  TEST(Math::abs(5) == 5, "abs(5) should be 5");
  TEST_APPROX(Math::abs(-3.14f), 3.14f, 0.001f, "abs(-3.14f) should be 3.14f");

  TEST(Math::min(5, 10) == 5, "min(5, 10) should be 5");
  TEST(Math::min(10, 5) == 5, "min(10, 5) should be 5");
  TEST_APPROX(Math::min(3.5f, 2.1f), 2.1f, 0.001f,
              "min(3.5f, 2.1f) should be 2.1f");

  TEST(Math::max(5, 10) == 10, "max(5, 10) should be 10");
  TEST(Math::max(10, 5) == 10, "max(10, 5) should be 10");
  TEST_APPROX(Math::max(3.5f, 2.1f), 3.5f, 0.001f,
              "max(3.5f, 2.1f) should be 3.5f");

  TEST(Math::clamp(5, 0, 10) == 5, "clamp(5, 0, 10) should be 5");
  TEST(Math::clamp(-5, 0, 10) == 0, "clamp(-5, 0, 10) should be 0");
  TEST(Math::clamp(15, 0, 10) == 10, "clamp(15, 0, 10) should be 10");
  TEST_APPROX(Math::clamp(5.5f, 0.0f, 10.0f), 5.5f, 0.001f,
              "clamp(5.5f, 0.0f, 10.0f) should be 5.5f");

  TEST(Math::sign(-5) == -1, "sign(-5) should be -1");
  TEST(Math::sign(0) == 0, "sign(0) should be 0");
  TEST(Math::sign(5) == 1, "sign(5) should be 1");
  TEST_APPROX(Math::sign(-3.14f), -1.0f, 0.001f,
              "sign(-3.14f) should be -1.0f");
  TEST_APPROX(Math::sign(3.14f), 1.0f, 0.001f, "sign(3.14f) should be 1.0f");

  TEST(Math::isEven(4) == true, "isEven(4) should be true");
  TEST(Math::isEven(5) == false, "isEven(5) should be false");
  TEST(Math::isOdd(4) == false, "isOdd(4) should be false");
  TEST(Math::isOdd(5) == true, "isOdd(5) should be true");

  TEST(Math::isPowerOfTwo(1) == true, "isPowerOfTwo(1) should be true");
  TEST(Math::isPowerOfTwo(2) == true, "isPowerOfTwo(2) should be true");
  TEST(Math::isPowerOfTwo(4) == true, "isPowerOfTwo(4) should be true");
  TEST(Math::isPowerOfTwo(8) == true, "isPowerOfTwo(8) should be true");
  TEST(Math::isPowerOfTwo(3) == false, "isPowerOfTwo(3) should be false");
  TEST(Math::isPowerOfTwo(6) == false, "isPowerOfTwo(6) should be false");
  TEST(Math::isPowerOfTwo(0) == false, "isPowerOfTwo(0) should be false");

  // Rounding functions
  TEST_APPROX(Math::floor(3.7f), 3.0f, 0.001f, "floor(3.7f) should be 3.0f");
  TEST_APPROX(Math::floor(-3.7f), -4.0f, 0.001f, "floor(-3.7f) should be -4.0f");
  TEST_APPROX(Math::ceil(3.2f), 4.0f, 0.001f, "ceil(3.2f) should be 4.0f");
  TEST_APPROX(Math::ceil(-3.2f), -3.0f, 0.001f, "ceil(-3.2f) should be -3.0f");
  TEST_APPROX(Math::trunc(3.7f), 3.0f, 0.001f, "trunc(3.7f) should be 3.0f");
  TEST_APPROX(Math::trunc(-3.7f), -3.0f, 0.001f, "trunc(-3.7f) should be -3.0f");
  TEST_APPROX(Math::round(3.5f), 4.0f, 0.001f, "round(3.5f) should be 4.0f");
  TEST_APPROX(Math::round(3.4f), 3.0f, 0.001f, "round(3.4f) should be 3.0f");
  TEST_APPROX(Math::round(-3.5f), -4.0f, 0.001f, "round(-3.5f) should be -4.0f");

  // Advanced math functions
  TEST_APPROX(Math::sqrt(4.0f), 2.0f, 0.01f, "sqrt(4.0f) should be 2.0f");
  TEST_APPROX(Math::sqrt(9.0f), 3.0f, 0.01f, "sqrt(9.0f) should be 3.0f");
  TEST_APPROX(Math::sqrt(16.0f), 4.0f, 0.01f, "sqrt(16.0f) should be 4.0f");

  const float PI = 3.14159265358979323846f;
  TEST_APPROX(Math::sin(0.0f), 0.0f, 0.01f, "sin(0) should be 0");
  TEST_APPROX(Math::sin(PI / 2.0f), 1.0f, 0.01f, "sin(PI/2) should be 1");
  TEST_APPROX(Math::cos(0.0f), 1.0f, 0.01f, "cos(0) should be 1");
  TEST_APPROX(Math::cos(PI), -1.0f, 0.01f, "cos(PI) should be -1");
  TEST_APPROX(Math::tan(0.0f), 0.0f, 0.01f, "tan(0) should be 0");

  TEST_APPROX(Math::atan(0.0f), 0.0f, 0.01f, "atan(0) should be 0");
  TEST_APPROX(Math::atan(1.0f), PI / 4.0f, 0.01f, "atan(1) should be PI/4");
  TEST_APPROX(Math::atan2(1.0f, 1.0f), PI / 4.0f, 0.01f,
              "atan2(1, 1) should be PI/4");
  TEST_APPROX(Math::asin(0.0f), 0.0f, 0.01f, "asin(0) should be 0");
  TEST_APPROX(Math::asin(1.0f), PI / 2.0f, 0.01f, "asin(1) should be PI/2");

  TEST_APPROX(Math::cosh(0.0f), 1.0f, 0.01f, "cosh(0) should be 1");

  // Exponential and logarithmic functions
  TEST_APPROX(Math::exp(0.0f), 1.0f, 0.01f, "exp(0) should be 1");
  TEST_APPROX(Math::exp(1.0f), 2.71828f, 0.01f, "exp(1) should be e");

  TEST_APPROX(Math::log(1.0f), 0.0f, 0.01f, "log(1) should be 0");
  TEST_APPROX(Math::log(2.71828f), 1.0f, 0.01f, "log(e) should be 1");

  TEST_APPROX(Math::log2(1.0f), 0.0f, 0.01f, "log2(1) should be 0");
  TEST_APPROX(Math::log2(2.0f), 1.0f, 0.01f, "log2(2) should be 1");
  TEST_APPROX(Math::log2(8.0f), 3.0f, 0.01f, "log2(8) should be 3");

  TEST_APPROX(Math::log10(1.0f), 0.0f, 0.01f, "log10(1) should be 0");
  TEST_APPROX(Math::log10(10.0f), 1.0f, 0.01f, "log10(10) should be 1");
  TEST_APPROX(Math::log10(100.0f), 2.0f, 0.01f, "log10(100) should be 2");

  TEST_APPROX(Math::exp2(0.0f), 1.0f, 0.01f, "exp2(0) should be 1");
  TEST_APPROX(Math::exp2(1.0f), 2.0f, 0.01f, "exp2(1) should be 2");
  TEST_APPROX(Math::exp2(3.0f), 8.0f, 0.01f, "exp2(3) should be 8");

  TEST_APPROX(Math::pow(2.0f, 3.0f), 8.0f, 0.01f, "pow(2, 3) should be 8");
  TEST_APPROX(Math::pow(3.0f, 2.0f), 9.0f, 0.01f, "pow(3, 2) should be 9");
  TEST_APPROX(Math::pow(5.0f, 0.0f), 1.0f, 0.01f, "pow(5, 0) should be 1");

  TEST(Math::pow2(0) == 1, "pow2(0) should be 1");
  TEST(Math::pow2(1) == 2, "pow2(1) should be 2");
  TEST(Math::pow2(3) == 8, "pow2(3) should be 8");
  TEST(Math::pow2(10) == 1024, "pow2(10) should be 1024");

  // Utility functions
  TEST_APPROX(Math::mod(7.5f, 3.0f), 1.5f, 0.01f, "mod(7.5, 3.0) should be 1.5");
  TEST_APPROX(Math::mod(10.0f, 3.0f), 1.0f, 0.01f,
              "mod(10.0, 3.0) should be 1.0");

  TEST_APPROX(Math::lerp(0.0f, 10.0f, 0.0f), 0.0f, 0.01f,
              "lerp(0, 10, 0) should be 0");
  TEST_APPROX(Math::lerp(0.0f, 10.0f, 0.5f), 5.0f, 0.01f,
              "lerp(0, 10, 0.5) should be 5");
  TEST_APPROX(Math::lerp(0.0f, 10.0f, 1.0f), 10.0f, 0.01f,
              "lerp(0, 10, 1) should be 10");

  TEST_APPROX(Math::step(5.0f, 3.0f), 0.0f, 0.01f,
              "step(5, 3) should be 0.0f");
  TEST_APPROX(Math::step(5.0f, 7.0f), 1.0f, 0.01f,
              "step(5, 7) should be 1.0f");

  TEST_APPROX(Math::degToRad(0.0f), 0.0f, 0.01f, "degToRad(0) should be 0");
  TEST_APPROX(Math::degToRad(180.0f), PI, 0.01f, "degToRad(180) should be PI");
  TEST_APPROX(Math::radToDeg(0.0f), 0.0f, 0.01f, "radToDeg(0) should be 0");
  TEST_APPROX(Math::radToDeg(PI), 180.0f, 0.01f, "radToDeg(PI) should be 180");

  TEST(Math::isFinite(1.0f) == true, "isFinite(1.0f) should be true");
  TEST(Math::isFinite(0.0f) == true, "isFinite(0.0f) should be true");

  TEST(Math::isNaN(0.0f) == false, "isNaN(0.0f) should be false");
  TEST(Math::isNaN(1.0f) == false, "isNaN(1.0f) should be false");

  // Report results
  if (errorCount == 0) {
    Log("All Math tests passed!");
    Exit(0);
  } else {
    LogError(errorCount, " test(s) failed");
    Exit(errorCount);
  }
}
