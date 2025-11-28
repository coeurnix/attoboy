# Globals

## Overview

The global functions in the attoboy library provide a small, focused set of **system-level utilities** and a **structured logging subsystem** that you can use anywhere in your program. They are designed to work without the C runtime (CRT) or standard library, and they integrate smoothly with attoboy’s own types, especially `String`.

At a high level, these functions cover:

* **Application entry and process control**

  * `atto_main()` – the custom entry point for attoboy programs.
  * `Exit()` – terminate the process with an exit code.
  * `Sleep()` – pause the current thread.

* **Environment and process information**

  * `GetEnv()` / `SetEnv()` – environment variables.
  * `GetUserName()` / `GetUserDisplayName()` – current user information.
  * `GetProcessId()` – current process ID.

* **Memory management**

  * `Alloc()` / `Realloc()` / `Free()` – heap allocation without CRT.

* **Logging subsystem**

  * `EnableLoggingToFile()` / `EnableLoggingToConsole()` – choose where logs go.
  * `Log()`, `LogDebug()`, `LogInfo()`, `LogWarning()`, `LogError()` – severity-aware logging APIs.

These utilities are intentionally minimal but powerful enough to cover most needs in small tools and utilities.

### System-Level Concepts

Before diving into the reference, it helps to clarify several system-level ideas that these functions abstract.

#### Processes and Exit Codes

A **process** is a running program instance managed by the operating system. When your program finishes, it returns an **exit code** (also called a “return code” or “status code”) to the OS:

* `0` usually means **success**.
* Non-zero values indicate different **error conditions** or reasons for failure.

attoboy’s `Exit(int exitCode)` provides a simple, explicit way to terminate the current process with a chosen exit code.

#### Environment Variables

An **environment variable** is a named key–value pair that:

* Is associated with a process.
* Is inherited by child processes (unless modified).
* Often stores configuration like `PATH`, `HOME`, API keys, or application settings.

attoboy wraps environment access through:

* `GetEnv(name)` – read a variable.
* `SetEnv(name, value)` – set or modify a variable.

Both use `String` for names and values, making them easy to combine with other attoboy APIs.

#### Heap Memory Management

In typical C++ programs, you might allocate memory using `new`/`delete` or `malloc`/`free`. In attoboy, to keep binaries small and avoid CRT, you instead use:

* `Alloc(size)` – allocate a block of `size` bytes.
* `Realloc(ptr, size)` – resize a previously allocated block.
* `Free(ptr)` – release memory.

These functions behave similarly to their CRT counterparts but are provided by attoboy itself. They return `void *` pointers and are **null-safe** where documented.

#### Logging, Log Levels, and Destinations

Logging is the practice of writing information about your program’s behavior to a **destination** such as:

* The **console** (standard output).
* A **log file** on disk.

The attoboy logging subsystem has three main parts:

1. **Log destination configuration**

   * `EnableLoggingToFile(path, truncate)` – send logs to a file.
   * `EnableLoggingToConsole()` – send logs to the console (default).

   Only one destination is active at a time; switching is **global** for the process.

2. **Log severity functions**

   * `Log()` – generic logging without severity prefix.
   * `LogDebug()` – debug-level messages.
   * `LogInfo()` – informational messages.
   * `LogWarning()` – warnings.
   * `LogError()` – errors.

   All of these accept any number of arguments, and each argument is converted to `String` internally. A newline is automatically appended, so you do not need to add `"\n"` manually.

3. **Compile-time log level selection (macros)**

   The header documents log level selection via preprocessor defines:

   * `ATTOBOY_LOG_DEBUG_ENABLE`   – enables Debug, Info, Warning, Error.
   * `ATTOBOY_LOG_INFO_ENABLE`    – enables Info, Warning, Error (default).
   * `ATTOBOY_LOG_WARNING_ENABLE` – enables Warning, Error.
   * `ATTOBOY_LOG_ERROR_ENABLE`   – enables Error only.
   * `ATTOBOY_LOG_DISABLE`        – disables all logging.

   You define **one** of these (or `ATTOBOY_LOG_DISABLE`) **before** including `attoboy.h` in a translation unit to control which logging functions are compiled in:

   * Calls to disabled log levels are compiled away, incurring no runtime cost.
   * If you do not define anything and do not disable logging, `ATTOBOY_LOG_INFO_ENABLE` is used by default.

