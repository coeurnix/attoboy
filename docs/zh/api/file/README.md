# File

## 概述

`attoboy::File` 类是在 attoboy 库中对**基于流的输入与输出**的统一抽象。一个类型即可表示：

* 磁盘上的普通文件。
* TCP 客户端套接字（主动发起连接）。
* TCP 服务器套接字（监听传入连接）。
* 命名管道（取决于底层平台是否支持）。

这种设计使你能够使用一致的 API 来对不同的 I/O 通道进行读取、写入和状态查询，而无需直接接触 Windows API 或其他底层函数。

### 流、文件与套接字（概念背景）

**流（stream）**是一个按顺序读写的字节序列。关键是你无需关心这些字节存储在何处或如何传输：

* 对于**普通文件**，字节存储在磁盘上。
* 对于 **TCP 套接字**，字节通过网络发送到另一台机器。
* 对于 **服务器套接字**，你等待其他机器连接进来，然后为每个连接创建单独的客户端套接字以与之通信。
* 对于 **命名管道**，字节在同一台机器上的进程间流动。

`File` 类把这些差异隐藏在一个通用接口之后：

* 你可以对文件或套接字调用 `readAllToBuffer()` 或 `readAllToString()`。
* 你可以在两者上调用 `write()` 和 `flush()`。
* 当需要调整行为时，你可以询问它是什么类型的句柄（`isRegularFile()`, `isSocket()` 等）。

### 有效性 vs. 打开状态

两个方法尤为重要：

* `isValid()` – 该对象是否**成功创建或打开**了底层句柄？
* `isOpen()` – 该句柄当前是否**处于打开状态**（尚未关闭或完全关闭）？

典型用法：

1. 构造一个 `File`（针对路径或套接字）。
2. 立即检查 `isValid()`。
3. 如果有效，则在 `isOpen()` 返回 `true` 的情况下进行读/写操作。
4. 完成后调用 `close()`，或让析构函数自动关闭句柄。

### 阻塞与可用数据

大多数 `File` 操作是**阻塞式**的：

* `readAllToBuffer()`, `readToBuffer()`, `readAllToString()`, `readToString()` 通常会阻塞，直到有数据可读或流结束。
* `write()` 和 `writeUpTo()` 会阻塞，直到写入了一些字节或发生错误。

为了在没有数据时避免阻塞，你可以使用：

* `hasAvailable()` – 快速检查是否有任意可读数据。
* `getAvailableCount()` – 当前可以在不阻塞的情况下读取多少字节（在支持的平台上）。

对于普通文件，“available” 通常对应于“距离文件结束剩余的字节数”。对于套接字和管道，则指已经缓冲好、可立即读取的字节数。

### RAII 与资源管理

`File` 使用 RAII（Resource Acquisition Is Initialization，资源获取即初始化）：

* 构造 `File` 时，它会获取一个 OS 资源（文件句柄、套接字、管道）。
* 当 `File` 对象被销毁时，如果底层句柄仍然打开，则会自动关闭它。

你也可以在使用完后显式调用 `close()`，这对网络套接字尤为重要，或者在你需要确保数据被刷新且文件描述符尽快释放时也很有用。

---

## 参考

下面每一条目都介绍了 `attoboy::File` 的一个 public 构造函数、方法或运算符。每个条目包括：

* **Signature** – 头文件中的精确声明。
* **Synopsis** – 原始的一行 Doxygen 注释。
* **Parameters** 和 **Return value** 描述。
* **In Depth** – 额外的行为细节、边界情况，以及简短示例。

> **注意：**所有示例都假定已使用 `using namespace attoboy;`。

---

#### `File(const Path &path)`

**Signature**

```cpp
File(const Path &path);
```

**Synopsis**  
Opens a file at the given path for reading and writing.

**Parameters**

* `path` – 指向要打开文件的 `Path`。

**Return value**

* *(构造函数；不适用)*

**In Depth**

该构造函数尝试在给定路径上打开一个普通文件：

* 如果操作成功，`isValid()` 将返回 `true`。
* 如果失败（例如文件不存在或权限不足），`isValid()` 将返回 `false`，`isOpen()` 也将为 `false`。

文件会按照库的内部策略以可读写模式打开。你可以在同一个 `File` 实例上使用 `read...` 和 `write...` 方法。

在从 `Path` 构造 `File` 之后，执行任何 I/O 操作之前务必先检查 `isValid()`。

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

