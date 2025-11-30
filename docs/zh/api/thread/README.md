# Thread

## 概览

`attoboy::Thread` 类为操作系统（OS）线程提供了一个轻量级封装。它允许你：

* 启动一个新的执行线程。
* 等待线程结束并获取其返回结果。
* 检查线程是否仍在运行。
* 在万不得已时强制取消线程。
* 安全地共享和比较线程句柄。

目标是在不依赖 C++ 标准库或复杂并发框架的前提下，提供一个简单、最小化的 API，覆盖最常见的线程使用场景。

### 什么是线程？

**线程（thread）** 是进程内部独立的执行路径。可以把进程看作一个正在运行的程序，而线程则是该程序内部可以并行运行的多个「工作者」：

* **单线程** 程序的所有工作都在一条指令序列中完成。
* **多线程** 程序可以同时运行多条指令序列，通常分布在多个 CPU 核心上。

典型的使用线程的原因包括：

* 在后台运行缓慢或阻塞的任务（例如网络请求、文件 I/O）。
* 并行执行大量 CPU 密集型工作。
* 在后台执行工作时保持用户界面响应。

但线程也会引入复杂性：

* 必须保护 **共享状态**（例如使用 `Mutex`），以避免数据竞争。
* 必须谨慎管理 **生命周期**，避免线程在意料之外的时间继续运行。

### 线程函数与 `void *(*func)(void *)` 这一函数签名

`Thread` 会启动执行一个符合如下函数签名的 **函数指针**：

```cpp
void *(*func)(void *)
```

这意味着：

* 线程入口函数接受一个类型为 `void *` 的单一参数。
* 它返回一个 `void *` 作为结果。

在函数内部，你通常会：

1. 将 `void *` 参数转换回具体的指针类型（例如 `String *`、一个小型结构体，或者在不需要数据时为 `nullptr`）。
2. 执行实际工作。
3. 返回一个结果指针（通常在不需要结果时为 `nullptr`）。

你在线程函数中返回的同一个指针值，之后会被提供给调用 `await()` 的一方。

### 线程生命周期与句柄共享

`Thread` 对象本身是一个指向底层 OS 线程的 **句柄**。关键点如下：

* **构造函数会立即创建并启动** 线程。
* **析构函数会“销毁句柄”，但不会终止线程**；底层 OS 线程可以继续运行。
* **拷贝构造函数和赋值运算符会“共享底层线程”**，也就是说多个 `Thread` 对象可以引用同一个正在运行的线程。
* 通常会在其中某一个句柄上调用 **`await()`**，以等待线程结束并获取其结果。

这种设计鼓励以下用法：

* 用 `Thread` 启动工作。
* 在需要控制线程的整个期间保留相应句柄。
* 当必须确认线程已完成时，调用 `await()`。
* 避免依赖对象析构来实现线程终止或同步。

---

## 参考

下面的每一节分别介绍 `attoboy::Thread` 的一个公共构造函数、方法或运算符。对于每个成员，你会看到：

* **Signature** – 头文件中的精确声明。
* **Synopsis** – 原始的一行 Doxygen 注释。
* 对 **Parameters** 和 **Return value** 的描述。
* **In Depth** – 额外细节、注意事项及简短示例。

> **注意：** 所有示例都假定已使用 `using namespace attoboy;`。

---

#### `Thread(void *(*func)(void *), void *arg = nullptr)`

**Signature**

```cpp
Thread(void *(*func)(void *), void *arg = nullptr);
```

**Synopsis**  
Creates and starts a thread running the given function.

**Parameters**

* `func` – 指向线程入口函数的指针，函数签名为 `void *worker(void *arg)`。
* `arg` – 作为参数传递给 `func` 的可选指针。默认为 `nullptr`。

**Return value**

* *(constructor; not applicable)*

**In Depth**

该构造函数会同时 **创建** 并 **启动** 一个新线程。一旦 `Thread` 对象构造完成，底层 OS 线程就会开始执行 `func(arg)`。

要点：

* 线程入口函数必须是一个符合要求函数签名的自由函数或静态成员函数。
* `arg` 指针会原样传递；你需要自行负责对其进行类型转换以及被指向数据的生命周期管理。
* 函数返回一个 `void *`，后续可通过 `await()` 获取。

由于线程会立即启动，你不需要单独的「start」调用。典型过程为：

1. 准备线程所需要的任何数据。
2. 构造 `Thread`。
3. 继续执行其他工作，或在适当时机调用 `await()`。

**Example**

```cpp
// Simple worker function that sleeps and returns a message pointer.
void *worker(void *arg) {
  String *label = (String *)arg;
  Sleep(1000);  // 1 second of simulated work
  return label; // return the same pointer back to the caller
}

// Usage:
String *msg = new String("done");
Thread t(&worker, msg);
```

