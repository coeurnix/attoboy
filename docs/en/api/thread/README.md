# `attoboy::Thread` User Guide

## Overview

The `attoboy::Thread` class provides a lightweight wrapper around an operating system (OS) thread. It lets you:

* Start a new thread of execution.
* Wait for the thread to finish and retrieve its result.
* Check whether the thread is still running.
* Cancel the thread forcefully (when absolutely necessary).
* Share and compare thread handles safely.

The goal is to expose a simple, minimal API that covers the most common threading needs without depending on the C++ standard library or complicated concurrency frameworks.

### What is a Thread?

A **thread** is an independent path of execution within a process. You can think of a process as a running program, and threads as separate “workers” inside that program that can run in parallel:

* A **single-threaded** program does all work in a single sequence of instructions.
* A **multi-threaded** program can run several sequences at once, often on multiple CPU cores.

Typical reasons to use threads include:

* Running slow or blocking tasks (e.g., network requests, file I/O) in the background.
* Performing CPU-heavy work in parallel.
* Keeping a user interface responsive while work happens in the background.

However, threads also introduce complexity:

* **Shared state** must be protected (for example, with `Mutex`) to avoid data races.
* **Lifetime** must be managed carefully, so threads are not left running unexpectedly.

### Thread Functions and the `void *(*func)(void *)` Signature

`Thread` starts executing a **function pointer** that matches this signature:

```cpp
void *(*func)(void *)
```

This means:

* The thread entry function takes a single argument of type `void *`.
* It returns a `void *` as a result.

Inside the function, you typically:

1. Cast the `void *` argument back to a specific pointer type (for example, a `String *`, a small struct, or `nullptr` if no data is needed).
2. Do the work.
3. Return a result pointer (often `nullptr` if no result is needed).

The same pointer value you return is later provided to the caller of `await()`.

### Thread Lifetimes and Handle Sharing

The `Thread` object itself is a **handle** to an underlying OS thread. Important points:

* The **constructor creates and starts** the thread immediately.
* The **destructor “destroys the handle” but does not terminate the thread**; the underlying OS thread can continue running.
* The **copy constructor and assignment operator “share the underlying thread”**, which means multiple `Thread` objects can refer to the same running thread.
* You typically call **`await()`** on one of the handles to wait until the thread finishes and retrieve its result.

This design encourages you to:

* Start work with `Thread`.
* Keep the handle(s) around as long as you need control.
* Call `await()` when you must know the thread has completed.
* Avoid relying on destruction for termination or synchronization.

---

## Reference

Each entry below covers one public constructor, method, or operator of `attoboy::Thread`. For each, you will find:

* **Signature** – the exact declaration from the header.
* **Synopsis** – the original one-line Doxygen comment.
* **Parameters** and **Return value** descriptions.
* **In Depth** – additional details, caveats, and a short example.

> **Note:** All examples assume `using namespace attoboy;`.

---

### `Thread(void *(*func)(void *), void *arg = nullptr)`

**Signature**

```cpp
Thread(void *(*func)(void *), void *arg = nullptr);
```

**Synopsis**
Creates and starts a thread running the given function.

**Parameters**

* `func` – Pointer to the thread entry function, with signature `void *worker(void *arg)`.
* `arg` – Optional pointer passed as the argument to `func`. Defaults to `nullptr`.

**Return value**

* *(constructor; not applicable)*

**In Depth**

This constructor both **creates** and **starts** a new thread. As soon as the `Thread` object is constructed, the underlying OS thread begins executing `func(arg)`.

Key points:

* The thread entry function must be a free function or static member function that matches the required signature.
* The `arg` pointer is passed through unchanged; you are responsible for any casting and lifetime management of the pointed-to data.
* The function returns a `void *` which is later available via `await()`.

Because the thread starts immediately, you do not need a separate “start” call. You typically:

1. Prepare any data the thread needs.
2. Construct the `Thread`.
3. Continue doing other work, or eventually call `await()`.

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

*This example starts a thread that sleeps for a second and then returns a pointer to a `String` result.*

---

### `Thread(const Thread &other)`

**Signature**

```cpp
Thread(const Thread &other);
```

**Synopsis**
Creates a copy (shares the underlying thread).

**Parameters**

* `other` – Existing `Thread` handle to copy.

**Return value**

* *(constructor; not applicable)*

**In Depth**

The copy constructor creates a new `Thread` object that **refers to the same underlying thread** as `other`. No new OS thread is created.

This is useful when:

* You want to pass a thread handle to another function or component.
* You need multiple objects that can all call `await()`, `isRunning()`, or `cancel()` on the same thread.

Sharing is purely about handle management; it does not duplicate the work being executed.

**Example**

```cpp
void *worker(void *arg) {
  Sleep(500);
  return nullptr;
}

Thread original(&worker, nullptr);
Thread copy(original);  // shares the same underlying thread
```

*This example creates a second handle to the same running thread.*

---

### `~Thread()`

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

The destructor releases the resources associated with the thread **handle**, but it **does not stop the underlying OS thread**. If the thread is still running, it continues to run even after all `Thread` objects referring to it are destroyed.