*该示例打开一个文件进行读写，检查有效性，读取全部文本，然后关闭。*

---

#### `File(const String &host, int port)`

**Signature**

```cpp
File(const String &host, int port);
```

**Synopsis**  
Opens a TCP socket connection to host:port.

**Parameters**

* `host` – 要连接的主机名或 IP 地址（例如 `"example.com"` 或 `"127.0.0.1"`）。
* `port` – 远程主机上的 TCP 端口号。

**Return value**

* *(构造函数；不适用)*

**In Depth**

此构造函数充当一个 **TCP 客户端**：

* 它尝试连接到 `host:port`。
* 成功时，`isValid()` 和 `isOpen()` 返回 `true`，`isSocket()` 返回 `true`。
* 失败时（DNS 错误、主机不可达、连接被拒绝），`isValid()` 返回 `false`。

连接建立后，你可以像对文件一样使用相同的 `read...` 和 `write...` 方法。完成后调用 `close()` 终止连接。

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

*该示例打开一个 TCP 连接，发送一个简单 HTTP 请求，读取响应，然后关闭套接字。*

---

#### `File(int port)`

**Signature**

```cpp
File(int port);
```

**Synopsis**  
Creates a listening server socket on the given port.

**Parameters**

* `port` – 要监听传入连接的 TCP 端口号。

**Return value**

* *(构造函数；不适用)*

**In Depth**

该构造函数创建一个监听新客户端连接的**服务器套接字**：

* 成功时，`isValid()` 返回 `true`，`isServerSocket()` 返回 `true`，`isSocket()` 也返回 `true`。
* 失败时（端口已被占用、权限不足），`isValid()` 返回 `false`。

要接受客户端连接，请调用 `accept()`，它会返回一个新的 `File` 实例来表示该客户端套接字。

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

*该示例创建一个服务器套接字，接受单个客户端，读取请求，返回 `"OK"`，并关闭客户端连接。*

---

#### `File(const File &other)`

**Signature**

```cpp
File(const File &other);
```

**Synopsis**  
Creates a copy (shares the underlying handle).

**Parameters**

* `other` – 其底层句柄将被共享的现有 `File`。

**Return value**

* *(构造函数；不适用)*

**In Depth**

拷贝构造函数会创建一个新的 `File` 对象，它引用与 `other` **相同的底层 OS 句柄**：

* 对任一 `File` 实例的操作都会作用在同一个文件或套接字上。
* 关闭一个实例通常会关闭这个共享句柄；另一个实例的行为将依赖于具体实现。

当你需要多个 `File` 对象引用同一底层资源时可以使用这一点，但必须小心协调生命周期和关闭语义。

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

*该示例演示了两个 `File` 对象通过同一底层句柄向同一个文件写入。*

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

* *(析构函数；不适用)*

**In Depth**

当 `File` 对象被销毁时，如果底层文件或套接字仍然处于打开状态，它会自动关闭该句柄。这确保了即使你忘记显式调用 `close()`，资源也不会泄漏。

在你仔细管理作用域时，RAII 能带来确定性的清理行为。

**Example**

```cpp
{
  File f(Path("data.bin"));
  if (f.isValid()) {
    // perform I/O
  }
} // f is destroyed here; handle is closed automatically
```

*该示例在对象离开作用域时，让析构函数自动关闭文件。*

---

#### `File &operator=(const File &other)`

**Signature**

```cpp
File &operator=(const File &other);
```

**Synopsis**  
Assigns another file (shares the underlying handle).

**Parameters**

* `other` – 其句柄将被赋给当前对象的 `File`。

**Return value**

* 返回对 `*this` 的引用，以支持链式赋值。

**In Depth**

拷贝赋值运算符会替换当前句柄（如有必要先关闭），并使该 `File` 与 `other` 共享底层句柄。赋值之后：

* `*this` 和 `other` 都引用同一资源。
* 与拷贝构造类似，关闭其中一个会影响共享句柄对另一个的行为。

在对 `File` 对象进行赋值时一定要意识到这种共享所有权。

**Example**

```cpp
File f1(Path("output.txt"));
File f2(Path("backup.txt"));

if (f1.isValid()) {
  f2 = f1;            // f2 now refers to the same handle as f1
  f2.write("Hello");  // writes through the shared handle
}
```

*该示例将一个 `File` 赋值给另一个，使它们共享同一底层句柄。*

---

#### `String getPath() const`

