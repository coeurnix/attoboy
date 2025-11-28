# Set

## Overview

The `attoboy::Set` class is an **unordered collection of unique values**. It is designed as a compact, flexible alternative to standard-library sets, but without depending on the C++ standard library. Instead, it is tightly integrated with other attoboy types:

* Primitive values: `bool`, `int`, `float`
* Text: `String`
* Nested containers: `List`, `Map`, `Set`

At a high level, `Set` gives you:

* **Uniqueness** – each value appears at most once.
* **Unordered storage** – the library does not guarantee any particular iteration order.
* **Mixed-type elements** – a single set can store values of different types.
* **Set operations** – union, intersection, and difference.
* **JSON integration** – conversion to and from JSON array strings.

The class is especially useful when you care about **membership** (“is this value present?”) rather than ordering or duplicates.

### Sets, Uniqueness, and Order

Conceptually, a set is a mathematical collection with the following properties:

* **No duplicates**
  If you add the same value multiple times, the set still contains just one copy of that value.
* **No guaranteed order**
  The set does not promise that values will appear in the order you inserted them. Internally, it may use hashing or other structures that do not preserve insertion order.

In `attoboy::Set`:

* `put(value)` adds a value if it is not already present.
* Adding a value that already exists does nothing (duplicates are silently ignored).
* `length()` counts how many distinct values are stored.
* `contains(value)` checks membership efficiently.

### Mixed Types and Equality

`Set` can store values of different types, such as:

* `Set(1, "two", 3.0f)`
* `Set("apple", String("banana"), List(1, 2))`

Equality and membership checks use internal type-aware comparisons. In practice:

* A stored `int` `1` is distinct from the string `"1"`.
* Two values of different types are not considered equal, even if they might look similar as text.

To avoid surprises when doing set operations, try to store comparable values with consistent types (for example, all integers or all strings) when possible.

### Mutability, RAII, and Chaining

Unlike `String`, `Set` is **mutable**:

* Methods like `put()`, `remove()`, `clear()`, and `setUnion()` modify the set in place.
* These methods return a reference to `*this`, which allows fluent **chaining**:

  ```cpp
  using namespace attoboy;

  Set items;
  items.put(1)
       .put(2)
       .put(2)    // ignored duplicate
       .remove(1);
  // items now contains only {2}
  ```

`Set` also follows the RAII (Resource Acquisition Is Initialization) pattern:

* Construction acquires any internal resources needed.
* The destructor automatically releases memory when the set goes out of scope.
* Copying and assignment manage ownership cleanly; you do not need to free anything manually.

### Integration with `List` and JSON

`Set` integrates smoothly with other attoboy containers:

* You can construct a set from a `List`, automatically **removing duplicates**.
* You can convert a set to a `List` with `toList()`, for cases where you need indexed access or sorting.
* You can convert a set to and from JSON:

  * `toJSONString()` produces a JSON array string representing the set contents.
  * `FromJSONString()` parses a JSON array string and builds a set from it.

These abilities make `Set` a good tool for representing collections of tags, unique identifiers, or any values where membership and uniqueness are more important than order.

---

## Reference

Each entry below covers one public constructor, method, static function, or operator of `attoboy::Set`. For each, you will find:

* **Signature** – the exact declaration from the header.
* **Synopsis** – the original one-line Doxygen comment.
* **Parameters** and **Return value** descriptions.
* **In Depth** – additional details, caveats, and a short example.

> **Note:** All examples assume `using namespace attoboy;`.

---

#### `Set()`

**Signature**

```cpp
Set();
```

**Synopsis**
Creates an empty set.

**Parameters**

* *(none)*

**Return value**

* *(constructor; not applicable)*

**In Depth**

This constructor creates a set with no values. Immediately after construction:

* `length()` is `0`.
* `isEmpty()` returns `true`.

You typically start with an empty set and add values later using `put()`.

**Example**

```cpp
Set flags;
flags.put("debug");
// flags now contains {"debug"}
```

*This example starts with an empty set and adds a single value.*

---

#### `Set(int capacity)`

**Signature**

```cpp
Set(int capacity);
```

**Synopsis**
Creates an empty set with reserved capacity.

**Parameters**

* `capacity` – Hint for the number of values you expect to store.

**Return value**

* *(constructor; not applicable)*

**In Depth**