*本例启动了一个线程，该线程休眠一秒，然后返回一个指向 `String` 结果的指针。*

---

#### `Thread(const Thread &other)`

**Signature**

```cpp
Thread(const Thread &other);
```

**Synopsis**  
Creates a copy (shares the underlying thread).

**Parameters**

* `other` – 要拷贝的现有 `Thread` 句柄。

**Return value**

* *(constructor; not applicable)*

**In Depth**

拷贝构造函数会创建一个新的 `Thread` 对象，该对象 **引用与 `other` 相同的底层线程**。不会创建新的 OS 线程。

这在以下场景中很有用：

* 希望将线程句柄传递给其他函数或组件。
* 需要多个对象都能在同一线程上调用 `await()`、`isRunning()` 或 `cancel()`。

共享仅仅体现在句柄管理上；不会复制正在执行的工作。

**Example**

```cpp
void *worker(void *arg) {
  Sleep(500);
  return nullptr;
}

Thread original(&worker, nullptr);
Thread copy(original);  // shares the same underlying thread
```

*本例创建了第二个指向同一运行中线程的句柄。*

---

#### `~Thread()`

**Signature**

```cpp
~Thread();
```

**Synopsis**  
Destroys the handle (does not terminate the thread).

**Parameters**

* *(none)*

**Return value**

* *(destructor; not applicable)*

**In Depth**

析构函数会释放与线程 **句柄** 相关的资源，但 **不会停止底层 OS 线程**。如果线程仍在运行，即便所有引用它的 `Thread` 对象都已销毁，它仍会继续运行。

这有几个重要影响：

* **不要** 依靠析构来 join 或取消线程。
* 如需等待线程结束，应调用 `await()`。
* 如果确实必须提前终止它，可以调用 `cancel()`（但务必谨慎）。

在实际使用中，你通常会在线程不再需要前，至少保持一个 `Thread` 对象存活，并通过显式协作来协调关闭。

**Example**

```cpp
void *worker(void *arg) {
  Sleep(1000);
  return nullptr;
}

{
  Thread t(&worker, nullptr);
  // t goes out of scope here; the underlying thread may still be running.
}
// The OS thread continues until it finishes on its own.
```

*本例表明，销毁 `Thread` 句柄并不会自动停止线程。*

---

#### `Thread &operator=(const Thread &other)`

**Signature**

```cpp
Thread &operator=(const Thread &other);
```

**Synopsis**  
Assigns another thread (shares the underlying thread).

**Parameters**

* `other` – 要赋值来源的现有 `Thread` 句柄。

**Return value**

* 对 `*this` 的引用，以便进行链式赋值。

**In Depth**

拷贝赋值运算符会使 `*this` 引用与 `other` 相同的底层线程。`*this` 之前与其他线程句柄之间的关联会被释放。

与拷贝构造相同：

* 不会创建新的 OS 线程。
* 不会重新启动或复制底层线程；两个对象只是同一线程的不同句柄。

**Example**

```cpp
void *worker(void *arg) {
  Sleep(100);
  return nullptr;
}

Thread a(&worker, nullptr);
Thread b(&worker, nullptr);

b = a;  // now both a and b refer to the same thread created for a
```

*本例将一个 `Thread` 句柄重新赋值为共享另一个句柄所管理的线程。*

---

#### `void *await()`

**Signature**

```cpp
void *await();
```

**Synopsis**  
Waits for the thread to complete and returns its result.

**Parameters**

* *(none)*

**Return value**

* 线程入口函数返回的 `void *` 值，或在出错时为空指针（依赖具体实现）。

**In Depth**

`await()` 是 `Thread` 的主要同步调用：

* 它会 **阻塞调用方**，直到底层线程结束。
* 一旦线程结束，`await()` 返回线程函数所返回的指针。
* 在同一线程句柄上再次调用 `await()` 通常是被允许的，但可能返回相同指针，或具有依赖实现的行为；最佳实践是对每个逻辑上的「join」只调用一次 `await()`。

你需要负责：

* 解释返回的指针（转换为正确的类型）。
* 如果线程函数动态分配了对象，管理该对象的生命周期。

**Example**

```cpp
void *worker(void *arg) {
  String *label = (String *)arg;
  // Do some work...
  return label;
}

// Start the thread
String *msg = new String("done");
Thread t(&worker, msg);

// Wait for completion and retrieve result
void *resultPtr = t.await();
String *result = (String *)resultPtr;
// result now points to "done"
```

*本例等待线程结束，并获取工作函数返回的指针结果。*

---

#### `void cancel()`

**Signature**

```cpp
void cancel();
```

**Synopsis**  
Forcefully terminates the thread (use sparingly).

**Parameters**

* *(none)*

**Return value**

* *(none)*

**In Depth**

