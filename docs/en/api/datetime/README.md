# DateTime

## Overview

The `attoboy::DateTime` class represents a point in time with **millisecond precision**, always in **UTC** (Coordinated Universal Time). It is the library’s primary way to capture “now”, store timestamps, measure elapsed time, and convert between machine-friendly numeric timestamps and human-readable strings.

You can think of `DateTime` as a compact, RAII-based wrapper around:

* An internal 64-bit timestamp measured as **milliseconds since the Unix epoch**, and
* A small set of operations for addition, comparison, and formatting as ISO-8601 strings.

Unlike `String`, `DateTime` is **mutable**: the `add()` function modifies the object in place and returns a reference to it for chaining. Copying and assignment are safe and cheap; they just copy the underlying timestamp.

### Underlying Concepts: UTC, Unix Epoch, and Milliseconds

If you are new to time handling in programming, a few core ideas are useful:

* **UTC (Coordinated Universal Time)**
  UTC is a global, time-zone–neutral standard. `DateTime` always represents UTC time; it does not track time zones or daylight saving rules. When you convert a `DateTime` to a string, you get an ISO-8601 UTC value ending with `"Z"` (for example, `"2024-01-01T12:00:00.000Z"`).

* **Unix Epoch**
  The Unix epoch is defined as **January 1, 1970 at 00:00:00 UTC**. Internally, `DateTime` uses a 64-bit integer that counts **milliseconds** since that point:

  * `0` → exactly `1970-01-01T00:00:00.000Z`
  * `1` → one millisecond after that
  * Negative values represent times before the epoch.

* **Millisecond Precision**
  All arithmetic and differences are done in milliseconds:

  * One second = 1,000 milliseconds.
  * One minute = 60,000 milliseconds.
  * One hour = 3,600,000 milliseconds.

This design makes it straightforward to:

* Measure elapsed time between two events (`diff()`),
* Shift a `DateTime` forward or backward (`add()`),
* Log timestamps or persist them as numbers (`timestamp()`),
* Produce and parse ISO-8601 strings (`toString()`, constructor from `String`).

> **Note:** `DateTime` uses the system’s wall-clock time. It is not a monotonic “timer” clock. If the system time is adjusted by the user or NTP, new `DateTime()` objects will reflect that change.

---

## Reference

Each entry below covers one public constructor, method, or operator of `attoboy::DateTime`. For each, you will find:

* **Signature** – exact declaration from the header.
* **Synopsis** – the original one-line Doxygen comment.
* **Parameters** and **Return value** descriptions.
* **In Depth** – behavior details, caveats, and a short example.

All examples assume:

```cpp
using namespace attoboy;
```

---

#### `DateTime()`

**Signature**

```cpp
DateTime();
```

**Synopsis**
Creates a DateTime representing the current time.

**Parameters**

* *(none)*

**Return value**

* *(constructor; not applicable)*

**In Depth**

The default constructor captures the **current UTC time** from the system clock, with millisecond precision. It is the simplest way to get a “timestamp for now”.

The captured time is fixed in the object; future calls to `timestamp()` or `toString()` on the same instance will return the same value unless you call `add()` to adjust it.

**Example**

```cpp
DateTime now;
String iso = now.toString();  // e.g., "2025-11-27T21:00:00.123Z"
```

*This example creates a `DateTime` for the current moment and converts it to an ISO-8601 string.*

---

#### `DateTime(long long millisSinceEpoch)`

**Signature**

```cpp
DateTime(long long millisSinceEpoch);
```

**Synopsis**
Creates a DateTime from milliseconds since Unix epoch (Jan 1, 1970).

**Parameters**

* `millisSinceEpoch` – Number of milliseconds elapsed since `1970-01-01T00:00:00.000Z` (UTC). May be negative for times before the epoch.

**Return value**

* *(constructor; not applicable)*

**In Depth**

Use this constructor when you already have a numeric timestamp (for example, from storage, a network protocol, or `timestamp()` on another `DateTime`). The value is stored as-is; no time zone conversion is applied.

Typical patterns:

* Store `timestamp()` in a file or settings.
* Later, read it back and construct a `DateTime` with the same instant.

**Example**

```cpp
DateTime now;
long long saved = now.timestamp();

// later...
DateTime restored(saved);
String iso = restored.toString();
```

*This example captures the current time, saves its millisecond timestamp, and later reconstructs the same instant from that value.*

---

#### `DateTime(const String &iso8601)`

**Signature**

```cpp
DateTime(const String &iso8601);
```

**Synopsis**
Creates a DateTime from an ISO-8601 string (YYYY-MM-DDTHH:MM:SS.fffZ).

**Parameters**

* `iso8601` – A `String` containing a UTC timestamp in ISO-8601 format such as `"2024-05-10T14:30:00.000Z"`.

**Return value**

* *(constructor; not applicable)*

**In Depth**

This constructor parses an ISO-8601 timestamp string and converts it into a `DateTime`. The expected format is:

