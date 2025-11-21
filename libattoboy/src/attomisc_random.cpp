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

double RandomDouble() {
  unsigned long long value = 0;
  GetRandomBytes(&value, sizeof(value));
  return (value >> 11) * (1.0 / (1ULL << 53));
}

long long RandomRange(long long start, long long end) {
  if (start >= end) {
    return start;
  }
  unsigned long long range = end - start;
  unsigned long long value = 0;
  GetRandomBytes(&value, sizeof(value));

  unsigned long long result;
  if (range <= 0xFFFFFFFFULL) {
    result = (unsigned long)(value) % (unsigned long)range;
  } else {
    double scaled = (value >> 11) * (1.0 / (1ULL << 53));
    result = (unsigned long long)(scaled * range);
  }

  return start + result;
}

bool RandomBool() {
  unsigned char value = 0;
  GetRandomBytes(&value, sizeof(value));
  return (value & 1) != 0;
}

} // namespace attoboy
