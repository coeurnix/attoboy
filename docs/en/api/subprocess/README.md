# `attoboy::Subprocess` User Guide

## Overview

The `attoboy::Subprocess` class provides a high-level, RAII-based interface for starting and interacting with external programs (child processes) from your attoboy application. It supports:

* **Simple one-shot helpers**:

  * `Start()` – fire-and-forget.
  * `Run()` – run and wait for an exit code.
  * `Capture()` – run and collect combined `stdout + stderr` as a `String`.
* **Streaming I/O**:

  * Construct a `Subprocess` object and use `write()` to send data to the process’s standard input.
  * Use `readToString()` or `readToBuffer()` to read incremental output.
  * Query `isRunning()` and `hasAvailable()` to drive interactive or long-running jobs.

`Subprocess` integrates with other attoboy types:

* `Path` – specifies the executable to run.
* `String` – for arguments, input, and output text.
* `Buffer` – for binary input and output.

### Processes, Standard I/O, and Exit Codes

A **process** is an instance of a program running in the operating system. When you launch a subprocess:

* The child process typically has:

  * **Standard input (stdin)** – data your program can send to it.
  * **Standard output (stdout)** – data it sends back.
  * **Standard error (stderr)** – error and diagnostic output.
* The process eventually **exits** with an **exit code** (also known as an exit status or return code):

  * `0` usually means success.
  * Non-zero often indicates an error or special condition.

`Subprocess` abstracts these details into simple methods:

* Exit code via `Run()` or `wait()`.
* Combined output via `Capture()` or the streaming `read*` methods.
* Process identity via `getProcessId()`.

### Fire-and-Forget vs. Streaming Subprocesses

There are two primary usage patterns:

1. **Fire-and-forget / simple calls**
   Use the static helpers when you only need a result and do not care about streaming I/O:

   * `Subprocess::Start(exe, args...)` – start and do not wait.
   * `Subprocess::Run(exe, args...)` – start and wait; get exit code.
   * `Subprocess::Capture(exe, args...)` – start, wait, and get full output.

2. **Streaming I/O (interactive)**
   Use the constructor and instance methods when you need to:

   * Read output incrementally.
   * Send commands to the process over time.
   * Monitor and control long-running tasks.

   In this mode, the `Subprocess` object manages a handle to the underlying process. The handle is released in the destructor, but **the process itself is not automatically terminated**; use `wait()` or `terminate()` to control its lifetime.

---

## Reference

Each entry below documents one public constructor, static function, or member of `attoboy::Subprocess`.

---

### Static Helpers

#### `template <typename... Args> static void Start(const Path &executable, const Args &...args)`

**Signature**

```cpp
template <typename... Args>
static void Start(const Path &executable, const Args &...args) {
  Start_impl(executable, List(String(args)...));
}
```

**Synopsis**
Starts a process without waiting (fire and forget).

**Parameters**

* `executable` – `Path` to the program to run.
* `args` – Zero or more command-line arguments. Each argument must be convertible to `String` (for example, `const char *`, `String`, `int`, `bool`).

**Return value**

* *(none; static function with `void` return type)*

**In Depth**

`Start()` launches a process and returns immediately without giving you a handle to it. This is useful when:

* You want to trigger a helper tool or background task.
* You do not need to read its output or wait for completion.
* The process’s exit code and I/O are not important to your logic.

Internally, the `args` are converted to `String` values and collected into a `List`, which is passed to a private implementation function. The called process sees them as traditional command-line arguments.

Because there is no returned `Subprocess` object, you cannot later query `isRunning()` or terminate the process. Use `Run()` or the constructor-based API if you need that control.

**Example**

```cpp
Path exe("helper_tool.exe");

// Start the tool with two arguments and do not wait for it.
Subprocess::Start(exe, "--cleanup", "temp");
```

*This example launches a background helper program to perform cleanup and immediately continues execution without waiting for it to finish.*

---

#### `template <typename... Args> static int Run(const Path &executable, const Args &...args)`

**Signature**

```cpp
template <typename... Args>
static int Run(const Path &executable, const Args &...args) {
  return Run_impl(executable, List(String(args)...));
}
```

**Synopsis**
Runs a process and waits for completion. Returns exit code.

**Parameters**

