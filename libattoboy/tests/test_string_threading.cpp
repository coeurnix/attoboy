#include "attoboy/attoboy.h"
#include <windows.h>

// Implement new/delete for no-CRT environment
void *operator new(size_t size) { return HeapAlloc(GetProcessHeap(), 0, size); }

void operator delete(void *ptr) noexcept {
  if (ptr)
    HeapFree(GetProcessHeap(), 0, ptr);
}

void operator delete(void *ptr, size_t) noexcept {
  if (ptr)
    HeapFree(GetProcessHeap(), 0, ptr);
}

using namespace attoboy;

// Global pointers for concurrent testing (avoid constructor/destructor issues)
String *g_sharedString = nullptr;
volatile long g_readsDone = 0;
volatile long g_writesDone = 0;

// Reader thread function
DWORD WINAPI ReaderThread(LPVOID param) {
  for (int i = 0; i < 1000; i++) {
    // Various read operations
    int len = g_sharedString->length();
    bool empty = g_sharedString->isEmpty();
    bool contains = g_sharedString->contains("a");
    String copy = g_sharedString->duplicate();
    int hash = g_sharedString->hash();
    bool starts = g_sharedString->startsWith("I");
    bool ends = g_sharedString->endsWith("l");
    int cnt = g_sharedString->count("i");
    String sub = g_sharedString->substring(0, 3);

    // Prevent compiler optimization
    if (len < 0 || hash == -1 || empty == contains) {
    }
  }
  InterlockedIncrement(&g_readsDone);
  return 0;
}

// Writer thread function - with immutable strings, creates new strings
// Note: With immutable strings, these operations don't modify g_sharedString,
// they just create new temporary strings. This tests that read operations
// work correctly even when many threads are creating new strings.
DWORD WINAPI WriterThread(LPVOID param) {
  for (int i = 0; i < 100; i++) {
    // Various operations that create new strings
    String temp = g_sharedString->append("x");
    temp = temp.trim();
    temp = temp.upper();
    temp = temp.lower();
    temp = temp.replace("x", "y");
    temp = temp.prepend("z");
    temp = temp.reverse();
    temp = temp.insert(0, "a");
    temp = temp.remove(0, 1);
    // temp goes out of scope, testing memory management
  }
  InterlockedIncrement(&g_writesDone);
  return 0;
}

// Mutator thread function - creates new strings repeatedly
// With immutable strings, each operation returns a new string
DWORD WINAPI MutatorThread(LPVOID param) {
  String localStr("Mutate");
  for (int i = 0; i < 500; i++) {
    localStr = localStr.append("!");
    localStr = localStr.trim();
    localStr = localStr.upper();
    localStr = localStr.lower();
    localStr = localStr.reverse();
    localStr = localStr.replace("!", "?");
    localStr = localStr.repeat(2);
    localStr = localStr.remove(0, localStr.length() / 2);
  }
  return 0;
}

// Copy constructor stress test
DWORD WINAPI CopyThread(LPVOID param) {
  for (int i = 0; i < 500; i++) {
    String copy1(*g_sharedString);
    String copy2 = g_sharedString->duplicate();
    copy1 = copy1.append("_copy");
    copy2 = copy2.prepend("copy_");
  }
  return 0;
}

void atto_main() {
  // Initialize shared string
  g_sharedString = new String("Initial");

  // Test 1: Multiple readers on shared string
  HANDLE readers[4];
  for (int i = 0; i < 4; i++) {
    readers[i] = CreateThread(nullptr, 0, ReaderThread, nullptr, 0, nullptr);
    if (!readers[i])
      Exit(101);
  }

  WaitForMultipleObjects(4, readers, TRUE, INFINITE);
  for (int i = 0; i < 4; i++) {
    CloseHandle(readers[i]);
  }

  if (g_readsDone != 4)
    Exit(102);

  // Test 2: Concurrent readers and writers
  g_readsDone = 0;
  g_writesDone = 0;
  delete g_sharedString;
  g_sharedString = new String("Reset");

  HANDLE mixed[6];
  for (int i = 0; i < 3; i++) {
    mixed[i] = CreateThread(nullptr, 0, ReaderThread, nullptr, 0, nullptr);
    if (!mixed[i])
      Exit(201);
  }
  for (int i = 3; i < 6; i++) {
    mixed[i] = CreateThread(nullptr, 0, WriterThread, nullptr, 0, nullptr);
    if (!mixed[i])
      Exit(202);
  }

  WaitForMultipleObjects(6, mixed, TRUE, INFINITE);
  for (int i = 0; i < 6; i++) {
    CloseHandle(mixed[i]);
  }

  if (g_readsDone != 3 || g_writesDone != 3)
    Exit(203);

  // Test 3: Independent string objects (no shared state)
  HANDLE mutators[4];
  for (int i = 0; i < 4; i++) {
    mutators[i] = CreateThread(nullptr, 0, MutatorThread, nullptr, 0, nullptr);
    if (!mutators[i])
      Exit(301);
  }

  WaitForMultipleObjects(4, mutators, TRUE, INFINITE);
  for (int i = 0; i < 4; i++) {
    CloseHandle(mutators[i]);
  }

  // Test 4: Copy constructor stress test
  delete g_sharedString;
  g_sharedString = new String("CopyTest");
  HANDLE copiers[4];
  for (int i = 0; i < 4; i++) {
    copiers[i] = CreateThread(nullptr, 0, CopyThread, nullptr, 0, nullptr);
    if (!copiers[i])
      Exit(401);
  }

  WaitForMultipleObjects(4, copiers, TRUE, INFINITE);
  for (int i = 0; i < 4; i++) {
    CloseHandle(copiers[i]);
  }

  // Test 5: Verify string is still in valid state
  int finalLen = g_sharedString->length();
  if (finalLen < 0)
    Exit(501);

  String finalCopy = g_sharedString->duplicate();
  if (finalCopy.length() != finalLen)
    Exit(502);

  delete g_sharedString;
  Exit(0);
}