`cancel()` 尝试 **强制停止** 底层线程，应将其视为 **最后手段**：

* 它可能阻止线程执行清理代码。
* 它可能使共享数据处于不一致状态。
* 线程拥有的资源（锁、文件句柄等）可能不会被正确释放。

在可能的情况下，应优先采用协作式关闭：

* 使用共享标志（必要时由 `Mutex` 保护），让线程周期性检查。
* 让线程正常退出其函数。
* 随后调用 `await()` 来 join 线程。

仅在别无更好选择且完全理解风险时才使用 `cancel()`。

**Example**

```cpp
void *worker(void *arg) {
  // Potentially long-running or blocking work...
  Sleep(10000);
  return nullptr;
}

Thread t(&worker, nullptr);

// In some error or shutdown condition:
t.cancel();  // force termination (use with care)
```

*本例在紧急情况下取消一个长时间运行的线程。*

---

#### `bool isRunning() const`

**Signature**

```cpp
bool isRunning() const;
```

**Synopsis**  
Returns true if the thread is still running.

**Parameters**

* *(none)*

**Return value**

* 若底层线程当前正在执行则为 `true`；如果已经结束或无法启动则为 `false`。

**In Depth**

`isRunning()` 提供了一种简单方式来检查线程状态：

* 构造后立即调用，如果线程成功启动，通常返回 `true`。
* 一旦线程函数返回，`isRunning()` 会变为 `false`。
* 从未启动或启动失败的线程也会被视为「未运行」。

你可以在轮询循环中使用它，或者基于此来决定是否可以安全地取消、等待或重用句柄（不过你仍应注意检查与操作之间可能存在竞争条件）。

**Example**

```cpp
void *worker(void *arg) {
  Sleep(500);
  return nullptr;
}

Thread t(&worker, nullptr);

while (t.isRunning()) {
  // Do something else while we wait
  Sleep(50);
}

void *result = t.await();  // safe: thread is no longer running
```

*本例通过轮询 `isRunning()` 在后台线程完成的同时处理其他工作。*

---

#### `bool equals(const Thread &other) const`

**Signature**

```cpp
bool equals(const Thread &other) const;
```

**Synopsis**  
Returns true if this thread equals the other.

**Parameters**

* `other` – 要比较的另一个 `Thread`。

**Return value**

* 如果两个对象引用同一个底层线程则为 `true`；否则为 `false`。

**In Depth**

`equals()` 用于测试两个 `Thread` 句柄是否表示同一个底层 OS 线程。这里比较的是 **身份** 而非运行状态：

* 通过拷贝或赋值创建的两个句柄引用的是同一线程，因此相等。
* 即便两个独立线程执行相同函数，它们也不相等。

在需要验证两个句柄是否是同一线程的别名时，可使用此函数。

**Example**

```cpp
void *worker(void *arg) {
  Sleep(100);
  return nullptr;
}

Thread a(&worker, nullptr);
Thread b(a);  // shares thread

bool same = a.equals(b);  // true
```

*本例表明，根据 `equals()`，拷贝得到的句柄与原始句柄相等。*

---

#### `bool operator==(const Thread &other) const`

**Signature**

```cpp
bool operator==(const Thread &other) const;
```

**Synopsis**  
Returns true if this thread equals the other.

**Parameters**

* `other` – 要比较的另一个 `Thread`。

**Return value**

* 如果两个句柄表示同一个底层线程则为 `true`；否则为 `false`。

**In Depth**

此运算符是 `equals(other)` 的惯用简写形式：

```cpp
if (a == b) { ... }
```

它比较的是线程的身份，而不是它们是否都在运行或具有相同行为。

**Example**

```cpp
void *worker(void *arg) {
  return nullptr;
}

Thread a(&worker, nullptr);
Thread b(a);       // same underlying thread
Thread c(&worker, nullptr);  // different thread

bool ab = (a == b); // true
bool ac = (a == c); // false
```

*本例使用相等运算符检查两个句柄是否表示同一线程。*

---

#### `bool operator!=(const Thread &other) const`

**Signature**

```cpp
bool operator!=(const Thread &other) const;
```

**Synopsis**  
Returns true if this thread does not equal the other.

**Parameters**

* `other` – 要比较的另一个 `Thread`。

**Return value**

* 如果两个句柄引用不同的底层线程则为 `true`；若引用同一线程则为 `false`。

**In Depth**

此运算符只是 `operator==` 的逻辑非。直接使用 `!=` 来表达不等通常更具可读性。

**Example**

```cpp
void *worker(void *arg) {
  return nullptr;
}

Thread a(&worker, nullptr);
Thread b(&worker, nullptr);

if (a != b) {
  // a and b manage different OS threads
}
```

*本例在两个线程句柄引用不同线程时分支执行。*