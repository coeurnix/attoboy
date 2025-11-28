# `attoboy::File` User Guide

## Overview

The `attoboy::File` class is a unified abstraction for **stream-based input and output** in the attoboy library. A single type represents:

* Regular files on disk.
* TCP client sockets (outgoing connections).
* TCP server sockets (listening for incoming connections).
* Named pipes (where supported by the underlying platform).

This design allows you to work with different I/O channels using a consistent API for reading, writing, and querying state, without directly touching the Windows API or other low-level functions.

### Streams, Files, and Sockets (Conceptual Background)

A **stream** is a sequence of bytes that you can read from or write to in order. The key idea is that you do not need to know where the bytes are stored or how they travel:

* For a **regular file**, bytes are stored on disk.
* For a **TCP socket**, bytes are sent over the network to another machine.
* For a **server socket**, you wait for other machines to connect, and then create separate client sockets to talk to each one.
* For a **named pipe**, bytes flow between processes on the same machine.

The `File` class hides these differences behind a common interface:

* You can call `readAllToBuffer()` or `readAllToString()` on either a file or a socket.
* You can call `write()` and `flush()` on both.
* You can ask what kind of handle it is (`isRegularFile()`, `isSocket()`, etc.) when you need to adjust behavior.

### Validity vs. Open State

Two methods are especially important:

* `isValid()` – Did the object **successfully create or open** an underlying handle?
* `isOpen()` – Is the handle **currently open** (not yet closed or fully shut down)?

Typical usage:

1. Construct a `File` (for a path or socket).
2. Immediately check `isValid()`.
3. If valid, perform reads/writes while `isOpen()` returns `true`.
4. Call `close()` when you are done, or let the destructor close the handle automatically.

### Blocking and Availability

Most `File` operations are **blocking**:

* `readAllToBuffer()`, `readToBuffer()`, `readAllToString()`, `readToString()` will typically block until data is available or the stream ends.
* `write()` and `writeUpTo()` will block until some bytes are written or an error occurs.

To avoid blocking when there is no data, you can use:

* `hasAvailable()` – quick check if there is any data ready to read.
* `getAvailableCount()` – how many bytes can be read without blocking (where supported).

For regular files, “available” usually corresponds to “remaining bytes until end-of-file.” For sockets and pipes, it refers to bytes buffered and ready to be read immediately.

### RAII and Resource Management

`File` uses RAII (Resource Acquisition Is Initialization):

* When you construct a `File`, it acquires an OS resource (file handle, socket, pipe).
* When the `File` object is destroyed, it automatically closes the underlying handle if still open.

You can also call `close()` explicitly when you are done, which is especially important for network sockets or when you need to ensure data is flushed and file descriptors are freed promptly.

---

## Reference

Each entry below covers one public constructor, method, or operator of `attoboy::File`. For each, you will find:

* **Signature** – the exact declaration from the header.
* **Synopsis** – the original one-line Doxygen comment.
* **Parameters** and **Return value** descriptions.
* **In Depth** – additional behavior details, edge cases, and a short example.

> **Note:** All examples assume `using namespace attoboy;`.

---

### Construction, Copy, Assignment, and Destruction

#### `File(const Path &path)`

**Signature**

```cpp
File(const Path &path);
```

**Synopsis**
Opens a file at the given path for reading and writing.

**Parameters**

* `path` – `Path` referring to the file to open.

**Return value**

* *(constructor; not applicable)*

**In Depth**

This constructor attempts to open a regular file at the given path:

* If the operation succeeds, `isValid()` will return `true`.
* If it fails (for example, if the file does not exist or permissions are insufficient), `isValid()` will return `false` and `isOpen()` will also be `false`.

The file is opened for both reading and writing according to the library’s internal policy. You can use `read...` and `write...` methods on the same `File` instance.

Always check `isValid()` after constructing a `File` from a `Path` before performing I/O.

**Example**

```cpp
Path path("config.txt");
File f(path);

if (f.isValid()) {
  String contents = f.readAllToString();
  // Use contents...
  f.close();
}
```

*This example opens a file for reading and writing, checks validity, reads all text, and then closes it.*

