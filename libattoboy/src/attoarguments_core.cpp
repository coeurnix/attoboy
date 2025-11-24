#include "attoarguments_internal.h"
#include <new>

namespace attoboy {

Arguments::Arguments() {
  impl = (ArgumentsImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                    sizeof(ArgumentsImpl));
  InitializeSRWLock(&impl->lock);

  impl->argDefs = (List *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(List));
  impl->cmdLineArgs = (List *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(List));
  impl->helpText = (String *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(String));

  if (impl->argDefs) new (impl->argDefs) List();
  if (impl->cmdLineArgs) new (impl->cmdLineArgs) List();
  if (impl->helpText) new (impl->helpText) String();

  LPWSTR cmdLineWide = GetCommandLineW();
  if (!cmdLineWide) {
    return;
  }

  int argc = 0;
  bool inQuote = false;
  bool inArg = false;
  int argStart = 0;

  int len = lstrlenW(cmdLineWide);
  for (int i = 0; i <= len; i++) {
    WCHAR c = cmdLineWide[i];

    if (c == L'"') {
      inQuote = !inQuote;
      if (!inArg) {
        argStart = i + 1;
        inArg = true;
      }
      continue;
    }

    if (!inQuote && (c == L' ' || c == L'\t' || c == L'\0')) {
      if (inArg) {
        int argLen = i - argStart;
        if (argLen > 0) {
          WCHAR *argWide = (WCHAR *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                              (argLen + 1) * sizeof(WCHAR));
          if (argWide) {
            int writePos = 0;
            for (int j = argStart; j < i; j++) {
              if (cmdLineWide[j] != L'"') {
                argWide[writePos++] = cmdLineWide[j];
              }
            }
            argWide[writePos] = L'\0';

            char* argUtf8 = WideToUtf8(argWide);
            if (argUtf8) {
              impl->cmdLineArgs->append(String(argUtf8));
              FreeConvertedString(argUtf8);
            }
            HeapFree(GetProcessHeap(), 0, argWide);
          }
        }
        inArg = false;
      }
    } else if (!inArg) {
      argStart = i;
      inArg = true;
    }
  }
}

Arguments::Arguments(const Arguments &other) {
  impl = (ArgumentsImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                    sizeof(ArgumentsImpl));
  InitializeSRWLock(&impl->lock);

  impl->argDefs = (List *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(List));
  impl->cmdLineArgs = (List *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(List));
  impl->helpText = (String *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(String));

  if (other.impl) {
    ReadLockGuard guard(&other.impl->lock);
    if (impl->argDefs) new (impl->argDefs) List(*other.impl->argDefs);
    if (impl->cmdLineArgs) new (impl->cmdLineArgs) List(*other.impl->cmdLineArgs);
    if (impl->helpText) new (impl->helpText) String(*other.impl->helpText);
  } else {
    if (impl->argDefs) new (impl->argDefs) List();
    if (impl->cmdLineArgs) new (impl->cmdLineArgs) List();
    if (impl->helpText) new (impl->helpText) String();
  }
}

Arguments::~Arguments() {
  if (impl) {
    if (impl->argDefs) {
      impl->argDefs->~List();
      HeapFree(GetProcessHeap(), 0, impl->argDefs);
    }
    if (impl->cmdLineArgs) {
      impl->cmdLineArgs->~List();
      HeapFree(GetProcessHeap(), 0, impl->cmdLineArgs);
    }
    if (impl->helpText) {
      impl->helpText->~String();
      HeapFree(GetProcessHeap(), 0, impl->helpText);
    }
    HeapFree(GetProcessHeap(), 0, impl);
  }
}

Arguments &Arguments::operator=(const Arguments &other) {
  if (this == &other) {
    return *this;
  }

  if (impl && other.impl) {
    WriteLockGuard guard(&impl->lock);
    ReadLockGuard otherGuard(&other.impl->lock);

    *impl->argDefs = *other.impl->argDefs;
    *impl->cmdLineArgs = *other.impl->cmdLineArgs;
    *impl->helpText = *other.impl->helpText;
  }

  return *this;
}

} // namespace attoboy
