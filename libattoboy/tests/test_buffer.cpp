#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError("Test failed: ", message);                                        \
    errorCount++;                                                              \
  }

void atto_main() {
  Log("Running Buffer tests...");

  // Test default constructor
  Buffer b1;
  TEST(b1.length() == 0, "Default buffer should be empty");
  TEST(b1.isEmpty(), "Default buffer should report isEmpty() as true");

  // Test constructor with size
  Buffer b2(1024);
  TEST(b2.length() == 0, "Buffer(size) should start with length 0");
  TEST(b2.isEmpty(), "Buffer(size) should be empty initially");

  // Test constructor from String
  String s1("Hello");
  Buffer b3(s1);
  TEST(b3.length() == 10, "Buffer from 'Hello' should be 10 bytes (5 wchars)");
  TEST(!b3.isEmpty(), "Buffer from string should not be empty");
  TEST(b3.toString() == s1, "Buffer toString() should match original string");

  // Test constructor from pointer
  const char data[] = {0x41, 0x00, 0x42, 0x00, 0x43, 0x00};
  Buffer b4(data, 6);
  TEST(b4.length() == 6, "Buffer from pointer should have 6 bytes");
  String s2 = b4.toString();
  TEST(s2 == "ABC", "Buffer with ABC bytes should convert to 'ABC'");

  // Test copy constructor
  Buffer b5(b3);
  TEST(b5.length() == b3.length(), "Copy constructor should preserve length");
  TEST(b5.compare(b3), "Copy should be equal to original");
  TEST(b5 == b3, "Operator== should work");

  // Test assignment operator
  Buffer b6;
  b6 = b3;
  TEST(b6.length() == b3.length(), "Assignment should preserve length");
  TEST(b6.compare(b3), "Assignment should create equal buffer");

  // Test clear
  Buffer b7(s1);
  b7.clear();
  TEST(b7.length() == 0, "clear() should set length to 0");
  TEST(b7.isEmpty(), "clear() should make buffer empty");

  // Test append with String
  Buffer b8;
  b8.append(String("Hi"));
  TEST(b8.length() == 4, "Appending 'Hi' should add 4 bytes");
  b8.append(String("!"));
  TEST(b8.length() == 6, "Appending '!' should add 2 more bytes");
  TEST(b8.toString() == "Hi!", "Appended result should be 'Hi!'");

  // Test append with Buffer
  Buffer b9;
  Buffer b10(String("A"));
  Buffer b11(String("B"));
  b9.append(b10).append(b11);
  TEST(b9.length() == 4, "Appending two 1-char buffers should be 4 bytes");
  TEST(b9.toString() == "AB", "Appended buffers should form 'AB'");

  // Test append with pointer
  Buffer b12;
  const char moreData[] = {0x58, 0x00, 0x59, 0x00};
  b12.append(moreData, 4);
  TEST(b12.length() == 4, "Appending 4 bytes should result in length 4");
  TEST(b12.toString() == "XY", "Appended bytes should form 'XY'");

  // Test prepend with String
  Buffer b13(String("World"));
  b13.prepend(String("Hello "));
  TEST(b13.toString() == "Hello World",
       "Prepending should put string at start");

  // Test prepend with Buffer
  Buffer b14(String("2"));
  Buffer b15(String("1"));
  b14.prepend(b15);
  TEST(b14.toString() == "12", "Prepending buffer should put it at start");

  // Test prepend with pointer
  Buffer b16(String("Y"));
  const char prefix[] = {0x58, 0x00};
  b16.prepend(prefix, 2);
  TEST(b16.toString() == "XY", "Prepending bytes should work");

  // Test insert at start
  Buffer b18(String("BC"));
  b18.insert(0, String("A"));
  TEST(b18.toString() == "ABC", "Inserting at 0 should prepend");

  // Test insert at end
  Buffer b19(String("AB"));
  int endPos = b19.length();
  b19.insert(endPos, String("C"));
  TEST(b19.toString() == "ABC", "Inserting at end should append");

  // Test insert in middle (at wchar boundary)
  Buffer b20(String("AC"));
  Buffer insertBuf(String("B"));
  b20.insert(2, insertBuf);
  TEST(b20.toString() == "ABC", "Inserting buffer in middle should work");

  // Test insert with pointer (at wchar boundary)
  Buffer b21(String("AC"));
  const char insertData[] = {0x42, 0x00};
  b21.insert(2, insertData, 2);
  TEST(b21.toString() == "ABC", "Inserting bytes should work");

  // Test slice
  Buffer b22(String("ABCDE"));
  Buffer sliced = b22.slice(0, 4);
  TEST(sliced.length() == 4, "Slice should extract 4 bytes");
  TEST(sliced.toString() == "AB", "Sliced 4 bytes should be 'AB'");

  // Test slice to end
  Buffer b23(String("ABCDE"));
  Buffer slicedEnd = b23.slice(4, -1);
  TEST(slicedEnd.toString() == "CDE", "Slice to end should work");

  // Test remove
  Buffer b24(String("ABCDE"));
  b24.remove(0, 2);
  TEST(b24.toString() == "BCDE", "Removing first byte should work");

  // Test remove to end
  Buffer b25(String("ABCDE"));
  b25.remove(4, -1);
  TEST(b25.toString() == "AB", "Remove to end should work");

  // Test reverse
  Buffer b26(String("ABC"));
  b26.reverse();
  String reversed = b26.toString();
  TEST(reversed.length() == 3, "Reversed buffer should have same length");

  // Test hash
  Buffer b27(String("Test"));
  Buffer b28(String("Test"));
  Buffer b29(String("Different"));
  TEST(b27.hash() == b28.hash(), "Equal buffers should have equal hashes");
  TEST(b27.hash() != b29.hash(), "Different buffers should likely have different hashes");

  // Test compare
  Buffer b30(String("Same"));
  Buffer b31(String("Same"));
  Buffer b32(String("Different"));
  TEST(b30.compare(b31), "Equal buffers should compare true");
  TEST(!b30.compare(b32), "Different buffers should compare false");

  // Test operators
  Buffer b33(String("A"));
  Buffer b34 = b33 + String("B");
  TEST(b34.toString() == "AB", "Operator+ with String should work");

  Buffer b35(String("C"));
  Buffer b36 = b34 + b35;
  TEST(b36.toString() == "ABC", "Operator+ with Buffer should work");

  TEST(b33 != b34, "Different buffers should be !=");
  Buffer b37(b33);
  TEST(b33 == b37, "Equal buffers should be ==");

  // Test null pointer handling
  Buffer b38(nullptr, 10);
  TEST(b38.isEmpty(), "Buffer from nullptr should be empty");

  const char *nullPtr = nullptr;
  Buffer b39;
  b39.append(nullPtr, 10);
  TEST(b39.isEmpty(), "Appending nullptr should not change buffer");

  // Report results
  if (errorCount == 0) {
    Log("All Buffer tests passed!");
    Exit(0);
  } else {
    LogError(errorCount, " test(s) failed");
    Exit(errorCount);
  }
}
