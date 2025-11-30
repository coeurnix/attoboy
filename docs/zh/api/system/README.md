# 全局函数

## 概览

attoboy 库中的全局函数提供了一小组专注的**系统级工具**以及一个**结构化日志子系统**，你可以在程序的任意位置使用。它们被设计为无需依赖 C 运行时（CRT）或标准库即可工作，并且与 attoboy 自身的类型（尤其是 `String`）紧密集成。

从高层来看，这些函数涵盖了：

* **应用入口和进程控制**

  * `atto_main()` – attoboy 程序的自定义入口点。
  * `Exit()` – 以退出码终止进程。
  * `Sleep()` – 让当前线程暂停。

* **环境与进程信息**

  * `GetEnv()` / `SetEnv()` – 环境变量。
  * `GetUserName()` / `GetUserDisplayName()` – 当前用户信息。
  * `GetProcessId()` – 当前进程 ID。

* **内存管理**

  * `Alloc()` / `Realloc()` / `Free()` – 在不使用 CRT 的情况下进行堆分配。

* **日志子系统**

  * `EnableLoggingToFile()` / `EnableLoggingToConsole()` – 选择日志输出目标。
  * `Log()`, `LogDebug()`, `LogInfo()`, `LogWarning()`, `LogError()` – 支持严重等级的日志 API。

这些工具刻意保持精简，但足够强大，可以覆盖小型工具和实用程序的大多数需求。

### 系统级概念

在进入参考部分之前，有必要先澄清一些这些函数所抽象的系统级概念。

#### 进程与退出码

**进程**是由操作系统管理的正在运行的程序实例。当你的程序结束时，会向操作系统返回一个**退出码**（也称为“返回码”或“状态码”）：

* `0` 通常表示**成功**。
* 非零值表示不同的**错误条件**或失败原因。

attoboy 的 `Exit(int exitCode)` 提供了一种简单、明确的方式，以指定的退出码终止当前进程。

#### 环境变量

**环境变量**是一个具名的键–值对，其特性包括：

* 与某个进程相关联。
* 会被子进程继承（除非被修改）。
* 常常存储诸如 `PATH`、`HOME`、API 密钥或应用配置等信息。

attoboy 通过如下封装来访问环境变量：

* `GetEnv(name)` – 读取变量。
* `SetEnv(name, value)` – 设置或修改变量。

两者都使用 `String` 作为名称和数值类型，便于与 attoboy 的其它 API 组合使用。

#### 堆内存管理

在典型的 C++ 程序中，你可能使用 `new`/`delete` 或 `malloc`/`free` 进行内存分配。为了保持二进制体积小且避免 CRT，attoboy 中改用：

* `Alloc(size)` – 分配一块 `size` 字节的内存。
* `Realloc(ptr, size)` – 调整先前分配内存块的大小。
* `Free(ptr)` – 释放内存。

这些函数的行为与 CRT 对应函数类似，但由 attoboy 自身提供。它们返回 `void *` 指针，并在文档说明处是**对 `nullptr` 安全的**。

#### 日志、日志级别与输出目标

日志记录是向某个**输出目标**写入程序行为信息的实践，例如：

* **控制台**（标准输出）。
* 磁盘上的**日志文件**。

attoboy 的日志子系统主要由三部分构成：

1. **日志输出目标配置**

   * `EnableLoggingToFile(path, truncate)` – 将日志输出到文件。
   * `EnableLoggingToConsole()` – 将日志输出到控制台（默认）。

   同一时间只有一个输出目标处于激活状态；切换是对整个进程**全局生效**的。

2. **日志严重级别函数**

   * `Log()` – 无严重级别前缀的通用日志。
   * `LogDebug()` – 调试级信息。
   * `LogInfo()` – 信息级消息。
   * `LogWarning()` – 警告。
   * `LogError()` – 错误。

   它们都接受任意数量的参数，每个参数在内部都会被转换为 `String`。函数会自动追加换行符，因此不需要手动添加 `"\n"`。