---

#### `File(const String &host, int port)`

**Signature**

```cpp
File(const String &host, int port);
```

**Synopsis**
Opens a TCP socket connection to host:port.

**Parameters**

* `host` – Hostname or IP address to connect to (for example, `"example.com"` or `"127.0.0.1"`).
* `port` – TCP port number on the remote host.

**Return value**

* *(constructor; not applicable)*

**In Depth**

This constructor acts as a **TCP client**:

* It attempts to connect to `host:port`.
* On success, `isValid()` and `isOpen()` return `true`, and `isSocket()` returns `true`.
* On failure (DNS errors, unreachable host, refused connection), `isValid()` returns `false`.

Once connected, you can use the same `read...` and `write...` methods as for files. When you are done, call `close()` to terminate the connection.

**Example**

```cpp
File socket("example.com", 80);

if (socket.isValid()) {
  String request = "GET / HTTP/1.0\r\nHost: example.com\r\n\r\n";
  socket.write(request);
  String response = socket.readAllToString();
  socket.close();
}
```

*This example opens a TCP connection, sends a simple HTTP request, reads the response, and closes the socket.*

---

#### `File(int port)`

**Signature**

```cpp
File(int port);
```

**Synopsis**
Creates a listening server socket on the given port.

**Parameters**

* `port` – TCP port number on which to listen for incoming connections.

**Return value**

* *(constructor; not applicable)*

**In Depth**

This constructor creates a **server socket** that listens for new client connections:

* On success, `isValid()` returns `true`, `isServerSocket()` returns `true`, and `isSocket()` also returns `true`.
* On failure (port in use, insufficient privileges), `isValid()` returns `false`.

To accept a client connection, call `accept()`, which returns a new `File` instance representing the client socket.

**Example**

```cpp
File server(8080);

if (server.isValid() && server.isServerSocket()) {
  File client = server.accept();
  if (client.isValid()) {
    String request = client.readAllToString();
    client.write("OK");
    client.close();
  }
}
```

*This example creates a server socket, accepts a single client, reads a request, responds with `"OK"`, and closes the client connection.*

---

#### `File(const File &other)`

**Signature**

```cpp
File(const File &other);
```

**Synopsis**
Creates a copy (shares the underlying handle).

**Parameters**

* `other` – Existing `File` whose underlying handle will be shared.

**Return value**

* *(constructor; not applicable)*

**In Depth**

The copy constructor creates a new `File` object that refers to the **same underlying OS handle** as `other`:

* Operations on either `File` instance affect the same file or socket.
* Closing one instance typically closes the shared handle; the behavior for the other instance becomes implementation-dependent.

Use this when you need multiple `File` objects referencing the same underlying resource, but be careful to coordinate lifetime and closing semantics.

**Example**

```cpp
Path path("log.txt");
File f1(path);

if (f1.isValid()) {
  File f2(f1);           // shares the same handle
  f1.write("First\n");
  f2.write("Second\n");  // writes to the same file
}
```

*This example demonstrates two `File` objects writing through the same underlying handle.*

---

#### `~File()`

**Signature**

```cpp
~File();
```

**Synopsis**
Closes the file/socket and frees resources.

**Parameters**

* *(none)*

**Return value**

* *(destructor; not applicable)*

**In Depth**

When a `File` object is destroyed, it automatically closes the underlying file or socket if it is still open. This ensures that resources are not leaked even if you forget to call `close()` explicitly.

RAII makes cleanup deterministic when you control scopes carefully.

**Example**

```cpp
{
  File f(Path("data.bin"));
  if (f.isValid()) {
    // perform I/O
  }
} // f is destroyed here; handle is closed automatically
```

*This example lets the destructor close the file when the object leaves scope.*

---

#### `File &operator=(const File &other)`

**Signature**

```cpp
File &operator=(const File &other);
```

**Synopsis**
Assigns another file (shares the underlying handle).

**Parameters**

* `other` – `File` whose handle should be assigned to this object.

**Return value**

* Reference to `*this`, enabling assignment chaining.

**In Depth**