```text
YYYY-MM-DDTHH:MM:SS.fffZ
```

for example: `"2025-11-27T21:15:30.250Z"`.

Key points:

* The trailing `'Z'` indicates UTC; no time zone offset parsing is performed.
* Parsing failures (bad format, invalid date) result in implementation-defined behavior; you should supply valid strings, typically those produced by `DateTime::toString()`.

**Example**

```cpp
String text("2024-01-01T00:00:00.000Z");
DateTime start(text);
```

*This example constructs a `DateTime` from a known ISO-8601 string.*

---

#### `DateTime(const DateTime &other)`

**Signature**

```cpp
DateTime(const DateTime &other);
```

**Synopsis**
Creates a copy of another DateTime.

**Parameters**

* `other` – Existing `DateTime` instance to copy.

**Return value**

* *(constructor; not applicable)*

**In Depth**

The copy constructor duplicates the internal timestamp and millisecond precision. Since `DateTime` is a simple value-like type, copying is cheap and safe. Changes to one instance (via `add()`) do not affect copies.

**Example**

```cpp
DateTime original;
DateTime copy(original);
// both represent the same instant initially
```

*This example demonstrates copying a `DateTime` value.*

---

#### `~DateTime()`

**Signature**

```cpp
~DateTime();
```

**Synopsis**
Destroys the DateTime and frees memory.

**Parameters**

* *(none)*

**Return value**

* *(destructor; not applicable)*

**In Depth**

When a `DateTime` object goes out of scope, its destructor releases any internal resources. Since `DateTime` primarily encapsulates a timestamp, destruction is usually trivial, but RAII guarantees that any implementation details are handled safely.

**Example**

```cpp
{
  DateTime temp;
  String iso = temp.toString();
}
// temp is destroyed here
```

*This example shows a `DateTime` that is created and destroyed automatically with scope.*

---

#### `DateTime &operator=(const DateTime &other)`

**Signature**

```cpp
DateTime &operator=(const DateTime &other);
```

**Synopsis**
Assigns another DateTime to this DateTime.

**Parameters**

* `other` – `DateTime` whose value should be copied into `*this`.

**Return value**

* Reference to `*this`, allowing assignment chaining.

**In Depth**

The assignment operator replaces the internal timestamp of the left-hand side with that of `other`. After assignment, both objects represent the same instant, but they can be modified independently using `add()`.

Self-assignment (`dt = dt`) is safe.

**Example**

```cpp
DateTime a;
DateTime b;

b = a;  // b now represents the same time as a
```

*This example reassigns a `DateTime` to match another.*

---

#### `DateTime &add(long long milliseconds)`

**Signature**

```cpp
DateTime &add(long long milliseconds);
```

**Synopsis**
Adds milliseconds (negative to subtract). Returns this DateTime for chaining.

**Parameters**

* `milliseconds` – Number of milliseconds to add to the current timestamp. Use a negative value to subtract.

**Return value**

* Reference to `*this` after modification, enabling chained calls.

**In Depth**

`add()` adjusts the `DateTime` by a relative amount of time. It modifies the object in place, shifting it forward or backward.

Common use cases:

* Add seconds: `dt.add(1000 * seconds);`
* Add minutes: `dt.add(60 * 1000 * minutes);`
* Add hours: `dt.add(60 * 60 * 1000 * hours);`

Because it returns a reference to `*this`, you can chain operations:

```cpp
deadline.add(60 * 1000).add(5 * 1000);
```

Be aware that adding very large positive or negative values may overflow the internal timestamp (implementation-defined behavior). For typical application durations, this is not a concern.

**Example**

```cpp
DateTime now;
DateTime deadline(now);
deadline.add(5 * 60 * 1000);  // add 5 minutes
```

*This example copies the current time into `deadline` and then shifts it 5 minutes into the future.*

---

#### `long long diff(const DateTime &other) const`

**Signature**

```cpp
long long diff(const DateTime &other) const;
```

**Synopsis**
Returns the difference in milliseconds (positive if this is later).

**Parameters**

* `other` – The `DateTime` to compare against.

**Return value**

* Difference in milliseconds: `timestamp() - other.timestamp()`.
  The result is:

  * Positive if `*this` is later than `other`.
  * Zero if they represent the same instant.
  * Negative if `*this` is earlier than `other`.

**In Depth**

`diff()` is the primary way to measure elapsed time or intervals:

* `end.diff(start)` → how many milliseconds passed from `start` to `end`.
* `now.diff(past)` → positive delay (if `now` is later).
* `future.diff(now)` → positive remaining time until `future`.

Internally, it behaves like subtracting the two timestamps. It does not change either object.

**Example**

```cpp
DateTime start;
// ... do some work ...
DateTime end;

long long elapsedMs = end.diff(start);
// elapsedMs is the duration between start and end in milliseconds
```

*This example measures elapsed time between two captured timestamps.*

---

