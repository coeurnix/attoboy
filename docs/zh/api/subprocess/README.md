# Subprocess

## 概览

`attoboy::Subprocess` 类为在 attoboy 应用中启动并与外部程序（子进程）交互提供了一个高层、基于 RAII 的接口。它支持：

* **简单的一次性辅助函数**：

  * `Start()` – 仅启动不等待（fire-and-forget）。
  * `Run()` – 启动并等待，返回退出码。
  * `Capture()` – 启动并收集合并后的 `stdout + stderr`，以 `String` 返回。
* **流式 I/O**：

  * 构造一个 `Subprocess` 对象并使用 `write()` 向进程的标准输入发送数据。
  * 使用 `readToString()` 或 `readToBuffer()` 增量地读取输出。
  * 查询 `isRunning()` 和 `hasAvailable()` 以驱动交互式或长时间运行的任务。

`Subprocess` 与其他 attoboy 类型集成良好：

* `Path` – 指定要运行的可执行文件。
* `String` – 用于参数、输入和输出文本。
* `Buffer` – 用于二进制输入和输出。

### 进程、标准 I/O 与退出码

**进程** 是操作系统中运行的程序实例。当你启动一个子进程时：

* 子进程通常具有：

  * **标准输入（stdin）** – 你的程序可以发送给它的数据。
  * **标准输出（stdout）** – 它发回的数据。
  * **标准错误（stderr）** – 错误和诊断输出。
* 进程最终会以一个 **退出码** 结束（也称为退出状态或返回码）：

  * `0` 通常表示成功。
  * 非零通常表示错误或特殊情况。

`Subprocess` 将这些细节抽象为简单的方法：

* 通过 `Run()` 或 `wait()` 获取退出码。
* 通过 `Capture()` 或流式 `read*` 方法获取组合输出。
* 通过 `getProcessId()` 获取进程标识。

### Fire-and-Forget 与流式子进程

主要有两种使用模式：

1. **Fire-and-forget / 简单调用**  
   当你只需要一个结果且不关心流式 I/O 时，使用静态辅助函数：

   * `Subprocess::Start(exe, args...)` – 启动但不等待。
   * `Subprocess::Run(exe, args...)` – 启动并等待；获取退出码。
   * `Subprocess::Capture(exe, args...)` – 启动、等待并获取完整输出。

2. **流式 I/O（交互式）**  
   当你需要：

   * 逐步读取输出。
   * 随时间向进程发送命令。
   * 监控和控制长时间运行的任务。

   时，请使用构造函数和实例方法。

   在此模式下，`Subprocess` 对象管理到底层进程的句柄。句柄在析构函数中释放，但**进程本身不会被自动终止**；需使用 `wait()` 或 `terminate()` 控制其生命周期。

---

## 参考

下面每一项都文档化了 `attoboy::Subprocess` 的一个公共构造函数、静态函数或成员函数。

---

### 静态辅助函数

#### `template <typename... Args> static void Start(const Path &executable, const Args &...args)`

**函数签名**

```cpp
template <typename... Args>
static void Start(const Path &executable, const Args &...args) {
  Start_impl(executable, List(String(args)...));
}
```

**概要**  
启动一个进程且不等待（fire and forget）。

**参数**

* `executable` – 要运行程序的 `Path`。
* `args` – 零个或多个命令行参数。每个参数必须可转换为 `String`（例如 `const char *`、`String`、`int`、`bool`）。

**返回值**

* *(无；返回类型为 `void` 的静态函数)*

**深入说明**

`Start()` 启动一个进程并立即返回，而不会提供该进程的句柄。这在以下场景中很有用：

* 你希望触发一个辅助工具或后台任务。
* 你不需要读取它的输出或等待它完成。
* 该进程的退出码和 I/O 对你的逻辑不重要。

在内部，`args` 会被转换为 `String` 值并收集到一个 `List` 中，然后传递给私有实现函数。被调用的进程将它们视为传统的命令行参数。

由于不会返回 `Subprocess` 对象，你之后无法查询 `isRunning()` 或终止该进程。如果需要这些控制，请使用 `Run()` 或基于构造函数的 API。

**示例**

```cpp
Path exe("helper_tool.exe");

// 启动该工具并传入两个参数，不等待其结束。
Subprocess::Start(exe, "--cleanup", "temp");
```

*此示例启动一个后台辅助程序执行清理操作，并立即继续执行而不等待其完成。*

---

#### `template <typename... Args> static int Run(const Path &executable, const Args &...args)`

**函数签名**

```cpp
template <typename... Args>
static int Run(const Path &executable, const Args &...args) {
  return Run_impl(executable, List(String(args)...));
}
```