The copy assignment operator replaces the current handle (closing it if necessary) and makes this `File` share the underlying handle of `other`. After assignment:

* Both `*this` and `other` refer to the same resource.
* Closing one affects the shared handle for both, similar to the copy constructor.

Always be aware of shared ownership when assigning `File` objects.

**Example**

```cpp
File f1(Path("output.txt"));
File f2(Path("backup.txt"));

if (f1.isValid()) {
  f2 = f1;            // f2 now refers to the same handle as f1
  f2.write("Hello");  // writes through the shared handle
}
```

*This example assigns one `File` to another, causing them to share the same underlying handle.*

---

### Basic Properties and Introspection

#### `const char *getPath() const`

**Signature**

```cpp
const char *getPath() const;
```

**Synopsis**
Returns the file path, or nullptr for sockets.

**Parameters**

* *(none)*

**Return value**

* Pointer to a null-terminated C string containing the path for regular files, or `nullptr` if the `File` represents a socket or other non-file handle.

**In Depth**

Use `getPath()` to query the path associated with a regular file. This is helpful when logging or debugging.

For sockets (client or server) and possibly named pipes, there is no file path, so the function returns `nullptr`.

The returned pointer remains valid while the `File` object exists and is not modified.

**Example**

```cpp
File f(Path("data.txt"));

if (f.isValid() && f.isRegularFile()) {
  const char *path = f.getPath();
  // path points to "data.txt" (or the full resolved path)
}
```

*This example retrieves the file path for a regular file handle.*

---

#### `const char *getHost() const`

**Signature**

```cpp
const char *getHost() const;
```

**Synopsis**
Returns the socket host, or nullptr for files.

**Parameters**

* *(none)*

**Return value**

* Pointer to a null-terminated C string containing the host name for sockets, or `nullptr` for file-based handles.

**In Depth**

For a TCP client socket created with `File(const String &host, int port)`, `getHost()` returns the host you connected to. For regular files and non-socket handles, there is no host, so it returns `nullptr`.

**Example**

```cpp
File socket("example.com", 80);

if (socket.isValid() && socket.isSocket()) {
  const char *host = socket.getHost();  // "example.com"
}
```

*This example queries the remote host name from a TCP client socket.*

---

#### `int getPort() const`

**Signature**

```cpp
int getPort() const;
```

**Synopsis**
Returns the socket port, or -1 for files.

**Parameters**

* *(none)*

**Return value**

* Port number associated with the socket, or `-1` if the handle is not a socket.

**In Depth**

For sockets created with `File(const String &host, int port)` or `File(int port)`:

* `getPort()` returns the port number (remote port for clients, local port for servers, depending on implementation).

For regular files and non-socket handles, it returns `-1`.

**Example**

```cpp
File server(8080);

if (server.isValid()) {
  int port = server.getPort();  // 8080
}
```

*This example reads the listening port from a server socket.*

---

#### `bool isValid() const`

**Signature**

```cpp
bool isValid() const;
```

**Synopsis**
Returns true if the file/socket was opened successfully.

**Parameters**

* *(none)*

**Return value**

* `true` if the underlying handle was created or opened successfully; `false` otherwise.

**In Depth**

`isValid()` is the primary way to check whether construction (or later operations that might replace the handle) succeeded. If it returns `false`:

* All I/O operations will typically fail or behave as no-ops.
* `isOpen()` will also usually be `false`.

Always test `isValid()` immediately after constructing a `File` to avoid attempting I/O on an invalid handle.

**Example**

```cpp
File f(Path("missing.txt"));

if (!f.isValid()) {
  // Handle error: file could not be opened
}
```

*This example checks for failure to open a file and branches accordingly.*

---

#### `bool isOpen() const`

**Signature**

```cpp
bool isOpen() const;
```

**Synopsis**
Returns true if the file/socket is currently open.

**Parameters**

* *(none)*

**Return value**

* `true` if the handle is currently open; `false` if it has been closed or was never opened correctly.

**In Depth**

`isOpen()` reports whether the underlying handle is still active:

* For regular files, it becomes `false` after `close()` or on certain fatal errors.
* For sockets, it becomes `false` after `close()` or when the connection is fully shut down.

A `File` can be invalid (`isValid() == false`) and also not open. For a valid handle, `isOpen()` indicates whether further I/O is possible.

**Example**

```cpp
File f(Path("data.txt"));

if (f.isValid()) {
  // Do some I/O...
  f.close();
  bool stillOpen = f.isOpen();  // false
}
```

*This example shows `isOpen()` becoming `false` after an explicit close.*

---

#### `void close()`

**Signature**

```cpp
void close();
```

**Synopsis**
Closes the file/socket.

**Parameters**

* *(none)*

**Return value**

* *(none)*

**In Depth**

`close()` explicitly releases the underlying resource:

* For files, further reads or writes will fail after closing.
* For sockets, the connection is closed, and buffered data is discarded according to OS behavior.

You may safely call `close()` multiple times; subsequent calls are typically no-ops.

Even if you rely on the destructor to close the handle, calling `close()` explicitly is useful when:

* You want to release file locks quickly.
* You want to signal the end of a network session before leaving scope.

**Example**

```cpp
File f(Path("output.txt"));

if (f.isValid()) {
  f.write("Done.\n");
  f.close();  // ensure data is flushed and handle is released
}
```

*This example writes to a file and then explicitly closes it.*

---

### Reading

#### `Buffer readAllToBuffer()`

**Signature**

```cpp
Buffer readAllToBuffer();
```

**Synopsis**
Reads all available data into a buffer.

**Parameters**

* *(none)*

**Return value**

* `Buffer` containing all bytes that can be read until the end of the stream or until no more data is immediately available.

**In Depth**

This method reads from the current position to the end:

* For regular files, it typically reads until end-of-file from the current position.
* For sockets and pipes, it reads until the remote side closes the connection or until the system indicates there is no more data immediately available.

The returned `Buffer` may be empty if there is nothing to read or if an error occurs.

**Example**

```cpp
File f(Path("image.bin"));

if (f.isValid()) {
  Buffer data = f.readAllToBuffer();
  int size = data.length();
}
```

*This example reads the entire contents of a binary file into a `Buffer`.*

---

#### `Buffer readToBuffer(int count)`

**Signature**

```cpp
Buffer readToBuffer(int count);
```

**Synopsis**
Reads up to count bytes into a buffer.

**Parameters**

* `count` – Maximum number of bytes to read.

**Return value**

* `Buffer` containing up to `count` bytes. May contain fewer bytes if the stream ends earlier or if fewer bytes are currently available.

**In Depth**

This method is useful when you want to control how much data is read in each step, for example when processing a file in chunks:

* It may block until at least one byte is available or the stream ends.
* It stops when either `count` bytes have been read or no more data is immediately available.

If `count <= 0`, the behavior is implementation-defined; pass positive values.

**Example**

```cpp
File f(Path("data.bin"));

if (f.isValid()) {
  Buffer chunk = f.readToBuffer(1024);  // read up to 1024 bytes
}
```

*This example reads a fixed-size chunk of up to 1024 bytes from a file.*

---

#### `String readAllToString()`

**Signature**

```cpp
String readAllToString();
```

**Synopsis**
Reads all available data into a string.

**Parameters**

* *(none)*

**Return value**

* `String` containing all available data decoded as UTF-8 text.

**In Depth**

`readAllToString()` is analogous to `readAllToBuffer()`, but it interprets the data as UTF-8 and returns a `String`. This is ideal for text files, HTTP responses, and other text-based protocols.

If the data is not valid UTF-8, the behavior is implementation-dependent (you may get replacement characters or truncated text).

**Example**

```cpp
File f(Path("notes.txt"));

if (f.isValid()) {
  String text = f.readAllToString();
}
```

*This example reads an entire text file into a `String`.*

---

#### `String readToString(int count)`

**Signature**

```cpp
String readToString(int count);
```

**Synopsis**
Reads up to count bytes into a string.

**Parameters**

* `count` – Maximum number of bytes to read.

**Return value**

* `String` containing up to `count` bytes interpreted as UTF-8 text.