#### `int compare(const DateTime &other) const`

**Signature**

```cpp
int compare(const DateTime &other) const;
```

**Synopsis**
Compares with another DateTime. Returns <0, 0, or >0.

**Parameters**

* `other` – `DateTime` to compare with.

**Return value**

* Negative value if `*this` is earlier than `other`.
* `0` if both represent the same instant.
* Positive value if `*this` is later than `other`.

**In Depth**

`compare()` provides a three-way comparison suitable for sorting or ordering. It is equivalent to:

```cpp
if (timestamp() < other.timestamp()) return negative;
if (timestamp() > other.timestamp()) return positive;
return 0;
```

Exact numeric values of negative/positive are not specified; only their sign matters.

**Example**

```cpp
DateTime a;            // now
DateTime b(a);
b.add(1000);           // a second later

int result = a.compare(b);  // result < 0 because a is earlier
```

*This example compares two `DateTime` instances to determine chronological order.*

---

#### `bool equals(const DateTime &other) const`

**Signature**

```cpp
bool equals(const DateTime &other) const;
```

**Synopsis**
Returns true if this DateTime equals the other.

**Parameters**

* `other` – `DateTime` to compare against.

**Return value**

* `true` if both represent the same millisecond since the epoch; `false` otherwise.

**In Depth**

`equals()` checks strict equality of timestamps, including millisecond precision. It is equivalent to checking that `timestamp()` is the same for both objects.

This method is functionally the same as `operator==`, but some codebases prefer the explicit method for clarity.

**Example**

```cpp
DateTime a;
DateTime b(a);

bool same = a.equals(b);  // true
```

*This example uses `equals()` to verify that two objects represent the same time.*

---

#### `bool operator==(const DateTime &other) const`

**Signature**

```cpp
bool operator==(const DateTime &other) const;
```

**Synopsis**
Returns true if this DateTime equals the other.

**Parameters**

* `other` – `DateTime` to compare against.

**Return value**

* `true` if timestamps are equal; `false` otherwise.

**In Depth**

This operator provides the idiomatic C++ equality comparison:

```cpp
if (a == b) { ... }
```

Internally, it is equivalent to `equals(other)`.

**Example**

```cpp
DateTime a;
DateTime b(a);

if (a == b) {
  String msg("Times are equal");
}
```

*This example checks equality using the `==` operator.*

---

#### `bool operator!=(const DateTime &other) const`

**Signature**

```cpp
bool operator!=(const DateTime &other) const;
```

**Synopsis**
Returns true if this DateTime does not equal the other.

**Parameters**

* `other` – `DateTime` to compare against.

**Return value**

* `true` if timestamps differ; `false` if they are equal.

**In Depth**

This operator is simply the logical negation of `operator==`. Use it when you want to branch on inequality:

```cpp
if (a != b) { ... }
```

**Example**

```cpp
DateTime scheduled;
DateTime actual;

if (actual != scheduled) {
  String msg("Schedule changed");
}
```

*This example detects whether the actual time differs from the scheduled one.*

---

#### `long long timestamp() const`

**Signature**

```cpp
long long timestamp() const;
```

**Synopsis**
Returns milliseconds since Unix epoch (Jan 1, 1970).

**Parameters**

* *(none)*

**Return value**

* 64-bit integer representing the number of milliseconds since `1970-01-01T00:00:00.000Z` (UTC).

**In Depth**

`timestamp()` exposes the underlying numeric representation of the `DateTime`. Use it when you need to:

* Persist times in a compact, machine-friendly form.
* Perform arithmetic outside the `DateTime` API.
* Interoperate with other systems that use millisecond timestamps.

Be aware that this is based on wall-clock time; if the system clock moves backward, future `DateTime()` instances may have smaller timestamps than earlier ones.

**Example**

```cpp
DateTime now;
long long ms = now.timestamp();
// ms can be saved to a file or sent over the network
```

*This example obtains the raw millisecond timestamp from a `DateTime`.*

---

#### `String toString() const`

**Signature**

```cpp
String toString() const;
```

**Synopsis**
Returns an ISO-8601 formatted string.

**Parameters**

* *(none)*

**Return value**

* `String` containing the UTC timestamp in ISO-8601 format, such as `"YYYY-MM-DDTHH:MM:SS.fffZ"`.

**In Depth**

`toString()` converts the internal timestamp into a human-readable ISO-8601 string in UTC, including milliseconds and a trailing `'Z'` to indicate UTC.

Typical uses:

* Logging timestamps.
* Embedding time values into JSON or configuration files.
* Round-tripping with the `DateTime(const String &iso8601)` constructor.

The combination of `timestamp()` plus `toString()` gives you both machine-friendly and human-friendly representations of the same instant.

**Example**

```cpp
DateTime now;
String iso = now.toString();  // e.g., "2025-11-27T21:05:30.123Z"
```

*This example converts a `DateTime` to a standard ISO-8601 UTC string suitable for logging or storage.*
