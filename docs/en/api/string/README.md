# String

## Overview

The `attoboy::String` class is the core text type in the attoboy library. It represents an **immutable UTF-8 string** with a rich set of operations for inspection, searching, transformation, splitting/joining, and formatting. It is designed to be:

* **Easy to use**: Constructors accept many common types (booleans, numbers, containers).
* **Safe by default**: Character-based operations respect UTF-8 code point boundaries.
* **Convenient**: Integration with `List`, `Map`, and `Set` allows simple JSON-style conversions and templated formatting.
* **Tiny-binary friendly**: Works without the C++ standard library or CRT.

At a high level, you can think of `String` as attoboy’s replacement for `std::string`, but with:

* **UTF-8 awareness** (characters vs. bytes).
* **Immutability**: once created, its contents never change.
* Built-in conversions from and to many attoboy types.

### Immutability and Value Semantics

`String` is **immutable**: none of its member functions modify the contents of an existing string object. Instead, any operation that “changes” the string returns a **new** `String` with the transformed content.

For example:

```cpp
using namespace attoboy;

String original("hello");
String upper = original.upper();

// original is still "hello"
// upper is "HELLO"
```

This immutability has several benefits:

* You can safely share `String` instances across multiple parts of your program (and across threads) without worrying about accidental modification.
* Functions that receive a `String` parameter do not need to copy it defensively; they know it cannot be changed.
* Internally, the implementation can optimize memory usage by sharing underlying storage when copying or assigning.

### UTF-8, Characters, and Bytes

The library uses **UTF-8 encoding** everywhere. This has two practical consequences:

1. A single user-visible character (Unicode **code point**) may take **multiple bytes**.
2. Some operations are defined in terms of **characters** (code points), while others work in terms of **raw bytes**.

The `String` API exposes both views:

* **Character-based (UTF-8 aware)**:

  * `length()`
  * `at()`
  * `substring()`
  * `getPositionOf()`
  * `trim()`, `upper()`, `lower()`, `reverse()` (for characters)
* **Byte-based (raw)**:

  * `byteLength()`
  * `byteAt()`
  * `byteSubstring()`
  * `FromCStr()` (uses raw byte length)

When working with human-readable text (user interface, logs, etc.), you almost always want the **character-based** operations. Use the byte-based versions when you are dealing with protocols or binary encodings where exact byte positions matter.

### Indexing and Negative Indices

Several `String` functions accept **indices** (positions) or **ranges**:

* Indices are **0-based**, where `0` refers to the first character (or byte).
* Some functions explicitly support **negative indices** to count from the end:

  * `at(int index)`
  * `byteAt(int index)`
  * `substring(int start, int end = -1)`

For these functions:

* `-1` means “last character”.
* `-2` means “second to last”, and so on.

For `substring(start, end)`:

* `start` and `end` are character indices.
* `end` is **exclusive** (the character at `end` is not included).
* If `end` is `-1` (the default), the substring extends to the end of the string.

### Integration with Other attoboy Types

The `String` class works closely with other core attoboy types:

* `String(const List &list)` and `String(const Set &set)` produce JSON array strings.
* `String(const Map &map)` produces a JSON object string.
* `format(const List &list)` and `format(const Map &map)` replace placeholders such as `{0}` or `{key}`.
* `lines()`, `split()`, and `join()` integrate with `List` for simple tokenization and reconstruction.

This makes it easy to move text between data structures, serialize to JSON, and build templated messages without additional libraries.

---

## Reference

Each entry below covers one public constructor, method, static function, or operator of `attoboy::String`. For each, you will find:

* **Signature** – the exact declaration from the header.
* **Synopsis** – the original one-line Doxygen comment.
* **Parameters** and **Return value** descriptions.
* **In Depth** – additional details, caveats, and a short example.

> **Note:** All examples assume `using namespace attoboy;`.

---

### Constructors, Copy, Assignment, and Destruction

#### `String()`

**Signature**

```cpp
String();
```

**Synopsis**
Creates an empty string.

**Parameters**

* *(none)*

**Return value**

* *(constructor; not applicable)*

**In Depth**

This default constructor creates a `String` that contains no characters and no bytes. For such a string:

* `length()` returns `0`.
* `byteLength()` returns `0`.
* `isEmpty()` returns `true`.
* `c_str()` returns a pointer to a single `'\0'` byte.

Empty strings are a natural default value and a common starting point for building up new strings with concatenation and other operations.

**Example**

```cpp
String s;
String message = s.append("Hello");  // message == "Hello"
```

*This example starts from an empty `String` and builds a new one by appending text.*

---

#### `String(const char *str)`

**Signature**

```cpp
String(const char *str);
```

**Synopsis**
Creates a string from a null-terminated C string.

**Parameters**

* `str` – Pointer to a null-terminated byte sequence in UTF-8 encoding.

**Return value**

* *(constructor; not applicable)*

**In Depth**

This constructor is the main bridge from raw C-style strings to `attoboy::String`. The content is copied into the internal representation, so you can safely modify or free the original buffer afterwards.

Key points:

* The input must be **null-terminated**.
* The bytes are interpreted as UTF-8. If the byte sequence is not valid UTF-8, character-based operations may behave unexpectedly.
* A `nullptr` input is treated as an empty string in most designs; to avoid ambiguity, pass a valid pointer whenever possible.

**Example**

```cpp
const char *raw = "Hello, UTF-8 🌍";
String s(raw);
int chars = s.length();      // character count (including emoji)
int bytes = s.byteLength();  // raw byte count
```

*This example wraps a C-style UTF-8 string into an immutable `String` and inspects characters vs. bytes.*

---

#### `String(bool value)`

