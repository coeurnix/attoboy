#include "attoboy/attoboy.h"
#include <windows.h>

extern "C" {
BOOLEAN NTAPI SystemFunction036(PVOID Buffer, ULONG BufferLength);
}

namespace attoboy {

static void GetRandomBytes(void *buffer, int size) {
  SystemFunction036(buffer, size);
}

long long Random() {
  long long value = 0;
  GetRandomBytes(&value, sizeof(value));
  return value;
}

float RandomFloat() {
  unsigned int value = 0;
  GetRandomBytes(&value, sizeof(value));
  return (value >> 8) * (1.0f / (1U << 24));
}

long long RandomRange(long long start, long long end) {
  if (start >= end) {
    return start;
  }
  unsigned long long range = end - start;
  unsigned long long value = 0;
  GetRandomBytes(&value, sizeof(value));

  // Avoid 64-bit modulo to prevent libgcc dependency
  // Simple division-based modulo: value % range = value - (value / range) * range
  unsigned long long quotient = 0;
  unsigned long long remainder = 0;

  // Binary long division for 64-bit modulo
  for (int i = 63; i >= 0; i--) {
    remainder <<= 1;
    remainder |= (value >> i) & 1;
    if (remainder >= range) {
      remainder -= range;
      quotient |= (1ULL << i);
    }
  }

  return start + remainder;
}

bool RandomBool() {
  unsigned char value = 0;
  GetRandomBytes(&value, sizeof(value));
  return (value & 1) != 0;
}

} // namespace attoboy
