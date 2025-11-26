#include "attobuffer_internal.h"

namespace attoboy {

#define LZ4_MAGIC 0x184D2204
#define LZ4_MIN_MATCH 4
#define LZ4_HASH_SIZE 4096
#define LZ4_MAX_DISTANCE 65535

static inline unsigned int LZ4Hash(const unsigned char *p) {
  unsigned int v = (p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24));
  return (v * 2654435761U) >> 20;
}

static inline void LZ4WriteU32(unsigned char *p, unsigned int v) {
  p[0] = (unsigned char)(v);
  p[1] = (unsigned char)(v >> 8);
  p[2] = (unsigned char)(v >> 16);
  p[3] = (unsigned char)(v >> 24);
}

static inline unsigned int LZ4ReadU32(const unsigned char *p) {
  return (p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24));
}

static int LZ4CompressCore(const unsigned char *src, int srcLen,
                           unsigned char *dst, int dstLen) {
  if (srcLen == 0)
    return 0;
  if (dstLen < 16)
    return -1;

  unsigned short *hashTable = (unsigned short *)HeapAlloc(
      GetProcessHeap(), HEAP_ZERO_MEMORY, LZ4_HASH_SIZE * sizeof(unsigned short));
  if (!hashTable)
    return -1;

  const unsigned char *ip = src;
  const unsigned char *anchor = src;
  const unsigned char *iend = src + srcLen;
  const unsigned char *mflimit = iend - 12;

  unsigned char *op = dst;
  unsigned char *oend = dst + dstLen;

  if (srcLen < 13)
    goto _last_literals;

  ip++;

  while (ip < mflimit) {
    unsigned int h = LZ4Hash(ip);
    unsigned short ref = hashTable[h];
    hashTable[h] = (unsigned short)(ip - src);

    if (ref >= (ip - src) || (ip - src) - ref > LZ4_MAX_DISTANCE ||
        LZ4ReadU32(src + ref) != LZ4ReadU32(ip)) {
      ip++;
      continue;
    }

    const unsigned char *match = src + ref;
    int litLen = (int)(ip - anchor);
    unsigned char *token = op++;

    if (op + litLen + 5 >= oend) {
      HeapFree(GetProcessHeap(), 0, hashTable);
      return -1;
    }

    if (litLen >= 15) {
      *token = 0xF0;
      int len = litLen - 15;
      while (len >= 255) {
        *op++ = 255;
        len -= 255;
      }
      *op++ = (unsigned char)len;
    } else {
      *token = (unsigned char)(litLen << 4);
    }

    for (int i = 0; i < litLen; i++)
      *op++ = *anchor++;

    const unsigned char *matchStart = ip;
    while (ip + LZ4_MIN_MATCH < iend && match + LZ4_MIN_MATCH < iend &&
           ip[LZ4_MIN_MATCH] == match[LZ4_MIN_MATCH]) {
      ip++;
      match++;
    }

    int matchLen = (int)(ip - matchStart) + LZ4_MIN_MATCH;
    ip += LZ4_MIN_MATCH;
    anchor = ip;

    int offset = (int)(matchStart - (src + ref));
    if (op + 2 >= oend) {
      HeapFree(GetProcessHeap(), 0, hashTable);
      return -1;
    }
    *op++ = (unsigned char)(offset);
    *op++ = (unsigned char)(offset >> 8);

    matchLen -= LZ4_MIN_MATCH;
    if (matchLen >= 15) {
      *token |= 0x0F;
      int len = matchLen - 15;
      while (len >= 255) {
        if (op >= oend) {
          HeapFree(GetProcessHeap(), 0, hashTable);
          return -1;
        }
        *op++ = 255;
        len -= 255;
      }
      if (op >= oend) {
        HeapFree(GetProcessHeap(), 0, hashTable);
        return -1;
      }
      *op++ = (unsigned char)len;
    } else {
      *token |= (unsigned char)matchLen;
    }
  }

_last_literals:
  int litLen = (int)(iend - anchor);
  if (op + litLen + 1 + (litLen / 255) >= oend) {
    HeapFree(GetProcessHeap(), 0, hashTable);
    return -1;
  }

  if (litLen >= 15) {
    *op++ = 0xF0;
    int len = litLen - 15;
    while (len >= 255) {
      *op++ = 255;
      len -= 255;
    }
    *op++ = (unsigned char)len;
  } else {
    *op++ = (unsigned char)(litLen << 4);
  }

  for (int i = 0; i < litLen; i++)
    *op++ = *anchor++;

  HeapFree(GetProcessHeap(), 0, hashTable);
  return (int)(op - dst);
}