**Signature**

```cpp
String(bool value);
```

**Synopsis**
Creates a string from a boolean ("true" or "false").

**Parameters**

* `value` – Boolean value to convert.

**Return value**

* *(constructor; not applicable)*

**In Depth**

This constructor converts a boolean into the literal text `"true"` or `"false"` (all lowercase). It is useful when logging, formatting messages, or constructing JSON without manual string building.

**Example**

```cpp
bool enabled = true;
String text(enabled);   // "true"
String msg = String("Feature enabled = ") + text;
```

*This example converts a boolean flag into a readable string and concatenates it into a message.*

---

#### `String(char value)`

**Signature**

```cpp
String(char value);
```

**Synopsis**
Creates a string from a character.

**Parameters**

* `value` – Single byte value to store as a one-character string.

**Return value**

* *(constructor; not applicable)*

**In Depth**

This constructor creates a `String` containing exactly one byte. For ASCII characters, this corresponds to one character. For general Unicode text, prefer constructing from UTF-8 encoded `const char *` or combining existing `String` instances.

**Example**

```cpp
String exclamation('!');
String word("Go");
String sentence = word + exclamation;  // "Go!"
```

*This example builds a word and then appends a single-character `String` created from a `char`.*

---

#### `String(int value)`

**Signature**

```cpp
String(int value);
```

**Synopsis**
Creates a string from an integer.

**Parameters**

* `value` – 32-bit integer to convert.

**Return value**

* *(constructor; not applicable)*

**In Depth**

This constructor converts the integer to its decimal representation (e.g., `42` → `"42"`, `-5` → `"-5"`). There is no thousands separator or additional formatting.

**Example**

```cpp
int score = 42;
String scoreText(score);  // "42"
String label = String("Score: ") + scoreText;  // "Score: 42"
```

*This example converts an integer score to text and concatenates it into a label.*

---

#### `String(long long value)`

**Signature**

```cpp
String(long long value);
```

**Synopsis**
Creates a string from a 64-bit integer.

**Parameters**

* `value` – 64-bit integer to convert.

**Return value**

* *(constructor; not applicable)*

**In Depth**

Behaves like `String(int)` but supports the full range of `long long`. Use this when you need to string-ify large counters, timestamps, or sizes that may not fit in a 32-bit integer.

**Example**

```cpp
long long totalBytes = 1048576;
String bytesText(totalBytes);  // "1048576"
```

*This example converts a 64-bit value (e.g., a size in bytes) into text for reporting.*

---

#### `String(float value)`

**Signature**

```cpp
String(float value);
```

**Synopsis**
Creates a string from a float.

**Parameters**

* `value` – Floating-point value to convert.

**Return value**

* *(constructor; not applicable)*

**In Depth**

Converts a `float` into a textual decimal representation. The exact formatting (number of decimal places, trailing zeros) follows the library’s internal formatting rules, but you can rely on it being a typical readable decimal string.

**Example**

```cpp
float ratio = 0.5f;
String text(ratio);  // e.g., "0.5"
String label = String("Ratio: ") + text;
```

*This example converts a floating-point value into a simple string.*

---

#### `String(const List &list)`

**Signature**

```cpp
String(const List &list);
```

**Synopsis**
Creates a JSON array string from a list (e.g., [1,"a",true]).

**Parameters**

* `list` – `List` whose elements will be serialized to JSON.

**Return value**

* *(constructor; not applicable)*

**In Depth**

This constructor renders a `List` as a JSON array, where each element is serialized according to its type:

* Numbers as JSON numbers.
* Booleans as `true`/`false`.
* Strings as quoted, escaped JSON strings.
* Nested `List`, `Map`, and `Set` values become nested arrays/objects.

This is especially useful for logging structured data or sending simple JSON payloads without a full JSON library.

**Example**

```cpp
List values;
values.append(1).append("two").append(true);

String json(values);  // e.g., "[1,\"two\",true]"
```

*This example converts a heterogeneous list into a JSON array string.*

---

#### `String(const Map &map)`

**Signature**

```cpp
String(const Map &map);
```

**Synopsis**
Creates a JSON object string from a map (e.g., {"k":"v"}).

**Parameters**

* `map` – `Map` whose key–value pairs will be serialized to JSON.

**Return value**

* *(constructor; not applicable)*

**In Depth**

This constructor renders a `Map` as a JSON object. Keys and values are converted to JSON; keys are typically strings, but other types are converted to strings as needed.

Keys must be unique in the map; the resulting JSON uses those keys as object property names.

**Example**

```cpp
Map user;
user.put("name", "Alice").put("age", 30);

String json(user);  // e.g., "{\"name\":\"Alice\",\"age\":30}"
```

*This example converts a map of properties into a JSON object string.*

---

#### `String(const Set &set)`

**Signature**

```cpp
String(const Set &set);
```

**Synopsis**
Creates a JSON array string from a set.

**Parameters**

* `set` – `Set` whose values will be serialized to JSON.

**Return value**

* *(constructor; not applicable)*

**In Depth**

This constructor renders a `Set` as a JSON array string, similar to `String(const List &list)`. Because sets are unordered, the JSON array order is not guaranteed.

Duplicate values are already eliminated by the `Set` itself.

**Example**

```cpp
Set tags;
tags.put("red").put("blue").put("green");

String json(tags);  // e.g., "[\"red\",\"blue\",\"green\"]"
```

*This example converts a set of strings into a JSON array string.*

---

#### `template <typename T, typename U, typename... Args> String(const T &first, const U &second, const Args &...rest)`

**Signature**

```cpp
template <typename T, typename U, typename... Args>
String(const T &first, const U &second, const Args &...rest)
    : String(String(first) + String(second) + String(rest...)) {}
```