**In Depth**

This method is similar to `readToBuffer(int)` but returns a `String`. It is useful when you want to process text in fixed-size chunks or when dealing with streaming protocols.

Be careful when reading in chunks that do not align with UTF-8 character boundaries: a multi-byte character may be split across reads, which can lead to invalid intermediate strings. For simple ASCII protocols, this is not an issue.

**Example**

```cpp
File f(Path("log.txt"));

if (f.isValid()) {
  String firstPart = f.readToString(1000);  // read up to 1000 bytes as text
}
```

*This example reads a limited amount of text from the file.*

---

#### `bool hasAvailable() const`

**Signature**

```cpp
bool hasAvailable() const;
```

**Synopsis**
Returns true if data is available to read.

**Parameters**

* *(none)*

**Return value**

* `true` if at least one byte can be read without blocking; `false` otherwise.

**In Depth**

`hasAvailable()` is a non-blocking way to check for pending data:

* For sockets and pipes, it can help you avoid blocking reads when no data is ready.
* For regular files, it generally indicates whether there are bytes remaining between the current position and end-of-file.

Use this before calling `read...` when you want to avoid blocking, especially in simple event loops.

**Example**

```cpp
File socket("example.com", 80);

if (socket.isValid()) {
  if (socket.hasAvailable()) {
    String response = socket.readAllToString();
  }
}
```

*This example checks whether data is available on a socket before reading it.*

---

#### `int getAvailableCount() const`

**Signature**

```cpp
int getAvailableCount() const;
```

**Synopsis**
Returns the number of bytes available to read.

**Parameters**

* *(none)*

**Return value**

* Number of bytes that can be read without blocking, or `0` if none are available or if not supported.

**In Depth**

`getAvailableCount()` refines `hasAvailable()` by giving a count of readable bytes:

* For regular files, this is typically `endPosition - currentPosition`, bounded by implementation limits.
* For sockets and pipes, it returns the number of bytes currently buffered and ready.

If the underlying platform does not support querying this information, it may return `0` or another sentinel.

**Example**

```cpp
File f(Path("data.bin"));

if (f.isValid()) {
  int available = f.getAvailableCount();
  Buffer chunk = f.readToBuffer(available);
}
```

*This example queries how many bytes are available and reads exactly that amount.*

---

### Writing

#### `int write(const Buffer &buf)`

**Signature**

```cpp
int write(const Buffer &buf);
```

**Synopsis**
Writes a buffer. Returns bytes written, or -1 on error.

**Parameters**

* `buf` – `Buffer` whose bytes will be written to the stream.

**Return value**

* Number of bytes successfully written, or `-1` if an error occurs.

**In Depth**

`write()` attempts to write the entire contents of `buf`:

* On success, the return value equals `buf.length()`.
* On partial writes (more common with sockets), it may return a smaller number.
* On failure, it returns `-1`.

You should always check the return value, particularly for network sockets, and handle partial writes if necessary.

**Example**

```cpp
Buffer data(String("Hello\n"));
File f(Path("output.txt"));

if (f.isValid()) {
  int written = f.write(data);
  // written is the number of bytes actually written
}
```

*This example writes a buffer to a file and inspects the number of bytes written.*

---

#### `int write(const String &str)`

**Signature**

```cpp
int write(const String &str);
```

**Synopsis**
Writes a string. Returns bytes written, or -1 on error.

**Parameters**

* `str` – UTF-8 `String` whose bytes will be written.

**Return value**

* Number of bytes written (in UTF-8), or `-1` on error.

**In Depth**

This is a convenience overload that converts the string to bytes and delegates to the underlying write logic. Use it whenever you are dealing with text rather than raw binary data.

As with the buffer overload, partial writes are possible; the return value may be less than `str.byteLength()`.

**Example**

```cpp
File f(Path("log.txt"));

if (f.isValid()) {
  f.write("Application started\n");
}
```

*This example writes a line of text directly from a `String` literal.*

---

#### `int writeUpTo(const Buffer &buf, int count = -1)`

**Signature**

```cpp
int writeUpTo(const Buffer &buf, int count = -1);
```

