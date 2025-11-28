# Buffer

## Overview

The `attoboy::Buffer` class is the core **binary data container** in the attoboy library. Where `String` is designed for human-readable text, `Buffer` is designed for **raw bytes**: file contents, network packets, encrypted payloads, compressed data, and any other non-text information.

Key characteristics:

* **Mutable**: operations like `append`, `prepend`, `insert`, `remove`, and `reverse` modify the buffer in place and return `*this` for chaining.
* **Byte-oriented**: all indices and lengths are expressed in bytes, not characters.
* **Feature-rich**: supports LZ4 compression, ChaCha20 encryption, Base64 encoding, and conversion to and from `String`.
* **RAII-based**: memory for the buffer is automatically managed; you never call `free()` yourself.

At a high level, you can think of `Buffer` as a small, focused replacement for a combination of `std::vector<unsigned char>` and various utility libraries for compression, encryption, and encoding—implemented in a tiny-binary-friendly way.

### Bytes and Binary Data

In contrast to `String`, which deals in **Unicode characters**, `Buffer` deals in **bytes**:

* Each element is an `unsigned char` (0–255).
* All indexing is by byte offset from the start (0-based).
* No attempt is made to interpret the bytes as text unless you explicitly call `toString()` or construct from a `String`.

Typical uses include:

* Reading and writing files via `File` and `Path`.
* Handling binary responses from `WebRequest` via `WebResponse::asBuffer()`.
* Managing compressed or encrypted data.

### Mutability and Chaining

Most mutating operations on `Buffer`:

* Modify the buffer **in place**.
* Return `Buffer &` so you can chain calls:

```cpp
using namespace attoboy;

Buffer buf;
buf.append("header")
   .append("payload")
   .reverse()
   .trim();
```

This is different from `String`, which is immutable and always returns new values.

### Compression (LZ4) and Encoding (Base64)

`Buffer` supports two common operations on binary data:

* **LZ4 compression** (`compress()` / `decompress()`):

  * LZ4 is a fast, lossless compression algorithm.
  * Use `compress()` to reduce storage or bandwidth for already-compressible data (e.g., text, JSON, logs).
  * Use `decompress()` to reverse the operation on data previously produced by `compress()`.

* **Base64 encoding** (`toBase64()` / `fromBase64()`):

  * Base64 turns arbitrary bytes into a `String` that uses only ASCII characters.
  * This is useful when you must send binary data through text-only channels (e.g., JSON, some APIs, logs).

### Encryption with ChaCha20

`Buffer::crypt()` provides symmetric encryption/decryption using **ChaCha20**, a modern stream cipher:

* **Symmetric** means the same function and key are used for both encryption and decryption.
* You must provide:

  * A **key** of at least 32 bytes.
  * A **nonce** (sometimes called an initialization vector) of at least 12 bytes.

The important property is:

```cpp
encrypted = plain.crypt(key, nonce);
decrypted = encrypted.crypt(key, nonce); // same key+nonce
// decrypted contains the original bytes
```

This makes it straightforward to protect sensitive data in files, configuration, or network messages.

### Interoperability with `String` and Other attoboy Types

`Buffer` integrates naturally with the rest of the library:

* Construct a `Buffer` from a `String`’s bytes and back via `toString()`.
* Use `File` to read and write `Buffer` objects to disk or sockets.
* Use `WebResponse::asBuffer()` to get the raw body of HTTP responses.
* Use `Buffer::toBase64()` and `Buffer::fromBase64()` to move between binary and text representations.

---

## Reference

Each entry below covers one public constructor, method, static function, or operator of `attoboy::Buffer`. For each, you will find:

* **Signature** – the exact declaration from the header.
* **Synopsis** – the original one-line Doxygen comment.
* **Parameters** and **Return value** descriptions.
* **In Depth** – details, caveats, and a short usage example.

> **Note:** All examples assume `using namespace attoboy;`.

---

#### `Buffer()`

**Signature**

```cpp
Buffer();
```

**Synopsis**
Creates an empty buffer.

**Parameters**

* *(none)*

**Return value**

* *(constructor; not applicable)*

**In Depth**

The default constructor creates a buffer with:

* Length `0`.
* No allocated contents (or a minimal internal representation).
* `isEmpty()` returning `true`.

You typically start with an empty buffer and then build it up with `append`, `prepend`, or `insert`.

**Example**

```cpp
Buffer buf;
bool empty = buf.isEmpty();  // true
```