This constructor creates an empty set but reserves internal storage sized for roughly `capacity` elements. This can reduce internal reallocations when you know up front how many distinct values you plan to insert.

The set is still empty; `length()` remains `0` until you call `put()`.

**Example**

```cpp
int expected = 100;
Set ids(expected);
for (int i = 0; i < expected; ++i) {
  ids.put(i);
}
// ids now contains integers 0..99 (in unspecified order)
```

*This example reserves capacity for 100 values and then fills the set with integers.*

---

#### `template <typename... Args> Set(const Args &...args)`

**Signature**

```cpp
template <typename... Args> Set(const Args &...args) : Set() {
  variadic_put(args...);
}
```

**Synopsis**
Creates a set with the given values: Set(1, 2, 3)

**Parameters**

* `args` – One or more values to insert. Each must be a type supported by `Set` (such as `bool`, `int`, `float`, `const char *`, `String`, `List`, `Map`, or another `Set`).

**Return value**

* *(constructor; not applicable)*

**In Depth**

This variadic constructor builds a new set and inserts each supplied value into it using `put()` semantics:

* Each argument is added to the set.
* Duplicates among the arguments are silently ignored.

Because order is not guaranteed and duplicates are discarded, this constructor is ideal for small, literal-like sets of values where uniqueness is desired.

**Example**

```cpp
Set numbers(1, 2, 2, 3);
// numbers contains {1, 2, 3}, with the duplicate 2 ignored
```

*This example creates a new set with initial values, demonstrating that duplicates are not stored twice.*

---

#### `Set(const Set &other)`

**Signature**

```cpp
Set(const Set &other);
```

**Synopsis**
Creates a copy of another set.

**Parameters**

* `other` – Existing set to copy.

**Return value**

* *(constructor; not applicable)*

**In Depth**

The copy constructor produces a new set that contains the same values as `other`. Because sets are unordered, the internal ordering may differ even if the contents match, but:

* `length()` is the same.
* `contains(value)` for any value gives identical results in both sets.
* `compare(other)` and `operator==` report equality.

**Example**

```cpp
Set original( "a", "b", "c" );
Set copy(original);
// copy initially has the same values as original
```

*This example duplicates a set so the two variables represent equivalent collections.*

---

#### `Set(const List &list)`

**Signature**

```cpp
Set(const List &list);
```

**Synopsis**
Creates a set from a list, removing duplicates.

**Parameters**

* `list` – Source `List` whose values will be inserted into the set.

**Return value**

* *(constructor; not applicable)*

**In Depth**

This constructor converts a `List` into a `Set` by:

1. Iterating through all elements in `list`.
2. Inserting each value into the set via `put()`.

As usual for sets, **duplicate values are removed**, so each distinct value appears at most once in the resulting set. Order is not preserved.

**Example**

```cpp
List values("red", "green", "red", "blue");
Set colors(values);
// colors contains {"red", "green", "blue"}, with duplicates removed
```

*This example converts a list with duplicates into a set of unique values.*

---

#### `~Set()`

**Signature**

```cpp
~Set();
```

**Synopsis**
Destroys the set and frees memory.

**Parameters**

* *(none)*

**Return value**

* *(destructor; not applicable)*

**In Depth**

When a `Set` goes out of scope or is otherwise destroyed, its destructor releases any internal resources (such as allocated tables or buffers). You do not need to free anything manually; the RAII pattern ensures memory is cleaned up automatically.

**Example**

```cpp
{
  Set temp(1, 2, 3);
  // use temp here
}
// temp is destroyed here; internal resources are freed
```

*This example shows a scoped set that is automatically cleaned up at the end of the block.*

---

#### `Set &operator=(const Set &other)`

**Signature**

```cpp
Set &operator=(const Set &other);
```

**Synopsis**
Assigns another set to this set.

**Parameters**

* `other` – Set whose contents should replace the current set.

**Return value**

* Reference to `*this`, allowing assignment chaining.

**In Depth**

The copy assignment operator makes the current set contain exactly the same values as `other`. Any previously stored values in `*this` are discarded.

After assignment:

* `length()` matches `other.length()`.
* `contains(value)` returns the same result in both sets for any value.

Self-assignment (`set = set`) is safe.

**Example**

```cpp
Set a("apple", "banana");
Set b("cherry");

a = b;
// a now contains only the values that were in b
```

*This example replaces the contents of one set with those of another.*

---

