# `attoboy::Map` User Guide

## Overview

The `attoboy::Map` class is a **key–value dictionary** with support for **mixed-type keys and values**. It is designed as a compact replacement for standard-library maps, but without depending on the C++ standard library. Instead, it integrates tightly with other attoboy types:

* Primitive keys and values: `bool`, `int`, `float`
* Text keys and values: `String` and `const char *`
* Nested containers as values: `List`, `Map`, `Set`

At a high level, `Map` gives you:

* **Unique keys** – each key appears at most once.
* **Flexible key types** – keys can be numbers, strings, or booleans.
* **Flexible value types** – values can be any supported type, including nested containers.
* **Efficient lookups** – retrieve values by key with `get()` or `operator[]`.
* **Inspection tools** – check key presence, value types, and value-to-key reverse lookups.
* **JSON integration** – convert to and from JSON object strings.

### Key–Value Maps and Mixed Types

Conceptually, a map (or dictionary) is a collection of **pairs**:

* Each pair consists of a **key** and a **value**.
* Keys are **unique**; inserting a new value for an existing key **overwrites** the previous value.
* Keys can be used to look up their associated values quickly.

In `attoboy::Map`:

* Keys can be `bool`, `int`, `float`, `const char *`, or `String`.
* Values can be any supported type: primitives, `String`, `List`, `Map`, `Set`.
* The map does **not guarantee order**; you should not rely on insertion order when iterating via `keys()` or `values()`.

### Types and `ValueType`

Because values can be mixed types, you may need to know **what type** is stored for a given key before you read it. The `ValueType` enumeration (shared with `List` and `Set`) includes:

* `TYPE_BOOL`, `TYPE_INT`, `TYPE_FLOAT`
* `TYPE_STRING`, `TYPE_LIST`, `TYPE_MAP`, `TYPE_SET`
* `TYPE_NULL`, `TYPE_INVALID`, `TYPE_UNKNOWN` for special or error states

You can use `typeAt()` to check the value’s type for a key before deciding which template parameter to use when calling `get<K,V>()` or `operator[]<K,V>()`.

### Mutability, RAII, and Chaining

`Map` is **mutable**:

* Methods like `put()`, `remove()`, `clear()`, and `merge()` modify the map in place.
* Most mutating methods return `*this`, which allows fluent **chaining**:

  ```cpp
  using namespace attoboy;

  Map config;
  config.put("host", "localhost")
        .put("port", 8080)
        .put("debug", true);
  ```

`Map` also follows **RAII**:

* Constructing a `Map` acquires internal resources as needed.
* The destructor automatically releases those resources.
* Copying and assignment manage the internal state safely; you never need to free memory manually.

### JSON and Configuration Use Cases

`Map` is especially useful for configuration-like data and JSON:

* `toJSONString()` converts the map into a JSON object string.
* `FromJSONString()` parses a JSON object string and builds a `Map`.

This makes it easy to:

* Represent settings: `"host" -> "example.com"`, `"timeout" -> 5000`.
* Pass structured arguments into APIs that consume or produce JSON.
* Store hierarchical data using nested `Map` and `List` values.

---

## Reference

Each entry below covers one public constructor, method, static function, or operator of `attoboy::Map`. For each, you will find:

* **Signature** – the exact declaration from the header.
* **Synopsis** – the original one-line Doxygen comment.
* **Parameters** and **Return value** descriptions.
* **In Depth** – additional details, caveats, and a short example.

> **Note:** All examples assume `using namespace attoboy;`.

---

### Class Description

**Synopsis (class)**
Key-value map with mixed types for both keys and values.
Keys must be unique. Order is not guaranteed.

This synopsis applies to the class as a whole. The sections below describe each member in detail.

---

### Constructors, Copy, Assignment, and Destruction

#### `Map()`

**Signature**

```cpp
Map();
```

**Synopsis**
Creates an empty map.

**Parameters**

* *(none)*

**Return value**

* *(constructor; not applicable)*

**In Depth**

This constructor creates a map with:

* No keys.
* No associated values.

Immediately after construction:

* `length()` returns `0`.
* `isEmpty()` returns `true`.

It is the standard starting point for building key–value collections incrementally using `put()`.

**Example**

```cpp
Map settings;
settings.put("debug", true);
settings.put("maxConnections", 100);
```