**概要**  
运行一个进程并等待其完成，返回退出码。

**参数**

* `executable` – 要运行程序的 `Path`。
* `args` – 零个或多个命令行参数，每个都可转换为 `String`。

**返回值**

* 子进程返回的退出码；或在出错时返回一个由实现定义的值（通常为 `-1`）。

**深入说明**

当你只关心外部程序的**退出码**而不关心其输出时，`Run()` 是最简单的执行方式。该函数会：

1. 使用给定的 `executable` 和参数启动进程。
2. 同步等待进程退出。
3. 返回退出码。

由于调用会阻塞，请在任务较短或你可以接受等待的情况下使用它。对于希望获取进度更新或保持 UI 响应的长时间任务，更适合使用流式 `Subprocess` 实例。

**示例**

```cpp
Path exe("validator.exe");

// 使用文件路径运行验证工具并检查退出码。
int exitCode = Subprocess::Run(exe, "config.json");

if (exitCode == 0) {
  // 验证成功。
} else {
  // 处理验证错误。
}
```

*此示例同步运行验证工具，并基于其退出码进行分支处理。*

---

#### `template <typename... Args> static String Capture(const Path &executable, const Args &...args)`

**函数签名**

```cpp
template <typename... Args>
static String Capture(const Path &executable, const Args &...args) {
  return Capture_impl(executable, List(String(args)...));
}
```

**概要**  
运行一个进程并捕获 stdout+stderr，将输出作为字符串返回。

**参数**

* `executable` – 要运行程序的 `Path`。
* `args` – 零个或多个命令行参数，每个都可转换为 `String`。

**返回值**

* 一个 `String`，包含子进程的标准输出和标准错误的合并内容。
* 在失败或进程未产生输出时通常为空。

**深入说明**

`Capture()` 是用于“运行并收集输出”工作流的便捷方法：

* 它启动进程。
* 捕获写至 `stdout` 与 `stderr` 的所有内容。
* 等待进程退出。
* 将收集到的文本作为一个 `String` 返回。

这对于以下类型的工具非常理想：

* 输出人类可读信息。
* 生成你希望解析的机器可读输出（JSON、CSV 等）。

如果你还需要退出码，则可能需要让你的工具通过输出报告状态，或在合适情况下分别调用 `Run()` 与 `Capture()` 运行两次。当你需要更精细的流式控制时，请改用基于构造函数的 API。

**示例**

```cpp
Path exe("tool.exe");

// 运行工具并捕获全部文本输出。
String output = Subprocess::Capture(exe, "--list", "--json");

// 现在可以根据需要解析或记录 'output'。
```

*此示例运行一个以 JSON 格式列出信息的工具，并将其输出收集到一个 `String` 中。*

---

#### `template <typename... Args> Subprocess(const Path &executable, const Args &...args)`

**函数签名**

```cpp
template <typename... Args>
Subprocess(const Path &executable, const Args &...args)
    : Subprocess(executable, List(String(args)...)) {}
```

**概要**  
创建一个带流式 I/O 支持的子进程。

**参数**

* `executable` – 要运行程序的 `Path`。
* `args` – 零个或多个命令行参数，每个都可转换为 `String`。

**返回值**

* *(构造函数；不适用)*

**深入说明**

该构造函数启动一个新进程，并创建一个 `Subprocess` 对象来管理其句柄。与 `Start()` 不同，你可以保留以下控制能力：

* 判断进程是否有效（`isValid()`）。
* 判断进程是否仍在运行（`isRunning()`）。
* 决定何时等待其完成（`wait()`）。
* 如何读写其 I/O 流（`readToString()`、`readToBuffer()`、`write()`）。
* 何时（以及是否）终止它（`terminate()`）。

在内部，参数会被转换为 `String` 并通过 `List` 传递到一个私有构造函数。创建的 `Subprocess` 对象可以被拷贝与赋值；拷贝对象共享同一个底层进程句柄。

在构造 `Subprocess` 之后，通常应先调用 `isValid()` 再继续使用。

**示例**

```cpp
Path exe("interactive_tool.exe");

// 启动一个带单个参数的交互式子进程。
Subprocess proc(exe, "--interactive");

if (proc.isValid()) {
  // 已准备好通过 write() 和 readToString() 进行通信。
}
```

*此示例启动一个交互式工具，并在继续前检查其是否成功创建。*

---

#### `Subprocess(const Subprocess &other)`

**函数签名**

```cpp
Subprocess(const Subprocess &other);
```

**概要**  
创建一个拷贝（共享底层进程）。

**参数**

