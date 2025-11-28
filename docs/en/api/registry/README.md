# `attoboy::Registry` User Guide

## Overview

The `attoboy::Registry` class provides a simple, RAII-style interface to the **Windows Registry**. It lets you:

* Open an existing registry key.
* Create or remove a key.
* Read and write **string**, **integer**, and **binary** values.
* Enumerate value names and subkeys.

All operations are performed through a single `Registry` object that owns (or shares) a handle to a specific key, for example:

* `HKEY_CURRENT_USER\Software\MyApp`
* `HKEY_LOCAL_MACHINE\Software\Vendor\Product`

The class is designed so that:

* **Construction** opens or references a key path.
* **Destruction** automatically closes the underlying handle.
* **Copies and assignments** share the same underlying key (lightweight handle sharing).

This makes it easy to interact with the registry without worrying about manual handle management.

### The Windows Registry in Brief

The Windows Registry is a hierarchical database used by Windows and many applications to store configuration data. Key concepts:

* **Hives** – Top-level root keys such as:

  * `HKEY_CURRENT_USER` (current user’s settings)
  * `HKEY_LOCAL_MACHINE` (machine-wide settings)
* **Keys** – Similar to directories in a filesystem. Example:
  `HKEY_CURRENT_USER\Software\MyApp`
* **Values** – Named entries under a key, each having:

  * A **name** (string, which may be empty for the **default** value).
  * A **type** (string, integer, binary, etc.).
  * Data (the actual value).

In `attoboy::Registry`, you specify keys using a **single string path**, such as:

```cpp
"HKEY_CURRENT_USER\\Software\\MyApp"
```

Note the doubled backslashes (`\\`) in source code to represent literal `\`.

### Registry Keys, Values, and the Default Value

Within a given key:

* Each named value has a unique **name** string, like `"InstallPath"` or `"WindowWidth"`.
* There may also be an **unnamed default value**. In `attoboy::Registry`, you refer to this default value by passing an **empty name** (`String()`) or omitting the parameter when a default argument is provided.

For example:

* `valueExists()` with no arguments checks whether the default value exists.
* `valueExists("InstallPath")` checks a named value.

### RAII and Handle Sharing

The `Registry` class follows RAII:

* When a `Registry` object is constructed, it opens or references a registry key path.
* When it is destroyed, it closes the underlying handle (if it owns one).

Copying or assigning `Registry` objects:

* Does **not** duplicate the key; instead, all copies **share the same underlying handle**.
* This is efficient and safe: closing any one `Registry` instance releases the shared resource when the last reference goes out of scope.

You can therefore pass `Registry` objects around by value without worrying about leaking or duplicating system resources.

---

## Reference

Each entry below covers one public constructor, method, or operator of `attoboy::Registry`. For each, you will find:

* **Signature** – the exact declaration from the header.
* **Synopsis** – the original one-line Doxygen comment.
* **Parameters** and **Return value** descriptions.
* **In Depth** – additional details and caveats.
* **Example** – a short, realistic usage snippet with a brief explanation.

All examples assume:

```cpp
using namespace attoboy;
```

---

### Constructors, Copy, Assignment, and Destruction

#### `Registry(const String &key)`

**Signature**

```cpp
Registry(const String &key);
```

**Synopsis**
Opens a registry key (e.g., "HKEY_CURRENT_USER\Software\MyApp").

**Parameters**

* `key` – Full registry path to open, including the root hive, for example
  `"HKEY_CURRENT_USER\\Software\\MyApp"`.

**Return value**

* *(constructor; not applicable)*

**In Depth**

This constructor represents a specific registry key path. Internally, it opens (or prepares to open) the registry key corresponding to the given string. Examples of valid `key` strings include:

* `HKEY_CURRENT_USER\\Software\\MyApp`
* `HKEY_LOCAL_MACHINE\\Software\\Vendor\\Product`

Typical usage patterns:

* Call `exists()` to see if the key already exists.
* Call `create()` to create the key if it does not exist.
* Use `setStringValue`, `setIntegerValue`, etc., to define values under the key.

You should usually choose a subkey under `HKEY_CURRENT_USER` or `HKEY_LOCAL_MACHINE` that is specific to your application (for example, `"HKEY_CURRENT_USER\\Software\\MyCompany\\MyApp"`).

**Example**

```cpp
Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");

