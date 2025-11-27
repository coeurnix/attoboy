#pragma once
#include <windows.h>

namespace attoboy {

class ReadLockGuard {
  SRWLOCK *lock;

public:
  ReadLockGuard(SRWLOCK *l) : lock(l) { AcquireSRWLockShared(lock); }
  ~ReadLockGuard() { ReleaseSRWLockShared(lock); }
};

class WriteLockGuard {
  SRWLOCK *lock;

public:
  WriteLockGuard(SRWLOCK *l) : lock(l) { AcquireSRWLockExclusive(lock); }
  ~WriteLockGuard() { ReleaseSRWLockExclusive(lock); }
};

inline WCHAR *Utf8ToWide(const char *utf8, int *outLen = nullptr) {
  if (!utf8)
    return nullptr;
  int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, nullptr, 0);
  if (len <= 0)
    return nullptr;
  WCHAR *wide = (WCHAR *)HeapAlloc(GetProcessHeap(), 0, len * sizeof(WCHAR));
  if (!wide)
    return nullptr;
  MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wide, len);
  if (outLen)
    *outLen = len - 1;
  return wide;
}

inline char *WideToUtf8(const WCHAR *wide, int *outLen = nullptr) {
  if (!wide)
    return nullptr;
  int len =
      WideCharToMultiByte(CP_UTF8, 0, wide, -1, nullptr, 0, nullptr, nullptr);
  if (len <= 0)
    return nullptr;
  char *utf8 = (char *)HeapAlloc(GetProcessHeap(), 0, len);
  if (!utf8)
    return nullptr;
  WideCharToMultiByte(CP_UTF8, 0, wide, -1, utf8, len, nullptr, nullptr);
  if (outLen)
    *outLen = len - 1;
  return utf8;
}

inline void FreeConvertedString(void *str) {
  if (str)
    HeapFree(GetProcessHeap(), 0, str);
}

inline int countUTF8Characters(const char *str, int byteLen) {
  if (!str || byteLen <= 0)
    return 0;
  int charCount = 0;
  const char *end = str + byteLen;
  while (str < end) {
    unsigned char c = *str;
    if (c < 0x80) {
      str += 1;
    } else if ((c & 0xE0) == 0xC0) {
      str += 2;
    } else if ((c & 0xF0) == 0xE0) {
      str += 3;
    } else if ((c & 0xF8) == 0xF0) {
      str += 4;
    } else {
      str += 1;
    }
    charCount++;
  }
  return charCount;
}

inline int getCharacterByteIndex(const char *str, int charIndex, int byteLen) {
  if (!str || charIndex < 0 || byteLen <= 0)
    return -1;

  int currentChar = 0;
  int byteIndex = 0;
  const char *end = str + byteLen;

  while (str < end && currentChar < charIndex) {
    unsigned char c = *str;
    if (c < 0x80) {
      str += 1;
      byteIndex += 1;
    } else if ((c & 0xE0) == 0xC0) {
      str += 2;
      byteIndex += 2;
    } else if ((c & 0xF0) == 0xE0) {
      str += 3;
      byteIndex += 3;
    } else if ((c & 0xF8) == 0xF0) {
      str += 4;
      byteIndex += 4;
    } else {
      str += 1;
      byteIndex += 1;
    }
    currentChar++;
  }

  if (currentChar == charIndex) {
    return byteIndex;
  }
  return -1;
}

inline int getCharacterLengthAtByteIndex(const char *str, int byteIndex,
                                         int byteLen) {
  if (!str || byteIndex < 0 || byteIndex >= byteLen)
    return 0;
  unsigned char c = str[byteIndex];
  if (c < 0x80) {
    return 1;
  } else if ((c & 0xE0) == 0xC0) {
    return 2;
  } else if ((c & 0xF0) == 0xE0) {
    return 3;
  } else if ((c & 0xF8) == 0xF0) {
    return 4;
  } else {
    return 1;
  }
}

inline int getByteCharacterIndex(const char *str, int byteIndex, int byteLen) {
  if (!str || byteIndex < 0 || byteIndex > byteLen)
    return -1;

  int currentChar = 0;
  int currentByte = 0;
  const char *end = str + byteLen;

  while (str < end && currentByte < byteIndex) {
    unsigned char c = *str;
    if (c < 0x80) {
      str += 1;
      currentByte += 1;
    } else if ((c & 0xE0) == 0xC0) {
      str += 2;
      currentByte += 2;
    } else if ((c & 0xF0) == 0xE0) {
      str += 3;
      currentByte += 3;
    } else if ((c & 0xF8) == 0xF0) {
      str += 4;
      currentByte += 4;
    } else {
      str += 1;
      currentByte += 1;
    }
    currentChar++;
  }

  if (currentByte == byteIndex) {
    return currentChar;
  }
  return -1;
}

inline bool validateUTF8Sequence(const char *str, int byteLen) {
  if (!str)
    return false;
  const char *end = str + byteLen;
  while (str < end) {
    unsigned char c = *str;
    if (c < 0x80) {
      str += 1;
    } else if ((c & 0xE0) == 0xC0) {
      if (str + 2 > end)
        return false;
      if ((str[1] & 0xC0) != 0x80)
        return false;
      str += 2;
    } else if ((c & 0xF0) == 0xE0) {
      if (str + 3 > end)
        return false;
      if ((str[1] & 0xC0) != 0x80)
        return false;
      if ((str[2] & 0xC0) != 0x80)
        return false;
      str += 3;
    } else if ((c & 0xF8) == 0xF0) {
      if (str + 4 > end)
        return false;
      if ((str[1] & 0xC0) != 0x80)
        return false;
      if ((str[2] & 0xC0) != 0x80)
        return false;
      if ((str[3] & 0xC0) != 0x80)
        return false;
      str += 4;
    } else {
      return false;
    }
  }
  return true;
}

} // namespace attoboy