*This example starts from an empty map and adds two key–value pairs.*

---

#### `Map(int capacity)`

**Signature**

```cpp
Map(int capacity);
```

**Synopsis**
Creates an empty map with reserved capacity.

**Parameters**

* `capacity` – Hint for how many key–value pairs you expect to store.

**Return value**

* *(constructor; not applicable)*

**In Depth**

This constructor allows you to reserve internal storage up front for approximately `capacity` entries. This can reduce internal reallocation when you know you will store many entries.

The map is still empty: `length() == 0` until you call `put()`.

**Example**

```cpp
int expectedEntries = 100;
Map cache(expectedEntries);

for (int i = 0; i < expectedEntries; ++i) {
  cache.put(i, String("value"));
}
```

*This example preallocates space for a cache of 100 entries.*

---

#### `template <typename... Args> Map(const Args &...args)`

**Signature**

```cpp
template <typename... Args> Map(const Args &...args) : Map() {
  variadic_put_pairs(args...);
}
```

**Synopsis**
Creates a map with key-value pairs: Map("name", "Alice", "age", 30)

**Parameters**

* `args` – A sequence of values that will be interpreted as key–value pairs:

  * `args` are consumed in order: `(key0, value0, key1, value1, ...)`.
  * Keys must be types supported as keys (`bool`, `int`, `float`, `const char *`, `String`).
  * Values must be types supported as values.

**Return value**

* *(constructor; not applicable)*

**In Depth**

This constructor is a convenient literal-style initializer:

* It starts with an empty map.
* It treats the argument list as `(key, value)` pairs.
* It calls `put(key, value)` for each pair.

If the number of arguments is odd, the last key is paired with a default boolean `false` (based on the internal `variadic_put_pairs()` behavior), but this usage is generally discouraged. For clarity and safety, always provide arguments in full key–value pairs.

If the same key appears multiple times, later values overwrite earlier ones (like repeated `put()` calls).

**Example**

```cpp
Map user(
  "name", "Alice",
  "age", 30,
  "active", true
);
// user["name"] == "Alice"
// user["age"] == 30
// user["active"] == true
```

*This example creates a small configuration map using literal-like syntax.*

---

#### `Map(const Map &other)`

**Signature**

```cpp
Map(const Map &other);
```

**Synopsis**
Creates a copy of another map.

**Parameters**

* `other` – Existing map to copy.

**Return value**

* *(constructor; not applicable)*

**In Depth**

The copy constructor produces a new map with the same key–value pairs as `other`:

* `length()` is the same.
* Key membership (`hasKey(key)`) matches.
* Values retrieved by `get()` or `operator[]` are equal to those in `other`.

Although internal implementation details may differ, the observable behavior is the same.

**Example**

```cpp
Map original("host", "localhost", "port", 8080);
Map copy(original);
// copy initially has the same key–value pairs as original
```

*This example duplicates a configuration map so that changes to one do not affect the other.*

---

#### `~Map()`

**Signature**

```cpp
~Map();
```

**Synopsis**
Destroys the map and frees memory.

**Parameters**

* *(none)*

**Return value**

* *(destructor; not applicable)*

**In Depth**

When a `Map` goes out of scope or is otherwise destroyed, its destructor automatically:

* Releases internal storage.
* Cleans up any associated resources.

Because attoboy follows RAII, you never need to free the map explicitly.

**Example**

```cpp
{
  Map temp("key", "value");
  // use temp here
}
// temp is destroyed here; its resources are freed
```

*This example illustrates automatic cleanup when a map leaves scope.*

---

#### `Map &operator=(const Map &other)`

**Signature**

```cpp
Map &operator=(const Map &other);
```

**Synopsis**
Assigns another map to this map.

**Parameters**

* `other` – Map whose contents should replace the current ones.

**Return value**

* Reference to `*this`, enabling assignment chaining.

**In Depth**

The copy assignment operator makes `*this` contain exactly the same key–value pairs as `other`. Any previous contents are discarded.

After assignment:

* `length()` matches `other.length()`.
* For any key `k`, `get(k, default)` returns the same value in both maps.

Self-assignment (`map = map`) is safe.

**Example**

```cpp
Map a("x", 1);
Map b("y", 2);

a = b;
// a now contains only the pairs that were in b
```