*This example constructs an empty buffer and checks that it contains no bytes.*

---

#### `Buffer(int capacity)`

**Signature**

```cpp
Buffer(int capacity);
```

**Synopsis**
Creates an empty buffer with reserved capacity.

**Parameters**

* `capacity` – Number of bytes to reserve internally.

**Return value**

* *(constructor; not applicable)*

**In Depth**

This constructor allocates internal storage for at least `capacity` bytes but keeps the **length** at zero. It is useful when you know in advance approximately how many bytes you will be appending and want to minimize reallocations.

Even with reserved capacity:

* `length()` is `0`.
* `isEmpty()` is `true`.

**Example**

```cpp
Buffer buf(1024);  // reserve space for ~1 KB
buf.append("header");
```

*This example reserves capacity for efficient growth before appending data.*

---

#### `Buffer(const String &str)`

**Signature**

```cpp
Buffer(const String &str);
```

**Synopsis**
Creates a buffer by copying a string's bytes.

**Parameters**

* `str` – `String` whose UTF-8 bytes will be copied into the buffer.

**Return value**

* *(constructor; not applicable)*

**In Depth**

This constructor copies the raw UTF-8 bytes from `str` (as returned by `str.c_str()` and `str.byteLength()`) into the buffer. The buffer length becomes the number of bytes in the string, and there is no null terminator added beyond the stored length.

Use this whenever you need a binary representation of text for hashing, encryption, or other binary protocols.

**Example**

```cpp
String text("Hello, world");
Buffer buf(text);  // contains the UTF-8 bytes of the string
int len = buf.length();  // equal to text.byteLength()
```

*This example converts a `String` into a binary buffer with the same UTF-8 bytes.*

---

#### `Buffer(const unsigned char *ptr, int size)`

**Signature**

```cpp
Buffer(const unsigned char *ptr, int size);
```

**Synopsis**
Creates a buffer by copying bytes from a pointer.

**Parameters**

* `ptr` – Pointer to a sequence of bytes.
* `size` – Number of bytes to copy from `ptr`.

**Return value**

* *(constructor; not applicable)*

**In Depth**

This constructor is the low-level bridge from raw memory to `Buffer`. It copies exactly `size` bytes from `ptr` into the buffer. The bytes are treated as opaque; no interpretation as text is performed.

Using this is appropriate when:

* You receive bytes from an external API.
* You have a fixed-size array or struct you want to store as-is.

**Example**

```cpp
unsigned char raw[4] = {0x01, 0x02, 0x03, 0x04};
Buffer buf(raw, 4);
int len = buf.length();  // 4
```

*This example constructs a buffer from a small in-memory byte array.*

---

#### `Buffer(const Buffer &other)`

**Signature**

```cpp
Buffer(const Buffer &other);
```

**Synopsis**
Creates a copy of another buffer.

**Parameters**

* `other` – Existing `Buffer` to copy.

**Return value**

* *(constructor; not applicable)*

**In Depth**

The copy constructor creates a new `Buffer` with the same contents as `other`. Since `Buffer` is mutable, future modifications to one buffer do not affect the other.

**Example**

```cpp
Buffer original;
original.append("data");

Buffer copy(original);
// copy now has the same bytes as original
```

*This example duplicates the contents of an existing buffer.*

---

#### `~Buffer()`

**Signature**

```cpp
~Buffer();
```

**Synopsis**
Destroys the buffer and frees memory.

**Parameters**

* *(none)*

**Return value**

* *(destructor; not applicable)*

**In Depth**

When a `Buffer` goes out of scope, its destructor releases any internally allocated memory. This uses RAII semantics; you do not need to remember to free or delete anything manually.

**Example**

```cpp
{
  Buffer temp;
  temp.append("temporary");
} // temp is destroyed here; memory is freed
```

*This example shows automatic cleanup of a buffer when it leaves scope.*

---

#### `Buffer &operator=(const Buffer &other)`

**Signature**

```cpp
Buffer &operator=(const Buffer &other);
```

**Synopsis**
Assigns another buffer to this buffer.

**Parameters**

* `other` – Buffer whose contents should be copied into `*this`.

**Return value**

* Reference to `*this` for assignment chaining.

**In Depth**

The assignment operator replaces the contents of the left-hand side with a copy of `other`. Any previous bytes in the buffer are discarded. Self-assignment is safe.

**Example**