#### `int length() const`

**Signature**

```cpp
int length() const;
```

**Synopsis**
Returns the number of values in the set.

**Parameters**

* *(none)*

**Return value**

* Number of distinct values stored in the set.

**In Depth**

`length()` counts how many unique values are currently present. Because duplicates are not stored, this is the number of **distinct** elements.

**Example**

```cpp
Set s(1, 2, 2, 3);
int n = s.length();  // 3
```

*This example shows that duplicates do not increase the length of the set.*

---

#### `bool isEmpty() const`

**Signature**

```cpp
bool isEmpty() const;
```

**Synopsis**
Returns true if the set is empty.

**Parameters**

* *(none)*

**Return value**

* `true` if `length() == 0`; `false` otherwise.

**In Depth**

`isEmpty()` is a convenience method for checking whether the set contains any values. It is semantically equivalent to `length() == 0` but often clearer in conditional code.

**Example**

```cpp
Set s;
if (s.isEmpty()) {
  s.put("first");
}
```

*This example initializes the set with a value only if it is currently empty.*

---

#### `template <typename T> Set &put(T value)`

**Signature**

```cpp
template <typename T> Set &put(T value) {
  put_impl(value);
  return *this;
}
```

**Synopsis**
Adds a value to the set. Duplicates are ignored. Returns this set for chaining.

**Parameters**

* `value` – Value to add. Must be a type supported by `Set` (`bool`, `int`, `float`, `const char *`, `String`, `List`, `Map`, `Set`, etc.).

**Return value**

* Reference to `*this`, enabling method chaining.

**In Depth**

`put()` inserts `value` into the set:

* If `value` is not already present, it is added and `length()` increases by one.
* If `value` is already present, nothing changes; duplicates are silently ignored.

The method is templated, and the internal `put_impl()` selects the appropriate overload based on the actual type.

Because order is not guaranteed, the position of `value` in the internal structure is not meaningful.

**Example**

```cpp
Set tags;
tags.put("alpha")
    .put("beta")
    .put("alpha");  // duplicate ignored
// tags contains {"alpha", "beta"}
```

*This example demonstrates that adding a duplicate value does not change the set.*

---

#### `template <typename T> Set &remove(T value)`

**Signature**

```cpp
template <typename T> Set &remove(T value) {
  remove_impl(value);
  return *this;
}
```

**Synopsis**
Removes a value from the set. Returns this set for chaining.

**Parameters**

* `value` – Value to remove.

**Return value**

* Reference to `*this`.

**In Depth**

`remove()` deletes `value` from the set if it is present. After a successful removal:

* `contains(value)` returns `false`.
* `length()` decreases by one.

If the value is not present, the operation is effectively a no-op.

Type matching matters: only values considered equal according to the set’s comparison rules will be removed.

**Example**

```cpp
Set ids(1, 2, 3);
ids.remove(2);
// ids contains {1, 3}
```

*This example removes a value from the set, leaving the others untouched.*

---

#### `Set &clear()`

**Signature**

```cpp
Set &clear();
```

**Synopsis**
Removes all values. Returns this set for chaining.

**Parameters**

* *(none)*

**Return value**

* Reference to `*this`.

**In Depth**

`clear()` empties the set completely:

* `length()` becomes `0`.
* `isEmpty()` returns `true`.
* All previous values are removed.

Internal capacity may remain allocated for efficiency, but the set behaves as if it were newly constructed.

**Example**

```cpp
Set s(1, 2, 3);
s.clear();
// s is now empty
```

*This example removes all values from a set.*

---

#### `Set &setUnion(const Set &other)`

**Signature**

```cpp
Set &setUnion(const Set &other);
```

**Synopsis**
Adds all values from another set (union). Returns this set for chaining.

**Parameters**

* `other` – Another set whose values will be incorporated.

**Return value**

* Reference to `*this`.

**In Depth**

`setUnion()` modifies the current set to become the **union** of `*this` and `other`:

* Every value present in either set ends up present in `*this`.
* Values from `other` that are already in `*this` are ignored as duplicates.

The `other` set remains unchanged.

**Example**

```cpp
Set a(1, 2);
Set b(2, 3);

a.setUnion(b);
// a contains {1, 2, 3}
// b remains {2, 3}
```

*This example merges all values from two sets into one, without introducing duplicates.*

---

#### `Set &intersect(const Set &other)`