The log functions use an internal helper to prefix messages with timestamps and, for levelled logs, severity tags like `"DEBUG YYYY-MM-DDTHH:MM:SS.fffZ: ..."`. You do not need to format timestamps yourself.

---

#### `atto_main`

**Signature**

```cpp
extern "C" void atto_main();
```

**Synopsis**
Application entry point. Define this instead of main().

**Parameters**

* *(none; you must define this function in your program)*

**Return value**

* *(function you define; not called by you directly)*

**In Depth**

attoboy replaces the usual `int main()` entry point with a custom function:

* You **define** `atto_main()` in your program.
* The attoboy runtime arranges for `atto_main()` to be called when the process starts.
* Since `atto_main()` returns `void`, you use `Exit(exitCode)` if you need a specific exit code.

This design allows the library to control the initialization and shutdown process and to avoid CRT dependencies, which helps keep binaries small.

**Example**

```cpp
using namespace attoboy;

void atto_main() {
  EnableLoggingToConsole();
  LogInfo("Application started; user = ", GetUserName());

  // ... your program logic here ...

  Exit(0);
}
```

*This example defines `atto_main()`, enables console logging, logs a startup message, and then exits with code 0.*

---

#### `Exit(int exitCode = 0)`

**Signature**

```cpp
void Exit(int exitCode = 0);
```

**Synopsis**
Terminates the process with the given exit code.

**Parameters**

* `exitCode` – Numeric status code returned to the operating system. `0` usually indicates success; non-zero codes indicate different error conditions.

**Return value**

* *(none; does not return to the caller)*

**In Depth**

`Exit()` immediately terminates the current process:

* Control does not return to the caller.
* The exit code is visible to shell scripts, parent processes, or other tools that launch your program.
* Use `Exit(0)` for normal completion and non-zero values to indicate specific error states.

Because `Exit()` does not return, you should:

* Perform necessary cleanup (closing files, flushing logs) **before** calling it, or ensure that resources are handled by RAII objects that will be cleaned up automatically.

**Example**

```cpp
using namespace attoboy;

if (!Path::GetWorkingDirectory().exists()) {
  LogError("Current working directory does not exist; exiting.");
  Exit(1);
}
```

*This example logs an error and exits with a non-zero code if a basic sanity check fails.*

---

#### `Sleep(int milliseconds)`

**Signature**

```cpp
void Sleep(int milliseconds);
```

**Synopsis**
Pauses the current thread for the given milliseconds.

**Parameters**

* `milliseconds` – Duration to pause the current thread, in milliseconds. Non-negative.

**Return value**

* *(none)*

**In Depth**

`Sleep()` suspends the **current thread** for approximately the specified duration. This is useful for:

* Throttling loops.
* Implementing simple retries.
* Waiting for an external process or resource without busy-waiting.

The actual pause time may be slightly longer than requested due to system scheduling.

**Example**

```cpp
using namespace attoboy;

// Simple retry loop with 100 ms delay between attempts.
int attempts = 0;
while (attempts < 3) {
  String response = WebRequest("https://example.com").doGet(1000).asString();
  if (!response.isEmpty()) {
    break;
  }
  attempts++;
  Sleep(100);  // wait 100 ms before next attempt
}
```

*This example retries a web request up to three times, sleeping between attempts.*

---

#### `String GetEnv(const String &name)`

**Signature**

```cpp
String GetEnv(const String &name);
```

**Synopsis**
Returns the value of an environment variable, or empty string if not set.

**Parameters**

* `name` – Name of the environment variable to read (for example, `"PATH"` or `"HOME"`).

**Return value**

* `String` containing the variable’s value if it exists; otherwise an empty `String`.

**In Depth**

`GetEnv()` provides read access to the process environment:

* If `name` exists, the returned `String` contains its value.
* If `name` does **not** exist, the function returns an empty `String`.