*This example replaces the contents of one map with another.*

---

### Basic Properties

#### `int length() const`

**Signature**

```cpp
int length() const;
```

**Synopsis**
Returns the number of key-value pairs.

**Parameters**

* *(none)*

**Return value**

* Number of key–value pairs currently stored.

**In Depth**

`length()` returns how many **distinct keys** are in the map. Because each key is unique:

* `length()` equals the number of entries.
* Inserting a value for an already existing key does **not** increase `length()`; it replaces the previous value.

**Example**

```cpp
Map m;
m.put("a", 1)
 .put("b", 2)
 .put("a", 3);  // overwrites key "a"

int n = m.length();  // 2
```

*This example shows that overwriting a key does not increase the map length.*

---

#### `bool isEmpty() const`

**Signature**

```cpp
bool isEmpty() const;
```

**Synopsis**
Returns true if the map is empty.

**Parameters**

* *(none)*

**Return value**

* `true` if `length() == 0`; `false` otherwise.

**In Depth**

`isEmpty()` is a convenience that expresses intent more clearly than checking `length() == 0`.

**Example**

```cpp
Map m;
if (m.isEmpty()) {
  m.put("initialized", true);
}
```

*This example uses `isEmpty()` to decide when to initialize the map.*

---

### Lookup and Inspection

#### `template <typename K, typename V> V get(K key, V defaultValue = V()) const`

**Signature**

```cpp
template <typename K, typename V> V get(K key, V defaultValue = V()) const;
```

**Synopsis**
Returns the value for key, or defaultValue if not found.

**Parameters**

* `key` – Key to search for. Can be `bool`, `int`, `float`, `const char *`, or `String`.
* `defaultValue` – Value to return if `key` is not present. Defaults to a default-constructed `V`.

**Return value**

* The value associated with `key` converted to type `V` if the key exists; otherwise `defaultValue`.

**In Depth**

`get()` is the safest way to look up values:

* If the key exists, the associated value is returned (converted to `V`).
* If the key does not exist, `defaultValue` is returned without modifying the map.

Considerations:

* You must choose `V` consistent with how the value was stored. For example, if the value for `"age"` is an integer, you should retrieve it with `get<String,int>` or simply `get(key, 0)` where the compiler infers `V = int`.
* If the stored type is incompatible with `V`, the result is undefined or may not be meaningful.

**Example**

```cpp
Map config("timeoutMs", 5000);

int timeout = config.get<String,int>("timeoutMs", 1000); // 5000
int retries = config.get<String,int>("retries", 3);      // 3 (default)
```

*This example retrieves a known key and falls back to a default value for a missing key.*

---

#### `template <typename K, typename V> V operator[](K key) const`

**Signature**

```cpp
template <typename K, typename V> V operator[](K key) const;
```

**Synopsis**
Returns the value for key, or null-type if not found.

**Parameters**

* `key` – Key to search for.

**Return value**

* The value associated with `key` converted to `V` if found; otherwise a default-constructed `V` (the “null-type” of `V`).

**In Depth**

This operator provides a more concise syntax for lookups:

```cpp
V value = map[key];
```

However, unlike `get()`, you cannot explicitly supply a default value; if the key is missing:

* A default-constructed `V` is returned (for example, `0` for numeric types, `false` for `bool`, empty `String` for `String`).

Because the class supports mixed types:

* You should ensure that `V` is compatible with the stored value for the key.
* For explicitness and clarity, many codebases prefer `get()` with an explicit default.

**Example**

```cpp
Map user("name", "Alice", "age", 30);

String name = user.operator[]<String,String>("name"); // "Alice"
int age = user.operator[]<String,int>("age");         // 30
int missing = user.operator[]<String,int>("score");   // 0 (default int)
```

*This example shows direct lookup using the bracket-like operator, including the default value on a missing key.*

---

#### `template <typename K> bool hasKey(K key) const`

**Signature**

```cpp
template <typename K> bool hasKey(K key) const;
```

**Synopsis**
Returns true if the map contains the key.

**Parameters**

* `key` – Key to test for existence.

**Return value**

* `true` if the key is present; `false` otherwise.

**In Depth**

`hasKey()` is the primary **existence check**:

* Use it when you need to know whether a key has been set.
* It avoids the ambiguity of default values returned by `operator[]`.