**Signature**

```cpp
Set &intersect(const Set &other);
```

**Synopsis**
Keeps only values in both sets (intersection). Returns this set for chaining.

**Parameters**

* `other` – Another set to intersect with.

**Return value**

* Reference to `*this`.

**In Depth**

`intersect()` modifies the current set to keep only values that are present in **both** `*this` and `other`:

* Values not present in `other` are removed from `*this`.
* Values present in `other` but absent in `*this` are not added.

In mathematical terms:

```text
*this := *this ∩ other
```

**Example**

```cpp
Set a(1, 2, 3);
Set b(2, 3, 4);

a.intersect(b);
// a contains {2, 3}
```

*This example computes the intersection of two sets, leaving only shared values.*

---

#### `Set &subtract(const Set &other)`

**Signature**

```cpp
Set &subtract(const Set &other);
```

**Synopsis**
Removes all values in other (difference). Returns this set for chaining.

**Parameters**

* `other` – Set containing values to remove from the current set.

**Return value**

* Reference to `*this`.

**In Depth**

`subtract()` performs a **set difference** operation:

* All values that are present in both `*this` and `other` are removed from `*this`.
* Values unique to `*this` remain.
* `other` is not modified.

In mathematical terms:

```text
*this := *this − other
```

**Example**

```cpp
Set a(1, 2, 3, 4);
Set b(2, 4);

a.subtract(b);
// a contains {1, 3}
```

*This example removes all values that appear in another set.*

---

#### `template <typename T> bool contains(T value) const`

**Signature**

```cpp
template <typename T> bool contains(T value) const;
```

**Synopsis**
Returns true if the set contains the value.

**Parameters**

* `value` – Value to test for membership.

**Return value**

* `true` if the value is present in the set; `false` otherwise.

**In Depth**

`contains()` is the primary membership test. It checks whether a value equal to `value` is stored in the set. You should pass the same type (or a compatible type) as the values you inserted for predictable results.

**Example**

```cpp
Set roles("admin", "user");
bool isAdmin = roles.contains("admin");  // true
bool isGuest = roles.contains("guest");  // false
```

*This example checks whether certain roles are present in the set.*

---

#### `Set duplicate() const`

**Signature**

```cpp
Set duplicate() const;
```

**Synopsis**
Returns a copy of this set.

**Parameters**

* *(none)*

**Return value**

* New `Set` containing the same values as the original.

**In Depth**

`duplicate()` explicitly creates a copy of the set. This can be helpful when you want to perform mutating operations (like `setUnion()` or `subtract()`) on a copy while keeping the original set unchanged.

**Example**

```cpp
Set original(1, 2, 3);
Set copy = original.duplicate();

copy.remove(2);
// original still contains {1, 2, 3}
// copy now contains {1, 3}
```

*This example shows how to modify a copy of a set while preserving the original.*

---

#### `List toList() const`

**Signature**

```cpp
List toList() const;
```

**Synopsis**
Returns a list of all values in the set.

**Parameters**

* *(none)*

**Return value**

* `List` containing the values of the set, in unspecified order.

**In Depth**

`toList()` converts the set to a `List`:

* Every value in the set appears exactly once in the resulting list.
* The order of elements is not guaranteed and may change between runs or versions.

This conversion is useful when you need indexed access, sorting, CSV export, or other list-based operations on data that is naturally unique.

**Example**

```cpp
Set numbers(3, 1, 2);
List asList = numbers.toList();
// asList contains the same values {1, 2, 3} in some order
```

*This example converts a set into a list for further processing.*

---

#### `String toJSONString() const`

**Signature**

```cpp
String toJSONString() const;
```

**Synopsis**
Converts this set to a JSON array string.

**Parameters**

* *(none)*

**Return value**

* `String` containing a JSON array representation of the set.

**In Depth**

`toJSONString()` serializes the set as a JSON array. For example, a set containing `1`, `"two"`, and `true` might be serialized as:

```json
[1,"two",true]
```

Important notes:

* The order of elements in the JSON array is not guaranteed.
* Values are converted to JSON based on their types:

  * Numbers → JSON numbers
  * Booleans → `true`/`false`
  * Strings → JSON strings
  * Nested containers (`List`, `Map`, `Set`) → JSON arrays/objects

**Example**

```cpp
Set flags("alpha", "beta");
String json = flags.toJSONString();
// e.g., ["alpha","beta"] (order not guaranteed)
```