* `executable` – `Path` to the program to run.
* `args` – Zero or more command-line arguments, each convertible to `String`.

**Return value**

* Exit code returned by the subprocess, or an implementation-defined value (commonly `-1`) on error.

**In Depth**

`Run()` is the simplest way to execute an external program when you only care about its **exit code** and not its output. The function:

1. Starts the process with the given `executable` and arguments.
2. Waits synchronously until the process exits.
3. Returns the exit code.

Because the call blocks, use it for short-lived tasks or when you are comfortable waiting. For long-running jobs where you want progress updates or to keep a responsive UI, prefer a streaming `Subprocess` instance.

**Example**

```cpp
Path exe("validator.exe");

// Run the validator with a file path and check the exit code.
int exitCode = Subprocess::Run(exe, "config.json");

if (exitCode == 0) {
  // Validation succeeded.
} else {
  // Handle validation error.
}
```

*This example runs a validator tool synchronously and branches based on its exit code.*

---

#### `template <typename... Args> static String Capture(const Path &executable, const Args &...args)`

**Signature**

```cpp
template <typename... Args>
static String Capture(const Path &executable, const Args &...args) {
  return Capture_impl(executable, List(String(args)...));
}
```

**Synopsis**
Runs a process and captures stdout+stderr. Returns output as string.

**Parameters**

* `executable` – `Path` to the program to run.
* `args` – Zero or more command-line arguments, each convertible to `String`.

**Return value**

* A `String` containing the combined standard output and standard error of the subprocess.
* Typically empty on failure or when the process produced no output.

**In Depth**

`Capture()` is a convenience for “run and collect output” workflows:

* It starts the process.
* Captures everything written to `stdout` and `stderr`.
* Waits for the process to exit.
* Returns the captured text as a single `String`.

This is ideal for tools that:

* Print human-readable messages.
* Generate machine-readable output (JSON, CSV) that you want to parse.

If you also need the exit code, you may have to design your tool to report status via its output or run it twice using both `Run()` and `Capture()` (if appropriate). When you need fine-grained control over streaming, use the constructor-based API instead.

**Example**

```cpp
Path exe("tool.exe");

// Run the tool and capture all text output.
String output = Subprocess::Capture(exe, "--list", "--json");

// You can now parse or log 'output' as needed.
```

*This example runs a tool that lists information in JSON format and collects its output into a `String`.*

---

### Constructors, Copy, Assignment, and Destruction

#### `template <typename... Args> Subprocess(const Path &executable, const Args &...args)`

**Signature**

```cpp
template <typename... Args>
Subprocess(const Path &executable, const Args &...args)
    : Subprocess(executable, List(String(args)...)) {}
```

**Synopsis**
Creates a subprocess with streaming I/O support.

**Parameters**

* `executable` – `Path` to the program to run.
* `args` – Zero or more command-line arguments, each convertible to `String`.

**Return value**

* *(constructor; not applicable)*

**In Depth**

This constructor launches a new process and creates a `Subprocess` object that manages a handle to it. Unlike `Start()`, you retain control over:

* Whether the process is valid (`isValid()`).
* Whether it is still running (`isRunning()`).
* When to wait for completion (`wait()`).
* How to read and write its I/O streams (`readToString()`, `readToBuffer()`, `write()`).
* When (and whether) to terminate it (`terminate()`).

Internally, the arguments are converted to `String` values and passed in a `List` to a private constructor. The created `Subprocess` object can be copied and assigned; copies share the same underlying process handle.

After constructing a `Subprocess`, you should typically check `isValid()` before using it.

**Example**

```cpp
Path exe("interactive_tool.exe");

// Start an interactive subprocess with one argument.
Subprocess proc(exe, "--interactive");

if (proc.isValid()) {
  // Ready to communicate using write() and readToString().
}
```

*This example launches an interactive tool and checks that it was created successfully before proceeding.*

---

#### `Subprocess(const Subprocess &other)`

**Signature**

```cpp
Subprocess(const Subprocess &other);
```

**Synopsis**
Creates a copy (shares the underlying process).

**Parameters**

* `other` – Existing `Subprocess` whose handle should be shared.

**Return value**

* *(constructor; not applicable)*

**In Depth**