**Synopsis**
Concatenates multiple values into one string.

**Parameters**

* `first` – First value to convert to `String`.
* `second` – Second value to convert to `String`.
* `rest` – Zero or more additional values to convert and append.

Each argument can be any type constructible as `String`, such as `const char *`, `String`, `int`, `bool`, `List`, and so on.

**Return value**

* *(constructor; not applicable)*

**In Depth**

This variadic constructor provides a concise way to build a `String` from a sequence of heterogeneous values. Internally, each argument is converted to `String` and concatenated in order.

You can think of it as:

```cpp
String(String(first)) + String(second) + String(rest)...
```

This is especially convenient for building messages or logs without manual `+` chaining.

**Example**

```cpp
int id = 7;
String status = "OK";

String line("Task #", id, " status: ", status);
// e.g., "Task #7 status: OK"
```

*This example builds a message from a mix of literals, integers, and other strings using the variadic constructor.*

---

#### `String(const String &other)`

**Signature**

```cpp
String(const String &other);
```

**Synopsis**
Creates a copy of another string.

**Parameters**

* `other` – Existing `String` to copy.

**Return value**

* *(constructor; not applicable)*

**In Depth**

The copy constructor creates a new `String` with the same contents as `other`. Because `String` is immutable, copies are conceptually independent and safe to share. The internal implementation may share storage for efficiency.

**Example**

```cpp
String original("hello");
String copy(original);
// both represent "hello"
```

*This example demonstrates copying a string value.*

---

#### `~String()`

**Signature**

```cpp
~String();
```

**Synopsis**
Destroys the string and frees memory.

**Parameters**

* *(none)*

**Return value**

* *(destructor; not applicable)*

**In Depth**

When a `String` goes out of scope or is otherwise destroyed, its destructor releases any internal resources (such as heap-allocated buffers). Because `String` uses RAII, you do not need to manually free memory.

**Example**

```cpp
{
  String temp("temporary");
  // temp is valid here
}
// temp is destroyed here; memory is released automatically
```

*This example shows automatic cleanup of a `String` when it goes out of scope.*

---

#### `static String FromCStr(const char *data, int size)`

**Signature**

```cpp
static String FromCStr(const char *data, int size);
```

**Synopsis**
Creates a string from raw bytes with specified length (does not require null termination).

**Parameters**

* `data` – Pointer to a buffer containing UTF-8 bytes.
* `size` – Number of bytes to copy from `data`.

**Return value**

* A `String` containing exactly `size` bytes from `data`, followed by an internal null terminator.

**In Depth**

Unlike the `const char *` constructor, this static function does **not** require a terminating `'\0'` in the source buffer. It is ideal when:

* You have a buffer that might include embedded nulls.
* You are working with a substring of a larger buffer that is not separately null-terminated.

The bytes are interpreted as UTF-8; you are responsible for ensuring that the slice boundaries do not split multi-byte characters if you intend to do character-based operations.

**Example**

```cpp
const char raw[] = "Hello, world!";
String partial = String::FromCStr(raw, 5);  // "Hello"
```

*This example creates a `String` from the first 5 bytes of a C-style buffer without requiring a null terminator at that position.*

---

#### `String &operator=(const String &other)`

**Signature**

```cpp
String &operator=(const String &other);
```

**Synopsis**
Assigns another string to this string.

**Parameters**

* `other` – String whose value should be assigned.

**Return value**

* Reference to `*this`, enabling assignment chaining.

**In Depth**

The copy assignment operator replaces the contents of the left-hand side with those of `other`. Because `String` is immutable, this operation just makes the variable refer to the same logical value as `other`.

Self-assignment is safe; typical implementations detect and handle it efficiently.

**Example**

```cpp
String name("Alice");
String other("Bob");

name = other;  // name now represents "Bob"
```

*This example reassigns a `String` variable to share the same text as another.*

---

### Basic Properties and Access

#### `int length() const`

**Signature**

```cpp
int length() const;
```

**Synopsis**
Returns the number of UTF-8 characters in the string.

**Parameters**

* *(none)*

**Return value**

* Number of Unicode code points in the string (may be less than `byteLength()`).

**In Depth**

`length()` counts **characters**, not bytes. For plain ASCII text, `length()` and `byteLength()` are equal. For non-ASCII characters (such as emoji or non-Latin scripts), a single character may be stored as multiple bytes, so `byteLength()` may be larger.

Use `length()` when you care about user-visible characters: cursor positions, validation rules like “max 80 characters”, and so on.

**Example**

```cpp
String ascii("hello");
String unicode("🌍");  // earth emoji

int la = ascii.length();    // 5
int ba = ascii.byteLength(); // 5

int lu = unicode.length();   // 1 (one character)
int bu = unicode.byteLength(); // > 1 bytes
```

*This example contrasts character count vs. byte count for ASCII and Unicode text.*

---

#### `int byteLength() const`

**Signature**

```cpp
int byteLength() const;
```

**Synopsis**
Returns the number of bytes in the string.

**Parameters**

* *(none)*

**Return value**

* Number of bytes in the internal UTF-8 representation (excluding the null terminator).

**In Depth**

`byteLength()` gives you the raw storage size of the string content. Use this when:

* Writing to binary protocols that expect byte counts.
* Allocating buffers to hold copies of the UTF-8 data.

Note that `byteLength()` can be larger than `length()` if the string contains non-ASCII characters.

**Example**

```cpp
String s("Hello, 🌍");
int bytes = s.byteLength();  // raw UTF-8 bytes
```

*This example retrieves the byte length of UTF-8 text.*

---