Because an environment variable can legitimately be set to an empty string, your code should distinguish between “not present” and “present but empty” using additional logic if needed (for example, by checking a default or using `SetEnv()` explicitly earlier in your program).

**Example**

```cpp
using namespace attoboy;

String apiKey = GetEnv("MY_APP_API_KEY");
if (apiKey.isEmpty()) {
  LogWarning("MY_APP_API_KEY is not set; using limited mode.");
} else {
  LogInfo("Using configured API key.");
}
```

*This example reads an environment variable and logs a warning if it is not set.*

---

#### `bool SetEnv(const String &name, const String &value)`

**Signature**

```cpp
bool SetEnv(const String &name, const String &value);
```

**Synopsis**
Sets an environment variable. Returns true on success.

**Parameters**

* `name` – Name of the environment variable to set or update.
* `value` – Value to assign to the variable.

**Return value**

* `true` if the environment variable was successfully set; `false` otherwise.

**In Depth**

`SetEnv()` modifies the environment for the **current process** and, typically, any child processes launched after the change.

Use cases include:

* Overriding configuration for subprocesses.
* Providing configuration to tools that expect environment variables.

Limitations and notes:

* Not all names or values are valid on all systems; extremely long names/values or invalid characters can cause `SetEnv()` to fail.
* Changing the environment does not retroactively affect already-running child processes.

**Example**

```cpp
using namespace attoboy;

if (!SetEnv("MY_APP_MODE", "test")) {
  LogError("Failed to set MY_APP_MODE environment variable.");
} else {
  LogInfo("MY_APP_MODE set to 'test'.");
}
```

*This example sets an environment variable and logs whether it succeeded.*

---

#### `void *Alloc(int size)`

**Signature**

```cpp
void *Alloc(int size);
```

**Synopsis**
Allocates memory. Returns nullptr on failure.

**Parameters**

* `size` – Number of bytes to allocate on the heap. Typically non-negative.

**Return value**

* Pointer to a block of memory of at least `size` bytes, or `nullptr` if allocation fails.

**In Depth**

`Alloc()` is attoboy’s low-level heap allocator, analogous to `malloc()` but provided by the library itself. Key points:

* The returned pointer is uninitialized; its contents are unspecified.
* You must eventually release the memory using `Free(ptr)`.
* If `size` is `0`, the behavior depends on the implementation; you should avoid requesting zero-length allocations unless you are prepared to handle a `nullptr` result.

While attoboy encourages the use of its higher-level types (`String`, `List`, `Buffer`, etc.), `Alloc()` is available for specialized cases, such as interfacing with APIs that expect raw pointers or implementing custom data structures.

**Example**

```cpp
using namespace attoboy;

int size = 256;
void *buffer = Alloc(size);
if (buffer == nullptr) {
  LogError("Failed to allocate ", size, " bytes.");
  Exit(1);
}

// ... use buffer ...

Free(buffer);
```

*This example allocates a 256-byte buffer, logs an error and exits on failure, and frees the buffer when done.*

---

#### `void *Realloc(void *ptr, int size)`

**Signature**

```cpp
void *Realloc(void *ptr, int size);
```

**Synopsis**
Reallocates memory. Returns nullptr on failure.

**Parameters**

* `ptr` – Pointer previously returned by `Alloc()` or `Realloc()`, or `nullptr`.
* `size` – New size in bytes for the allocation.

**Return value**

* Pointer to the resized memory block, which may be at a different address, or `nullptr` if reallocation fails.

**In Depth**

`Realloc()` changes the size of an existing allocation:

* If `ptr` is `nullptr`, `Realloc(ptr, size)` behaves like `Alloc(size)`.
* If the operation succeeds:

  * The original memory is freed if it needs to be moved.
  * The returned pointer refers to the resized block.
* If the operation fails:

  * `nullptr` is returned.
  * The original `ptr` remains **valid**; you must still call `Free(ptr)` when appropriate.

You should always assign the result to a **temporary** variable until you know reallocation succeeded, to avoid losing the original pointer:

```cpp
void *newPtr = Realloc(ptr, newSize);
if (newPtr != nullptr) {
  ptr = newPtr;
}
```

**Example**