*This example serializes a set of strings to a JSON array string.*

---

#### `static Set FromJSONString(const String &json)`

**Signature**

```cpp
static Set FromJSONString(const String &json);
```

**Synopsis**
Creates a set from a JSON array string.

**Parameters**

* `json` – JSON array text to parse.

**Return value**

* `Set` containing the values parsed from the JSON array.

**In Depth**

`FromJSONString()` parses a JSON array string and builds a set:

* Each element in the JSON array becomes a value in the set.
* If the JSON contains duplicate elements, the resulting set still keeps only one copy of each.
* Nested arrays and objects become `List` and `Map` values, respectively, mirroring other JSON-related functions in the library.

If the input is not a valid JSON array, the result may be an empty set or reflect an error state; you should validate or handle unexpected inputs appropriately.

**Example**

```cpp
String json("[\"alpha\",\"beta\",\"alpha\"]");
Set flags = Set::FromJSONString(json);
// flags contains {"alpha", "beta"}
```

*This example parses a JSON array into a set, automatically removing duplicates.*

---

#### `bool compare(const Set &other) const`

**Signature**

```cpp
bool compare(const Set &other) const;
```

**Synopsis**
Returns true if this set equals the other set.

**Parameters**

* `other` – Set to compare against.

**Return value**

* `true` if both sets contain exactly the same values; `false` otherwise.

**In Depth**

`compare()` checks equality based on set semantics:

* Two sets are equal if every value in one is present in the other, and vice versa.
* The internal order is irrelevant; only the membership of values matters.
* Differences in duplicates are impossible because sets do not store duplicates.

This is functionally equivalent to `operator==` and is mainly a named alternative.

**Example**

```cpp
Set a(1, 2, 3);
Set b(3, 2, 1);
Set c(1, 2);

bool ab = a.compare(b);  // true (same values)
bool ac = a.compare(c);  // false (c is missing 3)
```

*This example shows equality comparison that ignores order but requires the same set of values.*

---

#### `bool operator==(const Set &other) const`

**Signature**

```cpp
bool operator==(const Set &other) const;
```

**Synopsis**
Returns true if this set equals the other set.

**Parameters**

* `other` – Set to compare against.

**Return value**

* `true` if sets are equal; `false` otherwise.

**In Depth**

This operator provides the idiomatic syntax for set equality:

```cpp
if (a == b) { ... }
```

It uses the same logic as `compare()`: sets are considered equal if and only if they contain the same values, regardless of internal order.

**Example**

```cpp
Set a("x", "y");
Set b("y", "x");
Set c("x");

bool same = (a == b); // true
bool diff = (a == c); // false
```

*This example demonstrates equality comparisons of sets using the `==` operator.*

---

#### `bool operator!=(const Set &other) const`

**Signature**

```cpp
bool operator!=(const Set &other) const;
```

**Synopsis**
Returns true if this set does not equal the other set.

**Parameters**

* `other` – Set to compare against.

**Return value**

* `true` if sets are not equal; `false` if they are equal.

**In Depth**

This operator is the logical negation of `operator==`. It is convenient when you want to branch on inequality:

```cpp
if (a != b) { ... }
```

**Example**

```cpp
Set a(1, 2);
Set b(2, 3);

if (a != b) {
  // sets contain different values
}
```

*This example uses inequality to detect when two sets do not match.*

---

#### `template <typename T> Set operator+(T value) const`

**Signature**

```cpp
template <typename T> Set operator+(T value) const {
  Set result = duplicate();
  result.put(value);
  return result;
}
```

**Synopsis**
Returns a new set with the value added.

**Parameters**

* `value` – Value to add to a copy of the current set.

**Return value**

* New `Set` that contains all values from the original set, plus `value` (if it was not already present).

**In Depth**

This operator provides a **functional-style** way to extend a set without mutating the original:

1. It duplicates the current set.
2. It adds `value` to the duplicate (using `put()` semantics).
3. It returns the new set, leaving the original unchanged.

Because sets ignore duplicates, adding a value already present simply yields another set with the same contents.

**Example**

```cpp
Set base("a", "b");
Set extended = base + "c";
// base still contains {"a", "b"}
// extended contains {"a", "b", "c"}
```

*This example uses the `+` operator to create a new set with an additional value while preserving the original.*