#### `bool isEmpty() const`

**Signature**

```cpp
bool isEmpty() const;
```

**Synopsis**
Returns true if the string is empty.

**Parameters**

* *(none)*

**Return value**

* `true` if the string contains no characters, `false` otherwise.

**In Depth**

This is a convenience function equivalent to `length() == 0`. It is often clearer and more expressive when checking for empty input or optional fields.

**Example**

```cpp
String name;

if (name.isEmpty()) {
  name = "Anonymous";
}
```

*This example assigns a default name when the string is empty.*

---

#### `const char *c_str() const`

**Signature**

```cpp
const char *c_str() const;
```

**Synopsis**
Returns a pointer to the null-terminated C string.

**Parameters**

* *(none)*

**Return value**

* Pointer to a null-terminated UTF-8 byte sequence owned by the `String`.

**In Depth**

This function exposes a read-only C-style string view of the internal data. The returned pointer:

* Remains valid as long as the `String` object remains alive.
* Should not be modified.
* Can be passed to C APIs that expect `const char *`.

Because `String` is immutable, calls to other methods do not change the buffer for a given object.

**Example**

```cpp
String s("Hello");
const char *p = s.c_str();
// p can be passed to C-style APIs that expect a const char*.
```

*This example obtains a C-style pointer for interoperability with APIs that are not aware of `String`.*

---

### Character and Byte Access

#### `String at(int index) const`

**Signature**

```cpp
String at(int index) const;
```

**Synopsis**
Returns the character at index as a string. Negative indices count from end.

**Parameters**

* `index` – Character index (0-based). Negative values count from the end.

**Return value**

* A `String` containing exactly one character (one Unicode code point).

**In Depth**

`at()` returns the character at position `index` as a new single-character string. It respects UTF-8 code point boundaries, so it never returns half of a multi-byte character.

Negative indices are supported:

* `-1` – last character.
* `-2` – second-to-last character, and so on.

If `index` is out of range, the behavior is implementation-defined; in practice you should ensure `0 <= index < length()` (or the corresponding negative range) before calling.

**Example**

```cpp
String text("Hello");
String first = text.at(0);   // "H"
String last  = text.at(-1);  // "o"
```

*This example extracts the first and last characters of a string.*

---

#### `String byteAt(int index) const`

**Signature**

```cpp
String byteAt(int index) const;
```

**Synopsis**
Returns a single byte at index as a string. Negative indices count from end.

**Parameters**

* `index` – Byte index (0-based). Negative values count from the end of the byte sequence.

**Return value**

* A `String` containing exactly one byte.

**In Depth**

`byteAt()` provides byte-level access. This is mostly useful for debugging or low-level manipulation; most text logic should use `at()` instead.

If the string contains multi-byte characters, `byteAt()` may return a non-ASCII continuation byte, which is not meaningful as a standalone character.

**Example**

```cpp
String text("A");
String b0 = text.byteAt(0);   // "A"
```

*This example reads the first byte of a simple ASCII string.*

---

### Substrings and Duplication

#### `String substring(int start, int end = -1) const`

**Signature**

```cpp
String substring(int start, int end = -1) const;
```

**Synopsis**
Returns a substring from start to end (exclusive). Negative indices count from end.

**Parameters**

* `start` – Character index of the first character to include. May be negative to count from the end.
* `end` – Character index **after** the last character to include (exclusive). `-1` means “up to the end of the string”.

**Return value**

* New `String` containing the specified range of characters.

**In Depth**

`substring()` operates on **characters**, not bytes. It respects UTF-8 boundaries, so you can safely slice text that includes multi-byte characters.

Common patterns:

* `substring(0, n)` – first `n` characters.
* `substring(0)` or `substring(0, -1)` – full copy.
* `substring(-n)` – last `n` characters.

You should ensure that the effective range is within `[0, length()]`. Passing out-of-range indices produces implementation-defined results.

**Example**

```cpp
String text("Hello, world");
String hello = text.substring(0, 5);  // "Hello"
String world = text.substring(7);     // "world"
```

*This example extracts two substrings using start/end indices.*

---

#### `String byteSubstring(int start, int end = -1) const`

**Signature**

```cpp
String byteSubstring(int start, int end = -1) const;
```

**Synopsis**
Returns a byte-based substring from start to end (exclusive).

**Parameters**

* `start` – Byte index of the first byte to include.
* `end` – Byte index **after** the last byte to include (exclusive). `-1` means “up to the end of the byte sequence”.

**Return value**

* New `String` containing the specified range of bytes.

**In Depth**

`byteSubstring()` works in terms of raw bytes, not characters. If you cut through the middle of a multi-byte character, the result may contain invalid UTF-8. Use this function only when you are deliberately manipulating byte-level data (e.g., protocol framing, manual encoding).

**Example**

```cpp
String raw("abcdef");
String mid = raw.byteSubstring(2, 4);  // "cd"
```

*This example takes a two-byte slice from a simple ASCII string.*

---

#### `String duplicate() const`

**Signature**

```cpp
String duplicate() const;
```

**Synopsis**
Returns a copy of this string.

**Parameters**

* *(none)*

**Return value**

* A new `String` containing the same text as the original.

**In Depth**

`duplicate()` is an explicit way to obtain a copy of a `String`. Because `String` is immutable, copying is mainly useful for clarity when you want to emphasize that you are working with an independent value, even though modifications always return new strings anyway.

**Example**

```cpp
String original("hello");
String copy = original.duplicate();
// original and copy both represent "hello"
```

*This example obtains an explicit duplicate of a string value.*

---

### Searching and Matching

#### `bool contains(const String &sub) const`

**Signature**

```cpp
bool contains(const String &sub) const;
```