The copy constructor does **not** start a new OS process. Instead, it creates another `Subprocess` object that refers to the same underlying subprocess:

* Both instances see the same `isRunning()` state.
* Either can call `wait()`, `terminate()`, `write()`, or `read*()`.
* Ownership is shared; the handle is cleaned up only when all references are destroyed.

This makes it easy to pass `Subprocess` objects between functions without worrying about double-launching a process.

**Example**

```cpp
Path exe("server.exe");
Subprocess server(exe, "--mode", "child");

Subprocess shared(server);  // shares the same underlying process

// Both 'server' and 'shared' can interact with the same subprocess.
```

*This example copies a `Subprocess` handle so multiple parts of the code can interact with the same child process.*

---

#### `~Subprocess()`

**Signature**

```cpp
~Subprocess();
```

**Synopsis**
Destroys the handle (does not terminate the process).

**Parameters**

* *(none)*

**Return value**

* *(destructor; not applicable)*

**In Depth**

The destructor releases the underlying process handle owned by this `Subprocess` object. Crucially:

* It **does not** terminate the subprocess.
* If there are other `Subprocess` copies referring to the same handle, the process continues to be managed by them.
* The OS process will eventually exit on its own, unless you explicitly call `terminate()`.

You should call `wait()` or `terminate()` if you need to ensure a process has finished before `Subprocess` objects go out of scope.

**Example**

```cpp
{
  Path exe("worker.exe");
  Subprocess worker(exe);

  // Optionally wait or terminate here.
  // worker.wait();
} // 'worker' handle is released; the OS process may still be running.
```

*This example shows a `Subprocess` going out of scope; only the handle is destroyed, not the subprocess itself.*

---

#### `Subprocess &operator=(const Subprocess &other)`

**Signature**

```cpp
Subprocess &operator=(const Subprocess &other);
```

**Synopsis**
Assigns another subprocess (shares the underlying process).

**Parameters**

* `other` – `Subprocess` to assign from.

**Return value**

* Reference to `*this`, enabling assignment chaining.

**In Depth**

The assignment operator makes the left-hand side refer to the same underlying process as `other`. The previous handle (if any) is released:

* No new process is created.
* Both `*this` and `other` now share the same subprocess handle.
* Self-assignment is safe.

Use this when you want to update a `Subprocess` variable to refer to a new or different process.

**Example**

```cpp
Path exe("task.exe");

Subprocess a(exe, "one");
Subprocess b(exe, "two");

// Now 'a' and 'b' initially refer to different processes.
a = b;  // 'a' now refers to the same process as 'b'
```

*This example reassigns one `Subprocess` to share the handle of another.*

---

### Process State and Lifecycle

#### `bool isValid() const`

**Signature**

```cpp
bool isValid() const;
```

**Synopsis**
Returns true if the process was created successfully.

**Parameters**

* *(none)*

**Return value**

* `true` if the underlying process handle is valid; `false` if creation failed or the handle is invalid.

**In Depth**

`isValid()` checks whether the `Subprocess` object successfully created (or acquired) a subprocess handle. Common reasons for failure include:

* The executable path does not exist.
* Permission errors.
* OS resource limitations.

You should typically call `isValid()` after constructing a `Subprocess` or assigning one, before attempting I/O or lifecycle operations.

**Example**

```cpp
Path exe("maybe_missing.exe");
Subprocess proc(exe);

if (!proc.isValid()) {
  // Handle error: executable may not exist or could not be launched.
}
```

*This example verifies that the subprocess was successfully created before proceeding.*

---

#### `bool isRunning() const`

**Signature**

```cpp
bool isRunning() const;
```

**Synopsis**
Returns true if the process is still running.

**Parameters**

* *(none)*

**Return value**

* `true` if the subprocess has not yet exited; `false` if it has already finished or was never valid.

**In Depth**

`isRunning()` lets you observe the current state of the subprocess without blocking:

* For a valid, active process, returns `true`.
* After the process exits (whether normally or via `terminate()`), returns `false`.

Combine this with `hasAvailable()` and the I/O methods to drive a responsive loop that reads output and detects completion.

**Example**

```cpp
Path exe("long_task.exe");
Subprocess proc(exe);

while (proc.isRunning()) {
  // Poll for output, update UI, etc.
}
```

