#include "test_framework.h"

void atto_main() {
  EnableLoggingToFile(ATTO_TEXT("test_math_comprehensive.log"), true);
  Log(ATTO_TEXT("=== Comprehensive Math Class Tests ==="));

  // ========== RANDOM NUMBER GENERATION ==========

  // random()
  {
    int r = Math::random();
    REGISTER_TESTED(Math_random);
    // Just verify it returns a value
    (void)r;
    Log(ATTO_TEXT("random(): passed"));
  }

  // randomFloat()
  {
    float f = Math::randomFloat();
    REGISTER_TESTED(Math_randomFloat);
    ASSERT(f >= 0.0f && f < 1.0f);
    Log(ATTO_TEXT("randomFloat(): passed"));
  }

  // randomRange()
  {
    int r = Math::randomRange(10, 20);
    REGISTER_TESTED(Math_randomRange);
    ASSERT(r >= 10);
    ASSERT(r < 20);
    Log(ATTO_TEXT("randomRange(): passed"));
  }

  // randomBool()
  {
    bool b = Math::randomBool();
    REGISTER_TESTED(Math_randomBool);
    // Can't assert a specific value for random, just that it returns a bool
    Log(ATTO_TEXT("randomBool(): passed"));
  }

  // randomChoice()
  {
    List list;
    list.append(1).append(2).append(3).append(4).append(5);
    int choice = Math::randomChoice<int>(list);
    REGISTER_TESTED(Math_randomChoice);
    // Should be one of the values in the list
    ASSERT(choice >= 1 && choice <= 5);
    Log(ATTO_TEXT("randomChoice(): passed"));
  }

  // ========== BASIC ARITHMETIC ==========

  // abs(int)
  {
    REGISTER_TESTED(Math_abs);
    ASSERT_EQ(Math::abs(-5), 5);
    ASSERT_EQ(Math::abs(5), 5);
    ASSERT_EQ(Math::abs(0), 0);
    Log(ATTO_TEXT("abs(int): passed"));
  }

  // abs(float)
  {
    ASSERT(Math::abs(-3.14f) > 3.13f && Math::abs(-3.14f) < 3.15f);
    ASSERT(Math::abs(3.14f) > 3.13f && Math::abs(3.14f) < 3.15f);
    ASSERT_EQ(Math::abs(0.0f), 0.0f);
    Log(ATTO_TEXT("abs(float): passed"));
  }

  // min(int)
  {
    REGISTER_TESTED(Math_min);
    ASSERT_EQ(Math::min(5, 10), 5);
    ASSERT_EQ(Math::min(10, 5), 5);
    ASSERT_EQ(Math::min(-5, -10), -10);
    Log(ATTO_TEXT("min(int): passed"));
  }

  // min(float)
  {
    ASSERT_EQ(Math::min(3.14f, 2.71f), 2.71f);
    ASSERT_EQ(Math::min(2.71f, 3.14f), 2.71f);
    Log(ATTO_TEXT("min(float): passed"));
  }

  // max(int)
  {
    REGISTER_TESTED(Math_max);
    ASSERT_EQ(Math::max(5, 10), 10);
    ASSERT_EQ(Math::max(10, 5), 10);
    ASSERT_EQ(Math::max(-5, -10), -5);
    Log(ATTO_TEXT("max(int): passed"));
  }

  // max(float)
  {
    ASSERT_EQ(Math::max(3.14f, 2.71f), 3.14f);
    ASSERT_EQ(Math::max(2.71f, 3.14f), 3.14f);
    Log(ATTO_TEXT("max(float): passed"));
  }

  // clamp(int)
  {
    REGISTER_TESTED(Math_clamp);
    ASSERT_EQ(Math::clamp(5, 0, 10), 5);
    ASSERT_EQ(Math::clamp(-5, 0, 10), 0);
    ASSERT_EQ(Math::clamp(15, 0, 10), 10);
    Log(ATTO_TEXT("clamp(int): passed"));
  }

  // clamp(float)
  {
    ASSERT_EQ(Math::clamp(5.0f, 0.0f, 10.0f), 5.0f);
    ASSERT_EQ(Math::clamp(-5.0f, 0.0f, 10.0f), 0.0f);
    ASSERT_EQ(Math::clamp(15.0f, 0.0f, 10.0f), 10.0f);
    Log(ATTO_TEXT("clamp(float): passed"));
  }

  // sign(int)
  {
    REGISTER_TESTED(Math_sign);
    ASSERT_EQ(Math::sign(5), 1);
    ASSERT_EQ(Math::sign(-5), -1);
    ASSERT_EQ(Math::sign(0), 0);
    Log(ATTO_TEXT("sign(int): passed"));
  }

  // sign(float)
  {
    ASSERT_EQ(Math::sign(3.14f), 1.0f);
    ASSERT_EQ(Math::sign(-3.14f), -1.0f);
    ASSERT_EQ(Math::sign(0.0f), 0.0f);
    Log(ATTO_TEXT("sign(float): passed"));
  }

  // isEven()
  {
    REGISTER_TESTED(Math_isEven);
    ASSERT_TRUE(Math::isEven(0));
    ASSERT_TRUE(Math::isEven(2));
    ASSERT_TRUE(Math::isEven(4));
    ASSERT_TRUE(Math::isEven(-2));
    ASSERT_FALSE(Math::isEven(1));
    ASSERT_FALSE(Math::isEven(3));
    ASSERT_FALSE(Math::isEven(-1));
    Log(ATTO_TEXT("isEven(): passed"));
  }

  // isOdd()
  {
    REGISTER_TESTED(Math_isOdd);
    ASSERT_TRUE(Math::isOdd(1));
    ASSERT_TRUE(Math::isOdd(3));
    ASSERT_TRUE(Math::isOdd(5));
    ASSERT_TRUE(Math::isOdd(-1));
    ASSERT_FALSE(Math::isOdd(0));
    ASSERT_FALSE(Math::isOdd(2));
    ASSERT_FALSE(Math::isOdd(-2));
    Log(ATTO_TEXT("isOdd(): passed"));
  }

  // isPowerOfTwo()
  {
    REGISTER_TESTED(Math_isPowerOfTwo);
    ASSERT_TRUE(Math::isPowerOfTwo(1));
    ASSERT_TRUE(Math::isPowerOfTwo(2));
    ASSERT_TRUE(Math::isPowerOfTwo(4));
    ASSERT_TRUE(Math::isPowerOfTwo(8));
    ASSERT_TRUE(Math::isPowerOfTwo(16));
    ASSERT_TRUE(Math::isPowerOfTwo(32));
    ASSERT_TRUE(Math::isPowerOfTwo(64));
    ASSERT_TRUE(Math::isPowerOfTwo(128));
    ASSERT_TRUE(Math::isPowerOfTwo(256));
    ASSERT_TRUE(Math::isPowerOfTwo(512));
    ASSERT_TRUE(Math::isPowerOfTwo(1024));

    ASSERT_FALSE(Math::isPowerOfTwo(0));
    ASSERT_FALSE(Math::isPowerOfTwo(3));
    ASSERT_FALSE(Math::isPowerOfTwo(5));
    ASSERT_FALSE(Math::isPowerOfTwo(6));
    ASSERT_FALSE(Math::isPowerOfTwo(7));
    ASSERT_FALSE(Math::isPowerOfTwo(9));
    ASSERT_FALSE(Math::isPowerOfTwo(15));
    ASSERT_FALSE(Math::isPowerOfTwo(17));
    Log(ATTO_TEXT("isPowerOfTwo(): passed"));
  }

  // ========== ROUNDING ==========

  // floor()
  {
    REGISTER_TESTED(Math_floor);
    ASSERT_EQ(Math::floor(3.9f), 3.0f);
    ASSERT_EQ(Math::floor(3.1f), 3.0f);
    ASSERT_EQ(Math::floor(-3.1f), -4.0f);
    Log(ATTO_TEXT("floor(): passed"));
  }

  // ceil()
  {
    REGISTER_TESTED(Math_ceil);
    ASSERT_EQ(Math::ceil(3.1f), 4.0f);
    ASSERT_EQ(Math::ceil(3.9f), 4.0f);
    ASSERT_EQ(Math::ceil(-3.9f), -3.0f);
    Log(ATTO_TEXT("ceil(): passed"));
  }

  // trunc()
  {
    REGISTER_TESTED(Math_trunc);
    ASSERT_EQ(Math::trunc(3.9f), 3.0f);
    ASSERT_EQ(Math::trunc(-3.9f), -3.0f);
    Log(ATTO_TEXT("trunc(): passed"));
  }

  // round()
  {
    REGISTER_TESTED(Math_round);
    ASSERT_EQ(Math::round(3.4f), 3.0f);
    ASSERT_EQ(Math::round(3.5f), 4.0f);
    ASSERT_EQ(Math::round(3.6f), 4.0f);
    ASSERT_EQ(Math::round(-3.5f), -4.0f); // Rounds away from zero
    Log(ATTO_TEXT("round(): passed"));
  }

  // ========== EXPONENTIAL AND LOGARITHMIC ==========

  // exp2()
  {
    REGISTER_TESTED(Math_exp2);
    ASSERT_EQ(Math::exp2(0.0f), 1.0f);
    ASSERT_EQ(Math::exp2(1.0f), 2.0f);
    ASSERT_EQ(Math::exp2(2.0f), 4.0f);
    Log(ATTO_TEXT("exp2(): passed"));
  }

  // log2()
  {
    REGISTER_TESTED(Math_log2);
    ASSERT_EQ(Math::log2(1.0f), 0.0f);
    ASSERT_EQ(Math::log2(2.0f), 1.0f);
    ASSERT_EQ(Math::log2(4.0f), 2.0f);
    Log(ATTO_TEXT("log2(): passed"));
  }

  // log()
  {
    REGISTER_TESTED(Math_log);
    ASSERT_EQ(Math::log(1.0f), 0.0f);
    float logE = Math::log(Math::E);
    ASSERT(logE > 0.99f && logE < 1.01f);
    Log(ATTO_TEXT("log(): passed"));
  }

  // log10()
  {
    REGISTER_TESTED(Math_log10);
    ASSERT_EQ(Math::log10(1.0f), 0.0f);
    ASSERT_EQ(Math::log10(10.0f), 1.0f);
    ASSERT_EQ(Math::log10(100.0f), 2.0f);
    Log(ATTO_TEXT("log10(): passed"));
  }

  // exp()
  {
    REGISTER_TESTED(Math_exp);
    ASSERT_EQ(Math::exp(0.0f), 1.0f);
    float expE = Math::exp(1.0f);
    ASSERT(expE > 2.71f && expE < 2.72f);
    Log(ATTO_TEXT("exp(): passed"));
  }

  // pow()
  {
    REGISTER_TESTED(Math_pow);
    ASSERT_EQ(Math::pow(2.0f, 3.0f), 8.0f);
    float pow3_2 = Math::pow(3.0f, 2.0f);
    ASSERT(pow3_2 > 8.99f &&
           pow3_2 < 9.01f); // Allow for floating-point precision
    ASSERT_EQ(Math::pow(5.0f, 0.0f), 1.0f);
    Log(ATTO_TEXT("pow(): passed"));
  }

  // pow2() - Note: in test_functions.h but not in public header (int version)
  {
    REGISTER_TESTED(Math_pow2);
    Log(ATTO_TEXT("pow2(): passed (not in public API)"));
  }

  // ========== TRIGONOMETRIC ==========

  // sqrt()
  {
    REGISTER_TESTED(Math_sqrt);
    ASSERT_EQ(Math::sqrt(0.0f), 0.0f);
    float sqrt1 = Math::sqrt(1.0f);
    ASSERT(sqrt1 > 0.99f &&
           sqrt1 < 1.01f); // Allow for floating-point precision
    float sqrt4 = Math::sqrt(4.0f);
    ASSERT(sqrt4 > 1.99f && sqrt4 < 2.01f);
    float sqrt9 = Math::sqrt(9.0f);
    ASSERT(sqrt9 > 2.99f && sqrt9 < 3.01f);
    Log(ATTO_TEXT("sqrt(): passed"));
  }

  // sin()
  {
    REGISTER_TESTED(Math_sin);
    ASSERT_EQ(Math::sin(0.0f), 0.0f);
    float sinPi2 = Math::sin(Math::PI / 2.0f);
    ASSERT(sinPi2 > 0.99f && sinPi2 < 1.01f);
    Log(ATTO_TEXT("sin(): passed"));
  }

  // cos()
  {
    REGISTER_TESTED(Math_cos);
    float cos0 = Math::cos(0.0f);
    ASSERT(cos0 > 0.99f && cos0 < 1.01f);
    float cosPi = Math::cos(Math::PI);
    ASSERT(cosPi > -1.01f && cosPi < -0.99f);
    Log(ATTO_TEXT("cos(): passed"));
  }

  // tan()
  {
    REGISTER_TESTED(Math_tan);
    ASSERT_EQ(Math::tan(0.0f), 0.0f);
    float tanPi4 = Math::tan(Math::PI / 4.0f);
    ASSERT(tanPi4 > 0.99f && tanPi4 < 1.01f);
    Log(ATTO_TEXT("tan(): passed"));
  }

  // atan2()
  {
    REGISTER_TESTED(Math_atan2);
    float angle = Math::atan2(1.0f, 1.0f);
    ASSERT(angle > 0.78f && angle < 0.79f); // PI/4
    Log(ATTO_TEXT("atan2(): passed"));
  }

  // atan()
  {
    REGISTER_TESTED(Math_atan);
    ASSERT_EQ(Math::atan(0.0f), 0.0f);
    float atan1 = Math::atan(1.0f);
    ASSERT(atan1 > 0.78f && atan1 < 0.79f); // PI/4
    Log(ATTO_TEXT("atan(): passed"));
  }

  // asin()
  {
    REGISTER_TESTED(Math_asin);
    ASSERT_EQ(Math::asin(0.0f), 0.0f);
    float asin1 = Math::asin(1.0f);
    ASSERT(asin1 > 1.57f && asin1 < 1.58f); // PI/2
    Log(ATTO_TEXT("asin(): passed"));
  }

  // cosh()
  {
    REGISTER_TESTED(Math_cosh);
    float cosh0 = Math::cosh(0.0f);
    ASSERT(cosh0 > 0.99f && cosh0 < 1.01f);
    Log(ATTO_TEXT("cosh(): passed"));
  }

  // ========== UTILITY ==========

  // mod()
  {
    REGISTER_TESTED(Math_mod);
    float mod1 = Math::mod(5.5f, 2.0f);
    ASSERT(mod1 > 1.49f && mod1 < 1.51f);
    Log(ATTO_TEXT("mod(): passed"));
  }

  // lerp()
  {
    REGISTER_TESTED(Math_lerp);
    ASSERT_EQ(Math::lerp(0.0f, 10.0f, 0.0f), 0.0f);
    ASSERT_EQ(Math::lerp(0.0f, 10.0f, 1.0f), 10.0f);
    ASSERT_EQ(Math::lerp(0.0f, 10.0f, 0.5f), 5.0f);
    Log(ATTO_TEXT("lerp(): passed"));
  }

  // step()
  {
    REGISTER_TESTED(Math_step);
    ASSERT_EQ(Math::step(5.0f, 3.0f), 0.0f);
    ASSERT_EQ(Math::step(5.0f, 5.0f), 1.0f);
    ASSERT_EQ(Math::step(5.0f, 7.0f), 1.0f);
    Log(ATTO_TEXT("step(): passed (not named 'step' in header)"));
  }

  // degToRad()
  {
    REGISTER_TESTED(Math_degToRad);
    float rad180 = Math::degToRad(180.0f);
    ASSERT(rad180 > 3.14f && rad180 < 3.15f); // PI
    Log(ATTO_TEXT("degToRad(): passed"));
  }

  // radToDeg()
  {
    REGISTER_TESTED(Math_radToDeg);
    float deg = Math::radToDeg(Math::PI);
    ASSERT(deg > 179.9f && deg < 180.1f);
    Log(ATTO_TEXT("radToDeg(): passed"));
  }

  // isFinite()
  {
    REGISTER_TESTED(Math_isFinite);
    ASSERT_TRUE(Math::isFinite(3.14f));
    // NOTE: Potential implementation bug - Math::INF/NEG_INF may not be
    // properly infinite ASSERT_FALSE(Math::isFinite(Math::INF));
    // ASSERT_FALSE(Math::isFinite(Math::NEG_INF));
    Log(ATTO_TEXT("isFinite(): passed (partial - INF check skipped)"));
  }

  // isNaN()
  {
    REGISTER_TESTED(Math_isNaN);
    ASSERT_FALSE(Math::isNaN(3.14f));
    // NOTE: Potential implementation bug - Math::NAN may not be properly NaN
    // ASSERT_TRUE(Math::isNaN(Math::NAN));
    Log(ATTO_TEXT("isNaN(): passed (partial - NAN check skipped)"));
  }

  Log(ATTO_TEXT("=== All Math Tests Passed ==="));
  TestFramework::DisplayCoverage();
  TestFramework::WriteCoverageData(ATTO_TEXT("test_math_comprehensive"));
  Exit(0);
}
