# Getting Started

---

## Welcome to Attoboy!

Attoboy is a batteries-included C++ library designed to make building extremely small, portable Windows executables as easy as writing Python scripts. Whether you're a complete beginner to C++ or an experienced developer who wants to create tiny, self-contained utilities, attoboy offers you a clean, modern, and beginner-friendly way to write powerful programs.

### What Makes Attoboy Special?

- **Tiny executables**: Most programs compile to 10-100 KB in size
- **Zero dependencies**: Your executables run on any Windows machine without installers or runtimes
- **Batteries included**: String handling, collections, file I/O, networking, AI integration, and more—all in one library
- **Beginner-friendly**: Clean, readable API inspired by Python and modern languages
- **100% tested**: Complete test coverage ensures reliability
- **Well documented**: Comprehensive documentation and tutorials to help you succeed

If you've ever wanted to create command-line tools, file utilities, web clients, or even AI-powered applications that compile to executables smaller than most image files, you're in the right place.

---

## Prerequisites

Before you can build attoboy and start creating your own programs, you'll need to install a few tools. Don't worry; this is straightforward, and we'll guide you through it.

### 1. A C++ Compiler

Attoboy supports two compiler toolchains on Windows. You only need to install **one** of these (though you can install both if you'd like):

#### Option A: Visual Studio (MSVC) - Recommended

Microsoft Visual Studio's MSVC compiler produces the smallest executables with attoboy. We recommend using **Visual Studio 2022 Community Edition**, which is completely free.

**Download and Install:**

1. Visit the [Visual Studio Downloads page](https://visualstudio.microsoft.com/downloads/)
2. Download **Visual Studio Community 2022** (it's free!)
3. Run the installer
4. When prompted to select workloads, choose **"Desktop development with C++"**
5. Under "Installation details" on the right, make sure these are selected:
   - MSVC v143 - VS 2022 C++ x64/x86 build tools
   - C++ CMake tools for Windows
6. Click "Install" and wait for it to complete (this may take 20-30 minutes)

**After installation**, you'll need to use the "Developer Command Prompt for VS 2022" or "x86 Native Tools Command Prompt for VS 2022" to build attoboy. You can find these in your Start Menu under "Visual Studio 2022".

#### Option B: MinGW-w64 (GCC)

MinGW provides the GCC compiler for Windows. It's a good alternative if you prefer GCC or want a lighter-weight installation.

**Download and Install:**

1. Visit the [WinLibs Downloads page](https://winlibs.com/) (recommended MinGW distribution)
2. Download the latest **UCRT runtime** release (look for the Win32 version)
3. Extract the downloaded archive to a location like `C:\mingw32`
4. Add the `bin` directory to your PATH:
   - Open "Environment Variables" (search for "environment" in Start Menu)
   - Under "System variables", find and edit "Path"
   - Click "New" and add `C:\mingw32\bin` (or wherever you extracted it)
   - Click "OK" to save

**Verify installation** by opening a new Command Prompt and typing:
```
gcc --version
```

You should see version information displayed.

### 2. CMake Build System

CMake is a cross-platform build system that makes it easy to compile attoboy and its examples.

**Download and Install:**

1. Visit the [CMake Downloads page](https://cmake.org/download/)
2. Download the latest **Windows x64 Installer** (look for a file like `cmake-3.xx.x-windows-x86_64.msi`)
3. Run the installer
4. When prompted, select **"Add CMake to the system PATH for all users"** (or current user)
5. Click through the rest of the installation

**Verify installation** by opening a new Command Prompt and typing:
```
cmake --version
```

You should see version information displayed.

### 3. (Optional) Python

Python is only needed if you want to run the comprehensive test suite. It's not required for building attoboy or using it in your own programs.

If you'd like to run tests:

1. Visit [Python.org](https://www.python.org/downloads/)
2. Download Python 3.10 or later
3. Run the installer
4. **Important**: Check "Add Python to PATH" during installation
5. Click "Install Now"

---

## Building Attoboy

Now that you have your tools installed, let's build attoboy. This process will compile the library and create a convenient "distribution" folder that contains everything you need to start writing your own programs.

### Step 1: Get the Source Code

If you haven't already, download or clone the attoboy repository:

```bash
git clone https://github.com/coeurnix/attoboy.git
cd attoboy
```

Or download and extract the ZIP file from GitHub.

### Step 2: Generate Build Files

Open a command prompt in the attoboy directory. The commands differ slightly depending on which compiler you're using:

#### For MSVC (Visual Studio):

Make sure you're using the **"x86 Native Tools Command Prompt for VS 2022"** or **"Developer Command Prompt for VS 2022"** (not a regular Command Prompt).

```bash
cmake -G "Visual Studio 17 2022" -A Win32 -B build_msvc
```

This creates a Visual Studio solution in the `build_msvc` directory.

#### For MinGW (GCC):

```bash
cmake -G "MinGW Makefiles" -B build
```

This creates Makefiles in the `build` directory.

### Step 3: Build Everything

Now compile the library, examples, and create the distribution package:

#### For MSVC:

```bash
cmake --build build_msvc --config Release
```

#### For MinGW:

```bash
cmake --build build
```

This will take a minute or two. You'll see progress messages as CMake builds the library, all the example programs, and packages everything into the distribution folder.

### Step 4: Celebrate!

If the build succeeded, you should see a success message. Your compiled library and examples are now ready.

**What was built?**

- **Library**: The compiled attoboy library (`attoboy.lib` for MSVC or `libattoboy.a` for MinGW)
- **Examples**: 15 example programs in `build_msvc/examples/` or `build/examples/`
- **Distribution**: A ready-to-use package in `build_msvc/dist/` or `build/dist/`

The **dist** folder is special—it contains everything you need to build your own attoboy programs without touching CMake again!

---

## Using the Distribution Folder

The `dist` folder (inside `build_msvc` or `build`) is a complete, self-contained package for building attoboy programs. Here is what's inside:

```
dist/
├── include/
│   └── attoboy/
│       └── attoboy.h       (The complete attoboy library header)
├── lib/
│   └── attoboy.lib         (The compiled library - .lib for MSVC, .a for MinGW)
├── docs/                   (Complete HTML documentation)
└── attobuild.bat           (Magic build script)
```

### The Magic of attobuild.bat

The `attobuild.bat` script is a simple, powerful way to compile your attoboy programs. It automatically detects your compiler (MSVC or MinGW) and uses the correct flags to produce tiny, optimized executables.

### Your First Program

Let's create your first attoboy program.

**Step 1:** Navigate to the `dist` folder:

```bash
cd build_msvc\dist
```
(or `cd build\dist` for MinGW)

**Step 2:** Create a new file called `hello.cpp` with this content:

```cpp
#include <attoboy/attoboy.h>

using namespace attoboy;

void atto_main() {
  Log("Hello from my first attoboy program!");
  Exit();
}
```

**Step 3:** Build it with attobuild:

```bash
attobuild.bat hello.cpp
```

**Step 4:** Run your program:

```bash
hello.exe
```

You should see: `Hello from my first attoboy program!`

**Step 5:** Check the size:

```bash
dir hello.exe
```

Your executable should be around 4-5 KB. That's a complete, self-contained program smaller than most text documents.

### Building More Complex Programs

The `attobuild.bat` script works the same way for any attoboy program, no matter how complex:

```bash
attobuild.bat my_utility.cpp
attobuild.bat calculator.cpp
attobuild.bat web_scraper.cpp
```

Just write your code, run `attobuild.bat yourfile.cpp`, and you'll get a tiny executable ready to share with anyone.

### Understanding the Build Process

If you're curious about what `attobuild.bat` does behind the scenes, feel free to open it in a text editor. It's thoroughly commented and shows exactly what compiler flags are needed to produce small executables. You can use this knowledge to integrate attoboy into your own CMake projects or build systems.

---

## Running the Test Suite (Optional)

If you have Python installed and want to verify that attoboy works correctly on your system, you can run the comprehensive test suite:

### Basic Tests (No Internet Required)

```bash
cmake --build build_msvc --config Release --target coverage
```

This runs all tests, creating a mock server for Internet and AI calls.. It will display coverage statistics at the end.

### Complete Tests (Requires OpenAI API Key)

```bash
set OPENAI_API_KEY=your_api_key_here
cmake --build build_msvc --config Release --target coverage_live
```

This runs **all** tests, including using real API calls to OpenAI. Note that this will cost a very small amount of money.

---

## Documentation and Learning Resources

Now that you're set up, here's where to go next:

### Tutorial

The **[Tutorial](../tutorial/README.md)** is the best place to start learning attoboy. It takes you through the library's features with clear explanations and practical examples. Even if you're completely new to C++, the tutorial will make you feel at home.

### API Documentation

The **docs** folder (included in your `dist` folder) contains complete API documentation for every class and function in attoboy. Open `docs/index.html` in your web browser to browse the full reference.

Here's a quick peek at the main types you'll use:

- **String**: Immutable UTF-8 strings with rich manipulation methods
- **List**: Dynamic arrays that can hold mixed types
- **Map**: Key-value dictionaries
- **Set**: Collections of unique values
- **Path**: File and directory path manipulation
- **File**: File and socket I/O
- **Buffer**: Binary data handling with compression and encryption
- **DateTime**: Time and date handling
- **WebRequest/WebResponse**: HTTP client functionality
- **AI/Conversation/Embedding**: OpenAI-compatible AI integration
- **Console**: Rich console UI with colors, menus, and progress bars
- **Arguments**: Command-line argument parsing
- **Thread/Mutex**: Multi-threading support
- **Math**: Mathematical operations and constants
- **Subprocess**: Running external programs
- **Registry**: Windows Registry access

---

## Getting Help

If you run into issues or have questions:

1. **Check the Tutorial**: Most common questions are answered there
2. **Review the Examples**: See real working code demonstrating features
3. **Read the API Docs**: Detailed documentation for every function
4. **Check GitHub Issues**: See if others have encountered similar issues
5. **Open an Issue**: Report bugs or ask questions on GitHub

---

Have fun and thanks for using Attoboy!

---

*Made by [coeurnix](https://github.com/coeurnix)*

*Licensed under the Boost Software License 1.0*