3. **编译期日志级别选择（宏）**

   头文件通过预处理器宏来配置日志级别：

   * `ATTOBOY_LOG_DEBUG_ENABLE`   – 启用 Debug、Info、Warning、Error。
   * `ATTOBOY_LOG_INFO_ENABLE`    – 启用 Info、Warning、Error（默认）。
   * `ATTOBOY_LOG_WARNING_ENABLE` – 启用 Warning、Error。
   * `ATTOBOY_LOG_ERROR_ENABLE`   – 仅启用 Error。
   * `ATTOBOY_LOG_DISABLE`        – 禁用所有日志。

   你需要在某个翻译单元中 `#include "attoboy.h"` 之前，定义上述**一个**宏（或 `ATTOBOY_LOG_DISABLE`），以控制哪些日志函数会被编译进来：

   * 被禁用级别对应的函数调用会在编译期被消除，不产生任何运行时开销。
   * 如果既没有定义任何级别宏，也没有禁用日志，则默认使用 `ATTOBOY_LOG_INFO_ENABLE`。

日志函数内部会使用一个辅助工具，为消息添加时间戳前缀，并在有级别的日志中添加类似 `"DEBUG YYYY-MM-DDTHH:MM:SS.fffZ: ..."` 这样的严重级标签。你无需自己格式化时间戳。

---

#### `atto_main`

**函数签名**

```cpp
extern "C" void atto_main();
```

**概要说明**  
应用程序入口点。定义它以替代 main()。

**参数**

* *(无；你必须在程序中定义该函数)*

**返回值**

* *(由你定义的函数；不会被你直接调用)*

**深入说明**

attoboy 使用一个自定义函数来替代常见的 `int main()` 入口：

* 你需要在程序中**定义** `atto_main()`。
* attoboy 运行时会安排在进程启动时调用 `atto_main()`。
* 由于 `atto_main()` 返回 `void`，如果需要返回特定的退出码，应调用 `Exit(exitCode)`。

这种设计允许库控制初始化和关闭过程，并避免对 CRT 的依赖，从而有助于减小二进制大小。

**示例**

```cpp
using namespace attoboy;

void atto_main() {
  EnableLoggingToConsole();
  LogInfo("Application started; user = ", GetUserName());

  // ... your program logic here ...

  Exit(0);
}
```

*此示例定义了 `atto_main()`，启用了控制台日志，记录了一条启动消息，然后以退出码 0 退出。*

---

#### `Exit(int exitCode = 0)`

**函数签名**

```cpp
void Exit(int exitCode = 0);
```

**概要说明**  
以给定退出码终止进程。

**参数**

* `exitCode` – 返回给操作系统的数值状态码。`0` 通常表示成功；非零值表示不同的错误条件。

**返回值**

* *(无；不会返回到调用者)*

**深入说明**

`Exit()` 会立即终止当前进程：

* 控制流不会返回到调用者。
* 退出码对 shell 脚本、父进程或其它启动你程序的工具是可见的。
* 使用 `Exit(0)` 表示正常完成，使用非零值表示特定的错误状态。

由于 `Exit()` 不会返回，你应当：

* 在调用之前执行必要的清理（关闭文件、刷新日志），或者确保资源由 RAII 对象管理，从而能被自动清理。

**示例**

```cpp
using namespace attoboy;

if (!Path::GetWorkingDirectory().exists()) {
  LogError("Current working directory does not exist; exiting.");
  Exit(1);
}
```

*此示例在基本健全性检查失败时记录错误，并以非零退出码退出。*

---

#### `Sleep(int milliseconds)`

**函数签名**

```cpp
void Sleep(int milliseconds);
```

**概要说明**  
将当前线程暂停指定的毫秒数。

**参数**

* `milliseconds` – 暂停当前线程的时长（毫秒）。应为非负数。

**返回值**

* *(无)*

**深入说明**

`Sleep()` 会使**当前线程**挂起大约指定的时长。这在以下场景中很有用：

* 节流循环。
* 实现简单的重试机制。
* 在不忙等的情况下等待外部进程或资源。

由于系统调度的原因，实际暂停时间可能会略长于请求值。

**示例**

```cpp
using namespace attoboy;

// 带 100 ms 间隔的简单重试循环。
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

*此示例最多重试一个网络请求三次，并在每次尝试之间进行休眠。*

---

#### `String GetEnv(const String &name)`

**函数签名**

```cpp
String GetEnv(const String &name);
```

**概要说明**  
返回环境变量的值；若未设置则返回空字符串。

**参数**

* `name` – 要读取的环境变量名（例如 `"PATH"` 或 `"HOME"`）。

**返回值**

* 若变量存在，则返回包含其值的 `String`；否则返回空 `String`。

**深入说明**

`GetEnv()` 为进程环境提供读访问：

* 如果存在名为 `name` 的变量，返回的 `String` 中包含其值。
* 如果 `name` **不存在**，函数返回空 `String`。

由于环境变量有可能被合法地设置为空字符串，你的代码在需要时应区分“未存在”和“存在但值为空”这两种情况（比如通过检查默认值，或者在程序早期显式调用 `SetEnv()`）。

**示例**

```cpp
using namespace attoboy;

