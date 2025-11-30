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

## Why attoboy?

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

## Quick start

1. **Install:** Visual Studio 2022 (MSVC) or MinGW + CMake
2. **Build:**
   ```bash
   cmake -G "Visual Studio 17 2022" -A Win32 -B build_msvc
   cmake --build build_msvc --config Release
   ```
3. **Write code:** Navigate to `build_msvc/dist/` and create `hello.cpp`
4. **Compile:**
   ```bash
   attobuild.bat hello.cpp
   ```

The `dist` folder has everything you need: headers, library, docs, and the `attobuild.bat` script.

## Documentation

*Documentation available in [English](docs/en/README.md), [Русский](docs/ru/README.md), and [中文](docs/zh/README.md).*

- **[Getting Started](docs/en/getting-started/README.md)** — Installation and first steps
- **[Tutorial](docs/en/tutorial/README.md)** — Comprehensive walkthrough of all features
- **API Reference** — Full docs in `docs/html/index.html`



## Examples

The library includes 13 working examples (with MSVC build sizes):

- `example_hello.exe` (4KB) — "Hello world"
- `example_timer.exe` (23KB) — Command timer utility
- `example_crypt.exe` (24KB) — File encryption with ChaCha20
- `example_csv_to_json.exe` (25KB) — CSV/JSON converter
- `example_web_get.exe` (28KB) — HTTP downloader
- `example_notes.exe` (30KB) — Note-taking app with persistence
- `example_directx.exe` (30KB) — 3D sphere with Direct3D 11
- `example_simple_chat.exe` (40KB) — AI chat client
- `example_gui_chat.exe` (39KB) — Windows GUI chat app
- `example_console.exe` (44KB) — Console UI showcase
- `example_web_server.exe` (44KB) — HTTP server
- `example_minesweeper.exe` (13KB) — Minesweeper game
- `example_kitchen_sink.exe` (122KB) — Every library feature in one program

## Requirements

- **Windows** (32-bit executables, runs on all versions)
- **Compiler:** MSVC (recommended) or MinGW GCC
- **Build system:** CMake 3.15+
- **Testing (optional):** Python 3.10+

## License

MIT License — see [LICENSE](LICENSE) for details.

Created by [coeurnix](https://github.com/coeurnix).