**Signature**

```cpp
String getPath() const;
```

**Synopsis**  
Returns the file path, or empty string for sockets.

**Parameters**

* *(none)*

**Return value**

* 对于普通文件，返回包含路径的 `String`；如果 `File` 表示套接字或其他非文件句柄，则返回空字符串。

**In Depth**

使用 `getPath()` 查询与普通文件关联的路径，这在日志记录或调试时非常有用。

对于套接字（客户端或服务器）以及可能的命名管道，没有对应的文件路径，因此该函数返回空字符串。

**Example**

```cpp
File f(Path("data.txt"));

if (f.isValid() && f.isRegularFile()) {
  String path = f.getPath();
  if (!path.isEmpty()) {
    // path contains "data.txt" (or the full resolved path)
  }
}
```

*该示例获取普通文件句柄的文件路径。*

---

#### `String getHost() const`

**Signature**

```cpp
String getHost() const;
```

**Synopsis**  
Returns the socket host, or empty string for files.

**Parameters**

* *(none)*

**Return value**

* 对于套接字，返回主机名的 `String`；对于基于文件的句柄返回空字符串。

**In Depth**

对于通过 `File(const String &host, int port)` 创建的 TCP 客户端套接字，`getHost()` 返回你连接到的主机。对于普通文件和非套接字句柄，没有主机信息，因此返回空字符串。

**Example**

```cpp
File socket("example.com", 80);

if (socket.isValid() && socket.isSocket()) {
  String host = socket.getHost();  // "example.com"
  if (!host.isEmpty()) {
    // Use host name
  }
}
```

*该示例从 TCP 客户端套接字中查询远程主机名。*

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

* 返回与套接字关联的端口号；如果句柄不是套接字，则返回 `-1`。

**In Depth**

对于通过 `File(const String &host, int port)` 或 `File(int port)` 创建的套接字：

* `getPort()` 返回端口号（具体是远程端口还是本地端口取决于实现）。

对于普通文件和非套接字句柄，则返回 `-1`。

**Example**

```cpp
File server(8080);

if (server.isValid()) {
  int port = server.getPort();  // 8080
}
```

*该示例从服务器套接字读取监听端口。*

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

* 如果底层句柄已成功创建或打开，返回 `true`；否则返回 `false`。

**In Depth**

`isValid()` 是检查构造（或之后可能替换句柄的操作）是否成功的主要方式。如果它返回 `false`：

* 所有 I/O 操作通常都会失败或表现为无操作。
* `isOpen()` 也通常为 `false`。

在构造 `File` 之后应立即测试 `isValid()`，以免在无效句柄上执行 I/O。

**Example**

```cpp
File f(Path("missing.txt"));

if (!f.isValid()) {
  // Handle error: file could not be opened
}
```

*该示例检查打开文件失败的情况并做相应处理。*

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

* 如果句柄当前处于打开状态，返回 `true`；若已关闭或从未正确打开，则返回 `false`。

**In Depth**

`isOpen()` 用于报告底层句柄是否仍然活动：

* 对于普通文件，在调用 `close()` 之后或某些致命错误发生后它会变为 `false`。
* 对于套接字，在调用 `close()` 或连接完全关闭后它会变为 `false`。

一个 `File` 可能既无效（`isValid() == false`）又非打开状态。对于有效句柄，`isOpen()` 表示是否还可以继续进行 I/O。

**Example**

```cpp
File f(Path("data.txt"));

if (f.isValid()) {
  // Do some I/O...
  f.close();
  bool stillOpen = f.isOpen();  // false
}
```

*该示例显示，显式关闭后 `isOpen()` 变为 `false`。*

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

`close()` 显式释放底层资源：

* 对于文件，关闭后后续读写将失败。
* 对于套接字，连接被关闭，缓冲的数据会根据操作系统行为被丢弃或处理。

你可以安全地多次调用 `close()`；后续调用通常是空操作。

即便你依赖析构函数来关闭句柄，显式调用 `close()` 在以下场景仍然很有价值：

* 希望尽快释放文件锁。
* 希望在离开作用域前显式结束网络会话。

**Example**

```cpp
File f(Path("output.txt"));

if (f.isValid()) {
  f.write("Done.\n");
  f.close();  // ensure data is flushed and handle is released
}
```

*该示例向文件写入并显式关闭，确保数据已刷新且句柄被释放。*

---

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