if (!reg.exists()) {
  reg.create();
}
```

*This example opens a key under the current user’s hive and creates it if it does not already exist.*

---

#### `Registry(const Registry &other)`

**Signature**

```cpp
Registry(const Registry &other);
```

**Synopsis**
Creates a copy (shares the underlying key).

**Parameters**

* `other` – Existing `Registry` instance to copy.

**Return value**

* *(constructor; not applicable)*

**In Depth**

The copy constructor does not create a new registry key. Instead, it makes the new object share the same underlying handle as `other`. This is efficient and safe:

* You can pass `Registry` instances by value.
* All copies refer to the same key.
* The handle is freed only when the last `Registry` referring to it is destroyed.

Any operation performed via one copy (reading or writing values) affects the same underlying registry key.

**Example**

```cpp
Registry base("HKEY_CURRENT_USER\\Software\\MyApp");
Registry copy(base);  // shares the same key
```

*This example creates a second `Registry` object that refers to the same underlying registry key as the first.*

---

#### `~Registry()`

**Signature**

```cpp
~Registry();
```

**Synopsis**
Closes the key and frees resources.

**Parameters**

* *(none)*

**Return value**

* *(destructor; not applicable)*

**In Depth**

The destructor closes the underlying registry key handle when the last `Registry` object referencing it is destroyed. This is the RAII behavior:

* No manual cleanup is required.
* You cannot accidentally forget to close a key.

Be careful not to keep raw pointers returned by functions like `getStringValue()` or `getBinaryValue()` beyond the lifetime of the `Registry` object, because their validity is tied to the object’s internal resources.

**Example**

```cpp
{
  Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");
  // use reg here
} // reg is destroyed here; the key is closed automatically
```

*This example shows how the key is automatically closed when the `Registry` object goes out of scope.*

---

#### `Registry &operator=(const Registry &other)`

**Signature**

```cpp
Registry &operator=(const Registry &other);
```

**Synopsis**
Assigns another registry (shares the underlying key).

**Parameters**

* `other` – `Registry` whose underlying key handle should be shared.

**Return value**

* Reference to `*this`, enabling assignment chaining.

**In Depth**

The assignment operator:

1. Releases the current key handle held by `*this` (if any).
2. Makes `*this` share the same underlying handle as `other`.

After assignment, both objects refer to the same key. Self-assignment is safe; the implementation guards against incorrectly freeing the handle twice.

**Example**

```cpp
Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");
Registry other("HKEY_CURRENT_USER\\Software\\OtherApp");

reg = other;  // reg now refers to the "OtherApp" key
```

*This example reassigns a `Registry` object to reference a different key.*

---

### Key Lifecycle

#### `bool exists() const`

**Signature**

```cpp
bool exists() const;
```

**Synopsis**
Returns true if the key exists.

**Parameters**

* *(none)*

**Return value**

* `true` if the specified registry key currently exists; `false` otherwise.

**In Depth**

Use `exists()` to check whether the key identified by the path passed to the constructor is present in the registry. This is typically the first step before reading or writing values.

This function does not create the key; it only tests for its presence.

**Example**

```cpp
Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");

if (!reg.exists()) {
  // Key has not been created yet
}
```

*This example checks whether an application-specific key is already present.*

---

#### `bool create()`

**Signature**

```cpp
bool create();
```

**Synopsis**
Creates the key if it doesn't exist. Returns true on success.

**Parameters**

* *(none)*

**Return value**

* `true` if the key exists after the call (either newly created or already existing).
* `false` if creation failed (for example, due to permissions).

**In Depth**

`create()` ensures that the registry key exists. Typical semantics:

* If the key does not exist, it is created.
* If the key already exists, the call still succeeds and leaves the key unchanged.

This allows you to write code with a simple “ensure exists” pattern:

1. Construct a `Registry` object.
2. Call `create()` once.
3. Read or write values under the key.

You may need appropriate permissions depending on which hive and path you use (user vs. machine-wide settings).

**Example**

```cpp
Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");

