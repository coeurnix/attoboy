#include "attoboy/attoboy.h"

namespace attoboy {

float Math::mod(float x, float y) noexcept {
  if (y == 0.0f)
    return 0.0f;
  return x - trunc(x / y) * y;
}

float Math::lerp(float a, float b, float t) noexcept { return a + (b - a) * t; }

float Math::step(float edge, float x) noexcept {
  return x < edge ? 0.0f : 1.0f;
}

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

long long Math::Add64(long long a, long long b) noexcept { return a + b; }

long long Math::Sub64(long long a, long long b) noexcept { return a - b; }

long long Math::Div64(long long numerator, long long denominator) noexcept {
  if (denominator == 0)
    return 0;

  bool negative = false;
  if (numerator < 0) {
    numerator = -numerator;
    negative = !negative;
  }
  if (denominator < 0) {
    denominator = -denominator;
    negative = !negative;
  }

  unsigned long long num = (unsigned long long)numerator;
  unsigned long long den = (unsigned long long)denominator;

  unsigned int num_high = (unsigned int)(num >> 32);
  unsigned int num_low = (unsigned int)(num & 0xFFFFFFFFULL);
  unsigned int den_high = (unsigned int)(den >> 32);
  unsigned int den_low = (unsigned int)(den & 0xFFFFFFFFULL);

  unsigned int quotient_high = 0;
  unsigned int quotient_low = 0;
  unsigned int remainder_high = 0;
  unsigned int remainder_low = 0;

  for (int i = 63; i >= 0; i--) {
    unsigned int new_remainder_high =
        (remainder_high << 1) | (remainder_low >> 31);
    unsigned int new_remainder_low = remainder_low << 1;

    int bit_pos = i - 32;
    unsigned int bit;
    if (bit_pos >= 0) {
      bit = (num_high >> bit_pos) & 1;
    } else {
      bit = (num_low >> i) & 1;
    }
    new_remainder_low |= bit;

    remainder_high = new_remainder_high;
    remainder_low = new_remainder_low;

    bool greater_or_equal = false;
    if (remainder_high > den_high) {
      greater_or_equal = true;
    } else if (remainder_high == den_high) {
      if (remainder_low >= den_low) {
        greater_or_equal = true;
      }
    }

    if (greater_or_equal) {
      if (remainder_low < den_low) {
        remainder_high -= (den_high + 1);
        remainder_low = remainder_low + (0xFFFFFFFFU - den_low) + 1;
      } else {
        remainder_low -= den_low;
        remainder_high -= den_high;
      }

      if (i >= 32) {
        quotient_high |= (1U << (i - 32));
      } else {
        quotient_low |= (1U << i);
      }
    }
  }

  unsigned long long quotient =
      ((unsigned long long)quotient_high << 32) | quotient_low;

  return negative ? -(long long)quotient : (long long)quotient;
}

long long Math::Mod64(long long numerator, long long denominator) noexcept {
  if (denominator == 0)
    return 0;

  bool negative = false;
  if (numerator < 0) {
    numerator = -numerator;
    negative = !negative;
  }
  if (denominator < 0) {
    denominator = -denominator;
  }

  unsigned long long num = (unsigned long long)numerator;
  unsigned long long den = (unsigned long long)denominator;

  unsigned int num_high = (unsigned int)(num >> 32);
  unsigned int num_low = (unsigned int)(num & 0xFFFFFFFFULL);
  unsigned int den_high = (unsigned int)(den >> 32);
  unsigned int den_low = (unsigned int)(den & 0xFFFFFFFFULL);

  unsigned int remainder_high = 0;
  unsigned int remainder_low = 0;

  for (int i = 63; i >= 0; i--) {
    unsigned int new_remainder_high =
        (remainder_high << 1) | (remainder_low >> 31);
    unsigned int new_remainder_low = remainder_low << 1;

    int bit_pos = i - 32;
    unsigned int bit;
    if (bit_pos >= 0) {
      bit = (num_high >> bit_pos) & 1;
    } else {
      bit = (num_low >> i) & 1;
    }
    new_remainder_low |= bit;

    remainder_high = new_remainder_high;
    remainder_low = new_remainder_low;

    bool greater_or_equal = false;
    if (remainder_high > den_high) {
      greater_or_equal = true;
    } else if (remainder_high == den_high) {
      if (remainder_low >= den_low) {
        greater_or_equal = true;
      }
    }

    if (greater_or_equal) {
      if (remainder_low < den_low) {
        remainder_high -= (den_high + 1);
        remainder_low = remainder_low + (0xFFFFFFFFU - den_low) + 1;
      } else {
        remainder_low -= den_low;
        remainder_high -= den_high;
      }
    }
  }

  unsigned long long remainder =
      ((unsigned long long)remainder_high << 32) | remainder_low;

  return negative ? -(long long)remainder : (long long)remainder;
}

} // namespace attoboy