```cpp
Buffer a;
a.append("first");

Buffer b;
b.append("second");

a = b;  // a now contains "second"
```

*This example assigns one buffer’s contents to another.*

---

#### `int length() const`

**Signature**

```cpp
int length() const;
```

**Synopsis**
Returns the number of bytes in the buffer.

**Parameters**

* *(none)*

**Return value**

* Current length of the buffer, in bytes.

**In Depth**

`length()` is the primary way to measure how much data the buffer currently holds. It does not include any extra capacity reserved internally.

Use this when iterating over bytes, validating size constraints, or preparing to write the buffer to a file or socket.

**Example**

```cpp
Buffer buf;
buf.append("ABC");
int n = buf.length();  // 3
```

*This example checks how many bytes are stored in the buffer.*

---

#### `bool isEmpty() const`

**Signature**

```cpp
bool isEmpty() const;
```

**Synopsis**
Returns true if the buffer is empty.

**Parameters**

* *(none)*

**Return value**

* `true` if `length()` is `0`; `false` otherwise.

**In Depth**

`isEmpty()` is a convenience function equivalent to checking `length() == 0`. It is helpful for early-exit conditions or guarding operations that require data to be present.

**Example**

```cpp
Buffer buf;
if (buf.isEmpty()) {
  buf.append("default");
}
```

*This example assigns default bytes when the buffer is empty.*

---

#### `const unsigned char *c_ptr(int *len) const`

**Signature**

```cpp
const unsigned char *c_ptr(int *len) const;
```

**Synopsis**
Returns a pointer to the data and sets len to the size.

**Parameters**

* `len` – Pointer to an `int` that will be set to the current `length()`.

**Return value**

* Pointer to the internal read-only byte array, or `nullptr` if the buffer is empty (implementation-dependent).

**In Depth**

`c_ptr()` provides a low-level view of the buffer contents:

* It sets `*len` to the number of valid bytes.
* It returns a pointer that remains valid as long as:

  * The `Buffer` exists, and
  * You do not call mutating operations that may reallocate (such as `append`, `insert`, or `remove`).

You must not modify the memory through this pointer.

**Example**

```cpp
Buffer buf;
buf.append("ABC");

int len = 0;
const unsigned char *ptr = buf.c_ptr(&len);
// len == 3; ptr points to bytes 'A', 'B', 'C'
```

*This example obtains a raw pointer and length for interoperability with low-level APIs.*

---

#### `Buffer &clear()`

**Signature**

```cpp
Buffer &clear();
```

**Synopsis**
Removes all bytes. Returns this buffer for chaining.

**Parameters**

* *(none)*

**Return value**

* Reference to `*this`, enabling chained calls.

**In Depth**

`clear()` resets the buffer to an empty state (`length() == 0`). It may keep reserved capacity to reuse for future appends, but the logical content is removed.

**Example**

```cpp
Buffer buf;
buf.append("data");
buf.clear();  // buf is now empty
```

*This example removes all bytes from a buffer so it can be reused.*

---

#### `Buffer &append(const String &str)`

**Signature**

```cpp
Buffer &append(const String &str);
```

**Synopsis**
Appends a string's bytes. Returns this buffer for chaining.

**Parameters**

* `str` – `String` whose UTF-8 bytes will be appended to the buffer.

**Return value**

* Reference to `*this`.

**In Depth**

This method:

* Takes the raw bytes of `str` (via its UTF-8 representation).
* Appends them to the end of the current buffer contents.
* Increases `length()` accordingly.

It does not add any additional terminators or separators.

**Example**

```cpp
Buffer buf;
buf.append("header")
   .append("payload");
// buffer now contains "headerpayload" bytes
```

*This example appends two strings’ bytes to form a single binary sequence.*

---

#### `Buffer &append(const Buffer &other)`

**Signature**

```cpp
Buffer &append(const Buffer &other);
```

**Synopsis**
Appends another buffer's bytes. Returns this buffer for chaining.

**Parameters**

* `other` – `Buffer` whose bytes will be appended.

**Return value**

* Reference to `*this`.

**In Depth**

This method copies all bytes from `other` into the end of the current buffer. It is equivalent to:

```cpp
int len = 0;
const unsigned char *ptr = other.c_ptr(&len);
append(ptr, len);
```

**Example**

```cpp
Buffer a;
a.append("ABC");

Buffer b;
b.append("DEF");

a.append(b);  // a now contains "ABCDEF"
```

