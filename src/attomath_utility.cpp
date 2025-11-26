#include "attoboy/attoboy.h"

namespace attoboy {

float Math::mod(float x, float y) noexcept {
  if (y == 0.0f)
    return 0.0f;
  return x - trunc(x / y) * y;
}

float Math::lerp(float a, float b, float t) noexcept { return a + (b - a) * t; }

float Math::step(float edge, float x) noexcept { return x < edge ? 0.0f : 1.0f; }

float Math::degToRad(float degrees) noexcept {
  const float PI = 3.14159265358979323846f;
  return degrees * (PI / 180.0f);
}

float Math::radToDeg(float radians) noexcept {
  const float PI = 3.14159265358979323846f;
  return radians * (180.0f / PI);
}

bool Math::isFinite(float x) noexcept {
  unsigned int bits;
  char *src = reinterpret_cast<char *>(&x);
  char *dst = reinterpret_cast<char *>(&bits);
  for (int i = 0; i < 4; i++)
    dst[i] = src[i];
  unsigned int exponent = (bits >> 23) & 0xFF;
  return exponent != 0xFF;
}

bool Math::isNaN(float x) noexcept {
  unsigned int bits;
  char *src = reinterpret_cast<char *>(&x);
  char *dst = reinterpret_cast<char *>(&bits);
  for (int i = 0; i < 4; i++)
    dst[i] = src[i];
  unsigned int exponent = (bits >> 23) & 0xFF;
  unsigned int mantissa = bits & 0x7FFFFF;
  return exponent == 0xFF && mantissa != 0;
}

} // namespace attoboy