static int LZ4DecompressCore(const unsigned char *src, int srcLen,
                             unsigned char *dst, int dstLen) {
  if (srcLen == 0)
    return 0;

  const unsigned char *ip = src;
  const unsigned char *iend = src + srcLen;
  unsigned char *op = dst;
  unsigned char *oend = dst + dstLen;

  while (ip < iend) {
    unsigned char token = *ip++;

    int litLen = token >> 4;
    if (litLen == 15) {
      unsigned char s;
      do {
        if (ip >= iend)
          return -1;
        s = *ip++;
        litLen += s;
      } while (s == 255);
    }

    if (ip + litLen > iend || op + litLen > oend)
      return -1;

    for (int i = 0; i < litLen; i++)
      *op++ = *ip++;

    if (ip >= iend)
      break;

    if (ip + 2 > iend)
      return -1;

    int offset = ip[0] | (ip[1] << 8);
    ip += 2;

    if (offset == 0 || op - offset < dst)
      return -1;

    unsigned char *match = op - offset;

    int matchLen = token & 0x0F;
    if (matchLen == 15) {
      unsigned char s;
      do {
        if (ip >= iend)
          return -1;
        s = *ip++;
        matchLen += s;
      } while (s == 255);
    }
    matchLen += LZ4_MIN_MATCH;

    if (op + matchLen > oend)
      return -1;

    for (int i = 0; i < matchLen; i++)
      *op++ = *match++;
  }

  return (int)(op - dst);
}

Buffer Buffer::compress() const {
  Buffer result;

  if (!impl || impl->size == 0)
    return result;

  ReadLockGuard lock(&impl->lock);

  int maxCompSize = impl->size + (impl->size / 255) + 16 + 12;
  unsigned char *compData =
      (unsigned char *)HeapAlloc(GetProcessHeap(), 0, maxCompSize);
  if (!compData)
    return result;

  LZ4WriteU32(compData, LZ4_MAGIC);
  LZ4WriteU32(compData + 4, (unsigned int)impl->size);

  int compSize = LZ4CompressCore(impl->data, impl->size, compData + 8,
                                 maxCompSize - 8);

  if (compSize < 0) {
    HeapFree(GetProcessHeap(), 0, compData);
    return result;
  }

  result.impl =
      (BufferImpl *)HeapAlloc(GetProcessHeap(), 0, sizeof(BufferImpl));
  if (!result.impl) {
    HeapFree(GetProcessHeap(), 0, compData);
    return result;
  }

  InitializeSRWLock(&result.impl->lock);
  WriteLockGuard wlock(&result.impl->lock);

  result.impl->size = compSize + 8;
  result.impl->capacity = compSize + 8;
  result.impl->data = AllocBufferData(result.impl->capacity);

  if (!result.impl->data) {
    HeapFree(GetProcessHeap(), 0, result.impl);
    HeapFree(GetProcessHeap(), 0, compData);
    result.impl = nullptr;
    return result;
  }

  for (int i = 0; i < result.impl->size; i++)
    result.impl->data[i] = compData[i];

  HeapFree(GetProcessHeap(), 0, compData);
  return result;
}

Buffer Buffer::decompress() const {
  Buffer result;

  if (!impl || impl->size < 8)
    return result;

  ReadLockGuard lock(&impl->lock);

  if (LZ4ReadU32(impl->data) != LZ4_MAGIC)
    return result;

  unsigned int decompSize = LZ4ReadU32(impl->data + 4);
  if (decompSize == 0 || decompSize > 256 * 1024 * 1024)
    return result;

  unsigned char *decompData =
      (unsigned char *)HeapAlloc(GetProcessHeap(), 0, decompSize);
  if (!decompData)
    return result;

  int actualSize = LZ4DecompressCore(impl->data + 8, impl->size - 8,
                                     decompData, (int)decompSize);

  if (actualSize < 0 || actualSize != (int)decompSize) {
    HeapFree(GetProcessHeap(), 0, decompData);
    return result;
  }

  result.impl =
      (BufferImpl *)HeapAlloc(GetProcessHeap(), 0, sizeof(BufferImpl));
  if (!result.impl) {
    HeapFree(GetProcessHeap(), 0, decompData);
    return result;
  }

  InitializeSRWLock(&result.impl->lock);
  WriteLockGuard wlock(&result.impl->lock);

  result.impl->size = actualSize;
  result.impl->capacity = actualSize;
  result.impl->data = AllocBufferData(result.impl->capacity);

  if (!result.impl->data) {
    HeapFree(GetProcessHeap(), 0, result.impl);
    HeapFree(GetProcessHeap(), 0, decompData);
    result.impl = nullptr;
    return result;
  }

  for (int i = 0; i < result.impl->size; i++)
    result.impl->data[i] = decompData[i];

  HeapFree(GetProcessHeap(), 0, decompData);
  return result;
}

} // namespace attoboy
