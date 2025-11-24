#pragma once
#include "atto_internal_common.h"
#include "attoboy/attoboy.h"
#include <windows.h>

namespace attoboy {

struct DateTimeImpl {
  FILETIME fileTime;
  SRWLOCK lock;

  DateTimeImpl() : fileTime() { InitializeSRWLock(&lock); }
};

static inline long long Div64(long long numerator, long long denominator) {
#ifdef _MSC_VER
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

#else
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
  unsigned long long quotient = 0;
  unsigned long long remainder = 0;

  for (int i = 63; i >= 0; i--) {
    remainder <<= 1;
    remainder |= (num >> i) & 1;
    if (remainder >= den) {
      remainder -= den;
      quotient |= (1ULL << i);
    }
  }

  return negative ? -(long long)quotient : (long long)quotient;
#endif
}

} // namespace attoboy