**Synopsis**
Returns true if this string contains the substring.

**Parameters**

* `sub` – Substring to search for.

**Return value**

* `true` if `sub` appears at least once in this string; `false` otherwise.

**In Depth**

`contains()` is a convenience for substring presence checks. It typically performs a straightforward search over character or byte sequences (implementation-dependent) and does not use regular expressions.

If `sub` is empty, the behavior is implementation-defined; in most designs an empty substring is considered “present” at position 0, but you should avoid passing empty strings if possible.

**Example**

```cpp
String text("Hello, world");
bool hasWorld = text.contains("world");  // true
```

*This example checks whether a string contains a particular word.*

---

#### `bool startsWith(const String &sub) const`

**Signature**

```cpp
bool startsWith(const String &sub) const;
```

**Synopsis**
Returns true if this string starts with the substring.

**Parameters**

* `sub` – Prefix candidate.

**Return value**

* `true` if `sub` matches the prefix of this string; `false` otherwise.

**In Depth**

`startsWith()` is commonly used for protocol detection, path checking, and simple prefix-based routing. Matching is exact and case-sensitive.

**Example**

```cpp
String path("/api/users");
bool isApi = path.startsWith("/api/");  // true
```

*This example tests whether a path has a particular prefix.*

---

#### `bool endsWith(const String &sub) const`

**Signature**

```cpp
bool endsWith(const String &sub) const;
```

**Synopsis**
Returns true if this string ends with the substring.

**Parameters**

* `sub` – Suffix candidate.

**Return value**

* `true` if `sub` matches the suffix of this string; `false` otherwise.

**In Depth**

`endsWith()` is useful for file extensions, simple pattern checks, and similar suffix-based logic. Matching is exact and case-sensitive.

**Example**

```cpp
String filename("report.txt");
bool isTxt = filename.endsWith(".txt");  // true
```

*This example checks a file extension using `endsWith()`.*

---

#### `int count(const String &sub) const`

**Signature**

```cpp
int count(const String &sub) const;
```

**Synopsis**
Returns the number of non-overlapping occurrences of substring.

**Parameters**

* `sub` – Substring to count.

**Return value**

* Number of non-overlapping occurrences of `sub` within this string.

**In Depth**

`count()` walks through the string and increments the counter whenever it finds an occurrence of `sub` that does not overlap with the previous one. For example, counting `"aa"` in `"aaaa"` returns 2 (`"aa" + "aa"`).

If `sub` is empty, the result is not well-defined; avoid passing empty substrings.

**Example**

```cpp
String text("one, two, three, two");
int twos = text.count("two");  // 2
```

*This example counts how many times a word appears in a sentence.*

---

#### `int getPositionOf(const String &sub, int start = 0) const`

**Signature**

```cpp
int getPositionOf(const String &sub, int start = 0) const;
```

**Synopsis**
Returns the character index of substring, or -1 if not found.

**Parameters**

* `sub` – Substring to search for.
* `start` – Character index where the search should begin (default `0`).

**Return value**

* Character index of the first match at or after `start`, or `-1` if `sub` is not found.

**In Depth**

This function finds the **first occurrence** of `sub` starting at a given character position. It reports positions in terms of characters (not bytes), which makes it convenient to combine with `substring()`.

If you need to find all occurrences, you can loop and pass `start` as the last found index plus the length of `sub`.

**Example**

```cpp
String text("banana");
int pos = text.getPositionOf("na");  // 2
int next = text.getPositionOf("na", pos + 2);  // 4
```

*This example finds the first and second occurrence of `"na"` in `"banana"`.*

---

### Numeric and Boolean Conversion

#### `bool isNumber() const`

**Signature**

```cpp
bool isNumber() const;
```

**Synopsis**
Returns true if the string is a valid integer or decimal number.

**Parameters**

* *(none)*

**Return value**

* `true` if the contents can be parsed as a number; `false` otherwise.

**In Depth**

`isNumber()` checks whether the string can be interpreted as either:

* An integer (e.g., `"42"`, `"-7"`), or
* A decimal floating-point value (e.g., `"3.14"`, `"0.5"`).

Use this before calling `toInteger()` or `toFloat()` when you need to distinguish between a valid numeric `0` and an invalid string that would also produce `0` as a default.

**Example**

```cpp
String s("123");
if (s.isNumber()) {
  int n = s.toInteger();  // 123
}
```

*This example validates that a string is numeric before converting it to an integer.*

---

#### `int toInteger() const`

**Signature**

```cpp
int toInteger() const;
```

**Synopsis**
Converts the string to an integer. Returns 0 if invalid.

**Parameters**

* *(none)*

**Return value**

* Parsed integer value, or `0` if the string is not a valid integer.

**In Depth**

`toInteger()` attempts to parse the contents as a decimal integer (optionally with a sign). If parsing fails, it returns `0` as a fallback. This means you cannot distinguish `"0"` from invalid text using `toInteger()` alone; use `isNumber()` first if you need to detect invalid input.

**Example**

```cpp
String s("42");
int value = s.toInteger();  // 42

String bad("abc");
int fallback = bad.toInteger();  // 0
```

*This example shows successful and fallback integer conversions.*

---

#### `float toFloat() const`

**Signature**

```cpp
float toFloat() const;
```

**Synopsis**
Converts the string to a float. Returns 0.0 if invalid.

**Parameters**

* *(none)*

**Return value**

* Parsed floating-point value, or `0.0f` if the string is not a valid number.

**In Depth**

`toFloat()` parses the string as a decimal floating-point number. On failure, it returns `0.0f`. As with `toInteger()`, use `isNumber()` to distinguish a valid `0.0` from invalid text.