```cpp
using namespace attoboy;

int size = 128;
void *data = Alloc(size);
// ... after some time, need more space ...
int newSize = 256;
void *resized = Realloc(data, newSize);
if (resized == nullptr) {
  LogError("Failed to grow buffer from ", size, " to ", newSize, " bytes.");
  Free(data);  // still must free the original
  Exit(1);
}
data = resized;
// use data with newSize bytes...

Free(data);
```

*This example safely resizes a buffer and handles reallocation failure.*

---

#### `void Free(void *ptr)`

**Signature**

```cpp
void Free(void *ptr);
```

**Synopsis**
Frees allocated memory (null-safe).

**Parameters**

* `ptr` – Pointer returned by `Alloc()` or `Realloc()`, or `nullptr`.

**Return value**

* *(none)*

**In Depth**

`Free()` releases memory previously allocated by `Alloc()` or `Realloc()`:

* Passing a `nullptr` is explicitly safe and has no effect.
* After you call `Free(ptr)`, the pointer becomes invalid; you should not read from or write to it.
* It is good practice to set your pointer to `nullptr` after freeing to avoid accidental reuse.

**Example**

```cpp
using namespace attoboy;

void *buffer = Alloc(512);
// ... use buffer ...
Free(buffer);
buffer = nullptr;
```

*This example allocates and then safely frees a block of memory.*

---

#### `String GetUserName()`

**Signature**

```cpp
String GetUserName();
```

**Synopsis**
Returns the current user's login name.

**Parameters**

* *(none)*

**Return value**

* `String` containing the login name (for example, the username used to sign in to the operating system).

**In Depth**

`GetUserName()` retrieves the **login identifier** of the user running the process. This is typically the short account name (such as `"jdoe"`). It is useful for:

* Personalizing console output.
* Including user identity in logs or file paths.
* Tagging data with the current user.

If the name cannot be retrieved, the function may return an empty string.

**Example**

```cpp
using namespace attoboy;

String user = GetUserName();
LogInfo("Running as user: ", user);
```

*This example logs the login name of the current user.*

---

#### `String GetUserDisplayName()`

**Signature**

```cpp
String GetUserDisplayName();
```

**Synopsis**
Returns the current user's display name.

**Parameters**

* *(none)*

**Return value**

* `String` containing the user’s display name (for example, `"John Doe"`), or an empty string if unavailable.

**In Depth**

`GetUserDisplayName()` returns the **friendly**, human-readable name for the current user account, if available. This may differ from the login name:

* Login name: `"jdoe"`
* Display name: `"John Doe"`

Use this in user-facing messages, titles, or logs where readability matters.

**Example**

```cpp
using namespace attoboy;

String display = GetUserDisplayName();
if (!display.isEmpty()) {
  LogInfo("Welcome, ", display, "!");
}
```

*This example uses the display name in a friendly greeting.*

---

#### `int GetProcessId()`

**Signature**

```cpp
int GetProcessId();
```

**Synopsis**
Returns the current process ID.

**Parameters**

* *(none)*

**Return value**

* Integer identifier for the current process, as assigned by the operating system.

**In Depth**

The **process ID (PID)** uniquely identifies a running process on the system at a given time. `GetProcessId()` returns this value, which is useful for:

* Tagging log lines when multiple processes are writing to the same destination.
* Debugging and diagnostics (for example, when correlating logs with an external process monitor).

**Example**

```cpp
using namespace attoboy;

int pid = GetProcessId();
LogInfo("Process ID: ", pid);
```

*This example logs the current process’s ID for diagnostic purposes.*

---

#### `void EnableLoggingToFile(const String &path, bool truncate = false)`

**Signature**

```cpp
void EnableLoggingToFile(const String &path, bool truncate = false);
```

**Synopsis**
Enables logging to a file. Mutually exclusive with console logging.

**Parameters**

* `path` – File path where log messages should be written.
* `truncate` – If `true`, the log file is truncated (cleared) before writing. If `false`, messages are appended to the existing file.

**Return value**

* *(none)*

**In Depth**

`EnableLoggingToFile()` configures the **global log destination** to be a file:

