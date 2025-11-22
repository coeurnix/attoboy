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

#ifdef UNICODE
  int argc;
  LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);
  if (argv) {
    for (int i = 0; i < argc; i++) {
      impl->cmdLineArgs->append(String(argv[i]));
    }
    LocalFree(argv);
  }
#else
  LPSTR cmdLine = GetCommandLineA();
  if (!cmdLine) {
    return;
  }

  int argc = 0;
  bool inQuote = false;
  bool inArg = false;
  int argStart = 0;

  int len = lstrlenA(cmdLine);
  for (int i = 0; i <= len; i++) {
    char c = cmdLine[i];

    if (c == '"') {
      inQuote = !inQuote;
      if (!inArg) {
        argStart = i + 1;
        inArg = true;
      }
      continue;
    }

    if (!inQuote && (c == ' ' || c == '\t' || c == '\0')) {
      if (inArg) {
        int argLen = i - argStart;
        if (argLen > 0) {
          char *arg = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                        argLen + 1);
          if (arg) {
            int writePos = 0;
            for (int j = argStart; j < i; j++) {
              if (cmdLine[j] != '"') {
                arg[writePos++] = cmdLine[j];
              }
            }
            arg[writePos] = '\0';
            impl->cmdLineArgs->append(String(arg));
            HeapFree(GetProcessHeap(), 0, arg);
          }
        }
        inArg = false;
      }
    } else if (!inArg) {
      argStart = i;
      inArg = true;
    }
  }
#endif
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
