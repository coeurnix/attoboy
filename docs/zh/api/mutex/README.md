# Mutex

## 概览

`attoboy::Mutex` 类提供了一个简单的、低层级的同步原语，用于在多线程之间协调对共享数据的访问。它是 `attoboy::Thread` 的天然搭档：

* `Thread` 用于创建并运行并发的工作线程。
* `Mutex` 用于确保同一时间只有一个线程可以访问特定的代码临界区或共享资源。

通过封装底层操作系统（OS）的互斥量，`Mutex` 提供了：

* 一个紧凑、无依赖的接口（无需 C++ 标准库）。
* 在程序多个部分之间共享句柄的能力。
* 基本操作：加锁、解锁，以及非阻塞的尝试加锁。

### 什么是 Mutex？

**Mutex**（互斥锁）是一种同步对象，它允许在任意时刻**仅有一个线程**“拥有”它。概念上：

* 当某个线程调用 `lock()` 时：

  * 如果互斥锁当前是**未锁定**状态，它会被锁定，该线程继续执行。
  * 如果互斥锁**已经被其他线程锁定**，调用线程将被**阻塞**（等待），直到互斥锁被解锁。
* 当持有锁的线程调用 `unlock()` 时，其他正在等待的线程（如果有）就可以获取该锁。

通常在存在可被多个线程读取或修改的**共享状态**时使用互斥锁，例如：

* 共享计数器或统计数据。
* 共享缓冲区或队列。
* 共享配置或缓存。

如果没有互斥锁（或其他同步机制），同时访问可能导致：

* **数据竞争（data race）**——两个线程在另一个线程写入时进行读写操作，从而造成未定义行为。
* **状态不一致**——其他线程看到部分更新的状态。
* **难以复现的 bug**——只在高负载或特定时间点偶发出现的问题。

### 互斥锁句柄与共享

与 `Thread` 类似，`Mutex` 类将以下两者分离：

* 底层的 OS 级同步对象。
* 你在代码中持有的轻量级**句柄**（`Mutex` 对象）。

重要特性：

* **默认构造函数**会创建一个新的互斥锁。
* **拷贝构造函数**和**赋值运算符**会创建**共享同一底层互斥锁的句柄**。

这意味着你可以：

* 按值传递 `Mutex` 对象。
* 将它们存储在其他结构中。
* 使用任意一个拷贝，就像使用原始对象一样；它们都指向同一把锁。

### 加锁规约与最佳实践

由于 `Mutex` 是一个低层级原语，你需要自行负责：

* 在进入临界区之前调用 `lock()`。
* 在离开该临界区的所有执行路径中调用 `unlock()`（包括提前返回和错误分支）。
* 在使用多个互斥锁时，通过始终按一致的顺序获取锁来避免死锁。

你应当**假定互斥锁是不可重入的**（非递归）。也就是说：

* 如果同一线程在未解锁的情况下连续两次对同一个互斥锁调用 `lock()`，可能会导致死锁。

一个常见模式是：

```cpp
mutex.lock();
// critical section: access shared data
mutex.unlock();
```

在更复杂的逻辑中，你必须确保每个 `lock()` 都在任何情况下与一个 `unlock()` 成对出现，即便发生错误时也不例外。

---

## 参考说明

下面每一条目分别介绍 `attoboy::Mutex` 的一个公共构造函数、方法或运算符。对每个条目，你将看到：

* **Signature（签名）**——头文件中的精确声明。
* **Synopsis（摘要）**——原始的一行 Doxygen 注释。
* **Parameters（参数）**和 **Return value（返回值）**说明。
* **In Depth（深入说明）**——额外细节、注意事项以及简短示例。

> **注意：** 所有示例均假定使用 `using namespace attoboy;`。

---

#### `Mutex()`

**Signature**

```cpp
Mutex();
```

**Synopsis**  
Creates a mutex.

**Parameters**

* *(none)*

**Return value**

* *(constructor; not applicable)*

**In Depth**

默认构造函数会初始化一个新的互斥锁，其初始状态为**未锁定**。构造完成后：

