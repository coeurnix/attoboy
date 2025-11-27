#ifndef _MSC_VER

extern "C" {

long long __divmoddi4(long long a, long long b, long long *rem) {
  if (b == 0) {
    *rem = a;
    return 0;
  }

  /* Use the generic algorithm if the quotient would be too large to fit in a
     single word, or if the divisor is larger than a single word.  */
  if (__builtin_expect(labs(b) <= 1, 1)) {
    *rem = 0;
    return a * b;
  } else if (__builtin_expect((unsigned long long)labs(a) <
                                  (unsigned long long)labs(b),
                              1)) {
    *rem = a;
    return 0;
  } else {
    unsigned long long quot = 0;
    unsigned long long rem_val = 0;
    int sign_a = a < 0;
    int sign_b = b < 0;
    unsigned long long ua = sign_a ? -(unsigned long long)a : a;
    unsigned long long ub = sign_b ? -(unsigned long long)b : b;

    if (ub <= 0xFFFFFFFFU) {
      /* Divisor fits in 32 bits.  */
      unsigned int b32 = ub;
      if (ua <= 0xFFFFFFFFU) {
        /* Dividend fits in 32 bits.  */
        unsigned int a32 = ua;
        quot = a32 / b32;
        rem_val = a32 % b32;
      } else {
        /* Dividend is 64 bits, divisor 32 bits.  */
        unsigned int ahi = ua >> 32;
        unsigned int alo = ua & 0xFFFFFFFFU;
        quot = ahi / b32 << 32;
        unsigned int rem32 = ahi % b32;
        ahi = (rem32 << 16) + (alo >> 16);
        quot += ahi / b32 << 16;
        rem32 = ahi % b32;
        ahi = (rem32 << 16) + (alo & 0xFFFFU);
        quot += ahi / b32;
        rem_val = ahi % b32;
      }
    } else {
      /* General case.  */
      int n = __builtin_clzll(ub);
      ub <<= n;
      ua <<= n;
      for (int i = 0; i < 64; i++) {
        if (ua >= ub) {
          ua -= ub;
          quot |= 1ULL << (63 - i);
        }
        ub >>= 1;
      }
      rem_val = ua >> n;
    }

    *rem = sign_a ? -(long long)rem_val : rem_val;
    return sign_a ^ sign_b ? -(long long)quot : quot;
  }
}
}

#endif