**Example**

```cpp
String s("3.5");
float value = s.toFloat();  // 3.5f
```

*This example converts a decimal text into a `float`.*

---

#### `bool toBool() const`

**Signature**

```cpp
bool toBool() const;
```

**Synopsis**
Returns true for "true", "yes", "1", "on" (case-insensitive).

**Parameters**

* *(none)*

**Return value**

* `true` if the text matches one of the recognized truthy values; `false` otherwise.

**In Depth**

This helper interprets a variety of common user inputs as booleans:

* Recognized **truthy** values (case-insensitive): `"true"`, `"yes"`, `"1"`, `"on"`.
* Any other value, including `"false"`, `"no"`, `"0"`, and empty strings, yields `false`.

This is useful for configuration values or command-line arguments that accept flexible boolean input.

**Example**

```cpp
String flag("YES");
bool enabled = flag.toBool();  // true
```

*This example interprets a case-insensitive boolean string.*

---

### Building and Transforming Strings

#### `String append(const String &s) const`

**Signature**

```cpp
String append(const String &s) const;
```

**Synopsis**
Returns a new string with the substring appended.

**Parameters**

* `s` – Text to append to the end of this string.

**Return value**

* New `String` equal to `(*this + s)`.

**In Depth**

`append()` is equivalent to `operator+` but presented in a method form. It is often convenient in fluent chains when you prefer method syntax.

**Example**

```cpp
String base("Hello");
String result = base.append(", world");  // "Hello, world"
```

*This example appends text to a base string.*

---

#### `String prepend(const String &s) const`

**Signature**

```cpp
String prepend(const String &s) const;
```

**Synopsis**
Returns a new string with the substring prepended.

**Parameters**

* `s` – Text to put in front of this string.

**Return value**

* New `String` equal to `(s + *this)`.

**In Depth**

`prepend()` mirrors `append()` but places the argument before the original string.

**Example**

```cpp
String name("Alice");
String label = name.prepend("User: ");  // "User: Alice"
```

*This example prepends a label in front of a name.*

---

#### `String insert(int index, const String &s) const`

**Signature**

```cpp
String insert(int index, const String &s) const;
```

**Synopsis**
Returns a new string with the substring inserted at index.

**Parameters**

* `index` – Character index at which to insert `s`.
* `s` – Text to insert.

**Return value**

* New `String` with `s` inserted into the original string at the specified character position.

**In Depth**

`insert()` is a character-based operation:

* `index` is measured in characters (as for `length()`).
* You should supply a value between `0` and `length()`. Values outside this range produce implementation-defined behavior.

`insert(0, s)` is equivalent to `s + *this`. `insert(length(), s)` is equivalent to `append(s)`.

**Example**

```cpp
String base("Hello world");
String updated = base.insert(5, ",");  // "Hello, world"
```

*This example inserts a comma between two words.*

---

#### `String remove(int start, int end) const`

**Signature**

```cpp
String remove(int start, int end) const;
```

**Synopsis**
Returns a new string with characters from start to end removed.

**Parameters**

* `start` – Character index of the first character to remove.
* `end` – Character index **after** the last character to remove (exclusive).

**Return value**

* New `String` with the specified character range removed.

**In Depth**

`remove()` is character-based and returns a new string that skips the specified range. It is similar to:

```cpp
substring(0, start) + substring(end)
```

Indices must refer to valid character positions within the string.

**Example**

```cpp
String text("Hello, world");
String noComma = text.remove(5, 6);  // "Hello world"
```

*This example removes a single character (a comma) from a string.*

---

#### `String replace(const String &target, const String &replacement) const`

**Signature**

```cpp
String replace(const String &target, const String &replacement) const;
```

**Synopsis**
Returns a new string with all occurrences of target replaced.

**Parameters**

* `target` – Substring to search for.
* `replacement` – Text to substitute for each occurrence of `target`.

**Return value**

* New `String` where every non-overlapping occurrence of `target` is replaced with `replacement`.

**In Depth**

`replace()` performs a global, non-overlapping replacement. It does not use regular expressions. If `target` is empty, the behavior is undefined; always pass a non-empty `target`.

**Example**

```cpp
String text("color color");
String american = text.replace("color", "colour");  // "colour colour"
```

*This example replaces all occurrences of a word with another.*

---

#### `String trim() const`

**Signature**

```cpp
String trim() const;
```

**Synopsis**
Returns a new string with leading and trailing whitespace removed.

**Parameters**

* *(none)*

**Return value**

* New `String` with whitespace stripped from both ends.

**In Depth**

`trim()` removes whitespace at the beginning and end of the string. The exact definition of “whitespace” follows the implementation, but typically includes spaces, tabs, and newlines.

Internal whitespace (between non-whitespace characters) is not affected.

**Example**

```cpp
String raw("   hello  \n");
String cleaned = raw.trim();  // "hello"
```

*This example strips extra whitespace around a token.*

---

#### `String upper() const`

**Signature**

```cpp
String upper() const;
```

**Synopsis**
Returns a new string with all characters uppercase.

**Parameters**

* *(none)*

**Return value**

* New `String` where applicable characters are converted to uppercase.

**In Depth**

`upper()` converts alphabetic characters to uppercase. For ASCII letters, this is straightforward. For Unicode letters, the actual behavior depends on the implementation’s Unicode handling.

Use this for case-insensitive comparisons in combination with `lower()` or `upper()` on both operands.

**Example**

```cpp
String text("Hello");
String shout = text.upper();  // "HELLO"
```

*This example transforms all letters to uppercase.*

---

#### `String lower() const`

**Signature**

```cpp
String lower() const;
```