*This example concatenates two buffers by appending the second to the first.*

---

#### `Buffer &append(const unsigned char *ptr, int size)`

**Signature**

```cpp
Buffer &append(const unsigned char *ptr, int size);
```

**Synopsis**
Appends bytes from a pointer. Returns this buffer for chaining.

**Parameters**

* `ptr` – Pointer to the bytes to append.
* `size` – Number of bytes to append.

**Return value**

* Reference to `*this`.

**In Depth**

This is the low-level append operation for raw memory:

* It copies `size` bytes starting at `ptr`.
* It does not inspect or modify them.
* It is your responsibility to ensure `ptr` points to at least `size` valid bytes.

**Example**

```cpp
unsigned char footer[2] = {0x0D, 0x0A};  // CR LF
Buffer buf;
buf.append("line").append(footer, 2);
// buf now has "line\r\n"
```

*This example appends a raw two-byte sequence to existing data.*

---

#### `Buffer &prepend(const String &str)`

**Signature**

```cpp
Buffer &prepend(const String &str);
```

**Synopsis**
Prepends a string's bytes. Returns this buffer for chaining.

**Parameters**

* `str` – `String` whose bytes will be inserted at the beginning.

**Return value**

* Reference to `*this`.

**In Depth**

`prepend()` inserts the bytes of `str` at the front of the buffer, shifting existing bytes toward higher indices.

**Example**

```cpp
Buffer buf;
buf.append("body");
buf.prepend("header");
// buffer contains "headerbody"
```

*This example prepends a header in front of existing data.*

---

#### `Buffer &prepend(const Buffer &other)`

**Signature**

```cpp
Buffer &prepend(const Buffer &other);
```

**Synopsis**
Prepends another buffer's bytes. Returns this buffer for chaining.

**Parameters**

* `other` – Buffer whose bytes will be inserted at the beginning.

**Return value**

* Reference to `*this`.

**In Depth**

This method copies all bytes from `other` to the front of the current buffer, before any existing data.

**Example**

```cpp
Buffer header;
header.append("HDR");

Buffer payload;
payload.append("DATA");

payload.prepend(header);  // payload now "HDRDATA"
```

*This example prepends a header buffer to a payload buffer.*

---

#### `Buffer &prepend(const unsigned char *ptr, int size)`

**Signature**

```cpp
Buffer &prepend(const unsigned char *ptr, int size);
```

**Synopsis**
Prepends bytes from a pointer. Returns this buffer for chaining.

**Parameters**

* `ptr` – Pointer to bytes to prepend.
* `size` – Number of bytes to prepend.

**Return value**

* Reference to `*this`.

**In Depth**

This is the raw-memory variant of `prepend`. It inserts the given bytes at index `0`. Existing content is shifted to make room.

**Example**

```cpp
unsigned char magic[2] = {0xAB, 0xCD};
Buffer buf;
buf.append("DATA");
buf.prepend(magic, 2);
// buffer starts with magic bytes followed by "DATA"
```

*This example prepends a small binary prefix to the buffer.*

---

#### `Buffer &insert(int index, const String &str)`

**Signature**

```cpp
Buffer &insert(int index, const String &str);
```

**Synopsis**
Inserts a string's bytes at index. Returns this buffer for chaining.

**Parameters**

* `index` – Byte index at which to insert the bytes (0-based).
* `str` – `String` whose bytes will be inserted.

**Return value**

* Reference to `*this`.

**In Depth**

This method:

* Interprets `index` as a byte position between `0` and `length()`.
* Inserts the UTF-8 bytes of `str` at that position.
* Shifts existing bytes at and after `index` toward the end.

You should ensure `0 <= index <= length()`; values outside this range are implementation-defined.

**Example**

```cpp
Buffer buf;
buf.append("Hello world");
buf.insert(5, ",");  // insert comma after "Hello"
// buffer bytes now represent "Hello, world"
```

*This example inserts a string’s bytes into the middle of existing data.*

---

#### `Buffer &insert(int index, const Buffer &other)`

**Signature**

```cpp
Buffer &insert(int index, const Buffer &other);
```

**Synopsis**
Inserts another buffer's bytes at index. Returns this buffer for chaining.

**Parameters**

* `index` – Byte index at which to insert.
* `other` – Buffer whose bytes will be inserted.

**Return value**

* Reference to `*this`.

**In Depth**