String apiKey = GetEnv("MY_APP_API_KEY");
if (apiKey.isEmpty()) {
  LogWarning("MY_APP_API_KEY is not set; using limited mode.");
} else {
  LogInfo("Using configured API key.");
}
```

*此示例读取一个环境变量，并在其未设置时记录警告。*

---

#### `bool SetEnv(const String &name, const String &value)`

**函数签名**

```cpp
bool SetEnv(const String &name, const String &value);
```

**概要说明**  
设置环境变量。成功返回 true。

**参数**

* `name` – 要设置或更新的环境变量名称。
* `value` – 要赋给变量的值。

**返回值**

* 若环境变量设置成功则返回 `true`；否则返回 `false`。

**深入说明**

`SetEnv()` 会修改**当前进程**的环境，并通常会影响其后启动的子进程。

典型用例如下：

* 覆盖子进程的配置。
* 为期望从环境变量读取配置的工具提供配置。

限制与注意事项：

* 并非所有名称或值在所有系统上都是有效的；极长的名称/值或包含无效字符都有可能导致 `SetEnv()` 失败。
* 修改环境不会对已经在运行的子进程产生追溯影响。

**示例**

```cpp
using namespace attoboy;

if (!SetEnv("MY_APP_MODE", "test")) {
  LogError("Failed to set MY_APP_MODE environment variable.");
} else {
  LogInfo("MY_APP_MODE set to 'test'.");
}
```

*此示例设置一个环境变量，并记录其是否成功。*

---

#### `void *Alloc(int size)`

**函数签名**

```cpp
void *Alloc(int size);
```

**概要说明**  
分配内存。失败时返回 nullptr。

**参数**

* `size` – 要在堆上分配的字节数。通常为非负数。

**返回值**

* 指向至少 `size` 字节内存块的指针；若分配失败则为 `nullptr`。

**深入说明**

`Alloc()` 是 attoboy 的底层堆内存分配函数，类似于 `malloc()`，但由库自身提供。要点如下：

* 返回的指针未初始化；其内容未定义。
* 你必须最终使用 `Free(ptr)` 释放这块内存。
* 如果 `size` 为 `0`，行为取决于具体实现；除非准备好处理 `nullptr` 结果，否则应避免请求长度为 0 的分配。

尽管 attoboy 鼓励使用其更高层的类型（如 `String`、`List`、`Buffer` 等），但在以下特殊场景中仍可使用 `Alloc()`，例如与期望原始指针的 API 交互，或实现自定义数据结构。

**示例**

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

*此示例分配了一个 256 字节的缓冲区；若失败则记录错误并退出，最后在使用完毕后释放缓冲区。*

---

#### `void *Realloc(void *ptr, int size)`

**函数签名**

```cpp
void *Realloc(void *ptr, int size);
```

**概要说明**  
重新分配内存。失败时返回 nullptr。

**参数**

* `ptr` – 之前由 `Alloc()` 或 `Realloc()` 返回的指针，或 `nullptr`。
* `size` – 分配的新字节数。

**返回值**

* 指向调整大小后的内存块的指针，该块可能位于不同地址；若重新分配失败则为 `nullptr`。

**深入说明**

`Realloc()` 用于更改现有分配的大小：

* 如果 `ptr` 为 `nullptr`，则 `Realloc(ptr, size)` 的行为类似于 `Alloc(size)`。
* 如果操作成功：

  * 如需移动内存块，则原内存会被释放。
  * 返回的指针指向调整后大小的内存块。
* 如果操作失败：

  * 返回 `nullptr`。
  * 原始指针 `ptr` 仍然**有效**；你仍需在恰当时机调用 `Free(ptr)`。

在确认重新分配成功之前，应始终将结果赋给一个**临时变量**，以避免丢失原始指针：

```cpp
void *newPtr = Realloc(ptr, newSize);
if (newPtr != nullptr) {
  ptr = newPtr;
}
```

**示例**

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

*此示例安全地调整缓冲区大小，并处理重新分配失败的情况。*

---

#### `void Free(void *ptr)`

**函数签名**

```cpp
void Free(void *ptr);
```

**概要说明**  
释放已分配的内存（对 nullptr 安全）。

**参数**

* `ptr` – 由 `Alloc()` 或 `Realloc()` 返回的指针，或 `nullptr`。

**返回值**

* *(无)*

**深入说明**

`Free()` 会释放先前由 `Alloc()` 或 `Realloc()` 分配的内存：

* 传入 `nullptr` 是明确安全的，不会产生任何效果。
* 调用 `Free(ptr)` 之后，该指针即变为无效；不应再对其进行读写。
* 良好的实践是在释放后将指针置为 `nullptr`，以避免误用。

**示例**

```cpp
using namespace attoboy;

