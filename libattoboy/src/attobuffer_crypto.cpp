#include "attobuffer_internal.h"

namespace attoboy {

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

Buffer Buffer::encrypt() const {
  Buffer result;
  if (!impl || impl->size == 0) {
    return result;
  }

  ReadLockGuard guard(&impl->lock);
  WriteLockGuard resultGuard(&result.impl->lock);

  int outputSize = impl->size * 2;
  if (result.impl->capacity < outputSize) {
    FreeBufferData(result.impl->data);
    result.impl->data = AllocBufferData(outputSize);
    result.impl->capacity = outputSize;
  }

  if (!result.impl->data) {
    return result;
  }

  static const unsigned char hexDigits[] = "0123456789ABCDEF";
  for (int i = 0; i < impl->size; i++) {
    unsigned char byte = impl->data[i];
    result.impl->data[i * 2] = hexDigits[byte >> 4];
    result.impl->data[i * 2 + 1] = hexDigits[byte & 0x0F];
  }

  result.impl->size = outputSize;
  return result;
}

Buffer &Buffer::decrypt() {
  if (!impl || impl->size == 0 || (impl->size % 2) != 0) {
    return *this;
  }

  WriteLockGuard guard(&impl->lock);

  int outputSize = impl->size / 2;

  for (int i = 0; i < outputSize; i++) {
    unsigned char highNibble = impl->data[i * 2];
    unsigned char lowNibble = impl->data[i * 2 + 1];

    unsigned char highValue, lowValue;

    if (highNibble >= '0' && highNibble <= '9')
      highValue = highNibble - '0';
    else if (highNibble >= 'A' && highNibble <= 'F')
      highValue = highNibble - 'A' + 10;
    else
      return *this;

    if (lowNibble >= '0' && lowNibble <= '9')
      lowValue = lowNibble - '0';
    else if (lowNibble >= 'A' && lowNibble <= 'F')
      lowValue = lowNibble - 'A' + 10;
    else
      return *this;

    impl->data[i] = (highValue << 4) | lowValue;
  }

  impl->size = outputSize;
  return *this;
}

} // namespace attoboy