**Synopsis**
Writes up to count bytes from a buffer. Returns bytes written, or -1 on error.

**Parameters**

* `buf` – Buffer whose bytes will be written.
* `count` – Maximum number of bytes to write. If `-1`, the entire buffer is eligible.

**Return value**

* Number of bytes actually written, or `-1` on error.

**In Depth**

`writeUpTo()` is useful when you want to send data in smaller segments:

* If `count` is `-1`, the method treats it as “up to `buf.length()` bytes.”
* If `count` is positive, only the first `count` bytes of `buf` are eligible to be written.
* As always, the return value may be smaller than `count` due to partial writes.

**Example**

```cpp
Buffer data(String("chunked data"));
File socket("example.com", 80);

if (socket.isValid()) {
  int written = socket.writeUpTo(data, 5);  // write first 5 bytes
}
```

*This example writes only a portion of the buffer to a socket.*

---

#### `int writeUpTo(const String &str, int count = -1)`

**Signature**

```cpp
int writeUpTo(const String &str, int count = -1);
```

**Synopsis**
Writes up to count bytes from a string. Returns bytes written, or -1 on error.

**Parameters**

* `str` – `String` whose UTF-8 bytes will be written.
* `count` – Maximum number of bytes to write. If `-1`, the entire string is eligible.

**Return value**

* Number of bytes actually written, or `-1` on error.

**In Depth**

This overload mirrors the buffer version but starts from a `String`. Be cautious with multi-byte UTF-8 characters when limiting by bytes:

* If `count` cuts through a multi-byte character, only the prefix bytes are written; the receiver may see invalid UTF-8.
* For pure ASCII, this is not a concern.

Use the default `-1` when you want to write the entire string.

**Example**

```cpp
String msg("Hello, world\n");
File f(Path("partial.txt"));

if (f.isValid()) {
  int written = f.writeUpTo(msg, 5);  // write "Hello"
}
```

*This example writes only the first 5 bytes of a string to a file.*

---

#### `bool flush()`

**Signature**

```cpp
bool flush();
```

**Synopsis**
Flushes buffered data. Returns true on success.

**Parameters**

* *(none)*

**Return value**

* `true` if flushing succeeds or is not needed; `false` if an error occurs.

**In Depth**

`flush()` ensures that any buffered data is pushed out to the underlying system:

* For regular files, it typically forces data to be written from user-space buffers to the OS.
* For sockets, it may be a no-op if the system writes data immediately.

Calling `flush()` is most important when:

* You want to guarantee that a log message is on disk.
* You are writing protocols where timely delivery matters.

**Example**

```cpp
File f(Path("log.txt"));

if (f.isValid()) {
  f.write("Critical event\n");
  bool ok = f.flush();
}
```

*This example writes a log entry and then flushes it to ensure it is persisted.*

---

### Positioning (Files Only)

#### `bool setPosition(long long pos)`

**Signature**

```cpp
bool setPosition(long long pos);
```

**Synopsis**
Sets the read/write position (files only). Returns true on success.

**Parameters**

* `pos` – New position in bytes from the start of the file.

**Return value**

* `true` if the position was changed successfully; `false` if the operation is not supported or failed.

**In Depth**

`setPosition()` moves the file pointer to a new byte offset:

* Supported for regular files.
* Typically not supported for sockets or pipes; in those cases it returns `false`.

Use this to implement random access:

* Seek to the beginning (`pos = 0`).
* Seek to a specific record offset.

**Example**

```cpp
File f(Path("data.bin"));

if (f.isValid() && f.isRegularFile()) {
  f.setPosition(0);                // rewind to start
  Buffer head = f.readToBuffer(16); // read first 16 bytes
}
```

*This example rewinds a file to the beginning and reads a fixed-size header.*

---

#### `long long getPosition() const`

**Signature**

```cpp
long long getPosition() const;
```

**Synopsis**
Returns the current position, or -1 if not supported.

**Parameters**

* *(none)*

**Return value**

* Current byte offset from the start of the file, or `-1` if the operation is not supported.

**In Depth**