* After calling it, all subsequent calls to `Log()`, `LogDebug()`, `LogInfo()`, `LogWarning()`, and `LogError()` write to the specified file.
* Logging to the console is disabled while file logging is active; the two are mutually exclusive.
* If the file cannot be created or opened, the behavior is implementation-specific; typically, you should log a message to the console before switching, or verifyability via test.

Because logging is global, you normally call this **early** in your program, often in `atto_main()`.

**Example**

```cpp
using namespace attoboy;

void atto_main() {
  // Start fresh log file each run.
  EnableLoggingToFile("my_app.log", true);

  LogInfo("Logging to my_app.log; PID = ", GetProcessId());
  // ...
}
```

*This example switches the log destination to a file and logs a startup message.*

---

#### `void EnableLoggingToConsole()`

**Signature**

```cpp
void EnableLoggingToConsole();
```

**Synopsis**
Enables logging to console (default). Mutually exclusive with file logging.

**Parameters**

* *(none)*

**Return value**

* *(none)*

**In Depth**

`EnableLoggingToConsole()` configures the **global log destination** to be the console:

* This is typically the default behavior when the library is initialized.
* Calling `EnableLoggingToConsole()` switches logging away from any previously configured log file and back to the console.

Use this when:

* You want log messages to be visible directly in a terminal.
* You previously switched to file logging and now want to revert.

**Example**

```cpp
using namespace attoboy;

void atto_main() {
  EnableLoggingToConsole();
  LogInfo("Console logging enabled.");

  // Later, if you want to switch to file logging:
  EnableLoggingToFile("later.log", false);
  LogInfo("Now logging to later.log.");
}
```

*This example starts with console logging and then switches to file logging partway through execution.*

---

### Logging Functions

> All logging functions are **variadic templates** that accept any number of arguments. Each argument is converted to `String` internally using the `String` constructors, so you can log values of many types: `String`, `const char *`, `int`, `float`, `bool`, `List`, `Map`, `Set`, etc.

#### `template <typename... Args> void Log(const Args &...args)`

**Signature**

```cpp
template <typename... Args> void Log(const Args &...args);
```

**Synopsis**
Prints arguments to the log destination followed by a newline.

**Parameters**

* `args` – Zero or more values to log. Each value must be constructible as an `attoboy::String`.

**Return value**

* *(none)*

**In Depth**

`Log()` is the **base logging function**:

* It does not add a severity prefix (such as `"INFO"` or `"ERROR"`).
* It simply concatenates all arguments, converts them to `String`, and writes the result as a single line to the current log destination (console or file), followed by a newline.

Behavior with macros:

* `Log()` is **disabled** entirely if `ATTOBOY_LOG_DISABLE` is defined.
* If logging is enabled, calls to `Log()` are always active, regardless of the log level macros like `ATTOBOY_LOG_INFO_ENABLE`.

If you call `Log()` with no arguments, it still writes a newline (possibly with timestamp or other formatting, depending on implementation).

**Example**

```cpp
using namespace attoboy;

Log("Started task ", 1, " for user ", GetUserName());
// Example output: "Started task 1 for user jdoe"
```

*This example logs a simple message composed of multiple argument types.*

---

#### `template <typename... Args> void LogDebug(const Args &...args)`

**Signature**

```cpp
template <typename... Args> void LogDebug(const Args &...args);
```

**Synopsis**
Prints a debug message: "DEBUG YYYY-MM-DDTHH:MM:SS.fffZ: ..."

**Parameters**

* `args` – Zero or more values to include in the debug message.

**Return value**

* *(none)*

**In Depth**

`LogDebug()` is intended for **developer-focused** diagnostic messages:

* Messages typically include a `"DEBUG"` prefix and a timestamp.
* Use it to trace detailed internal state, algorithm decisions, or variable values that are helpful during development but too noisy for production.

Compile-time control:

* `LogDebug()` is only compiled when `ATTOBOY_LOG_DEBUG_ENABLE` is defined (and logging is not disabled).
* If a lower log level macro (such as `ATTOBOY_LOG_INFO_ENABLE`) is used or if `ATTOBOY_LOG_DISABLE` is defined, calls to `LogDebug()` are compiled away and incur no runtime cost.

**Example**

