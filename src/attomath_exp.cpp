#include "attoboy/attoboy.h"

namespace attoboy {

static const float LN2 = 0.693147180559945309417f;
static const float LN10 = 2.302585092994045684018f;

float Math::Exp(float x) noexcept {
  if (x == 0.0f)
    return 1.0f;
  if (x < -10.0f)
    return 0.0f;
  if (x > 88.0f)
    return 3.4028235e+38f;

  float result = 1.0f;
  float term = 1.0f;

  for (int i = 1; i <= 20; i++) {
    term *= x / static_cast<float>(i);
    result += term;
    if (Abs(term) < 0.000001f)
      break;
  }

  return result;
}

float Math::Exp2(float x) noexcept { return Exp(x * LN2); }

float Math::Ln(float x) noexcept {
  if (x <= 0.0f)
    return -3.4028235e+38f;
  if (x == 1.0f)
    return 0.0f;

  int exponent = 0;
  while (x < 0.5f) {
    x *= 2.0f;
    exponent--;
  }
  while (x > 1.0f) {
    x *= 0.5f;
    exponent++;
  }

  float y = (x - 1.0f) / (x + 1.0f);
  float y2 = y * y;
  float result = y;
  float term = y;

  for (int i = 1; i <= 10; i++) {
    term *= y2;
    result += term / static_cast<float>(2 * i + 1);
  }

  return 2.0f * result + static_cast<float>(exponent) * LN2;
}

float Math::Log2(float x) noexcept { return Ln(x) / LN2; }

float Math::Log10(float x) noexcept { return Ln(x) / LN10; }

float Math::Pow(float x, float y) noexcept {
  if (y == 0.0f)
    return 1.0f;
  if (x == 0.0f)
    return 0.0f;
  if (x < 0.0f && y != Floor(y))
    return 0.0f;

  bool negative = x < 0.0f && (static_cast<int>(y) & 1) != 0;
  x = Abs(x);

  float result = Exp(y * Ln(x));
  return negative ? -result : result;
}

} // namespace attoboy