For regular files, `getPosition()` returns the current position of the file pointer. For sockets and pipes, there is no meaningful file offset, so it usually returns `-1`.

This is useful for progress reporting or for saving/restoring positions in files.

**Example**

```cpp
File f(Path("data.bin"));

if (f.isValid()) {
  long long pos = f.getPosition();
}
```

*This example queries the current read/write position in a file.*

---

#### `long long getEndPosition() const`

**Signature**

```cpp
long long getEndPosition() const;
```

**Synopsis**
Returns the file size, or -1 if not supported.

**Parameters**

* *(none)*

**Return value**

* File size in bytes for regular files, or `-1` if not supported (e.g., for sockets).

**In Depth**

For regular files, `getEndPosition()` typically reports the total size of the file in bytes. Combined with `getPosition()`, you can compute progress:

```cpp
progress = (double)getPosition() / getEndPosition();
```

For non-seekable streams, the function usually returns `-1`.

**Example**

```cpp
File f(Path("data.bin"));

if (f.isValid() && f.isRegularFile()) {
  long long size = f.getEndPosition();
}
```

*This example obtains the total size of a regular file.*

---

#### `bool isAtEnd() const`

**Signature**

```cpp
bool isAtEnd() const;
```

**Synopsis**
Returns true if at end of file.

**Parameters**

* *(none)*

**Return value**

* `true` if the current position is at or beyond the end of the file; `false` otherwise.

**In Depth**

`isAtEnd()` is primarily meaningful for regular files:

* After reading to the end, it becomes `true`.
* For sockets and pipes, the semantics may be similar when the remote side closes the connection, but exact behavior is implementation-dependent.

Use this to detect when you have consumed all file content without having to compare positions manually.

**Example**

```cpp
File f(Path("data.txt"));

if (f.isValid()) {
  String all = f.readAllToString();
  bool done = f.isAtEnd();  // typically true after reading everything
}
```

*This example checks whether the file position is at the end after reading all contents.*

---

### Type Inspection

#### `bool isRegularFile() const`

**Signature**

```cpp
bool isRegularFile() const;
```

**Synopsis**
Returns true if this is a regular file.

**Parameters**

* *(none)*

**Return value**

* `true` if the `File` represents a disk file; `false` otherwise.

**In Depth**

Use `isRegularFile()` to distinguish files from sockets and pipes. This is important when you want to:

* Use `setPosition()`, `getPosition()`, or `getEndPosition()`, which are meaningful only for regular files.
* Apply file-specific logic like seeking or computing file size.

**Example**

```cpp
File f(Path("data.bin"));

if (f.isRegularFile()) {
  long long size = f.getEndPosition();
}
```

*This example confirms that a handle is a regular file before calling file-specific APIs.*

---

#### `bool isSocket() const`

**Signature**

```cpp
bool isSocket() const;
```

**Synopsis**
Returns true if this is a TCP socket.

**Parameters**

* *(none)*

**Return value**

* `true` if the `File` represents a TCP socket (client or server); `false` otherwise.

**In Depth**

`isSocket()` is a quick way to detect network handles. Both client sockets (`File(const String&, int)`) and server sockets (`File(int port)`) should return `true`.

Combining this with `isServerSocket()` lets you differentiate between listening sockets and connected ones.

**Example**

```cpp
File socket("example.com", 80);

if (socket.isSocket()) {
  // Perform network-specific logic
}
```

*This example checks whether the handle is a socket before performing network operations.*

---

#### `bool isServerSocket() const`

**Signature**

```cpp
bool isServerSocket() const;
```

**Synopsis**
Returns true if this is a server socket.

**Parameters**

* *(none)*

**Return value**

* `true` if the handle is a listening server socket; `false` otherwise.

**In Depth**

Server sockets are those constructed with `File(int port)`. They:

* Listen for incoming connections.
* Are used with `accept()` to create client sockets.

Client sockets created via `File(const String&, int)` should return `false` for `isServerSocket()`.

**Example**

```cpp
File listener(9000);

if (listener.isValid() && listener.isServerSocket()) {
  File client = listener.accept();
}
```

