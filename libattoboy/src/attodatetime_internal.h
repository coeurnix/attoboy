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

// Simple 64-bit division helper for 32-bit builds (avoiding libgcc dependency)
static inline long long Div64(long long numerator, long long denominator) {
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

  // Binary long division
  for (int i = 63; i >= 0; i--) {
    remainder <<= 1;
    remainder |= (num >> i) & 1;
    if (remainder >= den) {
      remainder -= den;
      quotient |= (1ULL << i);
    }
  }

  return negative ? -(long long)quotient : (long long)quotient;
}

} // namespace attoboy
