#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError(ATTO_TEXT("Test failed: "), message);                             \
    errorCount++;                                                              \
  }

void atto_main() {
  Log(ATTO_TEXT("Starting Buffer c_ptr simple tests..."));

  // Test 1: Empty buffer c_ptr
  Log(ATTO_TEXT("Test 1: Empty buffer c_ptr"));
  Buffer b1;
  int len1;
  const unsigned char *ptr1 = b1.c_ptr(&len1);
  TEST(ptr1 == nullptr, ATTO_TEXT("Empty buffer should return nullptr"));
  TEST(len1 == 0, ATTO_TEXT("Empty buffer should return length 0"));
  Log(ATTO_TEXT("Test 1 passed"));

  // Test 2: Buffer from String c_ptr
  Log(ATTO_TEXT("Test 2: Buffer from String c_ptr"));
  String s1(ATTO_TEXT("Hi"));
  Buffer b2(s1);
  int len2;
  const unsigned char *ptr2 = b2.c_ptr(&len2);
  TEST(ptr2 != nullptr,
       ATTO_TEXT("Non-empty buffer should return valid pointer"));

#ifdef UNICODE
  TEST(len2 == 4, ATTO_TEXT("Buffer from 'Hi' should be 4 bytes (2 wchars)"));
  // Verify data is correct (little-endian UTF-16LE)
  TEST(ptr2[0] == 0x48 && ptr2[1] == 0x00, ATTO_TEXT("First byte should be H"));
  TEST(ptr2[2] == 0x69 && ptr2[3] == 0x00, ATTO_TEXT("Third byte should be i"));
#else
  TEST(len2 == 2, ATTO_TEXT("Buffer from 'Hi' should be 2 bytes (2 chars)"));
  // Verify data is correct (ANSI)
  TEST(ptr2[0] == 0x48, ATTO_TEXT("First byte should be H"));
  TEST(ptr2[1] == 0x69, ATTO_TEXT("Second byte should be i"));
#endif

  Log(ATTO_TEXT("Test 2 passed"));

  // Test 3: Buffer from pointer c_ptr
  Log(ATTO_TEXT("Test 3: Buffer from pointer c_ptr"));
  const unsigned char data[] = {0x41, 0x42, 0x43, 0x44, 0x45};
  Buffer b3(data, 5);
  int len3;
  const unsigned char *ptr3 = b3.c_ptr(&len3);
  TEST(ptr3 != nullptr,
       ATTO_TEXT("Buffer from pointer should return valid pointer"));
  TEST(len3 == 5, ATTO_TEXT("Buffer should have length 5"));
  TEST(ptr3[0] == 0x41 && ptr3[1] == 0x42 && ptr3[2] == 0x43 &&
           ptr3[3] == 0x44 && ptr3[4] == 0x45,
       ATTO_TEXT("Data should match input"));
  Log(ATTO_TEXT("Test 3 passed"));

  // Test 4: Const correctness - returned pointer should not allow modification
  Log(ATTO_TEXT("Test 4: Const correctness"));
  Buffer b4(String(ATTO_TEXT("Test")));
  int len4;
  const unsigned char *ptr4 = b4.c_ptr(&len4);
  TEST(ptr4 != nullptr, ATTO_TEXT("Buffer should return valid pointer"));
#ifdef UNICODE
  TEST(len4 == 8, ATTO_TEXT("Buffer should have length 8 (4 wchars)"));
#else
  TEST(len4 == 4, ATTO_TEXT("Buffer should have length 4 (4 chars)"));
#endif
  // Note: We can't test const violation directly, but the API declares it const
  Log(ATTO_TEXT("Test 4 passed"));

  // Test 5: Multiple calls return same data
  Log(ATTO_TEXT("Test 5: Multiple calls consistency"));
  Buffer b5(String(ATTO_TEXT("ABC")));
  int len5a, len5b;
  const unsigned char *ptr5a = b5.c_ptr(&len5a);
  const unsigned char *ptr5b = b5.c_ptr(&len5b);
  TEST(ptr5a == ptr5b, ATTO_TEXT("Multiple calls should return same pointer"));
  TEST(len5a == len5b, ATTO_TEXT("Multiple calls should return same length"));
#ifdef UNICODE
  TEST(len5a == 6, ATTO_TEXT("Length should be 6 for 'ABC' (wchar)"));
#else
  TEST(len5a == 3, ATTO_TEXT("Length should be 3 for 'ABC' (char)"));
#endif
  Log(ATTO_TEXT("Test 5 passed"));

  // Test 6: Null length parameter
  Log(ATTO_TEXT("Test 6: Null length parameter"));
  Buffer b6(String(ATTO_TEXT("X")));
  const unsigned char *ptr6 = b6.c_ptr(nullptr);
  TEST(ptr6 == nullptr,
       ATTO_TEXT("Null length parameter should return nullptr"));
  Log(ATTO_TEXT("Test 6 passed"));

  // Report results
  Log(ATTO_TEXT("All c_ptr tests completed"));
  if (errorCount == 0) {
    Log(ATTO_TEXT("All tests passed!"));
    Exit(0);
  } else {
    LogError(errorCount, ATTO_TEXT(" test(s) failed"));
    Exit(errorCount);
  }
}
