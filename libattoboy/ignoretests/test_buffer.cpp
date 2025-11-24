#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError(ATTO_TEXT("Test failed: "), message);                                        \
    errorCount++;                                                              \
  }

void atto_main() {
  Log(ATTO_TEXT("Running Buffer tests..."));

  // Test default constructor
  Buffer b1;
  TEST(b1.length() == 0, ATTO_TEXT("Default buffer should be empty"));
  TEST(b1.isEmpty(), ATTO_TEXT("Default buffer should report isEmpty() as true"));

  // Test constructor with size
  Buffer b2(1024);
  TEST(b2.length() == 0, ATTO_TEXT("Buffer(size) should start with length 0"));
  TEST(b2.isEmpty(), ATTO_TEXT("Buffer(size) should be empty initially"));

  // Test constructor from String
  String s1(ATTO_TEXT("Hello"));
  Buffer b3(s1);
#ifdef UNICODE
  TEST(b3.length() == 10, ATTO_TEXT("Buffer from 'Hello' should be 10 bytes (5 wchars)"));
#else
  TEST(b3.length() == 5, ATTO_TEXT("Buffer from 'Hello' should be 5 bytes (5 chars)"));
#endif
  TEST(!b3.isEmpty(), ATTO_TEXT("Buffer from string should not be empty"));
  TEST(b3.toString() == s1, ATTO_TEXT("Buffer toString() should match original string"));

  // Test constructor from pointer
#ifdef UNICODE
  const char data[] = {0x41, 0x00, 0x42, 0x00, 0x43, 0x00};
  Buffer b4(reinterpret_cast<const unsigned char *>(data), 6);
  TEST(b4.length() == 6, ATTO_TEXT("Buffer from pointer should have 6 bytes"));
#else
  const char data[] = {0x41, 0x42, 0x43};
  Buffer b4(reinterpret_cast<const unsigned char *>(data), 3);
  TEST(b4.length() == 3, ATTO_TEXT("Buffer from pointer should have 3 bytes"));
#endif
  String s2 = b4.toString();
  TEST(s2 == ATTO_TEXT("ABC"), ATTO_TEXT("Buffer with ABC bytes should convert to 'ABC'"));

  // Test copy constructor
  Buffer b5(b3);
  TEST(b5.length() == b3.length(), ATTO_TEXT("Copy constructor should preserve length"));
  TEST(b5.compare(b3), ATTO_TEXT("Copy should be equal to original"));
  TEST(b5 == b3, ATTO_TEXT("Operator== should work"));

  // Test assignment operator
  Buffer b6;
  b6 = b3;
  TEST(b6.length() == b3.length(), ATTO_TEXT("Assignment should preserve length"));
  TEST(b6.compare(b3), ATTO_TEXT("Assignment should create equal buffer"));

  // Test clear
  Buffer b7(s1);
  b7.clear();
  TEST(b7.length() == 0, ATTO_TEXT("clear() should set length to 0"));
  TEST(b7.isEmpty(), ATTO_TEXT("clear() should make buffer empty"));

  // Test append with String
  Buffer b8;
  b8.append(String(ATTO_TEXT("Hi")));
#ifdef UNICODE
  TEST(b8.length() == 4, ATTO_TEXT("Appending 'Hi' should add 4 bytes"));
#else
  TEST(b8.length() == 2, ATTO_TEXT("Appending 'Hi' should add 2 bytes"));
#endif
  b8.append(String(ATTO_TEXT("!")));
#ifdef UNICODE
  TEST(b8.length() == 6, ATTO_TEXT("Appending '!' should add 2 more bytes"));
#else
  TEST(b8.length() == 3, ATTO_TEXT("Appending '!' should add 1 more byte"));
#endif
  TEST(b8.toString() == ATTO_TEXT("Hi!"), ATTO_TEXT("Appended result should be 'Hi!'"));

  // Test append with Buffer
  Buffer b9;
  Buffer b10(String(ATTO_TEXT("A")));
  Buffer b11(String(ATTO_TEXT("B")));
  b9.append(b10).append(b11);
#ifdef UNICODE
  TEST(b9.length() == 4, ATTO_TEXT("Appending two 1-char buffers should be 4 bytes"));
#else
  TEST(b9.length() == 2, ATTO_TEXT("Appending two 1-char buffers should be 2 bytes"));
#endif
  TEST(b9.toString() == ATTO_TEXT("AB"), ATTO_TEXT("Appended buffers should form 'AB'"));

  // Test append with pointer
  Buffer b12;
#ifdef UNICODE
  const char moreData[] = {0x58, 0x00, 0x59, 0x00};
  b12.append(reinterpret_cast<const unsigned char *>(moreData), 4);
  TEST(b12.length() == 4, ATTO_TEXT("Appending 4 bytes should result in length 4"));
#else
  const char moreData[] = {0x58, 0x59};
  b12.append(reinterpret_cast<const unsigned char *>(moreData), 2);
  TEST(b12.length() == 2, ATTO_TEXT("Appending 2 bytes should result in length 2"));
#endif
  TEST(b12.toString() == ATTO_TEXT("XY"), ATTO_TEXT("Appended bytes should form 'XY'"));

  // Test prepend with String
  Buffer b13(String(ATTO_TEXT("World")));
  b13.prepend(String(ATTO_TEXT("Hello ")));
  TEST(b13.toString() == ATTO_TEXT("Hello World"),
       ATTO_TEXT("Prepending should put string at start"));

  // Test prepend with Buffer
  Buffer b14(String(ATTO_TEXT("2")));
  Buffer b15(String(ATTO_TEXT("1")));
  b14.prepend(b15);
  TEST(b14.toString() == ATTO_TEXT("12"), ATTO_TEXT("Prepending buffer should put it at start"));

  // Test prepend with pointer
  Buffer b16(String(ATTO_TEXT("Y")));
#ifdef UNICODE
  const char prefix[] = {0x58, 0x00};
  b16.prepend(reinterpret_cast<const unsigned char *>(prefix), 2);
#else
  const char prefix[] = {0x58};
  b16.prepend(reinterpret_cast<const unsigned char *>(prefix), 1);
#endif
  TEST(b16.toString() == ATTO_TEXT("XY"), ATTO_TEXT("Prepending bytes should work"));

  // Test insert at start
  Buffer b18(String(ATTO_TEXT("BC")));
  b18.insert(0, String(ATTO_TEXT("A")));
  TEST(b18.toString() == ATTO_TEXT("ABC"), ATTO_TEXT("Inserting at 0 should prepend"));

  // Test insert at end
  Buffer b19(String(ATTO_TEXT("AB")));
  int endPos = b19.length();
  b19.insert(endPos, String(ATTO_TEXT("C")));
  TEST(b19.toString() == ATTO_TEXT("ABC"), ATTO_TEXT("Inserting at end should append"));

  // Test insert in middle (at wchar boundary)
  Buffer b20(String(ATTO_TEXT("AC")));
  Buffer insertBuf(String(ATTO_TEXT("B")));
#ifdef UNICODE
  b20.insert(2, insertBuf);
#else
  b20.insert(1, insertBuf);
#endif
  TEST(b20.toString() == ATTO_TEXT("ABC"), ATTO_TEXT("Inserting buffer in middle should work"));

  // Test insert with pointer (at wchar boundary)
  Buffer b21(String(ATTO_TEXT("AC")));
#ifdef UNICODE
  const char insertData[] = {0x42, 0x00};
  b21.insert(2, reinterpret_cast<const unsigned char *>(insertData), 2);
#else
  const char insertData[] = {0x42};
  b21.insert(1, reinterpret_cast<const unsigned char *>(insertData), 1);
#endif
  TEST(b21.toString() == ATTO_TEXT("ABC"), ATTO_TEXT("Inserting bytes should work"));

  // Test slice
  Buffer b22(String(ATTO_TEXT("ABCDE")));
#ifdef UNICODE
  Buffer sliced = b22.slice(0, 4);
  TEST(sliced.length() == 4, ATTO_TEXT("Slice should extract 4 bytes"));
#else
  Buffer sliced = b22.slice(0, 2);
  TEST(sliced.length() == 2, ATTO_TEXT("Slice should extract 2 bytes"));
#endif
  TEST(sliced.toString() == ATTO_TEXT("AB"), ATTO_TEXT("Sliced bytes should be 'AB'"));

  // Test slice to end
  Buffer b23(String(ATTO_TEXT("ABCDE")));
#ifdef UNICODE
  Buffer slicedEnd = b23.slice(4, -1);
#else
  Buffer slicedEnd = b23.slice(2, -1);
#endif
  TEST(slicedEnd.toString() == ATTO_TEXT("CDE"), ATTO_TEXT("Slice to end should work"));

  // Test remove
  Buffer b24(String(ATTO_TEXT("ABCDE")));
#ifdef UNICODE
  b24.remove(0, 2);
#else
  b24.remove(0, 1);
#endif
  TEST(b24.toString() == ATTO_TEXT("BCDE"), ATTO_TEXT("Removing first character should work"));

  // Test remove to end
  Buffer b25(String(ATTO_TEXT("ABCDE")));
#ifdef UNICODE
  b25.remove(4, -1);
#else
  b25.remove(2, -1);
#endif
  TEST(b25.toString() == ATTO_TEXT("AB"), ATTO_TEXT("Remove to end should work"));

  // Test reverse
  Buffer b26(String(ATTO_TEXT("ABC")));
  b26.reverse();
  String reversed = b26.toString();
  TEST(reversed.length() == 3, ATTO_TEXT("Reversed buffer should have same length"));

  // Test hash
  Buffer b27(String(ATTO_TEXT("Test")));
  Buffer b28(String(ATTO_TEXT("Test")));
  Buffer b29(String(ATTO_TEXT("Different")));
  TEST(b27.hash() == b28.hash(), ATTO_TEXT("Equal buffers should have equal hashes"));
  TEST(b27.hash() != b29.hash(),
       ATTO_TEXT("Different buffers should likely have different hashes"));

  // Test compare
  Buffer b30(String(ATTO_TEXT("Same")));
  Buffer b31(String(ATTO_TEXT("Same")));
  Buffer b32(String(ATTO_TEXT("Different")));
  TEST(b30.compare(b31), ATTO_TEXT("Equal buffers should compare true"));
  TEST(!b30.compare(b32), ATTO_TEXT("Different buffers should compare false"));

  // Test operators
  Buffer b33(String(ATTO_TEXT("A")));
  Buffer b34 = b33 + String(ATTO_TEXT("B"));
  TEST(b34.toString() == ATTO_TEXT("AB"), ATTO_TEXT("Operator+ with String should work"));

  Buffer b35(String(ATTO_TEXT("C")));
  Buffer b36 = b34 + b35;
  TEST(b36.toString() == ATTO_TEXT("ABC"), ATTO_TEXT("Operator+ with Buffer should work"));

  TEST(b33 != b34, ATTO_TEXT("Different buffers should be !="));
  Buffer b37(b33);
  TEST(b33 == b37, ATTO_TEXT("Equal buffers should be =="));

  // Test null pointer handling
  Buffer b38(nullptr, 10);
  TEST(b38.isEmpty(), ATTO_TEXT("Buffer from nullptr should be empty"));

  const unsigned char *nullPtr = nullptr;
  Buffer b39;
  b39.append(nullPtr, 10);
  TEST(b39.isEmpty(), ATTO_TEXT("Appending nullptr should not change buffer"));

  // Report results
  if (errorCount == 0) {
    Log(ATTO_TEXT("All Buffer tests passed!"));
    Exit(0);
  } else {
    LogError(errorCount, ATTO_TEXT(" test(s) failed"));
    Exit(errorCount);
  }
}
