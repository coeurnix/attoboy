#include "attoboy/attoboy.h"

namespace attoboy {

const float Math::INF = 1e30f;
const float Math::NEG_INF = -1e30f;

// Create NaN using union (IEEE 754 quiet NaN for little-endian)
union FloatBits {
  unsigned int bits;
  float value;
};
const FloatBits nanBits = {0x7FC00000};
const float Math::NAN = nanBits.value;

static const float PI = 3.14159265358979323846f;

float Math::Sqrt(float x) noexcept {
  if (x <= 0.0f)
    return 0.0f;

  float result = x;
  float xhalf = 0.5f * result;
  int i;
  char *src = reinterpret_cast<char *>(&result);
  char *dst = reinterpret_cast<char *>(&i);
  for (int j = 0; j < 4; j++)
    dst[j] = src[j];

  i = 0x5f3759df - (i >> 1);
  src = reinterpret_cast<char *>(&i);
  dst = reinterpret_cast<char *>(&result);
  for (int j = 0; j < 4; j++)
    dst[j] = src[j];

  result = result * (1.5f - xhalf * result * result);
  result = result * (1.5f - xhalf * result * result);
  return x * result;
}

float Math::Sin(float x) noexcept {
  x = Mod(x, 2.0f * PI);
  if (x < 0.0f)
    x += 2.0f * PI;

  if (x > PI)
    x -= 2.0f * PI;

  float x2 = x * x;
  float result = x;
  float term = x;

  term = term * x2 / (2.0f * 3.0f);
  result -= term;
  term = term * x2 / (4.0f * 5.0f);
  result += term;
  term = term * x2 / (6.0f * 7.0f);
  result -= term;
  term = term * x2 / (8.0f * 9.0f);
  result += term;

  return result;
}

float Math::Cos(float x) noexcept { return Sin(x + PI / 2.0f); }

float Math::Tan(float x) noexcept {
  float c = Cos(x);
  if (Abs(c) < 0.0001f)
    return 0.0f;
  return Sin(x) / c;
}

float Math::Atan(float x) noexcept {
  float absx = Abs(x);
  if (absx > 1.0f) {
    float result = PI / 2.0f - Atan(1.0f / x);
    return x < 0.0f ? -result : result;
  }

  float y = absx / (1.0f + Sqrt(1.0f + absx * absx));
  float y2 = y * y;
  float result = y;
  float term = y;

  term *= y2;
  result -= term / 3.0f;
  term *= y2;
  result += term / 5.0f;
  term *= y2;
  result -= term / 7.0f;
  term *= y2;
  result += term / 9.0f;

  result *= 2.0f;
  return x < 0.0f ? -result : result;
}

float Math::Atan2(float y, float x) noexcept {
  if (x > 0.0f)
    return Atan(y / x);
  if (x < 0.0f && y >= 0.0f)
    return Atan(y / x) + PI;
  if (x < 0.0f && y < 0.0f)
    return Atan(y / x) - PI;
  if (x == 0.0f && y > 0.0f)
    return PI / 2.0f;
  if (x == 0.0f && y < 0.0f)
    return -PI / 2.0f;
  return 0.0f;
}

float Math::Asin(float x) noexcept {
  if (x < -1.0f || x > 1.0f)
    return 0.0f;
  return Atan2(x, Sqrt(1.0f - x * x));
}

float Math::Cosh(float x) noexcept {
  float ex = Exp(x);
  float enx = Exp(-x);
  return (ex + enx) * 0.5f;
}

} // namespace attoboy