* 包含所有可读取字节的 `Buffer`，直到流结束或不再有可立即读取的数据为止。

**In Depth**

该方法会从当前读写位置一直读到结尾：

* 对于普通文件，它通常从当前位置一直读取到文件结束。
* 对于套接字和管道，它会一直读取，直到远端关闭连接或系统指示不再有可立即读取的数据。

如果没有可读数据或发生错误，返回的 `Buffer` 可能为空。

**Example**

```cpp
File f(Path("image.bin"));

if (f.isValid()) {
  Buffer data = f.readAllToBuffer();
  int size = data.length();
}
```

*该示例将一个二进制文件的全部内容读取到 `Buffer` 中。*

---

#### `Buffer readToBuffer(int count)`

**Signature**

```cpp
Buffer readToBuffer(int count);
```

**Synopsis**  
Reads up to count bytes into a buffer.

**Parameters**

* `count` – 要读取的最大字节数。

**Return value**

* 包含最多 `count` 字节的 `Buffer`。如果流提前结束或当前可用字节较少，则可能包含更少字节。

**In Depth**

当你希望分步控制读取量时（例如按块处理文件），该方法非常有用：

* 它可能会阻塞，直到至少有一个字节可读或流结束。
* 当已读取 `count` 字节，或不再有可立即读取的数据时停止。

如果 `count <= 0`，行为由实现定义；请传入正值。

**Example**

```cpp
File f(Path("data.bin"));

if (f.isValid()) {
  Buffer chunk = f.readToBuffer(1024);  // read up to 1024 bytes
}
```

*该示例从文件中读取一个最多 1024 字节的定长块。*

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

* 包含所有可用数据的 `String`，并按 UTF-8 文本进行解码。

**In Depth**

`readAllToString()` 类似于 `readAllToBuffer()`，但它将数据解释为 UTF-8 并返回 `String`。这适用于文本文件、HTTP 响应和其他基于文本的协议。

如果数据不是合法的 UTF-8，行为依赖具体实现（可能出现替换字符或文本被截断）。

**Example**

```cpp
File f(Path("notes.txt"));

if (f.isValid()) {
  String text = f.readAllToString();
}
```

*该示例将整个文本文件读入一个 `String`。*

---

#### `String readToString(int count)`

**Signature**

```cpp
String readToString(int count);
```

**Synopsis**  
Reads up to count bytes into a string.

**Parameters**

* `count` – 要读取的最大字节数。

**Return value**

* 包含最多 `count` 字节并按 UTF-8 文本解释的 `String`。

**In Depth**

该方法类似于 `readToBuffer(int)`，但返回 `String`。当你希望按固定大小的块处理文本，或者处理流式协议时非常实用。

在按块读取且块大小与 UTF-8 字符边界不对齐时要小心：一个多字节字符可能被拆分到多次读取中，这可能导致中间字符串无效。对于简单 ASCII 协议则无需担心。

**Example**

```cpp
File f(Path("log.txt"));

if (f.isValid()) {
  String firstPart = f.readToString(1000);  // read up to 1000 bytes as text
}
```

*该示例从文件中读取有限长度的文本。*

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

* 如果至少有一个字节可以在不阻塞的情况下读取，返回 `true`；否则返回 `false`。

**In Depth**

`hasAvailable()` 提供一种非阻塞方式来检查是否有待处理数据：

* 对于套接字和管道，它可以帮助你在没有数据就绪时避免阻塞读取。
* 对于普通文件，它通常表示当前读指针到文件末尾之间是否还有剩余字节。

在你希望避免阻塞，尤其在简单事件循环中调用 `read...` 前使用这一方法非常合适。

**Example**

```cpp
File socket("example.com", 80);

if (socket.isValid()) {
  if (socket.hasAvailable()) {
    String response = socket.readAllToString();
  }
}
```

*该示例在读取套接字前先检查是否有可用数据。*

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

* 可以在不阻塞的情况下读取的字节数；如果没有可用数据或不支持该操作，则返回 `0`。

**In Depth**

`getAvailableCount()` 是对 `hasAvailable()` 的细化，返回可读字节数：

* 对于普通文件，通常是 `endPosition - currentPosition`，受实现限制。
* 对于套接字和管道，它返回当前已缓冲、可立即读取的字节数。

如果底层平台不支持查询该信息，它可能返回 `0` 或其他哨兵值。

**Example**

```cpp
File f(Path("data.bin"));

if (f.isValid()) {
  int available = f.getAvailableCount();
  Buffer chunk = f.readToBuffer(available);
}
```