* `other` – 其句柄将被共享的现有 `Subprocess`。

**返回值**

* *(构造函数；不适用)*

**深入说明**

拷贝构造函数**不会**启动新的操作系统进程。它会创建另一个 `Subprocess` 对象，引用相同的底层子进程：

* 两个实例看到的 `isRunning()` 状态相同。
* 任一实例都可以调用 `wait()`、`terminate()`、`write()` 或 `read*()`。
* 所有权是共享的；只有在所有引用被销毁后，句柄才会被清理。

这使得在函数间传递 `Subprocess` 对象变得容易，而无需担心重复启动进程。

**示例**

```cpp
Path exe("server.exe");
Subprocess server(exe, "--mode", "child");

Subprocess shared(server);  // 共享相同的底层进程

// 'server' 和 'shared' 都可与同一个子进程交互。
```

*此示例拷贝一个 `Subprocess` 句柄，使代码中的多个部分可以与同一子进程交互。*

---

#### `~Subprocess()`

**函数签名**

```cpp
~Subprocess();
```

**概要**  
销毁句柄（不会终止进程）。

**参数**

* *(无)*

**返回值**

* *(析构函数；不适用)*

**深入说明**

析构函数会释放该 `Subprocess` 对象所拥有的底层进程句柄。关键点在于：

* 它**不会**终止子进程。
* 如果还有其他 `Subprocess` 拷贝引用同一句柄，该进程仍由它们管理。
* 除非你显式调用 `terminate()`，否则该 OS 进程最终会自行退出。

如果你需要在 `Subprocess` 对象离开作用域前确保进程已结束，应调用 `wait()` 或 `terminate()`。

**示例**

```cpp
{
  Path exe("worker.exe");
  Subprocess worker(exe);

  // 可选地在此等待或终止。
  // worker.wait();
} // 'worker' 句柄被释放；OS 进程可能仍在运行。
```

*此示例展示了一个 `Subprocess` 离开作用域；仅句柄被销毁，而子进程本身不会被自动终止。*

---

#### `Subprocess &operator=(const Subprocess &other)`

**函数签名**

```cpp
Subprocess &operator=(const Subprocess &other);
```

**概要**  
赋值为另一个子进程（共享底层进程）。

**参数**

* `other` – 赋值来源的 `Subprocess`。

**返回值**

* `*this` 的引用，以支持赋值链式调用。

**深入说明**

赋值运算符会让左侧对象引用与 `other` 相同的底层进程。先前的句柄（如果存在）会被释放：

* 不会创建新的进程。
* `*this` 与 `other` 现在共享同一子进程句柄。
* 自赋值是安全的。

当你希望让一个 `Subprocess` 变量改为引用新的或不同的进程时，可以使用该操作符。

**示例**

```cpp
Path exe("task.exe");

Subprocess a(exe, "one");
Subprocess b(exe, "two");

// 此时 'a' 和 'b' 最初引用不同的进程。
a = b;  // 'a' 现在引用与 'b' 相同的进程
```

*此示例通过赋值让一个 `Subprocess` 共享另一个的句柄。*

---

#### `bool isValid() const`

**函数签名**

```cpp
bool isValid() const;
```

**概要**  
如果进程创建成功，则返回 true。

**参数**

* *(无)*

**返回值**

* 若底层进程句柄有效则为 `true`；如果创建失败或句柄无效则为 `false`。

**深入说明**

`isValid()` 用于检查 `Subprocess` 对象是否成功创建（或获取）了一个子进程句柄。常见失败原因包括：

* 可执行文件路径不存在。
* 权限错误。
* 操作系统资源限制。

你通常应在构造或赋值 `Subprocess` 之后，在尝试进行 I/O 或生命周期操作之前调用 `isValid()`。

**示例**

```cpp
Path exe("maybe_missing.exe");
Subprocess proc(exe);

if (!proc.isValid()) {
  // 处理错误：可执行文件可能不存在或无法启动。
}
```

*此示例在继续前验证子进程是否被成功创建。*

---

#### `bool isRunning() const`

**函数签名**

```cpp
bool isRunning() const;
```

**概要**  
如果进程仍在运行，则返回 true。

**参数**

* *(无)*

**返回值**

* 若子进程尚未退出则为 `true`；若已结束或从未有效则为 `false`。

**深入说明**

`isRunning()` 允许你在不阻塞的情况下观察子进程的当前状态：

* 对于有效且正在运行的进程，返回 `true`。
* 在进程退出之后（无论是正常退出还是通过 `terminate()`），返回 `false`。