```cpp
using namespace attoboy;

int retryCount = 2;
LogDebug("Connecting to server; retryCount = ", retryCount);
```

*This example logs a debug-level message that would typically be visible only in debug builds.*

---

#### `template <typename... Args> void LogInfo(const Args &...args)`

**Signature**

```cpp
template <typename... Args> void LogInfo(const Args &...args);
```

**Synopsis**
Prints an info message: "INFO YYYY-MM-DDTHH:MM:SS.fffZ: ..."

**Parameters**

* `args` – Zero or more values to include in the informational message.

**Return value**

* *(none)*

**In Depth**

`LogInfo()` is used for **high-level informational messages**:

* Typical use cases: startup messages, configuration summaries, high-level progress updates.
* Messages are prefixed with `"INFO"` and a timestamp.

Compile-time control:

* `LogInfo()` is compiled in when:

  * `ATTOBOY_LOG_DEBUG_ENABLE` **or**
  * `ATTOBOY_LOG_INFO_ENABLE`
    is defined (and logging is not disabled).
* It is compiled out when only `ATTOBOY_LOG_WARNING_ENABLE` or `ATTOBOY_LOG_ERROR_ENABLE` is defined, or when `ATTOBOY_LOG_DISABLE` is defined.

By default, when you do not define any log-level macro and do not disable logging, `ATTOBOY_LOG_INFO_ENABLE` is automatically enabled, so `LogInfo()` calls are active.

**Example**

```cpp
using namespace attoboy;

LogInfo("Service started at ", DateTime().toString(),
        "; PID = ", GetProcessId());
```

*This example logs an informational startup message with a timestamp and process ID.*

---

#### `template <typename... Args> void LogWarning(const Args &...args)`

**Signature**

```cpp
template <typename... Args> void LogWarning(const Args &...args);
```

**Synopsis**
Prints a warning message: "WARNING YYYY-MM-DDTHH:MM:SS.fffZ: ..."

**Parameters**

* `args` – Zero or more values describing the warning.

**Return value**

* *(none)*

**In Depth**

`LogWarning()` is for conditions that are unusual or potentially problematic but not immediately fatal:

* Examples: configuration fallbacks, recoverable errors, degraded performance.
* Messages are prefixed with `"WARNING"` and a timestamp.

Compile-time control:

* `LogWarning()` is compiled in when any of the following are defined:

  * `ATTOBOY_LOG_DEBUG_ENABLE`
  * `ATTOBOY_LOG_INFO_ENABLE`
  * `ATTOBOY_LOG_WARNING_ENABLE`
* It is compiled out only when:

  * `ATTOBOY_LOG_ERROR_ENABLE` is the only level macro, or
  * `ATTOBOY_LOG_DISABLE` is defined.

**Example**

```cpp
using namespace attoboy;

String configPath("config.json");
Path cfg(configPath);

if (!cfg.exists()) {
  LogWarning("Config file ", configPath, " not found; using defaults.");
}
```

*This example logs a warning when a configuration file is missing but the program can continue with defaults.*

---

#### `template <typename... Args> void LogError(const Args &...args)`

**Signature**

```cpp
template <typename... Args> void LogError(const Args &...args);
```

**Synopsis**
Prints an error message: "ERROR YYYY-MM-DDTHH:MM:SS.fffZ: ..."

**Parameters**

* `args` – Zero or more values describing the error.

**Return value**

* *(none)*

**In Depth**

`LogError()` is intended for **errors** and serious issues:

* Use it when an operation fails and may require attention or termination.
* Messages are prefixed with `"ERROR"` and a timestamp.

Compile-time control:

* `LogError()` is active in all log-level configurations **except** when `ATTOBOY_LOG_DISABLE` is defined.
* In other words, unless you fully disable logging, error messages are always available.

Typical pattern:

* Log the error with `LogError()`.
* Optionally follow it with `Exit(exitCode)` if the error is fatal.

**Example**

```cpp
using namespace attoboy;

File f(Path("data.bin"));
if (!f.isValid()) {
  LogError("Failed to open data.bin for reading.");
  Exit(2);
}
```

*This example logs an error when a file cannot be opened and then exits with a non-zero code.*