* 可以安全地将该互斥锁传递给其他线程（例如通过指针或引用）。
* 任何线程都可以在其上调用 `lock()`、`unlock()` 或 `tryLock()`。

通常，会为每一个需要保护的共享资源，或一组必须一起保护的相关资源，创建一个互斥锁。

**Example**

```cpp
Mutex m;  // newly created, initially unlocked
```

*此示例创建了一个互斥锁，可用于在多个线程之间保护共享数据。*

---

#### `Mutex(const Mutex &other)`

**Signature**

```cpp
Mutex(const Mutex &other);
```

**Synopsis**  
Creates a copy (shares the underlying mutex).

**Parameters**

* `other` – Existing `Mutex` handle to copy.

**Return value**

* *(constructor; not applicable)*

**In Depth**

拷贝构造函数**不会**创建一个新的 OS 级互斥锁。它会创建一个新的句柄，该句柄与 `other` **共享同一个底层互斥锁**。

这意味着：

* 通过任意句柄进行的加锁或解锁操作，都会作用在同一把锁上。
* 你可以安全地存储和传递 `Mutex` 对象，而无需担心意外创建多个互不相关的锁。

这在以下场景中非常有用：

* 希望通过值语义将互斥锁封装进另一个对象。
* 希望让多个组件访问同一把锁，而不需要手动共享指针。

**Example**

```cpp
Mutex globalMutex;

void useCopy() {
  Mutex localCopy(globalMutex);  // shares the same underlying mutex
  localCopy.lock();
  // protected section
  localCopy.unlock();
}
```

*此示例展示了如何拷贝一个互斥锁句柄并用该拷贝保护临界区；两个句柄都引用同一底层锁。*

---

#### `~Mutex()`

**Signature**

```cpp
~Mutex();
```

**Synopsis**  
Destroys the mutex handle.

**Parameters**

* *(none)*

**Return value**

* *(destructor; not applicable)*

**In Depth**

析构函数释放与该**句柄**相关联的资源。底层 OS 互斥锁的具体行为取决于当前有多少句柄共享该互斥锁，但在所有情况下：

* 你必须确保在逻辑上“丢弃”该互斥锁时，**没有任何线程**持有该锁。
* 当另一个线程仍然持有锁或正在等待该锁时销毁句柄，会导致未定义行为。

一个实用的经验法则是：

* 确保所有使用该互斥锁的线程都已经结束（或者不会再调用 `lock()`/`unlock()`），然后再销毁最后一个句柄。

**Example**

```cpp
void useMutex() {
  Mutex m;
  m.lock();
  // critical section
  m.unlock();
} // m is destroyed here; mutex must not be used afterwards
```

*此示例在局部作用域中创建互斥锁，用它保护一段代码，在离开作用域时销毁该互斥锁。*

---

#### `Mutex &operator=(const Mutex &other)`

**Signature**

```cpp
Mutex &operator=(const Mutex &other);
```

**Synopsis**  
Assigns another mutex (shares the underlying mutex).

**Parameters**

* `other` – Existing `Mutex` handle to assign from.

**Return value**

* Reference to `*this`, enabling assignment chaining.

**In Depth**

拷贝赋值运算符会使 `*this` 指向与 `other` **相同的底层互斥锁**。`*this` 之前与其它互斥锁的关联将被释放。

赋值之后：

* 通过 `*this` 进行的加锁或解锁，将与通过 `other` 的操作作用在同一把锁上。
* 不会创建新的 OS 级互斥锁。

可在以下情况使用该操作：

* 需要让现有句柄开始保护另一个共享资源。
* 在连接或初始化包含 `Mutex` 成员的结构体或类时。

**Example**

```cpp
Mutex a;
Mutex b;

// Later, decide that b should use the same mutex as a:
b = a;  // b now shares a's underlying mutex
```

*此示例将一个互斥锁句柄赋值给另一个，使二者都引用同一底层锁。*

---

#### `void lock()`

**Signature**

```cpp
void lock();
```

**Synopsis**  
Locks the mutex, blocking until available.

**Parameters**

* *(none)*

**Return value**

