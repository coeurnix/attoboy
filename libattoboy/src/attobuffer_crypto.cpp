#include "attobuffer_internal.h"

namespace attoboy {

static inline unsigned int rotl32(unsigned int x, int n) {
  return (x << n) | (x >> (32 - n));
}

static void chacha20_quarter_round(unsigned int &a, unsigned int &b,
                                    unsigned int &c, unsigned int &d) {
  a += b;
  d ^= a;
  d = rotl32(d, 16);
  c += d;
  b ^= c;
  b = rotl32(b, 12);
  a += b;
  d ^= a;
  d = rotl32(d, 8);
  c += d;
  b ^= c;
  b = rotl32(b, 7);
}

static void chacha20_block(const unsigned int key[8],
                            const unsigned int nonce[3], unsigned int counter,
                            unsigned char output[64]) {
  unsigned int state[16];

  state[0] = 0x61707865;
  state[1] = 0x3320646e;
  state[2] = 0x79622d32;
  state[3] = 0x6b206574;

  for (int i = 0; i < 8; i++) {
    state[4 + i] = key[i];
  }

  state[12] = counter;
  state[13] = nonce[0];
  state[14] = nonce[1];
  state[15] = nonce[2];

  unsigned int working[16];
  for (int i = 0; i < 16; i++) {
    working[i] = state[i];
  }

  for (int i = 0; i < 10; i++) {
    chacha20_quarter_round(working[0], working[4], working[8], working[12]);
    chacha20_quarter_round(working[1], working[5], working[9], working[13]);
    chacha20_quarter_round(working[2], working[6], working[10], working[14]);
    chacha20_quarter_round(working[3], working[7], working[11], working[15]);
    chacha20_quarter_round(working[0], working[5], working[10], working[15]);
    chacha20_quarter_round(working[1], working[6], working[11], working[12]);
    chacha20_quarter_round(working[2], working[7], working[8], working[13]);
    chacha20_quarter_round(working[3], working[4], working[9], working[14]);
  }

  for (int i = 0; i < 16; i++) {
    working[i] += state[i];
  }

  for (int i = 0; i < 16; i++) {
    output[i * 4 + 0] = (unsigned char)(working[i] >> 0);
    output[i * 4 + 1] = (unsigned char)(working[i] >> 8);
    output[i * 4 + 2] = (unsigned char)(working[i] >> 16);
    output[i * 4 + 3] = (unsigned char)(working[i] >> 24);
  }
}

static void chacha20_crypt(const unsigned char *key, const unsigned char *nonce,
                            const unsigned char *input, unsigned char *output,
                            int length) {
  unsigned int keyInts[8];
  for (int i = 0; i < 8; i++) {
    keyInts[i] = ((unsigned int)key[i * 4 + 0] << 0) |
                 ((unsigned int)key[i * 4 + 1] << 8) |
                 ((unsigned int)key[i * 4 + 2] << 16) |
                 ((unsigned int)key[i * 4 + 3] << 24);
  }

  unsigned int nonceInts[3];
  for (int i = 0; i < 3; i++) {
    nonceInts[i] = ((unsigned int)nonce[i * 4 + 0] << 0) |
                   ((unsigned int)nonce[i * 4 + 1] << 8) |
                   ((unsigned int)nonce[i * 4 + 2] << 16) |
                   ((unsigned int)nonce[i * 4 + 3] << 24);
  }

  unsigned int counter = 0;
  int offset = 0;

  while (offset < length) {
    unsigned char block[64];
    chacha20_block(keyInts, nonceInts, counter, block);

    int blockSize = 64;
    if (offset + blockSize > length) {
      blockSize = length - offset;
    }

    for (int i = 0; i < blockSize; i++) {
      output[offset + i] = input[offset + i] ^ block[i];
    }

    offset += blockSize;
    counter++;
  }
}

static const unsigned char base64_table[65] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const unsigned char base64_decode_table[256] = {
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 62, 64, 64, 64, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60,
    61, 64, 64, 64, 64, 64, 64, 64, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
    11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64,
    64, 64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
    43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64};

String Buffer::toBase64() const {
  if (!impl || impl->size == 0) {
    return String();
  }

  ReadLockGuard guard(&impl->lock);

  int len = impl->size;
  int olen = 4 * ((len + 2) / 3);

  char *out = static_cast<char *>(Alloc(olen + 1));
  if (!out) {
    return String();
  }

  const unsigned char *in = impl->data;
  unsigned char *pos = (unsigned char *)out;
  const unsigned char *end = in + len;

  while (end - in >= 3) {
    *pos++ = base64_table[in[0] >> 2];
    *pos++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
    *pos++ = base64_table[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
    *pos++ = base64_table[in[2] & 0x3f];
    in += 3;
  }

  if (end - in) {
    *pos++ = base64_table[in[0] >> 2];
    if (end - in == 1) {
      *pos++ = base64_table[(in[0] & 0x03) << 4];
      *pos++ = '=';
    } else {
      *pos++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
      *pos++ = base64_table[(in[1] & 0x0f) << 2];
    }
    *pos++ = '=';
  }

  *pos = '\0';

#ifdef UNICODE
  int wideLen = olen;
  wchar_t *wideOut =
      static_cast<wchar_t *>(Alloc((wideLen + 1) * sizeof(wchar_t)));
  if (!wideOut) {
    Free(out);
    return String();
  }

  for (int i = 0; i < olen; i++) {
    wideOut[i] = (wchar_t)(unsigned char)out[i];
  }
  wideOut[wideLen] = L'\0';

  String result(wideOut);
  Free(wideOut);
  Free(out);
  return result;
#else
  String result(out);
  Free(out);
  return result;
#endif
}

Buffer Buffer::fromBase64(const String &base64String) {
  if (base64String.isEmpty()) {
    return Buffer();
  }

  String cleaned = base64String.trim();
  if (cleaned.isEmpty()) {
    return Buffer();
  }

  int len = cleaned.length();
  const ATTO_CHAR *str = cleaned.c_str();

  char *src = static_cast<char *>(Alloc(len + 1));
  if (!src) {
    return Buffer();
  }

  for (int i = 0; i < len; i++) {
    ATTO_CHAR wc = str[i];
    if (wc > 127) {
      Free(src);
      return Buffer();
    }
    src[i] = (char)wc;
  }
  src[len] = '\0';

  int pad = 0;
  if (len > 0 && src[len - 1] == '=') {
    pad++;
    if (len > 1 && src[len - 2] == '=') {
      pad++;
    }
  }

  int olen = len / 4 * 3 - pad;
  if (olen <= 0) {
    Free(src);
    return Buffer();
  }

  unsigned char *out = static_cast<unsigned char *>(Alloc(olen));
  if (!out) {
    Free(src);
    return Buffer();
  }

  unsigned char *pos = out;
  int count = 0;
  unsigned int tmp = 0;

  for (int i = 0; i < len; i++) {
    unsigned char c = (unsigned char)src[i];
    unsigned char d = base64_decode_table[c];

    if (d == 64) {
      if (c == '=') {
        break;
      }
      continue;
    }

    tmp = (tmp << 6) | d;
    count++;

    if (count == 4) {
      *pos++ = (tmp >> 16) & 0xff;
      *pos++ = (tmp >> 8) & 0xff;
      *pos++ = tmp & 0xff;
      tmp = 0;
      count = 0;
    }
  }

  if (count == 3) {
    *pos++ = (tmp >> 10) & 0xff;
    *pos++ = (tmp >> 2) & 0xff;
  } else if (count == 2) {
    *pos++ = (tmp >> 4) & 0xff;
  }

  Buffer result;
  result.append(out, olen);

  Free(out);
  Free(src);
  return result;
}

static void crypt_impl(BufferImpl *resultImpl, const BufferImpl *bufImpl,
                       const unsigned char *keyBytes, int keyLen,
                       const unsigned char *nonceBytes, int nonceLen) {
  if (!bufImpl || bufImpl->size == 0 || !resultImpl) {
    return;
  }

  if (keyLen < 32 || nonceLen < 12) {
    return;
  }

  ReadLockGuard guard(&bufImpl->lock);

  int outputSize = bufImpl->size;
  if (!EnsureBufferCapacity(resultImpl, outputSize)) {
    return;
  }

  chacha20_crypt(keyBytes, nonceBytes, bufImpl->data, resultImpl->data,
                 outputSize);
  resultImpl->size = outputSize;
}

Buffer Buffer::crypt(const String &key, const String &nonce) const {
  Buffer result;
  if (!impl) {
    return result;
  }

#ifdef UNICODE
  int keyByteLen = key.length();
  int nonceByteLen = nonce.length();

  unsigned char *keyBytes = (unsigned char *)Alloc(keyByteLen);
  unsigned char *nonceBytes = (unsigned char *)Alloc(nonceByteLen);

  if (!keyBytes || !nonceBytes) {
    Free(keyBytes);
    Free(nonceBytes);
    return result;
  }

  const wchar_t *keyStr = key.c_str();
  const wchar_t *nonceStr = nonce.c_str();

  for (int i = 0; i < keyByteLen; i++) {
    keyBytes[i] = (unsigned char)(keyStr[i] & 0xFF);
  }
  for (int i = 0; i < nonceByteLen; i++) {
    nonceBytes[i] = (unsigned char)(nonceStr[i] & 0xFF);
  }

  crypt_impl(result.impl, impl, keyBytes, keyByteLen, nonceBytes, nonceByteLen);

  Free(keyBytes);
  Free(nonceBytes);

  return result;
#else
  crypt_impl(result.impl, impl, (const unsigned char *)key.c_str(), key.length(),
             (const unsigned char *)nonce.c_str(), nonce.length());
  return result;
#endif
}

Buffer Buffer::crypt(const String &key, const Buffer &nonce) const {
  Buffer result;
  if (!impl || !nonce.impl) {
    return result;
  }

#ifdef UNICODE
  int keyByteLen = key.length();

  unsigned char *keyBytes = (unsigned char *)Alloc(keyByteLen);
  if (!keyBytes) {
    return result;
  }

  const wchar_t *keyStr = key.c_str();
  for (int i = 0; i < keyByteLen; i++) {
    keyBytes[i] = (unsigned char)(keyStr[i] & 0xFF);
  }

  crypt_impl(result.impl, impl, keyBytes, keyByteLen, nonce.impl->data,
             nonce.impl->size);

  Free(keyBytes);
  return result;
#else
  crypt_impl(result.impl, impl, (const unsigned char *)key.c_str(), key.length(),
             nonce.impl->data, nonce.impl->size);
  return result;
#endif
}

Buffer Buffer::crypt(const Buffer &key, const String &nonce) const {
  Buffer result;
  if (!impl || !key.impl) {
    return result;
  }

#ifdef UNICODE
  int nonceByteLen = nonce.length();

  unsigned char *nonceBytes = (unsigned char *)Alloc(nonceByteLen);
  if (!nonceBytes) {
    return result;
  }

  const wchar_t *nonceStr = nonce.c_str();
  for (int i = 0; i < nonceByteLen; i++) {
    nonceBytes[i] = (unsigned char)(nonceStr[i] & 0xFF);
  }

  crypt_impl(result.impl, impl, key.impl->data, key.impl->size, nonceBytes,
             nonceByteLen);

  Free(nonceBytes);
  return result;
#else
  crypt_impl(result.impl, impl, key.impl->data, key.impl->size,
             (const unsigned char *)nonce.c_str(), nonce.length());
  return result;
#endif
}

Buffer Buffer::crypt(const Buffer &key, const Buffer &nonce) const {
  Buffer result;
  if (!impl || !key.impl || !nonce.impl) {
    return result;
  }

  crypt_impl(result.impl, impl, key.impl->data, key.impl->size, nonce.impl->data,
             nonce.impl->size);
  return result;
}

} // namespace attoboy