This variant inserts all bytes from `other` at the specified position. It behaves like splicing the contents of `other` into this buffer.

**Example**

```cpp
Buffer buf;
buf.append("Hello world");

Buffer comma;
comma.append(",");

buf.insert(5, comma);
// same result as using the string variant
```

*This example inserts one buffer into another at a specific byte index.*

---

#### `Buffer &insert(int index, const unsigned char *ptr, int size)`

**Signature**

```cpp
Buffer &insert(int index, const unsigned char *ptr, int size);
```

**Synopsis**
Inserts bytes from a pointer at index. Returns this buffer for chaining.

**Parameters**

* `index` – Byte index where the insertion begins.
* `ptr` – Pointer to the bytes to insert.
* `size` – Number of bytes to insert.

**Return value**

* Reference to `*this`.

**In Depth**

This is the raw-memory form of `insert`. Use it when you have bytes in an array or external buffer that you want to splice into this `Buffer`.

**Example**

```cpp
unsigned char marker[3] = {0x01, 0x02, 0x03};
Buffer buf;
buf.append("DATA");
buf.insert(0, marker, 3);
// buf begins with three marker bytes, followed by "DATA"
```

*This example splices a marker sequence at the beginning of the buffer.*

---

#### `Buffer &remove(int start, int end = -1)`

**Signature**

```cpp
Buffer &remove(int start, int end = -1);
```

**Synopsis**
Removes bytes from start to end. Returns this buffer for chaining.

**Parameters**

* `start` – Byte index of the first byte to remove.
* `end` – Byte index **after** the last byte to remove (exclusive). If `end` is `-1`, removal continues to the end of the buffer.

**Return value**

* Reference to `*this`.

**In Depth**

`remove()` deletes a contiguous range of bytes. After removal, later bytes are shifted to fill the gap and `length()` is reduced.

Typical patterns:

* `remove(0, n)` – remove the first `n` bytes.
* `remove(offset, -1)` – remove all bytes from `offset` to the end.

Indices must refer to valid byte positions within `[0, length()]`; behavior is undefined otherwise.

**Example**

```cpp
Buffer buf;
buf.append("Hello, world");
buf.remove(5, 6);  // remove the comma
// buffer now represents "Hello world"
```

*This example removes one byte from the buffer by specifying a start/end range.*

---

#### `Buffer &reverse()`

**Signature**

```cpp
Buffer &reverse();
```

**Synopsis**
Reverses byte order in place. Returns this buffer for chaining.

**Parameters**

* *(none)*

**Return value**

* Reference to `*this`.

**In Depth**

`reverse()` inverts the order of bytes:

* Byte at index `0` swaps with byte at index `length() - 1`, and so on.

This is purely a byte-wise operation. If the buffer contains multi-byte structures (like UTF-8 text or multi-byte integers), their internal order is not preserved and may become invalid.

**Example**

```cpp
Buffer buf;
buf.append("ABC");
buf.reverse();
// buffer now contains "CBA"
```

*This example reverses the order of bytes in the buffer.*

---

#### `Buffer &trim()`

**Signature**

```cpp
Buffer &trim();
```

**Synopsis**
Shrinks capacity to match length. Returns this buffer for chaining.

**Parameters**

* *(none)*

**Return value**

* Reference to `*this`.

**In Depth**

`trim()` reduces the internal capacity of the buffer to match its current `length()`. This can:

* Free unused memory when you know no further growth is needed.
* Make the buffer more compact before serialization or long-term storage.

It does not change the content or `length()`.

**Example**

```cpp
Buffer buf(1024);  // initially reserved
buf.append("small");
buf.trim();        // capacity shrinks to a small size
```

*This example reserves extra space, then trims it away after finalizing the content.*

---

#### `Buffer slice(int start, int end = -1) const`

**Signature**

```cpp
Buffer slice(int start, int end = -1) const;
```

**Synopsis**
Returns a new buffer with bytes from start to end.

**Parameters**

* `start` – Byte index of the first byte to include.
* `end` – Byte index **after** the last byte to include (exclusive). If `end` is `-1`, the slice extends to the end of the buffer.

**Return value**

* New `Buffer` containing the bytes from the specified range.

**In Depth**

`slice()` is the non-mutating counterpart to `remove()`. It creates a new buffer from the selected portion without modifying the original.

Use it to extract segments such as headers, payloads, or individual records from a larger buffer.

**Example**

