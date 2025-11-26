#include "attoboy/attoboy.h"

namespace attoboy {

int Math::abs(int x) noexcept { return x < 0 ? -x : x; }

float Math::abs(float x) noexcept { return x < 0.0f ? -x : x; }

int Math::min(int a, int b) noexcept { return a < b ? a : b; }

float Math::min(float a, float b) noexcept { return a < b ? a : b; }

int Math::max(int a, int b) noexcept { return a > b ? a : b; }

float Math::max(float a, float b) noexcept { return a > b ? a : b; }

int Math::clamp(int x, int minVal, int maxVal) noexcept {
  return x < minVal ? minVal : (x > maxVal ? maxVal : x);
}

float Math::clamp(float x, float minVal, float maxVal) noexcept {
  return x < minVal ? minVal : (x > maxVal ? maxVal : x);
}

int Math::sign(int x) noexcept { return x < 0 ? -1 : (x > 0 ? 1 : 0); }

float Math::sign(float x) noexcept {
  return x < 0.0f ? -1.0f : (x > 0.0f ? 1.0f : 0.0f);
}

bool Math::isEven(int x) noexcept { return (x & 1) == 0; }

bool Math::isOdd(int x) noexcept { return (x & 1) != 0; }

bool Math::isPowerOfTwo(int x) noexcept { return x > 0 && (x & (x - 1)) == 0; }

float Math::floor(float x) noexcept {
  int i = static_cast<int>(x);
  return (x < i) ? static_cast<float>(i - 1) : static_cast<float>(i);
}

float Math::ceil(float x) noexcept {
  int i = static_cast<int>(x);
  return (x > i) ? static_cast<float>(i + 1) : static_cast<float>(i);
}

float Math::trunc(float x) noexcept { return static_cast<float>(static_cast<int>(x)); }

float Math::round(float x) noexcept {
  return (x >= 0.0f) ? floor(x + 0.5f) : ceil(x - 0.5f);
}

int Math::pow2(int x) noexcept {
  if (x < 0)
    return 0;
  if (x >= 31)
    return 0x7FFFFFFF;
  return 1 << x;
}

} // namespace attoboy
