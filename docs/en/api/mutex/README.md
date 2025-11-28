# Mutex

## Overview

The `attoboy::Mutex` class provides a simple, low-level synchronization primitive to coordinate access to shared data between threads. It is the natural companion to `attoboy::Thread`:

* `Thread` creates and runs concurrent workers.
* `Mutex` ensures that only one thread at a time can access specific critical sections of code or shared resources.

By wrapping the underlying operating system (OS) mutex, `Mutex` offers:

* A compact, dependency-free interface (no C++ standard library).
* Handle sharing across multiple parts of your program.
* Basic operations: lock, unlock, and non-blocking try-lock.

### What is a Mutex?

A **mutex** (mutual exclusion lock) is a synchronization object that allows **only one thread at a time** to “own” it. Conceptually:

* When a thread calls `lock()`:

  * If the mutex is **unlocked**, it becomes locked and the thread continues.
  * If the mutex is **already locked** by another thread, the calling thread **blocks** (waits) until the mutex is unlocked.
* When the owning thread calls `unlock()`, another waiting thread (if any) can acquire the lock.

You typically use a mutex when you have **shared state** that can be read or modified by multiple threads:

* Shared counters or statistics.
* Shared buffers or queues.
* Shared configuration or caches.

Without a mutex (or some other synchronization mechanism), simultaneous access can lead to:

* **Data races** – two threads writing or reading while another writes, causing undefined behavior.
* **Inconsistent state** – partial updates seen by other threads.
* **Hard-to-reproduce bugs** – errors that only appear intermittently under load.

### Mutex Handles and Sharing

Like `Thread`, the `Mutex` class separates:

* The underlying OS-level synchronization object.
* The lightweight **handle** (`Mutex` object) you hold in your code.

Important properties:

* The **default constructor** creates a new mutex.
* The **copy constructor** and **assignment operator** create **handles that share the underlying mutex**.

This means you can:

* Pass `Mutex` objects by value.
* Store them in other structures.
* Use any copy as if it were the original; all refer to the same lock.

### Locking Discipline and Best Practices

Because `Mutex` is a low-level primitive, you are responsible for:

* Calling `lock()` before entering a critical section.
* Calling `unlock()` in all execution paths leaving that critical section (including early returns and error branches).
* Avoiding deadlocks by always acquiring multiple mutexes in a consistent order, if you use more than one.

You should **assume the mutex is not recursive** (reentrant). That is:

* If a thread calls `lock()` twice in a row on the same mutex without unlocking in between, it may deadlock.

A common pattern is:

```cpp
mutex.lock();
// critical section: access shared data
mutex.unlock();
```

In more complex logic, you must be careful that every `lock()` is matched by an `unlock()` even when errors occur.

---

## Reference

Each entry below covers one public constructor, method, or operator of `attoboy::Mutex`. For each, you will find:

* **Signature** – the exact declaration from the header.
* **Synopsis** – the original one-line Doxygen comment.
* **Parameters** and **Return value** descriptions.
* **In Depth** – additional details, caveats, and a short example.

> **Note:** All examples assume `using namespace attoboy;`.

---

### `Mutex()`

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

The default constructor initializes a new mutex that starts in the **unlocked** state. After construction:

* The mutex can be safely passed to other threads (for example, via pointers or references).
* Any thread can call `lock()`, `unlock()`, or `tryLock()` on it.

You typically create one mutex for each shared resource or group of related resources that must be protected together.

**Example**

```cpp
Mutex m;  // newly created, initially unlocked
```

*This example creates a mutex that can be used to guard shared data between threads.*

---

### `Mutex(const Mutex &other)`

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

The copy constructor does **not** create a new OS-level mutex. Instead, it creates another handle that **shares the same underlying mutex** as `other`.

This means:

* Locking or unlocking through any handle affects the same lock.
* You can safely store and pass `Mutex` objects around without worrying about accidentally creating separate, unrelated locks.

This is useful when:

* You want to bundle a mutex into another object by value.
* You want to give multiple components access to the same lock without manually sharing pointers.

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

*This example shows copying a mutex handle and using the copy to protect a critical section; both handles refer to the same underlying lock.*

---

### `~Mutex()`

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

The destructor releases resources associated with the **handle**. The exact behavior with respect to the underlying OS mutex depends on how many handles share it, but in all cases:

* You must ensure that the mutex is **not locked** by any thread at the moment you logically “discard” it.
* Destroying a handle while another thread holds the lock or is waiting on it leads to undefined behavior.

A good rule of thumb is:

* Ensure that all threads using a mutex have finished (or will no longer call `lock()`/`unlock()`) before the last handle is destroyed.

**Example**

```cpp
void useMutex() {
  Mutex m;
  m.lock();
  // critical section
  m.unlock();
} // m is destroyed here; mutex must not be used afterwards
```

*This example creates a mutex in a local scope, uses it to protect a section of code, and then destroys it when leaving the scope.*

---

### `Mutex &operator=(const Mutex &other)`

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

The copy assignment operator makes `*this` refer to the **same underlying mutex** as `other`. Any previous association of `*this` with a different mutex is released.

After assignment:

* Locking and unlocking through `*this` affects the same lock as operations through `other`.
* No new OS-level mutex is created.

Use this when you want to redirect an existing handle to protect a different shared resource, or when wiring up structures that hold `Mutex` members.

**Example**

```cpp
Mutex a;
Mutex b;

// Later, decide that b should use the same mutex as a:
b = a;  // b now shares a's underlying mutex
```

*This example assigns one mutex handle to another so they both refer to the same underlying lock.*

---

### `void lock()`

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

`lock()` is the primary way to enter a **critical section**:

* If the mutex is currently unlocked, `lock()` immediately locks it and returns.
* If another thread already holds the lock, `lock()` **blocks** the calling thread until the mutex becomes available.

Once a thread has successfully called `lock()`, it is responsible for calling `unlock()` when leaving the critical section. Failing to do so will:

* Leave the mutex permanently locked.
* Cause other threads calling `lock()` or `tryLock()` to block or fail indefinitely.

You should avoid calling `lock()` twice in a row on the same mutex from the same thread without unlocking in between; this can lead to deadlocks if the mutex is non-recursive.

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

*This example uses `lock()` to protect a shared counter increment from concurrent access in two worker threads.*

---

### `void unlock()`

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

`unlock()` releases ownership of the mutex:

* It must be called by the thread that previously acquired the mutex with `lock()` or `tryLock()`.
* After `unlock()`, other threads waiting in `lock()` may proceed, and `tryLock()` may succeed.

Calling `unlock()` on a mutex that is not locked by the current thread results in undefined behavior. Always structure code so that every `lock()` is paired with an `unlock()` in all possible control paths.

A common pattern is:

```cpp
m.lock();
// critical section
m.unlock();
```

In real programs, you must also handle early returns and error conditions:

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

*This example locks a mutex before updating shared state and unlocks it afterwards to allow other threads to proceed.*

---

### `bool tryLock()`

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

`tryLock()` is a **non-blocking** alternative to `lock()`:

* If the mutex is free, it locks the mutex and returns `true`.
* If the mutex is already locked, it returns `false` immediately, without blocking.

This is useful in scenarios where:

* You want to do “best effort” work without waiting.
* You are building lock-free-like behavior with fallbacks.
* You want to avoid potential deadlocks by backing off when the lock is not available.

If `tryLock()` returns `true`, you **must** eventually call `unlock()` from the same thread to release the mutex.

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

*This example attempts to update a shared counter only if the mutex can be acquired without blocking; otherwise, it skips the update or does something else.*