*This example periodically checks whether a long-running subprocess is still active.*

---

#### `int wait()`

**Signature**

```cpp
int wait();
```

**Synopsis**
Waits for completion and returns exit code (-1 on error).

**Parameters**

* *(none)*

**Return value**

* Exit code of the subprocess on successful completion.
* `-1` (or another implementation-defined error code) if waiting fails or the process is invalid.

**In Depth**

`wait()` blocks until the subprocess exits. Once `wait()` returns:

* `isRunning()` will return `false`.
* You can still call `readToString()` or `readToBuffer()` to consume any remaining buffered output, if supported.
* Calling `wait()` multiple times is usually either a no-op after the first call or returns the cached exit code; behavior is implementation-defined, so it is best to call it once.

Use `wait()` when you have a streaming `Subprocess` and you want to:

* Send one or more commands via `write()`.
* Read ongoing output.
* Eventually block until completion and obtain the exit code.

**Example**

```cpp
Path exe("job.exe");
Subprocess proc(exe, "--mode", "batch");

if (proc.isValid()) {
  int exitCode = proc.wait();
  // Examine exitCode to determine success or failure.
}
```

*This example starts a batch job and blocks until it finishes, then inspects the exit code.*

---

#### `bool terminate()`

**Signature**

```cpp
bool terminate();
```

**Synopsis**
Forcefully terminates the process. Returns true on success.

**Parameters**

* *(none)*

**Return value**

* `true` if the termination request succeeded; `false` if it failed or the process was invalid.

**In Depth**

`terminate()` attempts to forcibly stop the subprocess. This is a strong operation:

* The child process may not have a chance to clean up (flush files, save state, etc.).
* Use it only when the subprocess is misbehaving (hung, stuck, or ignoring polite signals).

After a successful `terminate()` call:

* `isRunning()` will eventually return `false`.
* `wait()` can be used if you need to ensure the process has fully exited.

**Example**

```cpp
Path exe("worker.exe");
Subprocess proc(exe);

if (proc.isValid()) {
  // ... some time passes, and we decide to stop the worker ...
  bool ok = proc.terminate();
  // Optionally call proc.wait() to ensure it has fully exited.
}
```

*This example forcefully terminates a worker subprocess when it is no longer needed or is misbehaving.*

---

#### `int getProcessId() const`

**Signature**

```cpp
int getProcessId() const;
```

**Synopsis**
Returns the process ID (-1 on error).

**Parameters**

* *(none)*

**Return value**

* OS process identifier (PID) of the subprocess.
* `-1` if the process is invalid or the ID cannot be retrieved.

**In Depth**

The PID uniquely identifies the subprocess at the operating-system level. You can use it for:

* Logging or debugging.
* External tools that monitor processes by ID.

Note that PIDs can be reused by the OS once a process exits, so they are only meaningful while the process is alive.

**Example**

```cpp
Path exe("worker.exe");
Subprocess proc(exe);

if (proc.isValid()) {
  int pid = proc.getProcessId();
  // You can log 'pid' or use it in external monitoring tools.
}
```

*This example retrieves and records the process ID of a worker subprocess.*

---

### Streaming I/O

#### `bool hasAvailable() const`

**Signature**

```cpp
bool hasAvailable() const;
```

**Synopsis**
Returns true if output data is available to read.

**Parameters**

* *(none)*

**Return value**

* `true` if there is output ready to be read; `false` otherwise.

**In Depth**

`hasAvailable()` checks whether any data is currently available from the subprocess’s output streams. It is designed to be used before calling:

* `readToBuffer()`
* `readToString()`

A typical pattern is to avoid blocking by checking `hasAvailable()` in a loop, reading only when it returns `true`. This allows you to remain responsive while still reacting quickly to output.

**Example**

```cpp
Path exe("chatty.exe");
Subprocess proc(exe);

while (proc.isRunning() || proc.hasAvailable()) {
  if (proc.hasAvailable()) {
    String chunk = proc.readToString();
    // Process 'chunk' (log it, parse it, etc.).
  }
  // Perform other work here if needed.
}
```

*This example polls for available output while the subprocess is running, reading text chunks as they become available.*

---

#### `Buffer readToBuffer()`

**Signature**

```cpp
Buffer readToBuffer();
```