void *buffer = Alloc(512);
// ... use buffer ...
Free(buffer);
buffer = nullptr;
```

*此示例分配了一块内存并在使用完后安全释放。*

---

#### `String GetUserName()`

**函数签名**

```cpp
String GetUserName();
```

**概要说明**  
返回当前用户的登录名。

**参数**

* *(无)*

**返回值**

* 包含登录名的 `String`（例如用于登录操作系统的用户名）。

**深入说明**

`GetUserName()` 会获取运行该进程的用户的**登录标识**。通常是简短的帐户名（如 `"jdoe"`）。可用于：

* 个性化控制台输出。
* 在日志或文件路径中记录用户标识。
* 在数据中标记当前用户。

若无法获取用户名，该函数可能返回空字符串。

**示例**

```cpp
using namespace attoboy;

String user = GetUserName();
LogInfo("Running as user: ", user);
```

*此示例记录当前用户的登录名。*

---

#### `String GetUserDisplayName()`

**函数签名**

```cpp
String GetUserDisplayName();
```

**概要说明**  
返回当前用户的显示名称。

**参数**

* *(无)*

**返回值**

* 包含用户显示名称的 `String`（例如 `"John Doe"`）；若不可用则为空字符串。

**深入说明**

`GetUserDisplayName()` 会返回当前用户帐户的**友好、可读**名称（如果可用）。该名称可能与登录名不同：

* 登录名：`"jdoe"`
* 显示名称：`"John Doe"`

在面向用户的消息、标题或注重可读性的日志中，应优先使用显示名称。

**示例**

```cpp
using namespace attoboy;

String display = GetUserDisplayName();
if (!display.isEmpty()) {
  LogInfo("Welcome, ", display, "!");
}
```

*此示例在友好的问候语中使用显示名称。*

---

#### `int GetProcessId()`

**函数签名**

```cpp
int GetProcessId();
```

**概要说明**  
返回当前进程 ID。

**参数**

* *(无)*

**返回值**

* 由操作系统分配的当前进程的整数标识符。

**深入说明**

**进程 ID（PID）**是在给定时间唯一标识系统中一个正在运行的进程的值。`GetProcessId()` 返回这个值，可用于：

* 当多个进程写入同一日志目标时，为日志行添加标记。
* 调试与诊断（例如在与外部进程监控工具的日志进行关联时）。

**示例**

```cpp
using namespace attoboy;

int pid = GetProcessId();
LogInfo("Process ID: ", pid);
```

*此示例为诊断目的记录当前进程的 ID。*

---

#### `void EnableLoggingToFile(const String &path, bool truncate = false)`

**函数签名**

```cpp
void EnableLoggingToFile(const String &path, bool truncate = false);
```

**概要说明**  
启用文件日志。与控制台日志互斥。

**参数**

* `path` – 日志消息要写入的文件路径。
* `truncate` – 若为 `true`，在写入前会截断（清空）日志文件；若为 `false`，则将消息追加到已有文件末尾。

**返回值**

* *(无)*

**深入说明**

`EnableLoggingToFile()` 将**全局日志输出目标**配置为文件：

* 调用之后，所有后续的 `Log()`、`LogDebug()`、`LogInfo()`、`LogWarning()` 和 `LogError()` 调用都会写入指定文件。
* 在文件日志激活期间，控制台日志会被禁用；两者互斥。
* 若文件无法创建或打开，其行为依赖具体实现；通常建议在切换前先向控制台记录一条消息，或通过测试进行验证。

由于日志是全局的，通常应在程序**早期**调用，例如在 `atto_main()` 中。

**示例**

```cpp
using namespace attoboy;