Because keys are unique, `hasKey()` tells you if there is exactly one value associated with that key.

**Example**

```cpp
Map config("debug", true);

if (config.hasKey("debug")) {
  bool debugEnabled = config.get<String,bool>("debug", false);
}
```

*This example checks that a key exists before retrieving its value.*

---

#### `template <typename K> ValueType typeAt(K key) const`

**Signature**

```cpp
template <typename K> ValueType typeAt(K key) const;
```

**Synopsis**
Returns the type of the value for key, or TYPE_INVALID if not found.

**Parameters**

* `key` – Key whose value type you want to inspect.

**Return value**

* A `ValueType` enumerator describing the stored type, or `TYPE_INVALID` if the key does not exist.

**In Depth**

`typeAt()` allows you to **inspect** the type of the value associated with a key before retrieving it. This is especially important in a map that can store mixed types:

* Use `typeAt()` to guard calls to `get()` or `operator[]` with a specific `V`.
* If it returns `TYPE_INVALID`, the key is not present.

**Example**

```cpp
Map data("name", "Alice", "age", 30);

ValueType nameType = data.typeAt("name"); // likely TYPE_STRING
ValueType ageType  = data.typeAt("age");  // likely TYPE_INT
ValueType missing  = data.typeAt("score"); // TYPE_INVALID
```

*This example inspects the types of several keys to decide how to read their values.*

---

#### `template <typename K, typename V> K findValue(V value) const`

**Signature**

```cpp
template <typename K, typename V> K findValue(V value) const;
```

**Synopsis**
Returns the first key with the given value, or null-type if not found.

**Parameters**

* `value` – Value to search for.
* Template parameter `K` – The key type you expect to retrieve.

**Return value**

* The first key of type `K` whose associated value equals `value`, or a default-constructed `K` (the “null-type” key) if not found.

**In Depth**

`findValue()` performs a reverse lookup:

* It scans the map and looks for an entry whose value equals `value`.
* If it finds one, it returns the corresponding key cast to type `K`.
* If none is found, it returns `K()` (for example, `0` for numeric keys, `false` for `bool`, empty `String` for `String`).

Important notes:

* Because the map can have multiple keys storing the same value, `findValue()` returns the **first** matching key according to internal iteration order (which is not guaranteed).
* You should choose `K` that matches the actual key type used for those entries; otherwise the result may not be meaningful.

**Example**

```cpp
Map status(
  "ready", true,
  "connected", true,
  "error", false
);

String key = status.findValue<String,bool>(false);
// key may be "error" (if that is the first entry with value false)
```

*This example finds the first key whose value is `false`.*

---

#### `List keys() const`

**Signature**

```cpp
List keys() const;
```

**Synopsis**
Returns a list of all keys.

**Parameters**

* *(none)*

**Return value**

* A `List` containing all keys in the map. Order is not guaranteed.

**In Depth**

`keys()` gathers all keys into a `List`:

* Each element in the returned list is a key, most commonly of type `String`, `int`, or `bool`.
* The order of keys in the list is unspecified and may vary between runs or versions.

This is useful for iteration, debugging, or exporting the map.

**Example**

```cpp
Map user("name", "Alice", "age", 30);
List keyList = user.keys();
// keyList holds all the keys (e.g., ["name", "age"] in some order)
```

*This example obtains the list of keys for iteration or inspection.*

---

#### `List values() const`

**Signature**

```cpp
List values() const;
```

**Synopsis**
Returns a list of all values.

**Parameters**

* *(none)*

**Return value**

* A `List` containing all values in the map. Order corresponds to the internal key order and is not guaranteed.

**In Depth**

`values()` collects all values into a `List`:

* Each element in the returned list is the value associated with some key.
* The order of values corresponds to the internal ordering of keys, which is unspecified.

The returned list may contain mixed types (`int`, `String`, `bool`, containers, etc.), and you can use `typeAt()` on the `List` to inspect them.

**Example**

```cpp
Map user("name", "Alice", "age", 30);
List vals = user.values();
// vals holds ["Alice", 30] (order unspecified but consistent with keys())
```

*This example retrieves the list of values for further processing.*

---

### Mutation and Combination

#### `template <typename K, typename V> Map &put(K key, V value)`

**Signature**

