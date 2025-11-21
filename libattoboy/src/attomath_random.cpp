#include "attoboy/attoboy.h"
#include <windows.h>

extern "C" {
BOOLEAN NTAPI SystemFunction036(PVOID Buffer, ULONG BufferLength);
}

namespace attoboy {

static void GetRandomBytes(void *buffer, int size) {
  SystemFunction036(buffer, size);
}

int Math::random() noexcept {
  int value = 0;
  GetRandomBytes(&value, sizeof(value));
  return value;
}

float Math::randomFloat() noexcept {
  unsigned int value = 0;
  GetRandomBytes(&value, sizeof(value));
  return (value >> 8) * (1.0f / (1U << 24));
}

int Math::randomRange(int start, int end) noexcept {
  if (start >= end) {
    return start;
  }
  unsigned int range = end - start;
  unsigned int value = 0;
  GetRandomBytes(&value, sizeof(value));
  return start + (value % range);
}

bool Math::randomBool() noexcept {
  unsigned char value = 0;
  GetRandomBytes(&value, sizeof(value));
  return (value & 1) != 0;
}

} // namespace attoboy