void atto_main() {
  // 每次运行都从空日志文件开始。
  EnableLoggingToFile("my_app.log", true);

  LogInfo("Logging to my_app.log; PID = ", GetProcessId());
  // ...
}
```

*此示例将日志输出目标切换到文件，并记录一条启动消息。*

---

#### `void EnableLoggingToConsole()`

**函数签名**

```cpp
void EnableLoggingToConsole();
```

**概要说明**  
启用控制台日志（默认）。与文件日志互斥。

**参数**

* *(无)*

**返回值**

* *(无)*

**深入说明**

`EnableLoggingToConsole()` 将**全局日志输出目标**配置为控制台：

* 这通常是库初始化时的默认行为。
* 调用 `EnableLoggingToConsole()` 会将日志从之前配置的日志文件切换回控制台。

在以下场景中使用：

* 希望日志消息直接在终端中可见。
* 之前切换到了文件日志，现在希望恢复到控制台日志。

**示例**

```cpp
using namespace attoboy;

void atto_main() {
  EnableLoggingToConsole();
  LogInfo("Console logging enabled.");

  // 稍后如果希望切换到文件日志：
  EnableLoggingToFile("later.log", false);
  LogInfo("Now logging to later.log.");
}
```

*此示例以控制台日志开始执行，然后在运行过程中切换为文件日志。*

---

### 日志函数

> 所有日志函数都是**可变参数模板**，可以接受任意数量的参数。每个参数会在内部通过 `String` 构造函数被转换为 `String`，因此你可以记录很多类型的值：`String`、`const char *`、`int`、`float`、`bool`、`List`、`Map`、`Set` 等。

#### `template <typename... Args> void Log(const Args &...args)`

**函数签名**

```cpp
template <typename... Args> void Log(const Args &...args);
```

**概要说明**  
将参数打印到当前日志目标，并在末尾追加换行。

**参数**

* `args` – 零个或多个要记录的值。每个值都必须可以构造为 `attoboy::String`。

**返回值**

* *(无)*

**深入说明**

`Log()` 是**基础日志函数**：

* 不会添加严重级别前缀（例如 `"INFO"` 或 `"ERROR"`）。
* 仅仅将所有参数拼接，转换为 `String`，并作为一行写入当前日志输出目标（控制台或文件），最后跟随一个换行。

与宏相关的行为：

* 如果定义了 `ATTOBOY_LOG_DISABLE`，则 `Log()` 会被**完全禁用**。
* 若启用了日志功能，则不论日志级别宏（如 `ATTOBOY_LOG_INFO_ENABLE`）如何配置，对 `Log()` 的调用始终有效。

如果你调用 `Log()` 而不传入任何参数，它仍会写入一行（可能包含时间戳或其他格式信息，取决于实现）。

**示例**

```cpp
using namespace attoboy;

Log("Started task ", 1, " for user ", GetUserName());
// Example output: "Started task 1 for user jdoe"
```

*此示例记录了一条由多种参数类型组成的简单消息。*

---

#### `template <typename... Args> void LogDebug(const Args &...args)`

**函数签名**

```cpp
template <typename... Args> void LogDebug(const Args &...args);
```

**概要说明**  
打印调试消息："DEBUG YYYY-MM-DDTHH:MM:SS.fffZ: ..."

**参数**

* `args` – 零个或多个要包含在调试消息中的值。

**返回值**

* *(无)*

**深入说明**

`LogDebug()` 用于**面向开发者**的诊断信息：

* 消息通常包含 `"DEBUG"` 前缀和时间戳。
* 用于跟踪详细的内部状态、算法决策或变量值，这些信息在开发阶段很有帮助，但在生产环境中可能过于冗杂。

编译期控制：

* 仅当定义了 `ATTOBOY_LOG_DEBUG_ENABLE`（且未禁用日志）时，`LogDebug()` 才会被编译进来。
* 如果只使用较低日志级别宏（如 `ATTOBOY_LOG_INFO_ENABLE`），或定义了 `ATTOBOY_LOG_DISABLE`，则对 `LogDebug()` 的调用会被编译器去除，不产生任何运行时开销。

**示例**

```cpp
using namespace attoboy;

