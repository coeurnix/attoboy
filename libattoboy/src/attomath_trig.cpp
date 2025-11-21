#include "attoboy/attoboy.h"

namespace attoboy {

static const float PI = 3.14159265358979323846f;

float Math::sqrt(float x) noexcept {
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

float Math::sin(float x) noexcept {
  x = mod(x, 2.0f * PI);
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

float Math::cos(float x) noexcept { return sin(x + PI / 2.0f); }

float Math::tan(float x) noexcept {
  float c = cos(x);
  if (abs(c) < 0.0001f)
    return 0.0f;
  return sin(x) / c;
}

float Math::atan(float x) noexcept {
  if (abs(x) > 1.0f) {
    float result = PI / 2.0f - atan(1.0f / x);
    return x < 0.0f ? -result : result;
  }

  float x2 = x * x;
  float result = x;
  float term = x;

  term = term * x2;
  result -= term / 3.0f;
  term = term * x2;
  result += term / 5.0f;
  term = term * x2;
  result -= term / 7.0f;
  term = term * x2;
  result += term / 9.0f;

  return result;
}

float Math::atan2(float y, float x) noexcept {
  if (x > 0.0f)
    return atan(y / x);
  if (x < 0.0f && y >= 0.0f)
    return atan(y / x) + PI;
  if (x < 0.0f && y < 0.0f)
    return atan(y / x) - PI;
  if (x == 0.0f && y > 0.0f)
    return PI / 2.0f;
  if (x == 0.0f && y < 0.0f)
    return -PI / 2.0f;
  return 0.0f;
}

float Math::asin(float x) noexcept {
  if (x < -1.0f || x > 1.0f)
    return 0.0f;
  return atan2(x, sqrt(1.0f - x * x));
}

float Math::cosh(float x) noexcept {
  float ex = exp(x);
  float enx = exp(-x);
  return (ex + enx) * 0.5f;
}

} // namespace attoboy