*该示例查询可用字节数并精确读取该数量的数据。*

---

#### `int write(const Buffer &buf)`

**Signature**

```cpp
int write(const Buffer &buf);
```

**Synopsis**  
Writes a buffer. Returns bytes written, or -1 on error.

**Parameters**

* `buf` – 其字节将被写入流的 `Buffer`。

**Return value**

* 成功写入的字节数；如果发生错误，则返回 `-1`。

**In Depth**

`write()` 尝试写入 `buf` 的全部内容：

* 成功时，返回值等于 `buf.length()`。
* 在部分写入（在套接字场景中更常见）时，可能返回更小的数。
* 失败时，返回 `-1`。

你应当始终检查返回值，尤其是在网络套接字上，并在必要时处理部分写入。

**Example**

```cpp
Buffer data(String("Hello\n"));
File f(Path("output.txt"));

if (f.isValid()) {
  int written = f.write(data);
  // written is the number of bytes actually written
}
```

*该示例将一个缓冲区写入文件并检查实际写入的字节数。*

---

#### `int write(const String &str)`

**Signature**

```cpp
int write(const String &str);
```

**Synopsis**  
Writes a string. Returns bytes written, or -1 on error.

**Parameters**

* `str` – 其字节将被写入的 UTF-8 `String`。

**Return value**

* 写入的字节数（按 UTF-8 计算）；错误时返回 `-1`。

**In Depth**

这是一个便捷重载，它将字符串转换为字节，并委托给底层写入逻辑。当你处理文本而非原始二进制数据时可以使用它。

与缓冲区重载一样，也可能发生部分写入；返回值可能小于 `str.byteLength()`。

**Example**

```cpp
File f(Path("log.txt"));

if (f.isValid()) {
  f.write("Application started\n");
}
```

*该示例直接从 `String` 字面值写入一行文本。*

---

#### `int writeUpTo(const Buffer &buf, int count = -1)`

**Signature**

```cpp
int writeUpTo(const Buffer &buf, int count = -1);
```

**Synopsis**  
Writes up to count bytes from a buffer. Returns bytes written, or -1 on error.

**Parameters**

* `buf` – 其字节将被写入的 Buffer。
* `count` – 要写入的最大字节数。如果为 `-1`，则整个缓冲区都可被写入。

**Return value**

* 实际写入的字节数；如果发生错误，则为 `-1`。

**In Depth**

当你希望将数据分段发送时，`writeUpTo()` 非常有用：

* 若 `count` 为 `-1`，该方法视为“最多写入 `buf.length()` 字节”。
* 若 `count` 为正数，则只会尝试写入 `buf` 的前 `count` 个字节。
* 如往常一样，由于部分写入，返回值可能小于 `count`。

**Example**

```cpp
Buffer data(String("chunked data"));
File socket("example.com", 80);

if (socket.isValid()) {
  int written = socket.writeUpTo(data, 5);  // write first 5 bytes
}
```

*该示例只将缓冲区的一部分写入套接字。*

---

#### `int writeUpTo(const String &str, int count = -1)`

**Signature**

```cpp
int writeUpTo(const String &str, int count = -1);
```

**Synopsis**  
Writes up to count bytes from a string. Returns bytes written, or -1 on error.

**Parameters**

* `str` – 其 UTF-8 字节将被写入的 `String`。
* `count` – 要写入的最大字节数。如果为 `-1`，则整个字符串都可被写入。

**Return value**

* 实际写入的字节数；如果错误则为 `-1`。

**In Depth**

该重载与缓冲区版本相对应，但起点是 `String`。在按字节限制写入时，对多字节 UTF-8 字符要格外小心：

* 如果 `count` 截断在某个多字节字符中间，则只会写入其前缀字节；接收方可能会看到无效的 UTF-8。
* 对纯 ASCII 文本则不存在这个问题。

当你希望写入整个字符串时使用默认的 `-1` 即可。

**Example**

```cpp
String msg("Hello, world\n");
File f(Path("partial.txt"));

if (f.isValid()) {
  int written = f.writeUpTo(msg, 5);  // write "Hello"
}
```

*该示例将字符串的前 5 个字节写入文件（得到 "Hello"）。*

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

* 如果刷新成功或无需刷新，返回 `true`；发生错误时返回 `false`。

**In Depth**