int retryCount = 2;
LogDebug("Connecting to server; retryCount = ", retryCount);
```

*此示例记录了一条调试级消息，通常只在调试构建中可见。*

---

#### `template <typename... Args> void LogInfo(const Args &...args)`

**函数签名**

```cpp
template <typename... Args> void LogInfo(const Args &...args);
```

**概要说明**  
打印信息消息："INFO YYYY-MM-DDTHH:MM:SS.fffZ: ..."

**参数**

* `args` – 零个或多个要包含在信息消息中的值。

**返回值**

* *(无)*

**深入说明**

`LogInfo()` 用于记录**高层次的信息消息**：

* 典型用例：启动消息、配置摘要、重要进度更新。
* 消息前缀为 `"INFO"` 和时间戳。

编译期控制：

* 在以下情况下会编译进来：

  * 定义了 `ATTOBOY_LOG_DEBUG_ENABLE` **或**
  * 定义了 `ATTOBOY_LOG_INFO_ENABLE`
    （且未禁用日志）。
* 当仅定义了 `ATTOBOY_LOG_WARNING_ENABLE` 或 `ATTOBOY_LOG_ERROR_ENABLE`，或定义了 `ATTOBOY_LOG_DISABLE` 时，`LogInfo()` 会被编译器去除。

在默认情况下，如果你既没有定义任何日志级别宏，也没有显式禁用日志，则会自动启用 `ATTOBOY_LOG_INFO_ENABLE`，因此 `LogInfo()` 调用是有效的。

**示例**

```cpp
using namespace attoboy;

LogInfo("Service started at ", DateTime().toString(),
        "; PID = ", GetProcessId());
```

*此示例记录了一条包含时间戳和进程 ID 的服务启动信息。*

---

#### `template <typename... Args> void LogWarning(const Args &...args)`

**函数签名**

```cpp
template <typename... Args> void LogWarning(const Args &...args);
```

**概要说明**  
打印警告消息："WARNING YYYY-MM-DDTHH:MM:SS.fffZ: ..."

**参数**

* `args` – 零个或多个用于描述警告的值。

**返回值**

* *(无)*

**深入说明**

`LogWarning()` 用于那些异常或潜在有问题但尚未致命的状况：

* 示例：配置回退、可恢复错误、性能下降等。
* 消息前缀为 `"WARNING"` 和时间戳。

编译期控制：

* 在以下任意宏被定义时会编译进来：

  * `ATTOBOY_LOG_DEBUG_ENABLE`
  * `ATTOBOY_LOG_INFO_ENABLE`
  * `ATTOBOY_LOG_WARNING_ENABLE`
* 仅在以下情况下被编译器去除：

  * 只定义了 `ATTOBOY_LOG_ERROR_ENABLE` 作为级别宏，或
  * 定义了 `ATTOBOY_LOG_DISABLE`。

**示例**

```cpp
using namespace attoboy;

String configPath("config.json");
Path cfg(configPath);

if (!cfg.exists()) {
  LogWarning("Config file ", configPath, " not found; using defaults.");
}
```

*此示例在找不到配置文件但程序仍可使用默认配置继续运行时记录一条警告。*

---

#### `template <typename... Args> void LogError(const Args &...args)`

**函数签名**

```cpp
template <typename... Args> void LogError(const Args &...args);
```

**概要说明**  
打印错误消息："ERROR YYYY-MM-DDTHH:MM:SS.fffZ: ..."

**参数**

* `args` – 零个或多个用于描述错误的值。

**返回值**

* *(无)*

**深入说明**

`LogError()` 用于记录**错误**和严重问题：

* 当某个操作失败且需要关注或终止程序时使用。
* 消息前缀为 `"ERROR"` 和时间戳。

编译期控制：

* 在所有日志级别配置中，只要**未**定义 `ATTOBOY_LOG_DISABLE`，`LogError()` 都是有效的。
* 换句话说，除非完全禁用日志，否则错误消息始终可以记录。

典型使用模式：

* 使用 `LogError()` 记录错误。
* 如果错误是致命的，可以随后调用 `Exit(exitCode)`。

**示例**

```cpp
using namespace attoboy;

File f(Path("data.bin"));
if (!f.isValid()) {
  LogError("Failed to open data.bin for reading.");
  Exit(2);
}
```

*此示例在无法打开文件时记录一条错误消息，并以非零退出码退出。*