*This example confirms that a `File` is a server socket before calling `accept()`.*

---

#### `bool isNamedPipe() const`

**Signature**

```cpp
bool isNamedPipe() const;
```

**Synopsis**
Returns true if this is a named pipe.

**Parameters**

* *(none)*

**Return value**

* `true` if the underlying handle is a named pipe; `false` otherwise.

**In Depth**

Named pipes provide inter-process communication on the same machine. Whether and how they are used depends on how the `File` was created by other parts of the attoboy library or system integration.

`File` exposes this method so you can adapt your logic when handling such handles.

**Example**

```cpp
File f(Path("\\\\.\\pipe\\my_pipe"));

if (f.isNamedPipe()) {
  // Handle pipe-specific behavior
}
```

*This example checks whether a handle represents a named pipe.*

---

### Server-Side Connections

#### `File accept()`

**Signature**

```cpp
File accept();
```

**Synopsis**
Accepts a client connection on a server socket.

**Parameters**

* *(none)*

**Return value**

* `File` representing the accepted client connection. If acceptance fails, the returned `File` will be invalid (`isValid() == false`).

**In Depth**

`accept()` is only meaningful when called on a server socket (`isServerSocket() == true`):

* It blocks until a client connects or an error occurs.
* On success, it returns a new `File` representing the client connection. This new `File` is a regular socket (not a server socket).
* The original server `File` remains open and can accept more clients if you call `accept()` again.

If you call `accept()` on a non-server handle, behavior is undefined; always check `isServerSocket()` first.

**Example**

```cpp
File server(8080);

if (server.isValid() && server.isServerSocket()) {
  File client = server.accept();
  if (client.isValid()) {
    String msg = client.readAllToString();
    client.write("OK");
    client.close();
  }
}
```

*This example accepts a single connection on a server socket, exchanges a simple text message, and closes the client connection.*

---

### Equality and Comparison

#### `bool equals(const File &other) const`

**Signature**

```cpp
bool equals(const File &other) const;
```

**Synopsis**
Returns true if this file equals the other.

**Parameters**

* `other` – Another `File` to compare with.

**Return value**

* `true` if both `File` objects refer to the same underlying handle; `false` otherwise.

**In Depth**

`equals()` compares the identity of the underlying OS handle rather than the contents of the file or socket. Two `File` instances are considered equal if they share the same internal handle (for example, created via copy construction or assignment).

It is not a comparison of paths or remote endpoints; it is a comparison of the handle itself.

**Example**

```cpp
File f1(Path("data.txt"));
File f2(f1);  // shares handle with f1

bool same = f1.equals(f2);  // true
```

*This example shows two `File` objects that are equal because they share the same handle.*

---

#### `bool operator==(const File &other) const`

**Signature**

```cpp
bool operator==(const File &other) const;
```

**Synopsis**
Returns true if this file equals the other.

**Parameters**

* `other` – Another `File` to compare with.

**Return value**

* `true` if both `File` objects refer to the same underlying handle; `false` otherwise.

**In Depth**

`operator==` is a shorthand for `equals(other)`. It enables idiomatic comparisons:

```cpp
if (a == b) { ... }
```

As with `equals()`, the comparison is about handle identity, not file contents.

**Example**

```cpp
File a(Path("data.txt"));
File b(a);

if (a == b) {
  // a and b share the same handle
}
```

*This example uses the equality operator to check if two `File` instances refer to the same underlying handle.*

---

#### `bool operator!=(const File &other) const`

**Signature**

```cpp
bool operator!=(const File &other) const;
```

**Synopsis**
Returns true if this file does not equal the other.

**Parameters**

* `other` – Another `File` to compare with.

**Return value**

* `true` if the two `File` instances do not refer to the same handle; `false` if they do.

**In Depth**

This operator is simply the logical negation of `operator==`. It is convenient when you want to ensure two handles are distinct.

**Example**

```cpp
File f1(Path("a.txt"));
File f2(Path("b.txt"));

if (f1 != f2) {
  // f1 and f2 refer to different handles (likely different files)
}
```

*This example checks that two `File` objects do not refer to the same underlying resource.*
