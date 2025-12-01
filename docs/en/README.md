# Attoboy

A batteries-included C++ library for building tiny Windows utilities. Write clean, readable code that compiles to executables under 100KB.

```cpp
#include <attoboy/attoboy.h>
using namespace attoboy;

void atto_main() {
  Console con;
  String name = con.input("What's your name? ");
  con.println("Hello, " + name + "!", CON_CYAN);
  Exit();
}
```

Compiles to ~12KB. No installers, no dependencies, runs on any Windows machine.

## Why Attoboy?

Most C++ programs pull in the STL and CRT, which adds hundreds of kilobytes of overhead. attoboy skips all that and talks directly to the Windows API, giving you a clean interface without the bloat.

It's designed for programmers who are **new to C++** or anyone building command-line tools, file utilities, web scrapers, or other small programs where portability and size matter.

## What's included?

**Core types:**
- `String` — UTF-8 strings with Python-like methods (`split`, `replace`, `trim`, `contains`, etc.)
- `List` — Dynamic arrays holding mixed types with JSON/CSV support
- `Map` — Key-value dictionaries with type flexibility
- `Set` — Unique value collections with set operations

**File I/O:**
- `Path` — File metadata, navigation, and simple read/write
- `File` — Stream-based I/O for files and sockets
- `Buffer` — Binary data with LZ4 compression and ChaCha20 encryption

**Networking:**
- `WebRequest` / `WebResponse` — HTTP client with JSON support
- `AI` / `Conversation` / `Embedding` — OpenAI-compatible API integration

**Console UI:**
- `Console` — Colors, menus, progress bars, input with history and tab completion
- `Arguments` — Command-line parsing with flags and parameters

**System:**
- `DateTime` — Timestamps with millisecond precision
- `Thread` / `Mutex` — Lightweight threading
- `Subprocess` — Run external programs with I/O streaming
- `Registry` — Windows Registry access
- `Math` — Standard math functions and random number generation

All types use RAII, so you don't manually manage memory. Strings are immutable. Everything is thread-safe.

## Documentation

Ready to learn more?

- **[Getting Started](getting-started/README.md)** — Installation and first steps
- **[Tutorial](tutorial/README.md)** — Comprehensive walkthrough of all features
- **[API Reference](api/README.md)** — Detailed documentation of every class and function

## Requirements

- **Windows** (32-bit executables, runs on all versions)
- **Compiler:** MSVC (recommended) or MinGW GCC
- **Build system:** CMake 3.15+
- **Testing (optional):** Python 3.10+

## License

Boost Software License 1.0  — see [LICENSE](https://github.com/coeurnix/attoboy/blob/main/LICENSE) for details.

Created by [coeurnix](https://github.com/coeurnix).