`flush()` 确保任何缓冲数据被推送到底层系统：

* 对于普通文件，它通常会强制数据从用户空间缓冲区写入操作系统。
* 对于套接字，如果系统会立即写出数据，它可能是空操作。

在以下情况中调用 `flush()` 尤为重要：

* 你希望确保日志消息已经落盘。
* 你在编写对交付时效性有要求的协议。

**Example**

```cpp
File f(Path("log.txt"));

if (f.isValid()) {
  f.write("Critical event\n");
  bool ok = f.flush();
}
```

*该示例写入一条日志并立即刷新，以确保其被持久化。*

---

#### `bool setPosition(long long pos)`

**Signature**

```cpp
bool setPosition(long long pos);
```

**Synopsis**  
Sets the read/write position (files only). Returns true on success.

**Parameters**

* `pos` – 从文件开头起算的新字节位置。

**Return value**

* 若成功改变位置则为 `true`；若不支持该操作或失败则为 `false`。

**In Depth**

`setPosition()` 会将文件指针移动到新的字节偏移处：

* 对普通文件支持。
* 对于套接字或管道通常不支持；在这些情况下会返回 `false`。

你可以用它来实现随机访问：

* 跳转到文件开头（`pos = 0`）。
* 跳转到特定记录的偏移处。

**Example**

```cpp
File f(Path("data.bin"));

if (f.isValid() && f.isRegularFile()) {
  f.setPosition(0);                // rewind to start
  Buffer head = f.readToBuffer(16); // read first 16 bytes
}
```

*该示例将文件回绕至开头并读取一个固定大小的头部。*

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

* 从文件开头起算的当前字节偏移；若不支持该操作则为 `-1`。

**In Depth**

对于普通文件，`getPosition()` 返回当前文件指针的位置。对于套接字和管道，不存在有意义的文件偏移，因此通常返回 `-1`。

这在进度报告或保存/恢复文件位置时非常有用。

**Example**

```cpp
File f(Path("data.bin"));

if (f.isValid()) {
  long long pos = f.getPosition();
}
```

*该示例查询文件当前的读/写位置。*

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

* 对于普通文件，返回文件大小（字节数）；若不支持（如套接字）则为 `-1`。

**In Depth**

对于普通文件，`getEndPosition()` 通常返回文件的总字节大小。结合 `getPosition()`，你可以计算进度：

```cpp
progress = (double)getPosition() / getEndPosition();
```

对于不可寻址的流，该函数通常返回 `-1`。

**Example**

```cpp
File f(Path("data.bin"));

if (f.isValid() && f.isRegularFile()) {
  long long size = f.getEndPosition();
}
```

*该示例获取普通文件的总大小。*

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

* 如果当前位置位于文件末尾或之后，返回 `true`；否则返回 `false`。

**In Depth**

`isAtEnd()` 主要对普通文件有意义：

* 在读到文件末尾之后，它会变为 `true`。
* 对于套接字和管道，当远端关闭连接时语义可能类似，但具体行为依赖实现。

你可以利用它来检测是否已消耗完所有文件内容，而无需手动比较位置。

**Example**

```cpp
File f(Path("data.txt"));

if (f.isValid()) {
  String all = f.readAllToString();
  bool done = f.isAtEnd();  // typically true after reading everything
}
```

*该示例在读取全部内容后检查文件位置是否已经到达末尾。*

---

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

* 若 `File` 表示磁盘文件则为 `true`；否则为 `false`。

**In Depth**

使用 `isRegularFile()` 来区分文件与套接字、管道。这在以下场景中非常重要：

* 使用 `setPosition()`、`getPosition()` 或 `getEndPosition()` 时——这些只对普通文件有意义。
* 需要执行如寻址或计算文件大小等文件特有逻辑时。

**Example**

```cpp
File f(Path("data.bin"));

if (f.isRegularFile()) {
  long long size = f.getEndPosition();
}
```

*该示例在调用文件特定 API 之前确认句柄是普通文件。*

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

* 若 `File` 表示 TCP 套接字（客户端或服务器）则为 `true`；否则为 `false`。

**In Depth**

`isSocket()` 是检测网络句柄的快速方式。客户端套接字（`File(const String&, int)`）和服务器套接字（`File(int port)`）都应返回 `true`。

结合 `isServerSocket()` 可以区分监听套接字和已连接套接字。

**Example**

```cpp
File socket("example.com", 80);

if (socket.isSocket()) {
  // Perform network-specific logic
}
```

