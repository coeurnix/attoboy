#include "attoboy/attoboy.h"

namespace attoboy {

int Math::Abs(int x) noexcept { return x < 0 ? -x : x; }

float Math::Abs(float x) noexcept { return x < 0.0f ? -x : x; }

int Math::Min(int a, int b) noexcept { return a < b ? a : b; }

float Math::Min(float a, float b) noexcept { return a < b ? a : b; }

int Math::Max(int a, int b) noexcept { return a > b ? a : b; }

float Math::Max(float a, float b) noexcept { return a > b ? a : b; }

int Math::Clamp(int x, int minVal, int maxVal) noexcept {
  return x < minVal ? minVal : (x > maxVal ? maxVal : x);
}

float Math::Clamp(float x, float minVal, float maxVal) noexcept {
  return x < minVal ? minVal : (x > maxVal ? maxVal : x);
}

int Math::Sign(int x) noexcept { return x < 0 ? -1 : (x > 0 ? 1 : 0); }

float Math::Sign(float x) noexcept {
  return x < 0.0f ? -1.0f : (x > 0.0f ? 1.0f : 0.0f);
}

bool Math::IsEven(int x) noexcept { return (x & 1) == 0; }

bool Math::IsOdd(int x) noexcept { return (x & 1) != 0; }

bool Math::IsPowerOfTwo(int x) noexcept { return x > 0 && (x & (x - 1)) == 0; }

float Math::Floor(float x) noexcept {
  int i = static_cast<int>(x);
  return (x < i) ? static_cast<float>(i - 1) : static_cast<float>(i);
}

float Math::Ceil(float x) noexcept {
  int i = static_cast<int>(x);
  return (x > i) ? static_cast<float>(i + 1) : static_cast<float>(i);
}

float Math::Trunc(float x) noexcept {
  return static_cast<float>(static_cast<int>(x));
}

float Math::Round(float x) noexcept {
  return (x >= 0.0f) ? Floor(x + 0.5f) : Ceil(x - 0.5f);
}

int Math::Pow2(int x) noexcept {
  if (x < 0)
    return 0;
  if (x >= 31)
    return 0x7FFFFFFF;
  return 1 << x;
}

} // namespace attoboy