将其与 `hasAvailable()` 以及 I/O 方法组合使用，可以驱动一个响应式循环来读取输出并检测完成。

**示例**

```cpp
Path exe("long_task.exe");
Subprocess proc(exe);

while (proc.isRunning()) {
  // 轮询输出、更新 UI 等。
}
```

*此示例周期性地检查一个长时间运行的子进程是否仍处于活动状态。*

---

#### `int wait()`

**函数签名**

```cpp
int wait();
```

**概要**  
等待进程结束并返回退出码（出错时返回 -1）。

**参数**

* *(无)*

**返回值**

* 子进程正常结束时的退出码。
* 如果等待失败或进程无效，则返回 `-1`（或其他由实现定义的错误码）。

**深入说明**

`wait()` 会阻塞直到子进程退出。一旦 `wait()` 返回：

* `isRunning()` 将返回 `false`。
* 如果实现支持，你仍可以调用 `readToString()` 或 `readToBuffer()` 来读取任何剩余的缓冲输出。
* 多次调用 `wait()` 通常在第一次调用后要么成为空操作，要么返回缓存的退出码；该行为由实现定义，因此最好只调用一次。

当你拥有一个流式 `Subprocess` 并希望：

* 通过 `write()` 发送一个或多个命令。
* 读取持续产生的输出。
* 最终阻塞等待其完成并获取退出码。

时，可使用 `wait()`。

**示例**

```cpp
Path exe("job.exe");
Subprocess proc(exe, "--mode", "batch");

if (proc.isValid()) {
  int exitCode = proc.wait();
  // 检查 exitCode 以判断成功或失败。
}
```

*此示例启动一个批处理任务并阻塞直到其结束，然后检查退出码。*

---

#### `bool terminate()`

**函数签名**

```cpp
bool terminate();
```

**概要**  
强制终止进程。成功时返回 true。

**参数**

* *(无)*

**返回值**

* 如果终止请求成功则为 `true`；如果失败或进程无效则为 `false`。

**深入说明**

`terminate()` 试图强制停止子进程。这是一个强制性操作：

* 子进程可能没有机会进行清理（刷新文件、保存状态等）。
* 仅在子进程行为异常（挂起、卡死或忽略友好信号）时使用。

成功调用 `terminate()` 之后：

* `isRunning()` 最终会返回 `false`。
* 如果你需要确保进程已经完全退出，可以调用 `wait()`。

**示例**

```cpp
Path exe("worker.exe");
Subprocess proc(exe);

if (proc.isValid()) {
  // ……一段时间后，我们决定停止该 worker ……
  bool ok = proc.terminate();
  // 如有需要，可调用 proc.wait() 确保其已完全退出。
}
```

*此示例在不再需要或子进程行为异常时强制终止一个 worker 子进程。*

---

#### `int getProcessId() const`

**函数签名**

```cpp
int getProcessId() const;
```

**概要**  
返回进程 ID（出错时返回 -1）。

**参数**

* *(无)*

**返回值**

* 子进程的操作系统进程标识符（PID）。
* 如果进程无效或无法获取 ID，则返回 `-1`。

**深入说明**

PID 在操作系统层面唯一标识子进程。你可以将其用于：

* 日志记录或调试。
* 通过 ID 监控进程的外部工具。

注意，进程退出后，OS 可能会重用 PID，因此 PID 仅在进程存活期间具有意义。

**示例**

```cpp
Path exe("worker.exe");
Subprocess proc(exe);

if (proc.isValid()) {
  int pid = proc.getProcessId();
  // 你可以记录 'pid' 或在外部监控工具中使用它。
}
```

*此示例获取并记录一个 worker 子进程的进程 ID。*

---

#### `bool hasAvailable() const`

**函数签名**

```cpp
bool hasAvailable() const;
```

**概要**  
如果有可读取的输出数据，则返回 true。

**参数**

* *(无)*

**返回值**

* 若有可立即读取的输出则为 `true`；否则为 `false`。

**深入说明**

`hasAvailable()` 用于检查子进程输出流当前是否有任何可用数据。它被设计用于在调用以下方法之前使用：

* `readToBuffer()`
* `readToString()`

典型模式是，在循环中通过检查 `hasAvailable()` 来避免阻塞，仅在返回 `true` 时读取数据。这让你能够在保持响应的同时快速响应输出。

**示例**

```cpp
Path exe("chatty.exe");
Subprocess proc(exe);

while (proc.isRunning() || proc.hasAvailable()) {
  if (proc.hasAvailable()) {
    String chunk = proc.readToString();
    // 处理 'chunk'（记录日志、解析等）。
  }
  // 如有需要，在此执行其他工作。
}
```