**Synopsis**
Returns a new string with all characters lowercase.

**Parameters**

* *(none)*

**Return value**

* New `String` where applicable characters are converted to lowercase.

**In Depth**

`lower()` is the counterpart to `upper()`. It is particularly useful for simple case-insensitive comparisons:

```cpp
a.lower().equals(b.lower())
```

**Example**

```cpp
String text("HeLLo");
String normalized = text.lower();  // "hello"
```

*This example converts mixed-case text to lowercase.*

---

#### `String reverse() const`

**Signature**

```cpp
String reverse() const;
```

**Synopsis**
Returns a new string with characters in reverse order.

**Parameters**

* *(none)*

**Return value**

* New `String` containing the characters of the original in reverse order.

**In Depth**

`reverse()` operates at the **character** level, so multi-byte UTF-8 characters remain intact and are not broken into bytes.

**Example**

```cpp
String text("abc");
String reversed = text.reverse();  // "cba"
```

*This example reverses the order of characters in a string.*

---

#### `String repeat(int count) const`

**Signature**

```cpp
String repeat(int count) const;
```

**Synopsis**
Returns a new string with content repeated count times.

**Parameters**

* `count` – Number of times to repeat the entire string.

**Return value**

* New `String` equal to this string concatenated with itself `count` times.

**In Depth**

If `count` is:

* `0` – The result is an empty string.
* `1` – The result is a copy of the original.
* Negative – Behavior is implementation-defined; pass non-negative counts.

**Example**

```cpp
String dot(".");
String ellipsis = dot.repeat(3);  // "..."
```

*This example repeats a single-character string to build an ellipsis.*

---

### Equality, Comparison, Concatenation, and Hashing

#### `bool equals(const String &other) const`

**Signature**

```cpp
bool equals(const String &other) const;
```

**Synopsis**
Returns true if this string equals the other string.

**Parameters**

* `other` – String to compare against.

**Return value**

* `true` if both strings contain exactly the same sequence of bytes; `false` otherwise.

**In Depth**

`equals()` performs an exact, case-sensitive equality check. It is equivalent to `operator==`, but some developers prefer the named method for clarity, especially when chaining or passing as a callback.

**Example**

```cpp
String a("test");
String b("test");
bool same = a.equals(b);  // true
```

*This example compares two strings using the `equals()` method.*

---

#### `int compare(const String &other) const`

**Signature**

```cpp
int compare(const String &other) const;
```

**Synopsis**
Compares lexicographically. Returns <0, 0, or >0.

**Parameters**

* `other` – String to compare with.

**Return value**

* Negative value if `*this` is lexicographically less than `other`.
* `0` if they are equal.
* Positive value if `*this` is lexicographically greater than `other`.

**In Depth**

`compare()` performs a lexicographical comparison based on the underlying byte sequence (or character sequence, depending on implementation). It is useful for sorting and ordering operations.

**Example**

```cpp
String a("apple");
String b("banana");

int result = a.compare(b);  // result < 0
```

*This example compares two strings to determine their sort order.*

---

#### `bool operator==(const String &other) const`

**Signature**

```cpp
bool operator==(const String &other) const;
```

**Synopsis**
Returns true if this string equals the other string.

**Parameters**

* `other` – String to compare against.

**Return value**

* `true` if strings are equal; `false` otherwise.

**In Depth**

This operator is a more idiomatic shorthand for `equals(other)`. It is used in the usual C++ way:

```cpp
if (a == b) { ... }
```

**Example**

```cpp
String a("hello");
String b("hello");
String c("world");

bool ab = (a == b);  // true
bool ac = (a == c);  // false
```

*This example uses the equality operator to compare strings.*

---

#### `bool operator!=(const String &other) const`

**Signature**

```cpp
bool operator!=(const String &other) const;
```

**Synopsis**
Returns true if this string does not equal the other string.

**Parameters**

* `other` – String to compare against.

**Return value**

* `true` if strings are not equal; `false` if they are equal.

**In Depth**

This is simply the logical negation of `operator==`.

**Example**

```cpp
String a("hello");
String b("world");

if (a != b) {
  // strings differ
}
```

*This example branches when two strings are not equal.*

---

#### `String operator+(const String &other) const`

**Signature**

```cpp
String operator+(const String &other) const;
```

**Synopsis**
Returns the concatenation of this string and other.

**Parameters**

* `other` – String to append to the end of this string.

**Return value**

* New `String` containing the concatenation of `*this` and `other`.

**In Depth**

This operator is the primary way to join strings. It does not modify either operand; it returns a new string containing the combined text.

**Example**

```cpp
String first("Hello");
String second(" world");
String combined = first + second;  // "Hello world"
```

*This example concatenates two strings using the `+` operator.*

---

#### `int hash() const`

**Signature**

```cpp
int hash() const;
```

**Synopsis**
Returns a hash code for this string.

**Parameters**

* *(none)*

**Return value**

* Integer hash code derived from the string content.

**In Depth**

The hash code is suitable for use in hash tables or sets. The exact algorithm is not specified and may change between versions of the library, so you should not persist hash codes or rely on them being stable.

**Example**

```cpp
String key("user:42");
int h = key.hash();
// h can be used in a custom hash-based structure
```

*This example computes a hash code from a string key.*

---

### Splitting, Joining, and Line Handling

#### `List lines() const`

**Signature**

```cpp
List lines() const;
```

**Synopsis**
Splits this string by newlines into a list.

**Parameters**

* *(none)*

**Return value**

* A `List` of `String` values, each representing one line.

**In Depth**

`lines()` breaks the string into individual lines at newline boundaries. The exact handling of `'\r\n'` vs. `'\n'` is implementation-specific, but typical behavior is to split on `'\n'` and strip a trailing `'\r'`.