*该示例检查句柄是否为套接字，然后再执行网络相关操作。*

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

* 若该句柄是监听服务器套接字则为 `true`；否则为 `false`。

**In Depth**

服务器套接字由 `File(int port)` 构造。它们：

* 监听传入连接。
* 与 `accept()` 一起使用以创建客户端套接字。

通过 `File(const String&, int)` 创建的客户端套接字，其 `isServerSocket()` 应返回 `false`。

**Example**

```cpp
File listener(9000);

if (listener.isValid() && listener.isServerSocket()) {
  File client = listener.accept();
}
```

*该示例在调用 `accept()` 之前确认某个 `File` 是服务器套接字。*

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

* 若底层句柄是命名管道则为 `true`；否则为 `false`。

**In Depth**

命名管道提供同一台机器上进程间通信。是否以及如何使用由 attoboy 库的其他部分或系统集成方式决定。

`File` 暴露该方法，以便在处理此类句柄时调整你的逻辑。

**Example**

```cpp
File f(Path("\\\\.\\pipe\\my_pipe"));

if (f.isNamedPipe()) {
  // Handle pipe-specific behavior
}
```

*该示例检查某个句柄是否表示命名管道。*

---

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

* 表示已接受客户端连接的 `File`。如果接受失败，返回的 `File` 将是无效的（`isValid() == false`）。

**In Depth**

`accept()` 只有在服务器套接字（`isServerSocket() == true`）上调用才有意义：

* 它会阻塞，直到某个客户端连接或发生错误。
* 成功时，返回一个新的 `File` 来表示该客户端连接。这个新的 `File` 是普通套接字（非服务器套接字）。
* 原始服务器 `File` 仍保持打开，可以再次调用 `accept()` 接受更多客户端。

如果在非服务器句柄上调用 `accept()`，行为未定义；因此始终应先检查 `isServerSocket()`。

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

*该示例在服务器套接字上接受一个连接，交换简单文本消息，然后关闭客户端连接。*

---

#### `bool equals(const File &other) const`

**Signature**

```cpp
bool equals(const File &other) const;
```

**Synopsis**  
Returns true if this file equals the other.

**Parameters**

* `other` – 要比较的另一个 `File`。

**Return value**

* 若两个 `File` 对象引用同一底层句柄则为 `true`；否则为 `false`。

**In Depth**

`equals()` 比较的是底层 OS 句柄的身份，而不是文件或套接字内容。若两个 `File` 实例共享同一内部句柄（例如通过拷贝构造或赋值创建），则它们被认为是相等的。

这不是路径或远程端点的比较，而是句柄本身的比较。

**Example**

```cpp
File f1(Path("data.txt"));
File f2(f1);  // shares handle with f1

bool same = f1.equals(f2);  // true
```

*该示例展示了两个共享同一句柄的 `File` 对象被视为相等。*

---

#### `bool operator==(const File &other) const`

**Signature**

```cpp
bool operator==(const File &other) const;
```

**Synopsis**  
Returns true if this file equals the other.

**Parameters**

* `other` – 要比较的另一个 `File`。

**Return value**

* 若两个 `File` 对象引用同一底层句柄则为 `true`；否则为 `false`。

**In Depth**

`operator==` 是 `equals(other)` 的简写。它支持更惯用的比较方式：

```cpp
if (a == b) { ... }
```

与 `equals()` 一样，该比较关注的是句柄身份，而非文件内容。

**Example**

```cpp
File a(Path("data.txt"));
File b(a);

if (a == b) {
  // a and b share the same handle
}
```

*该示例使用相等运算符检查两个 `File` 实例是否引用同一底层句柄。*

---

#### `bool operator!=(const File &other) const`

**Signature**

```cpp
bool operator!=(const File &other) const;
```

**Synopsis**  
Returns true if this file does not equal the other.

**Parameters**

* `other` – 要比较的另一个 `File`。

**Return value**

* 若两个 `File` 实例不引用同一句柄则为 `true`；若引用同一句柄则为 `false`。

**In Depth**

该运算符只是 `operator==` 的逻辑非。当你希望确保两个句柄不同的时候很方便。

**Example**

```cpp
File f1(Path("a.txt"));
File f2(Path("b.txt"));

if (f1 != f2) {
  // f1 and f2 refer to different handles (likely different files)
}
```

*该示例检查两个 `File` 对象不引用同一底层资源。*