* *(none)*

**In Depth**

`lock()` 是进入**临界区**的主要方式：

* 如果互斥锁当前未被锁定，`lock()` 会立即将其锁定并返回。
* 如果另一个线程已持有该锁，`lock()` 会**阻塞**调用线程，直到互斥锁可用。

一旦线程成功调用 `lock()`，它就有责任在离开临界区时调用 `unlock()`。否则将会：

* 使互斥锁永久处于锁定状态。
* 导致其他线程对 `lock()` 或 `tryLock()` 的调用无限期阻塞或失败。

你应避免在同一线程中，对同一互斥锁在未解锁的情况下连续两次调用 `lock()`；在互斥锁非递归的情况下，这会导致死锁。

**Example**

```cpp
Mutex m;
int sharedCounter = 0;

// Worker function that increments a shared counter safely
void *worker(void *arg) {
  for (int i = 0; i < 1000; ++i) {
    m.lock();
    sharedCounter += 1;  // critical section
    m.unlock();
  }
  return nullptr;
}

// Usage:
Thread t1(&worker, nullptr);
Thread t2(&worker, nullptr);
```

*此示例使用 `lock()` 来在两个工作线程中保护对共享计数器的自增操作，避免并发访问问题。*

---

#### `void unlock()`

**Signature**

```cpp
void unlock();
```

**Synopsis**  
Unlocks the mutex.

**Parameters**

* *(none)*

**Return value**

* *(none)*

**In Depth**

`unlock()` 用于释放对互斥锁的所有权：

* 它必须由先前通过 `lock()` 或 `tryLock()` 获取该互斥锁的线程调用。
* 在执行 `unlock()` 之后，等待在 `lock()` 调用中的其他线程可以继续执行，而 `tryLock()` 也有可能成功。

在当前线程并未持有该锁的情况下调用 `unlock()` 会导致未定义行为。始终要结构化你的代码，确保每一次 `lock()` 在所有可能的控制路径中都有对应的 `unlock()`。

一个常见模式是：

```cpp
m.lock();
// critical section
m.unlock();
```

在实际程序中，还必须处理提前返回和错误情况：

```cpp
m.lock();
// ...
if (someError) {
  m.unlock();
  return;  // ensure unlock happens before returning
}
// ...
m.unlock();
```

**Example**

```cpp
Mutex m;
int sharedValue = 0;

void *worker(void *arg) {
  m.lock();
  sharedValue = 42;  // critical section: write shared data
  m.unlock();
  return nullptr;
}
```

*此示例在更新共享状态前加锁，更新后解锁，以便其他线程可以继续执行。*

---

#### `bool tryLock()`

**Signature**

```cpp
bool tryLock();
```

**Synopsis**  
Tries to lock without blocking. Returns true if acquired.

**Parameters**

* *(none)*

**Return value**

* `true` if the mutex was successfully locked by the calling thread.
* `false` if the mutex was already locked by another thread and could not be acquired immediately.

**In Depth**

`tryLock()` 是 `lock()` 的一种**非阻塞**替代方案：

* 如果互斥锁空闲，它会将互斥锁锁定并返回 `true`。
* 如果互斥锁已经被锁定，它会立即返回 `false`，而不会阻塞。

这在以下场景中很有用：

* 你希望“尽力而为”地完成工作，而不希望等待。
* 你正在构建类似无锁（lock-free）的行为，并需要某些回退逻辑。
* 你希望在锁不可用时退让，以避免潜在的死锁。

如果 `tryLock()` 返回 `true`，你**必须**最终在同一线程中调用 `unlock()` 来释放互斥锁。

**Example**

```cpp
Mutex m;
int sharedCounter = 0;

void *worker(void *arg) {
  // Attempt to update the shared counter without blocking
  if (m.tryLock()) {
    sharedCounter += 1;  // critical section
    m.unlock();
  } else {
    // Could not acquire the lock immediately; skip or do alternative work
  }
  return nullptr;
}
```

*此示例仅在可以无阻塞地获取互斥锁时才更新共享计数器；否则跳过更新或执行其他工作。*