*此示例在子进程运行期间轮询可用输出，并在有数据时读取文本块。*

---

#### `Buffer readToBuffer()`

**函数签名**

```cpp
Buffer readToBuffer();
```

**概要**  
将可用输出读取到一个缓冲区中。

**参数**

* *(无)*

**返回值**

* 一个 `Buffer`，包含从子进程输出中读取的字节。如果无数据可用或出错，则可能为空。

**深入说明**

`readToBuffer()` 从子进程读取输出并以 `Buffer` 返回，适用于二进制数据：

* 当子进程输出非文本数据（例如协议帧、压缩数据）时使用此方法。
* 结合 `hasAvailable()` 使用，可以避免在无数据时阻塞。

如果你处理的是文本输出，通常 `readToString()` 更为方便。

**示例**

```cpp
Path exe("binary_emitter.exe");
Subprocess proc(exe);

if (proc.isValid()) {
  if (proc.hasAvailable()) {
    Buffer data = proc.readToBuffer();
    // 处理 'data' 中的原始二进制数据。
  }
}
```

*此示例将子进程的二进制输出读取到一个 `Buffer` 中。*

---

#### `String readToString()`

**函数签名**

```cpp
String readToString();
```

**概要**  
将可用输出读取到一个字符串中。

**参数**

* *(无)*

**返回值**

* 一个 `String`，包含从子进程输出中读取的文本。如果无数据可用或出错，则可能为空。

**深入说明**

`readToString()` 是 `readToBuffer()` 的文本版本：

* 它读取可用输出并将其按 UTF-8 解码成 `String`。
* 你可以在更多输出变得可用时多次调用它。
* 将其与 `hasAvailable()` 和 `isRunning()` 在循环中结合使用，以分块构建完整输出。

如果你需要一次性获取全部输出且不关心流式处理，通常使用 `Capture()` 更简单。

**示例**

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

 // 'allOutput' 现在包含脚本产生的全部文本。
```

*此示例增量地读取脚本的文本输出直到其结束，并将其累积到一个 `String` 中。*

---

#### `int write(const Buffer &buf)`

**函数签名**

```cpp
int write(const Buffer &buf);
```

**概要**  
向 stdin 写入缓冲区数据。返回写入的字节数，出错时返回 -1。

**参数**

* `buf` – 包含要发送到子进程标准输入的字节的 `Buffer`。

**返回值**

* 成功写入的字节数。
* 出错时为 `-1`（进程无效、管道关闭或其他 I/O 错误）。

**深入说明**

`write(const Buffer &)` 将二进制数据发送到子进程的标准输入：

* 当子进程期望接收二进制协议或非文本命令时使用此方法。
* 需注意并非所有程序都会从标准输入读取；如果子进程没有设计为消费输入数据，这些数据可能会被忽略。
* 在子进程退出或关闭其输入后，随后的 `write()` 调用通常会失败并返回 `-1`。

你可能需要自行设计子程序来正确解释这些字节。

**示例**

```cpp
Path exe("binary_consumer.exe");
Subprocess proc(exe);

if (proc.isValid()) {
  Buffer payload;
  // ……向 'payload' 填充数据 ……

  int written = proc.write(payload);
  // 检查 'written' 以判断是否成功或是否存在部分写入。
}
```

*此示例通过标准输入向子进程发送二进制数据。*

---

#### `int write(const String &str)`

**函数签名**

```cpp
int write(const String &str);
```

**概要**  
向 stdin 写入字符串。返回写入的字节数，出错时返回 -1。

**参数**

* `str` – 包含要发送到子进程标准输入的 UTF-8 文本的 `String`。

**返回值**

* 成功写入的字节数。
* 出错时为 `-1`。

**深入说明**

`write(const String &)` 是向交互式子进程发送文本命令或输入的最便捷方式：

* 文本会以 UTF-8 字节写出。
* 许多命令行工具将换行符（`"\n"`）解释为“命令结束”，因此发送命令时通常会附带行结束符。
* 在进程退出或关闭其输入流后，后续写操作会失败。

将此方法与 `readToString()` 和 `isRunning()` 配合使用，可以实现请求–响应模式。

**示例**

```cpp
Path exe("interactive_tool.exe");
Subprocess proc(exe, "--interactive");

if (proc.isValid()) {
  // 发送一条命令，并附带换行符以便工具进行解析。
  proc.write(String("status\n"));

  // 随后读取响应。
  if (proc.hasAvailable()) {
    String response = proc.readToString();
    // 根据需要处理 'response'。
  }
}
```

*此示例向交互式工具发送一行命令，并读取其响应。*