// Ensure that the key exists before writing values
if (reg.create()) {
  reg.setStringValue("InstallPath", "C:\\MyApp");
}
```

*This example creates the key if needed and then writes a configuration value.*

---

#### `bool remove(bool isRecursive = true)`

**Signature**

```cpp
bool remove(bool isRecursive = true);
```

**Synopsis**
Removes the key. Returns true on success.

**Parameters**

* `isRecursive` – If `true`, attempts to remove the key and all its subkeys. If `false`, typically only removes the key if it is empty (implementation-dependent).

**Return value**

* `true` if the key is successfully removed.
* `false` if removal fails (for example, due to permissions or non-empty key when `isRecursive` is `false`).

**In Depth**

`remove()` deletes the registry key associated with this `Registry` object. Use it carefully:

* Prefer to remove only keys your application created.
* Avoid deleting system or third-party keys.
* When `isRecursive` is `true` (the default), subkeys and values are also removed, which is convenient for application uninstallation scenarios.

After a successful `remove()`, the key no longer exists in the registry. The `Registry` object itself remains valid, but calls such as `exists()` will return `false`.

**Example**

```cpp
Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");

// Remove this key and all its subkeys/values
bool ok = reg.remove(true);
```

*This example removes an application’s registry key recursively, which might be part of an uninstall routine.*

---

### Value Existence and Retrieval

#### `bool valueExists(const String &name = String()) const`

**Signature**

```cpp
bool valueExists(const String &name = String()) const;
```

**Synopsis**
Returns true if the value exists (empty name = default value).

**Parameters**

* `name` – Name of the value to check. When `name` is an empty `String()` (the default), this refers to the key’s **default (unnamed)** value.

**Return value**

* `true` if the specified value exists under this key.
* `false` otherwise.

**In Depth**

Before reading a value, you can use `valueExists()` to determine whether it is defined:

* This is especially useful for `getIntegerValue()`, which returns `0` both for “not found” and for the actual integer `0`.
* For string and binary values, you can also check the pointer returned by the `get*Value()` functions, but `valueExists()` provides a cleaner intention.

**Example**

```cpp
Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");

if (reg.valueExists("WindowWidth")) {
  unsigned int width = reg.getIntegerValue("WindowWidth");
}
```

*This example checks whether a specific integer value is present before reading it.*

---

#### `const char *getStringValue(const String &name = String()) const`

**Signature**

```cpp
const char *getStringValue(const String &name = String()) const;
```

**Synopsis**
Returns the string value, or nullptr if not found.

**Parameters**

* `name` – Name of the value to read. An empty `String()` (default) refers to the default value.

**Return value**

* Pointer to a null-terminated UTF-8 string representing the value.
* `nullptr` if the value does not exist or cannot be represented as a string.

**In Depth**

`getStringValue()` reads a **string-type** value from the registry. Important points:

* The returned pointer is **owned by the `Registry` object**; do not `Free` or modify it.
* The pointer remains valid only as long as the `Registry` object is alive and unchanged.
* If you need to store the value for longer, construct an `attoboy::String` from it:

  ```cpp
  String copy(p);
  ```

If the value does not exist (or is not a string), the function returns `nullptr`, which you should check before use.

**Example**

```cpp
Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");