```cpp
template <typename K, typename V> Map &put(K key, V value);
```

**Synopsis**
Sets key to value. Returns this map for chaining.

**Parameters**

* `key` – Key to insert or update.
* `value` – Value to associate with the key.

**Return value**

* Reference to `*this` to allow chaining.

**In Depth**

`put()` inserts or updates a key–value pair:

* If `key` is **not present**, `put()` adds a new entry and `length()` increases by one.
* If `key` **already exists**, `put()` overwrites the existing value for that key; `length()` does not change.

Type considerations:

* `key` must be a supported key type.
* `value` must be a supported value type.

This is the primary method for building and updating maps.

**Example**

```cpp
Map config;
config.put("host", "localhost")
      .put("port", 8080)
      .put("port", 9090); // overwrite previous port
```

*This example sets and later updates configuration values for specific keys.*

---

#### `template <typename K> Map &remove(K key)`

**Signature**

```cpp
template <typename K> Map &remove(K key);
```

**Synopsis**
Removes the key-value pair. Returns this map for chaining.

**Parameters**

* `key` – Key whose entry should be removed.

**Return value**

* Reference to `*this`.

**In Depth**

`remove()` deletes the entry for `key` if it exists:

* After a successful removal, `hasKey(key)` returns `false`.
* `length()` decreases by one.
* If `key` was not present, the map is unchanged.

This is the primary way to delete entries from a map.

**Example**

```cpp
Map config("host", "localhost", "port", 8080);
config.remove("port");
// "port" no longer exists in config
```

*This example removes a key–value pair from a configuration map.*

---

#### `Map &clear()`

**Signature**

```cpp
Map &clear();
```

**Synopsis**
Removes all key-value pairs. Returns this map for chaining.

**Parameters**

* *(none)*

**Return value**

* Reference to `*this`.

**In Depth**

`clear()` empties the map:

* `length()` becomes `0`.
* `isEmpty()` returns `true`.
* All existing key–value pairs are removed.

Internal capacity may be retained for performance, but logically the map behaves as newly constructed.

**Example**

```cpp
Map cache("a", 1, "b", 2);
cache.clear();
// cache is now empty
```

*This example clears all entries from a cache map.*

---

#### `Map &merge(const Map &other)`

**Signature**

```cpp
Map &merge(const Map &other);
```

**Synopsis**
Merges another map into this one. Returns this map for chaining.

**Parameters**

* `other` – Source map whose entries will be merged into `*this`.

**Return value**

* Reference to `*this`.

**In Depth**

`merge()` combines two maps:

* For each key in `other`:

  * If the key is **not present** in `*this`, it is added.
  * If the key **is present**, the value from `other` **overwrites** the existing value in `*this`.

`other` is not modified.

This is useful for layering configurations or applying overrides.

**Example**

```cpp
Map defaults("host", "localhost", "port", 8080);
Map overrides("port", 9090, "debug", true);

defaults.merge(overrides);
// defaults now contains:
//   "host"  -> "localhost"
//   "port"  -> 9090      (overridden)
//   "debug" -> true      (added)
```

*This example merges user-provided overrides into a set of default settings.*

---

#### `Map duplicate() const`

**Signature**

```cpp
Map duplicate() const;
```

**Synopsis**
Returns a copy of this map.

**Parameters**

* *(none)*

**Return value**

* New `Map` containing the same key–value pairs as the original.

**In Depth**

`duplicate()` provides an explicit, method-style way to copy a map. This is particularly useful when you want to:

* Preserve the original map.
* Apply modifications to a working copy.

The behavior is the same as using the copy constructor, but it can be more readable when used in chains:

```cpp
Map modified = original.duplicate().merge(overrides);
```

**Example**

```cpp
Map base("mode", "production");
Map work = base.duplicate();

work.put("mode", "development");
// base still has "mode" = "production"
// work has "mode" = "development"
```

*This example shows how to modify a duplicate without affecting the original.*

---

### JSON Serialization

#### `String toJSONString() const`

**Signature**

```cpp
String toJSONString() const;
```

**Synopsis**
Converts this map to a JSON object string.

**Parameters**

* *(none)*

**Return value**

* `String` containing a JSON object representation of the map.

**In Depth**

`toJSONString()` serializes the map as a JSON object, for example:

```json
{"name":"Alice","age":30,"active":true}
```

Key and value handling:

* Keys are converted to JSON object keys (strings). Non-string keys are converted to string form (for example, numeric keys become their decimal string representation).
* Values are converted to JSON based on their type:

  * Numbers → JSON numbers.
  * Booleans → `true`/`false`.
  * `String` → JSON strings with proper escaping.
  * `List`, `Map`, `Set` → nested JSON arrays or objects.

Order of keys in the resulting JSON is not guaranteed.

**Example**

```cpp
Map user("name", "Alice", "age", 30, "active", true);
String json = user.toJSONString();
// e.g., {"name":"Alice","age":30,"active":true}
```

*This example converts a user map into a JSON object string suitable for logging or network transmission.*

---

#### `static Map FromJSONString(const String &json)`

**Signature**

```cpp
static Map FromJSONString(const String &json);
```

**Synopsis**
Creates a map from a JSON object string.

**Parameters**

* `json` – JSON object text to parse.

**Return value**

* A `Map` representing the parsed JSON object.

**In Depth**

`FromJSONString()` parses a JSON object string and constructs a `Map` where:

* Each JSON key becomes a key in the map (typically a `String`).
* Each JSON value becomes an appropriate attoboy value:

  * JSON numbers → numeric values.
  * Booleans → `bool`.
  * Strings → `String`.
  * Arrays → `List`.
  * Objects → nested `Map`.

If the input string is not a valid JSON object, the resulting map may be empty or reflect an error state; you should validate or handle this as needed.

**Example**

```cpp
String json("{\"name\":\"Alice\",\"age\":30,\"active\":true}");
Map user = Map::FromJSONString(json);

// user.get<String,String>("name", String())  -> "Alice"
// user.get<String,int>("age", 0)             -> 30
// user.get<String,bool>("active", false)     -> true
```

*This example parses a JSON object into a map for further processing.*

---

### Equality Operators

#### `bool compare(const Map &other) const`

**Signature**

```cpp
bool compare(const Map &other) const;
```

**Synopsis**
Returns true if this map equals the other map.

**Parameters**

* `other` – Map to compare against.

**Return value**

* `true` if both maps contain exactly the same keys and associated values; `false` otherwise.

**In Depth**

`compare()` checks map equality using dictionary semantics:

* The two maps must have the same number of entries.
* For every key in one map, the other map must:

  * Contain that key.
  * Store an equal value under that key.

Order does not matter; only keys and their associated values must match.

This method is functionally equivalent to `operator==`.

**Example**

```cpp
Map a("x", 1, "y", 2);
Map b("y", 2, "x", 1);
Map c("x", 1);

bool ab = a.compare(b);  // true (same key–value pairs)
bool ac = a.compare(c);  // false (c is missing "y")
```

*This example shows equality checks that ignore order but require identical key–value contents.*

---

#### `bool operator==(const Map &other) const`

**Signature**

```cpp
bool operator==(const Map &other) const;
```

**Synopsis**
Returns true if this map equals the other map.

**Parameters**

* `other` – Map to compare against.

**Return value**

* `true` if maps are equal; `false` otherwise.

**In Depth**

This operator provides the idiomatic equality syntax:

```cpp
if (a == b) { ... }
```

It uses the same criteria as `compare()`:

* Same set of keys.
* Same value for each key.

Order is irrelevant.

**Example**

```cpp
Map a("a", 1, "b", 2);
Map b("b", 2, "a", 1);
Map c("a", 1, "b", 3);

bool same = (a == b); // true
bool diff = (a == c); // false (different value for "b")
```

*This example compares maps using the `==` operator.*

---

#### `bool operator!=(const Map &other) const`

**Signature**

```cpp
bool operator!=(const Map &other) const;
```

**Synopsis**
Returns true if this map does not equal the other map.

**Parameters**

* `other` – Map to compare against.

**Return value**

* `true` if maps are not equal; `false` if they are equal.

**In Depth**

This operator is simply the logical negation of `operator==`:

* It returns `true` whenever `operator==` would return `false`.

It is convenient when you want to branch on inequality.

**Example**

```cpp
Map a("key", 1);
Map b("key", 2);

if (a != b) {
  // the value for "key" differs between the two maps
}
```

*This example uses inequality to detect differing map contents.*