```cpp
Buffer buf;
buf.append("HEADERPAYLOAD");

Buffer header = buf.slice(0, 6);  // "HEADER"
Buffer payload = buf.slice(6);    // "PAYLOAD"
```

*This example splits a buffer into header and payload sub-buffers using `slice()`.*

---

#### `Buffer compress() const`

**Signature**

```cpp
Buffer compress() const;
```

**Synopsis**
Returns an LZ4-compressed version of this buffer.

**Parameters**

* *(none)*

**Return value**

* New `Buffer` containing the compressed data.

**In Depth**

`compress()` applies LZ4 compression to the current bytes:

* The result can be smaller than the original for compressible data (such as text or JSON).
* For already-compressed or random data, the result may be similar in size or slightly larger.

You should only call `decompress()` on data previously produced by `compress()` (or compatible LZ4 encoders).

**Example**

```cpp
Buffer original;
original.append("Lots of repetitive text... Lots of repetitive text...");

Buffer compressed = original.compress();
```

*This example compresses a buffer that contains compressible text.*

---

#### `Buffer decompress() const`

**Signature**

```cpp
Buffer decompress() const;
```

**Synopsis**
Returns a decompressed version of this buffer.

**Parameters**

* *(none)*

**Return value**

* New `Buffer` containing the decompressed data.

**In Depth**

`decompress()` reverses LZ4 compression on data previously produced by `compress()`. If the buffer content is not valid compressed data, the behavior is implementation-defined (typically an error or an empty result).

You should:

* Ensure you know whether a given buffer is compressed before calling this.
* Use simple conventions (such as file extensions or header markers) to keep track of compression status in your application.

**Example**

```cpp
Buffer original;
original.append("Text to compress");

Buffer compressed = original.compress();
Buffer restored   = compressed.decompress();
// restored should contain the same bytes as original
```

*This example shows compressing and then decompressing a buffer to recover the original bytes.*

---

#### `Buffer crypt(const String &key, const String &nonce) const`

**Signature**

```cpp
Buffer crypt(const String &key, const String &nonce) const;
```

**Synopsis**
Encrypts/decrypts using ChaCha20 (symmetric). Key ≥32 bytes, nonce ≥12 bytes.

**Parameters**

* `key` – `String` whose bytes act as the encryption key (at least 32 bytes).
* `nonce` – `String` whose bytes act as the nonce (at least 12 bytes).

**Return value**

* New `Buffer` containing the encrypted or decrypted bytes.

**In Depth**

This overload uses both key and nonce supplied as `String` values. ChaCha20 is a symmetric cipher: applying `crypt()` twice with the **same** key and nonce recovers the original bytes.

Notes:

* The key and nonce lengths must meet or exceed the stated thresholds.
* If they do not, or if other low-level errors occur, the behavior is implementation-defined; treat this as a failure case.

**Example**

```cpp
String key("0123456789abcdef0123456789abcdef");  // 32-byte key
String nonce("unique-nonce-123");               // ≥12 bytes

Buffer plain;
plain.append("secret data");

Buffer encrypted = plain.crypt(key, nonce);
Buffer decrypted = encrypted.crypt(key, nonce);
// decrypted should match plain
```

*This example encrypts and then decrypts data using `String`-based key and nonce.*

---

#### `Buffer crypt(const String &key, const Buffer &nonce) const`

**Signature**

```cpp
Buffer crypt(const String &key, const Buffer &nonce) const;
```

**Synopsis**
Encrypts/decrypts using ChaCha20 (symmetric). Key ≥32 bytes, nonce ≥12 bytes.

**Parameters**

* `key` – Encryption key as a `String` (bytes used as key material).
* `nonce` – Encryption nonce as a `Buffer` (its bytes are used as the nonce).

**Return value**

* New `Buffer` containing the encrypted or decrypted data.

**In Depth**

This variant is useful when your nonce is naturally binary (e.g., generated randomness) but you still want to manage the key as a `String`. The semantics are identical to the other overload: same key+nonce pair must be used for both encryption and decryption.

**Example**

```cpp
String key("0123456789abcdef0123456789abcdef");

Buffer nonce;
nonce.append("nonce-binary");  // binary nonce

Buffer data;
data.append("payload");

Buffer encrypted = data.crypt(key, nonce);
Buffer decrypted = encrypted.crypt(key, nonce);
```

*This example uses a string key with a binary nonce stored in a buffer.*

---