const char *p = reg.getStringValue("InstallPath");
if (p != nullptr) {
  String installPath(p);
  // use installPath as a String
}
```

*This example reads a string value and wraps it into an `attoboy::String` for further use.*

---

#### `const unsigned char *getBinaryValue(const String &name = String()) const`

**Signature**

```cpp
const unsigned char *getBinaryValue(const String &name = String()) const;
```

**Synopsis**
Returns the binary value, or nullptr if not found.

**Parameters**

* `name` – Name of the binary value to read. An empty `String()` (default) refers to the default value.

**Return value**

* Pointer to a binary buffer (`const unsigned char *`) containing the value data.
* `nullptr` if the value does not exist or is not a binary value.

**In Depth**

`getBinaryValue()` retrieves a binary registry value. The pointer:

* Is read-only and owned by the `Registry` object.
* Remains valid only while the `Registry` object remains alive and unchanged.
* Does **not** directly expose the data length via this API; how you interpret the bytes is up to your application.

Because the size is not directly available, this function is best suited when:

* You control both writing and reading sides and embed size or structure information inside the binary data.
* You treat the binary data as a fixed-size structure known by your application.

For simple configuration, string or integer values are usually more convenient.

**Example**

```cpp
Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");

const unsigned char *data = reg.getBinaryValue("Signature");
if (data != nullptr) {
  // Interpret data using your own format or protocol.
}
```

*This example reads a binary value and leaves interpretation of the bytes to the application.*

---

#### `unsigned int getIntegerValue(const String &name = String()) const`

**Signature**

```cpp
unsigned int getIntegerValue(const String &name = String()) const;
```

**Synopsis**
Returns the integer value (0 if not found).

**Parameters**

* `name` – Name of the integer value to read. An empty `String()` (default) refers to the default value.

**Return value**

* The stored integer value.
* `0` if the value does not exist or cannot be read as an integer.

**In Depth**

This function reads an integer (for example, a `REG_DWORD` value) from the registry. Because it returns `0` on failure, you cannot distinguish between:

* A missing value, and
* A value that is legitimately `0`.

To detect absence, combine it with `valueExists()`:

```cpp
if (reg.valueExists("Timeout")) {
  unsigned int timeout = reg.getIntegerValue("Timeout");
}
```

Use integer values for simple counts, flags, or configuration options.

**Example**

```cpp
Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");

unsigned int timeoutMs = reg.getIntegerValue("TimeoutMs");  // 0 if missing
```

*This example reads an integer timeout value; if the value is not present, the result is `0`.*

---

### Value Modification and Deletion

#### `bool setStringValue(const String &name, const String &str)`

**Signature**

```cpp
bool setStringValue(const String &name, const String &str);
```

**Synopsis**
Sets a string value. Returns true on success.

**Parameters**

* `name` – Name of the value to set. Use an empty `String()` to set the default value.
* `str` – `String` containing the UTF-8 text to write.

**Return value**

* `true` if the value is successfully written.
* `false` if writing fails (for example, due to permissions or invalid key).

**In Depth**

`setStringValue()` writes a text value to the registry. If a value with the same name already exists:

* It is overwritten with the new content.

If the key itself does not exist, you should call `create()` first.

String values are typically used for paths, identifiers, or configuration flags that are naturally represented as text.

**Example**

```cpp
Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");