This has important implications:

* Do **not** rely on destruction to join or cancel the thread.
* If you need to wait for the thread to finish, call `await()`.
* If you truly must terminate it early, call `cancel()` (with caution).

In practice, you usually keep at least one `Thread` object alive until the thread is no longer needed, and coordinate shutdown explicitly.

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

*This example shows that destroying the `Thread` handle does not automatically stop the thread.*

---

### `Thread &operator=(const Thread &other)`

**Signature**

```cpp
Thread &operator=(const Thread &other);
```

**Synopsis**
Assigns another thread (shares the underlying thread).

**Parameters**

* `other` – Existing `Thread` handle to assign from.

**Return value**

* Reference to `*this`, enabling assignment chaining.

**In Depth**

The copy assignment operator makes `*this` refer to the same underlying thread as `other`. Any previous association of `*this` with another thread handle is released.

Like the copy constructor:

* No new OS thread is created.
* The underlying thread is not restarted or duplicated; both objects are just handles to the same thread.

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

*This example reassigns one `Thread` handle to share the thread managed by another.*

---

### `void *await()`

**Signature**

```cpp
void *await();
```

**Synopsis**
Waits for the thread to complete and returns its result.

**Parameters**

* *(none)*

**Return value**

* The `void *` value returned by the thread entry function, or a null pointer on error (implementation-dependent).

**In Depth**

`await()` is the primary synchronization call for `Thread`:

* It **blocks the caller** until the underlying thread finishes.
* Once the thread has completed, `await()` returns the pointer that the thread function returned.
* Subsequent calls to `await()` on the same thread handle are typically allowed but may return the same pointer or behave implementation-dependently; best practice is to call `await()` exactly once per logical “join”.

You are responsible for:

* Interpreting the returned pointer (casting to the correct type).
* Managing the lifetime of any pointed-to object if your thread function dynamically allocated it.

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

*This example waits for a thread to complete and retrieves a pointer result returned by the worker function.*

---

### `void cancel()`

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

`cancel()` attempts to **forcefully stop** the underlying thread. This should be considered a **last resort**:

* It may prevent the thread from running cleanup code.
* It may leave shared data in an inconsistent state.
* Resources owned by the thread (locks, file handles, etc.) may not be released properly.

Whenever possible, prefer cooperative shutdown:

* Use shared flags (guarded by `Mutex` if necessary) that the thread checks periodically.
* Let the thread exit its function normally.
* Then call `await()` to join it.

Only use `cancel()` when you have no better option and you fully understand the risks.

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

*This example shows cancelling a long-running thread in an emergency situation.*

---

### `bool isRunning() const`

**Signature**

```cpp
bool isRunning() const;
```

**Synopsis**
Returns true if the thread is still running.

**Parameters**

* *(none)*

**Return value**

* `true` if the underlying thread is currently executing; `false` if it has finished or could not be started.

**In Depth**

`isRunning()` provides a simple way to check the thread’s state:

* Immediately after construction, it is typically `true` if the thread started successfully.
* Once the thread function has returned, `isRunning()` becomes `false`.
* A thread that was never started or failed to start is also reported as not running.

You can use this in polling loops or to make decisions about whether it is safe to cancel, await, or reuse a handle (though you should still be careful with races between checks and operations).

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

*This example polls `isRunning()` to do other work while the thread finishes in the background.*

---

### `bool equals(const Thread &other) const`

**Signature**

```cpp
bool equals(const Thread &other) const;
```

**Synopsis**
Returns true if this thread equals the other.

**Parameters**

* `other` – Another `Thread` to compare with.

**Return value**

* `true` if both objects refer to the same underlying thread; `false` otherwise.

**In Depth**

`equals()` tests whether two `Thread` handles represent the same underlying OS thread. This is about **identity**, not running state:

* Two handles created via copy or assignment refer to the same thread and compare equal.
* Two independent threads running the same function are not equal.

Use this when you need to verify that two handles are aliases for the same thread.

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

*This example shows that a copied handle is equal to the original according to `equals()`.*

---

### `bool operator==(const Thread &other) const`

**Signature**

```cpp
bool operator==(const Thread &other) const;
```

**Synopsis**
Returns true if this thread equals the other.

**Parameters**

* `other` – Another `Thread` to compare with.

**Return value**

* `true` if both handles represent the same underlying thread; `false` otherwise.

**In Depth**

This operator is the idiomatic shorthand for `equals(other)`:

```cpp
if (a == b) { ... }
```

It compares thread identity, not whether both are running or have the same behavior.

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

*This example uses the equality operator to check whether two handles represent the same thread.*

---

### `bool operator!=(const Thread &other) const`

**Signature**

```cpp
bool operator!=(const Thread &other) const;
```

**Synopsis**
Returns true if this thread does not equal the other.

**Parameters**

* `other` – Another `Thread` to compare with.

**Return value**

* `true` if the two handles refer to different underlying threads; `false` if they refer to the same thread.

**In Depth**

This operator is simply the logical negation of `operator==`. It is often more readable to express inequality directly with `!=`.

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

*This example branches when two thread handles refer to different threads.*