#### `Buffer crypt(const Buffer &key, const String &nonce) const`

**Signature**

```cpp
Buffer crypt(const Buffer &key, const String &nonce) const;
```

**Synopsis**
Encrypts/decrypts using ChaCha20 (symmetric). Key ≥32 bytes, nonce ≥12 bytes.

**Parameters**

* `key` – Encryption key bytes stored in a `Buffer`.
* `nonce` – Nonce stored in a `String`.

**Return value**

* New `Buffer` with encrypted or decrypted data.

**In Depth**

This overload is the mirror of the previous one: key as `Buffer`, nonce as `String`. This can be useful if your key material originates as binary data (e.g., loaded from a file or generated randomly) but your nonce is text-based.

**Example**

```cpp
Buffer key;
key.append("0123456789abcdef0123456789abcdef");

String nonce("nonce-text-123");

Buffer payload;
payload.append("binary payload");

Buffer encrypted = payload.crypt(key, nonce);
Buffer decrypted = encrypted.crypt(key, nonce);
```

*This example uses a binary key buffer combined with a textual nonce.*

---

#### `Buffer crypt(const Buffer &key, const Buffer &nonce) const`

**Signature**

```cpp
Buffer crypt(const Buffer &key, const Buffer &nonce) const;
```

**Synopsis**
Encrypts/decrypts using ChaCha20 (symmetric). Key ≥32 bytes, nonce ≥12 bytes.

**Parameters**

* `key` – Encryption key bytes as a `Buffer`.
* `nonce` – Nonce bytes as a `Buffer`.

**Return value**

* New `Buffer` containing encrypted or decrypted data.

**In Depth**

This is the most general overload: both key and nonce are binary. Use it when you manage both as pure binary data, perhaps generated through a secure random API or stored in binary files.

As always:

* The same key+nonce must be used to decrypt.
* Length requirements must be respected.

**Example**

```cpp
Buffer key;
key.append("0123456789abcdef0123456789abcdef");  // ≥32 bytes

Buffer nonce;
nonce.append("0123456789ab");                    // ≥12 bytes

Buffer secret;
secret.append("top secret");

Buffer encrypted = secret.crypt(key, nonce);
Buffer decrypted = encrypted.crypt(key, nonce);
```

*This example encrypts and decrypts using binary key and nonce buffers.*

---

#### `String toBase64() const`

**Signature**

```cpp
String toBase64() const;
```

**Synopsis**
Converts the buffer to a Base64-encoded string.

**Parameters**

* *(none)*

**Return value**

* `String` containing the Base64 representation of the buffer’s bytes.

**In Depth**

Base64 encoding maps arbitrary binary data to a limited set of ASCII characters. Use this when:

* You need to embed binary data into JSON or other text-based formats.
* You want a printable representation of a buffer for logs or debugging.

The resulting `String` can be converted back to a `Buffer` via `fromBase64()`.

**Example**

```cpp
Buffer data;
data.append("binary data");

String encoded = data.toBase64();
// encoded can safely be placed in JSON or logs
```

*This example encodes buffer contents as Base64 text.*

---

#### `static Buffer fromBase64(const String &base64)`

**Signature**

```cpp
static Buffer fromBase64(const String &base64);
```

**Synopsis**
Creates a buffer from a Base64-encoded string.

**Parameters**

* `base64` – `String` containing Base64-encoded text.

**Return value**

* New `Buffer` containing the decoded bytes.

**In Depth**

`fromBase64()` decodes Base64 text back into raw bytes:

* It expects `base64` to be valid Base64.
* On invalid input, behavior is implementation-defined (likely an empty buffer or partial result). You should treat malformed Base64 as an error in your application.

**Example**

```cpp
Buffer original;
original.append("binary data");

String encoded = original.toBase64();
Buffer decoded = Buffer::fromBase64(encoded);
// decoded should contain the same bytes as original
```

*This example round-trips data through Base64 encoding and decoding.*

---

#### `String toString() const`

**Signature**

```cpp
String toString() const;
```

**Synopsis**
Converts the buffer's bytes to a string.

**Parameters**

* *(none)*

**Return value**

* `String` built from the buffer’s bytes, interpreted as UTF-8.

**In Depth**

`toString()` assumes that the buffer contains valid UTF-8 text and constructs a `String` accordingly. If the bytes are not valid UTF-8, the result may contain replacement characters or undefined behavior depending on the implementation.