if (reg.create()) {
  reg.setStringValue("InstallPath", "C:\\MyApp");
}
```

*This example ensures the key exists and then writes a string setting.*

---

#### `bool setBinaryValue(const String &name, const Buffer &buf)`

**Signature**

```cpp
bool setBinaryValue(const String &name, const Buffer &buf);
```

**Synopsis**
Sets a binary value. Returns true on success.

**Parameters**

* `name` – Name of the binary value to set.
* `buf` – `Buffer` whose raw bytes will be stored as the value.

**Return value**

* `true` if the value is successfully written.
* `false` if writing fails.

**In Depth**

`setBinaryValue()` stores arbitrary binary data in the registry. The value kind corresponds to a binary registry type (for example, `REG_BINARY`).

Typical use cases:

* Caching small binary blobs (signatures, checksums).
* Storing fixed-format data that your application knows how to decode.

Since there is no direct size accessor when reading via `getBinaryValue()`, it is good practice to encode any necessary length or structure information within the binary payload itself.

**Example**

```cpp
unsigned char signatureBytes[4] = {0xDE, 0xAD, 0xBE, 0xEF};
Buffer sigBuf(signatureBytes, 4);

Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");
if (reg.create()) {
  reg.setBinaryValue("Signature", sigBuf);
}
```

*This example writes a small binary signature value to the registry.*

---

#### `bool setIntegerValue(const String &name, unsigned int num)`

**Signature**

```cpp
bool setIntegerValue(const String &name, unsigned int num);
```

**Synopsis**
Sets an integer value. Returns true on success.

**Parameters**

* `name` – Name of the integer value to set.
* `num` – Unsigned integer to store.

**Return value**

* `true` if the value is successfully written.
* `false` if writing fails.

**In Depth**

`setIntegerValue()` writes a numeric value (for example, `REG_DWORD`) under the current key. It overwrites any existing value with the same name.

Use integer values for counts, sizes, flags, enabling/disabling features, or other numeric configuration.

**Example**

```cpp
Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");
if (reg.create()) {
  reg.setIntegerValue("WindowWidth", 1280);
  reg.setIntegerValue("WindowHeight", 720);
}
```

*This example stores window dimensions as integer settings.*

---

#### `bool deleteValue(const String &name = String())`

**Signature**

```cpp
bool deleteValue(const String &name = String());
```

**Synopsis**
Deletes a value. Returns true on success.

**Parameters**

* `name` – Name of the registry value to delete. An empty `String()` (default) refers to the default value.

**Return value**

* `true` if the value was deleted or did not exist by the end of the call (implementation-dependent success).
* `false` if deletion fails (for example, due to permissions).

**In Depth**

`deleteValue()` removes an individual value from the key without deleting the key itself. Use this when:

* A setting is no longer needed.
* You want to revert to hard-coded defaults by removing stored overrides.

Be careful to distinguish deleting a **value** (within a key) from deleting the entire **key** with `remove()`.

**Example**

```cpp
Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");
// Remove an obsolete configuration value
reg.deleteValue("OldSetting");
```

*This example deletes a single named value from an application’s registry key.*

---

### Enumeration

#### `List listValueNames() const`

**Signature**

```cpp
List listValueNames() const;
```

**Synopsis**
Returns a list of all value names.

**Parameters**

* *(none)*

**Return value**

* A `List` containing the names of all values under the current key. Each element is typically a `String`.

**In Depth**

`listValueNames()` lets you discover which values are currently defined under a key. This is useful for:

* Debugging and inspection tools.
* Generic configuration editors.
* “Import everything” scenarios.

The treatment of the default (unnamed) value is implementation-dependent; it may appear as an empty string name or be handled specially.

You can combine this with `getStringValue`, `getIntegerValue`, or `getBinaryValue` to iterate over all values.

**Example**

```cpp
Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");

List names = reg.listValueNames();
for (int i = 0; i < names.length(); ++i) {
  String name = names.at<String>(i);
  // For each value name, you could read or display it.
}
```

*This example enumerates all value names under a key and iterates over them.*

---

#### `List listSubkeys() const`

**Signature**

```cpp
List listSubkeys() const;
```

**Synopsis**
Returns a list of all subkey names.

**Parameters**

* *(none)*

**Return value**

* A `List` of `String` values, each representing the name of a subkey under the current key.

**In Depth**

`listSubkeys()` provides the names of child keys beneath the current key. This allows you to:

* Discover dynamically created subkeys (for example, one per user profile or profile name).
* Build tree navigation in configuration tools.
* Enumerate all instances of a particular setting nested under one root.

Combined with the constructor, you can recursively walk a registry subtree:

```cpp
List subs = reg.listSubkeys();
for (int i = 0; i < subs.length(); ++i) {
  String childName = subs.at<String>(i);
  Registry child("HKEY_CURRENT_USER\\Software\\MyApp\\" + childName);
  // Work with child...
}
```

**Example**

```cpp
Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");

List subkeys = reg.listSubkeys();
for (int i = 0; i < subkeys.length(); ++i) {
  String childName = subkeys.at<String>(i);
  // childName is the name of a subkey under MyApp
}
```
