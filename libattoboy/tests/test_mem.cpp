#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError("Test failed: ", message);                                        \
    errorCount++;                                                              \
  }

void atto_main() {
  Log("Running memory allocation tests...");

  // Test Alloc with valid size
  void *ptr1 = Alloc(100);
  TEST(ptr1 != nullptr, "Alloc(100) should return non-null pointer");
  if (ptr1 != nullptr) {
    char *data = static_cast<char *>(ptr1);
    for (int i = 0; i < 100; i++) {
      data[i] = static_cast<char>(i);
    }
    bool valid = true;
    for (int i = 0; i < 100; i++) {
      if (data[i] != static_cast<char>(i)) {
        valid = false;
        break;
      }
    }
    TEST(valid, "Memory should be readable and writable");
    Free(ptr1);
  }

  // Test Alloc with zero size
  void *ptr2 = Alloc(0);
  TEST(ptr2 == nullptr, "Alloc(0) should return nullptr");

  // Test Alloc with negative size
  void *ptr3 = Alloc(-10);
  TEST(ptr3 == nullptr, "Alloc(-10) should return nullptr");

  // Test Free with nullptr
  Free(nullptr);
  TEST(true, "Free(nullptr) should not crash");

  // Test Realloc with nullptr (acts as Alloc)
  void *ptr4 = Realloc(nullptr, 50);
  TEST(ptr4 != nullptr, "Realloc(nullptr, 50) should act as Alloc");
  if (ptr4 != nullptr) {
    Free(ptr4);
  }

  // Test Realloc with valid pointer
  void *ptr5 = Alloc(50);
  TEST(ptr5 != nullptr, "Alloc(50) should succeed");
  if (ptr5 != nullptr) {
    char *data = static_cast<char *>(ptr5);
    for (int i = 0; i < 50; i++) {
      data[i] = static_cast<char>(i);
    }
    void *ptr6 = Realloc(ptr5, 100);
    TEST(ptr6 != nullptr, "Realloc(ptr, 100) should succeed");
    if (ptr6 != nullptr) {
      data = static_cast<char *>(ptr6);
      bool valid = true;
      for (int i = 0; i < 50; i++) {
        if (data[i] != static_cast<char>(i)) {
          valid = false;
          break;
        }
      }
      TEST(valid, "Realloc should preserve existing data");
      Free(ptr6);
    }
  }

  // Test Realloc with size 0 (acts as Free)
  void *ptr7 = Alloc(50);
  TEST(ptr7 != nullptr, "Alloc(50) should succeed");
  if (ptr7 != nullptr) {
    void *ptr8 = Realloc(ptr7, 0);
    TEST(ptr8 == nullptr, "Realloc(ptr, 0) should free and return nullptr");
  }

  // Test multiple allocations
  void *ptrs[10];
  for (int i = 0; i < 10; i++) {
    ptrs[i] = Alloc(100 * (i + 1));
    TEST(ptrs[i] != nullptr, "Multiple allocations should succeed");
  }
  for (int i = 0; i < 10; i++) {
    Free(ptrs[i]);
  }
  TEST(true, "Multiple frees should succeed");

  // Report results
  if (errorCount == 0) {
    Log("All memory tests passed!");
    Exit(0);
  } else {
    LogError(errorCount, " test(s) failed");
    Exit(errorCount);
  }
}