**Synopsis**
Reads available output into a buffer.

**Parameters**

* *(none)*

**Return value**

* A `Buffer` containing the bytes read from the subprocess’s output. May be empty if no data is available or on error.

**In Depth**

`readToBuffer()` reads output from the subprocess and returns it as a `Buffer`, suitable for binary data:

* Use this when your subprocess emits non-text output (e.g., protocol frames, compressed data).
* Combine with `hasAvailable()` to avoid blocking until data is present.

If you are working with text output, `readToString()` is usually more convenient.

**Example**

```cpp
Path exe("binary_emitter.exe");
Subprocess proc(exe);

if (proc.isValid()) {
  if (proc.hasAvailable()) {
    Buffer data = proc.readToBuffer();
    // Handle raw binary data in 'data'.
  }
}
```

*This example reads binary output from a subprocess into a `Buffer`.*

---

#### `String readToString()`

**Signature**

```cpp
String readToString();
```

**Synopsis**
Reads available output into a string.

**Parameters**

* *(none)*

**Return value**

* A `String` containing text read from the subprocess’s output. May be empty if no data is available or on error.

**In Depth**

`readToString()` is the text-oriented counterpart to `readToBuffer()`:

* It reads available output and decodes it as UTF-8 into a `String`.
* You can call it multiple times as more output becomes available.
* Combine with `hasAvailable()` and `isRunning()` in a loop to build up the full output in chunks.

If you need the entire output in one step and do not care about streaming, `Capture()` is often simpler.

**Example**

```cpp
Path exe("script.exe");
Subprocess proc(exe);

String allOutput;

while (proc.isRunning() || proc.hasAvailable()) {
  if (proc.hasAvailable()) {
    String chunk = proc.readToString();
    allOutput = allOutput + chunk;
  }
}

 // 'allOutput' now contains all text produced by the script.
```

*This example incrementally reads text output from a script until it finishes, accumulating it into one `String`.*

---

#### `int write(const Buffer &buf)`

**Signature**

```cpp
int write(const Buffer &buf);
```

**Synopsis**
Writes a buffer to stdin. Returns bytes written, or -1 on error.

**Parameters**

* `buf` – `Buffer` containing bytes to send to the subprocess’s standard input.

**Return value**

* Number of bytes successfully written.
* `-1` on error (invalid process, closed pipe, or other I/O error).

**In Depth**

`write(const Buffer &)` sends binary data to the subprocess’s standard input:

* Use this when the child process expects binary protocols or non-text commands.
* Be aware that not all programs read from standard input; data may be ignored if the child is not designed to consume it.
* After the subprocess has exited or closed its input, further `write()` calls will typically fail and return `-1`.

You may need to design your child program to interpret these bytes appropriately.

**Example**

```cpp
Path exe("binary_consumer.exe");
Subprocess proc(exe);

if (proc.isValid()) {
  Buffer payload;
  // ... fill 'payload' with data ...

  int written = proc.write(payload);
  // Check 'written' for success or partial writes.
}
```

*This example sends binary data to a subprocess through its standard input.*

---

#### `int write(const String &str)`

**Signature**

```cpp
int write(const String &str);
```

**Synopsis**
Writes a string to stdin. Returns bytes written, or -1 on error.

**Parameters**

* `str` – `String` containing UTF-8 text to send to the subprocess’s standard input.

**Return value**

* Number of bytes successfully written.
* `-1` on error.

**In Depth**

`write(const String &)` is the most convenient way to send textual commands or input to an interactive subprocess:

* The text is written as UTF-8 bytes.
* Many command-line tools interpret newline (`"\n"`) as “end of command”, so it is common to include line breaks when sending commands.
* After the process exits or closes its input stream, further writes will fail.

Use this method along with `readToString()` and `isRunning()` to implement request–response patterns.

**Example**

```cpp
Path exe("interactive_tool.exe");
Subprocess proc(exe, "--interactive");

if (proc.isValid()) {
  // Send a command, followed by a newline so the tool can parse it.
  proc.write(String("status\n"));

  // Later, read the response.
  if (proc.hasAvailable()) {
    String response = proc.readToString();
    // Process 'response' as needed.
  }
}
```

*This example sends a command line to an interactive tool and reads its response.*