Use this when processing configuration files, logs, or any multi-line text.

**Example**

```cpp
String text("first line\nsecond line");
List ls = text.lines();
// ls[0] == "first line"
// ls[1] == "second line"
```

*This example splits a multi-line string into separate lines.*

---

#### `String join(const List &list) const`

**Signature**

```cpp
String join(const List &list) const;
```

**Synopsis**
Joins list elements using this string as separator.

**Parameters**

* `list` – `List` of values to join. Each element is converted to `String`.

**Return value**

* New `String` consisting of all elements joined with `*this` inserted between them.

**In Depth**

`join()` treats the current `String` as a **separator**. For example, `String(", ").join(items)` joins items with `", "` between them.

If `list` is empty, the result is an empty string. If `list` has one element, no separator is inserted.

**Example**

```cpp
List items;
items.append("a").append("b").append("c");

String sep(", ");
String joined = sep.join(items);  // "a, b, c"
```

*This example joins list items with a comma-and-space separator.*

---

#### `List split(const String &sep, int max = 1) const`

**Signature**

```cpp
List split(const String &sep, int max = 1) const;
```

**Synopsis**
Splits by separator into a list. max controls maximum splits.

**Parameters**

* `sep` – Non-empty separator substring.
* `max` – Maximum number of splits to perform. The default `1` means “at most one split”, resulting in up to two elements.

**Return value**

* `List` of `String` segments.

**In Depth**

This function splits the string at each occurrence of `sep`, up to `max` times:

* If `max == 1` (default), you get at most two pieces: before the first `sep`, and everything after it.
* If `max <= 0`, behavior is implementation-defined; pass `max >= 1`.

The separator is not included in the result.

**Example**

```cpp
String text("key=value=extra");
List parts = text.split("=", 1);
// parts[0] == "key"
// parts[1] == "value=extra"
```

*This example splits at the first `=` only, leaving the rest of the string intact.*

---

#### `List split() const`

**Signature**

```cpp
List split() const;
```

**Synopsis**
Splits by whitespace into a list.

**Parameters**

* *(none)*

**Return value**

* `List` of `String` tokens separated by one or more whitespace characters.

**In Depth**

This convenience overload splits the string into tokens separated by whitespace (spaces, tabs, newlines, etc.). Consecutive whitespace is typically treated as a single separator.

Empty tokens (from leading or trailing whitespace) are generally discarded.

**Example**

```cpp
String text(" one  two   three ");
List tokens = text.split();
// tokens[0] == "one"
// tokens[1] == "two"
// tokens[2] == "three"
```

*This example tokenizes a string based on whitespace.*

---

### Formatting

#### `String format(const List &list) const`

**Signature**

```cpp
String format(const List &list) const;
```

**Synopsis**
Replaces {0}, {1}, etc. with list elements.

**Parameters**

* `list` – `List` whose elements will be substituted into the template string.

**Return value**

* New `String` with `{n}` placeholders replaced by the corresponding element’s string representation.

**In Depth**

This function treats the current string as a **template** and replaces placeholders of the form `{0}`, `{1}`, `{2}`, … with the `String` representation of `list[0]`, `list[1]`, etc.

Example template:

* Template: `"Hello, {0}! You have {1} messages."`
* List: `["Alice", 3]`
* Result: `"Hello, Alice! You have 3 messages."`

If a placeholder index is outside the bounds of `list`, the behavior is implementation-defined; you should ensure the list contains all needed values.

**Example**

```cpp
String tmpl("Hello, {0}! You have {1} messages.");

List args;
args.append("Alice").append(3);

String message = tmpl.format(args);
// "Hello, Alice! You have 3 messages."
```

*This example uses list-based placeholders to format a greeting.*

---

#### `String format(const Map &map) const`

**Signature**

```cpp
String format(const Map &map) const;
```

**Synopsis**
Replaces {key} placeholders with map values.

**Parameters**

* `map` – `Map` providing values for named placeholders.

**Return value**

* New `String` with `{key}` placeholders replaced by the corresponding map value’s string representation.

**In Depth**

This overload supports **named placeholders**, which is often clearer than numeric indices. For example:

* Template: `"Name: {name}, Age: {age}"`
* Map: `{ "name": "Alice", "age": 30 }`
* Result: `"Name: Alice, Age: 30"`

If a placeholder key does not exist in the map, the behavior is implementation-defined; best practice is to ensure the map includes all referenced keys.

**Example**

```cpp
String tmpl("Name: {name}, Age: {age}");

Map args;
args.put("name", "Alice").put("age", 30);

String result = tmpl.format(args);
// "Name: Alice, Age: 30"
```

*This example uses a map to fill named placeholders in a template string.*

---

### Related Non-member Operator

#### `String operator+(const char *lhs, const String &rhs)`

**Signature**

```cpp
inline String operator+(const char *lhs, const String &rhs) {
  return String(lhs) + rhs;
}
```

**Synopsis**
String concatenation operator for natural syntax: "Hello " + myString

**Parameters**

* `lhs` – C-style UTF-8 string on the left-hand side.
* `rhs` – `String` on the right-hand side.

**Return value**

* New `String` equal to `String(lhs).operator+(rhs)`.

**In Depth**

This non-member operator allows you to write expressions like:

```cpp
"Hello " + myString
```

instead of needing to construct a `String` explicitly on the left. It improves readability when combining string literals and `String` objects.

**Example**

```cpp
String name("Alice");
String greeting = "Hello, " + name;  // "Hello, Alice"
```

*This example uses the free `operator+` to concatenate a literal and a `String` in natural order.*