Use this only when you know the buffer is holding text (for example, when reading a UTF-8 file or HTTP response body that you know is text).

**Example**

```cpp
Buffer buf;
buf.append("Hello, world");

String text = buf.toString();  // "Hello, world"
```

*This example converts buffer bytes into a text string when the data is known to be UTF-8.*

---

#### `bool compare(const Buffer &other) const`

**Signature**

```cpp
bool compare(const Buffer &other) const;
```

**Synopsis**
Returns true if this buffer equals the other.

**Parameters**

* `other` – Buffer to compare against.

**Return value**

* `true` if both buffers have exactly the same bytes; `false` otherwise.

**In Depth**

`compare()` checks for exact byte-for-byte equality, including length. It is equivalent to `operator==` in behavior but presented as a named method.

**Example**

```cpp
Buffer a;
a.append("DATA");

Buffer b;
b.append("DATA");

bool same = a.compare(b);  // true
```

*This example compares two buffers for exact equality.*

---

#### `int hash() const`

**Signature**

```cpp
int hash() const;
```

**Synopsis**
Returns a hash code for this buffer.

**Parameters**

* *(none)*

**Return value**

* Integer hash code derived from the buffer’s bytes.

**In Depth**

The hash is suitable for organizing buffers in custom hash-based data structures. The algorithm is not specified and may change across versions, so you should not persist hash codes or rely on them being stable between runs.

**Example**

```cpp
Buffer key;
key.append("binary-key");

int h = key.hash();
// use h in a custom hash table or map
```

*This example computes a hash code from buffer contents.*

---

#### `bool operator==(const Buffer &other) const`

**Signature**

```cpp
bool operator==(const Buffer &other) const;
```

**Synopsis**
Returns true if this buffer equals the other.

**Parameters**

* `other` – Buffer to compare with.

**Return value**

* `true` if buffers are equal; `false` otherwise.

**In Depth**

This operator performs a byte-wise equality comparison between two buffers. It is a more idiomatic way to write equality checks than `compare()`.

**Example**

```cpp
Buffer a;
a.append("X");

Buffer b;
b.append("X");

bool equal = (a == b);  // true
```

*This example uses the equality operator to compare two buffers.*

---

#### `bool operator!=(const Buffer &other) const`

**Signature**

```cpp
bool operator!=(const Buffer &other) const;
```

**Synopsis**
Returns true if this buffer does not equal the other.

**Parameters**

* `other` – Buffer to compare with.

**Return value**

* `true` if buffers differ; `false` if they are equal.

**In Depth**

This is simply the negation of `operator==`. It lets you express inequality checks in a natural way.

**Example**

```cpp
Buffer a;
a.append("A");

Buffer b;
b.append("B");

if (a != b) {
  // buffers are different
}
```

*This example branches when two buffers do not contain the same bytes.*

---

#### `Buffer operator+(const String &str) const`

**Signature**

```cpp
Buffer operator+(const String &str) const;
```

**Synopsis**
Returns a new buffer with the string's bytes appended.

**Parameters**

* `str` – `String` whose bytes should be appended to this buffer.

**Return value**

* New `Buffer` containing the original bytes followed by the string’s bytes.

**In Depth**

This operator provides a convenient way to form a new buffer by appending a `String` to an existing buffer without modifying either operand. It behaves like:

```cpp
Buffer result(*this);
result.append(str);
return result;
```

**Example**

```cpp
Buffer prefix;
prefix.append("HDR");

Buffer combined = prefix + String("BODY");
// combined contains bytes for "HDRBODY"
// prefix is unchanged
```

*This example uses `operator+` to build a new buffer from an existing buffer plus a string.*

---

#### `Buffer operator+(const Buffer &other) const`

**Signature**

```cpp
Buffer operator+(const Buffer &other) const;
```

**Synopsis**
Returns a new buffer with the other buffer's bytes appended.

**Parameters**

* `other` – Buffer whose bytes should be appended to this buffer.

**Return value**

* New `Buffer` containing this buffer’s bytes followed by `other`’s bytes.

**In Depth**

This operator is the non-mutating counterpart to `append(other)`. It lets you combine two buffers into a new one without altering the originals.

**Example**

```cpp
Buffer a;
a.append("AAA");

Buffer b;
b.append("BBB");

Buffer combined = a + b;
// combined: "AAABBB"
// a and b remain unchanged
```

*This example concatenates two buffers into a new buffer using `operator+`.*
