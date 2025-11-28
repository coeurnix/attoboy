# List

## Overview

The `attoboy::List` class is a **dynamic array** that can store values of different types in a single sequence. It is designed as a flexible, lightweight replacement for containers like `std::vector` or `std::list`, but without depending on the C++ standard library. Instead, it integrates directly with other attoboy types:

* Primitive values: `bool`, `int`, `float`
* Text: `String`
* Nested containers: `List`, `Map`, `Set`

At its core, `List` gives you:

* **Indexed access** to elements (with support for negative indices to count from the end).
* **In-place mutation** (append, insert, remove, sort, reverse).
* **Chaining-friendly methods** (most mutating methods return `*this`).
* **Easy serialization** to and from CSV and JSON strings.

### Mixed Types and `ValueType`

Because `List` can hold mixed types, each element has an associated **type tag** from the `ValueType` enumeration:

* `TYPE_BOOL`, `TYPE_INT`, `TYPE_FLOAT`
* `TYPE_STRING`, `TYPE_LIST`, `TYPE_MAP`, `TYPE_SET`
* `TYPE_NULL`, `TYPE_INVALID`, `TYPE_UNKNOWN` for special or error states

The `typeAt()` function lets you inspect the type stored at a given index before retrieving it, so you can safely decide which template parameter to use with `at<T>()`, `pop<T>()`, or `operator[]<T>()`.

### Indexing and Negative Indices

Elements are accessed by **index**, and indices support a useful extension:

* `0` is the first element.
* `1` is the second, and so on.
* `-1` refers to the **last** element.
* `-2` refers to the second-to-last, etc.

This behavior applies to all operations that take an element index (such as `at()`, `pop()`, `remove()`, and `set()`), unless otherwise noted. Negative indices are often convenient when you want to access or modify elements relative to the end of the list.

### Mutability and Chaining

Unlike `String`, `List` is **mutable**: methods like `append()`, `remove()`, and `sort()` modify the list in place. Most of these methods return a reference to `*this`, which allows **chaining**:

```cpp
using namespace attoboy;

List values;
values.append(1)
      .append(2)
      .append(3)
      .reverse()
      .append(4);
// values now holds [3, 2, 1, 4]
```

This style makes it easy to describe transformations on a single list in a compact and readable way.

### Ownership and RAII

`List` uses **RAII** (Resource Acquisition Is Initialization):

* Construction allocates any necessary internal resources.
* The destructor automatically releases memory when the list goes out of scope.
* Copying and assignment perform appropriate management of shared or copied internal state.

This means you do not have to manually free memory; simply let the `List` object fall out of scope.

### Integration with CSV and JSON

`List` includes built-in support for **CSV** (Comma-Separated Values) and **JSON**:

* `toCSVString()` and `FromCSVString()` allow simple import/export of tabular data represented as a list of lists.
* `toJSONString()` and `FromJSONString()` allow you to treat the list as a JSON array, compatible with other parts of the attoboy library that understand JSON strings.

These helpers make `List` a natural bridge between structured data in memory and text-based formats on disk or over the network.

---

## Reference

Each entry below covers one public constructor, method, static function, or operator of `attoboy::List`. For each, you will find:

* **Signature** – the exact declaration from the header.
* **Synopsis** – the original one-line Doxygen comment.
* **Parameters** and **Return value** descriptions.
* **In Depth** – additional details, caveats, and a short example.

> **Note:** All examples assume `using namespace attoboy;`.

---

#### `List()`

**Signature**

```cpp
List();
```

**Synopsis**
Creates an empty list.

**Parameters**

* *(none)*

**Return value**

* *(constructor; not applicable)*

**In Depth**

This constructor creates a list with no elements and no reserved capacity beyond what the implementation chooses by default. Immediately after construction:

* `length()` returns `0`.
* `isEmpty()` returns `true`.

It is the standard starting point for building lists incrementally using `append()`, `prepend()`, or `insert()`.

**Example**

```cpp
List values;
values.append(1).append(2);
// values now holds [1, 2]
```

*This example starts from an empty list and adds two elements.*

---

#### `List(int capacity)`

**Signature**

```cpp
List(int capacity);
```

**Synopsis**
Creates an empty list with reserved capacity.

**Parameters**

* `capacity` – Hint for how many elements you plan to store.

**Return value**

* *(constructor; not applicable)*

**In Depth**

This constructor creates an empty list but pre-allocates enough internal storage for approximately `capacity` elements. This can reduce reallocations and copying when you know the size in advance, such as when reading a known-length dataset.

The list is still empty; `length()` is `0` until you append or insert values.

**Example**

```cpp
int expected = 100;
List values(expected);
for (int i = 0; i < expected; ++i) {
  values.append(i);
}
```

*This example reserves enough space for 100 integers up front to reduce internal resizing.*

---

#### `template <typename... Args> List(const Args &...args)`

**Signature**

```cpp
template <typename... Args> List(const Args &...args) : List() {
  variadic_append(args...);
}
```

**Synopsis**
Creates a list with the given values: List(1, "two", 3.0f)

**Parameters**

* `args` – One or more values to insert into the list in order. Each must be a type that `List` can store (for example, `bool`, `int`, `float`, `const char *`, `String`, `List`, `Map`, or `Set`).

**Return value**

* *(constructor; not applicable)*

**In Depth**

This variadic constructor provides a compact way to create a `List` with an initial set of values. Internally, it:

1. Constructs an empty list.
2. Appends each `args` value in order using the same machinery as `append()`.

This is convenient for small lists or when you want to express literal-like sequences.

**Example**

```cpp
List mixed(1, "two", 3.0f);
// mixed holds [1, "two", 3.0f]
```

*This example creates a heterogeneous list in a single expression.*

---

#### `List(const List &other)`

**Signature**

```cpp
List(const List &other);
```

**Synopsis**
Creates a copy of another list.

**Parameters**

* `other` – Existing list whose contents should be copied.

**Return value**

* *(constructor; not applicable)*

**In Depth**

The copy constructor creates a new list that contains the same sequence of values as `other`. The exact ownership model is internal, but from the user’s perspective:

* Modifying one list does not change the contents of the other.
* Memory and handles are cleaned up automatically when each list is destroyed.

**Example**

```cpp
List original(1, 2, 3);
List copy(original);
// copy initially has the same elements as original
```

*This example duplicates a list so both variables start with the same contents.*

---

#### `List(const Set &set)`

**Signature**

```cpp
List(const Set &set);
```

**Synopsis**
Creates a list from a set.

**Parameters**

* `set` – Source `Set` whose values will be copied into the list.

**Return value**

* *(constructor; not applicable)*

**In Depth**

This constructor converts a `Set` into a `List` by copying all values from the set. The resulting list:

* Contains each unique value exactly once (since sets do not allow duplicates).
* Does not promise any particular ordering; it depends on how the set stores its contents.

It is useful when you want list-like operations (index access, sorting, CSV export) on a set of values.

**Example**

```cpp
Set unique(3, 1, 2);   // set with {3, 1, 2}
List asList(unique);   // list with the same values, order unspecified
```

*This example converts a set of numbers into a list to enable indexed operations.*

---

#### `~List()`

**Signature**

```cpp
~List();
```

**Synopsis**
Destroys the list and frees memory.

**Parameters**

* *(none)*

**Return value**

* *(destructor; not applicable)*

**In Depth**

When a `List` goes out of scope or is otherwise destroyed, its destructor:

* Releases internal buffers.
* Cleans up any associated resources.

Because attoboy uses RAII, you never need to manually free memory associated with a list.

**Example**

```cpp
{
  List temp(1, 2, 3);
  // use temp here
}
// temp is destroyed; associated memory is released
```

*This example shows a scoped list that is automatically cleaned up.*

---

#### `List &operator=(const List &other)`

**Signature**

```cpp
List &operator=(const List &other);
```

**Synopsis**
Assigns another list to this list.

**Parameters**

* `other` – List whose contents should replace the current ones.

**Return value**

* Reference to `*this`, enabling assignment chaining.

**In Depth**

The copy assignment operator replaces the contents of the current list with those of `other`. After assignment:

* `length()` matches `other.length()`.
* Element values match those in `other`, in the same order.

Self-assignment (`list = list`) is safe.

**Example**

```cpp
List a(1, 2);
List b(3, 4, 5);

a = b;
// a now holds [3, 4, 5]
```

*This example overwrites one list with another.*

---

#### `int length() const`

**Signature**

```cpp
int length() const;
```

**Synopsis**
Returns the number of elements in the list.

**Parameters**

* *(none)*

**Return value**

* Number of elements currently stored.

**In Depth**

`length()` is the primary way to determine how many elements are in the list. Valid indices for element access range from `0` to `length() - 1` (and the corresponding negative range from `-length()` to `-1`).

**Example**

```cpp
List values(1, 2, 3);
int n = values.length();  // 3
```

*This example queries the current number of elements.*

---

#### `bool isEmpty() const`

**Signature**

```cpp
bool isEmpty() const;
```

**Synopsis**
Returns true if the list is empty.

**Parameters**

* *(none)*

**Return value**

* `true` if `length() == 0`; `false` otherwise.

**In Depth**

This is a convenience function equivalent to checking `length() == 0`. It is often clearer and more expressive in conditions.

**Example**

```cpp
List values;
if (values.isEmpty()) {
  values.append("first");
}
```

*This example initializes a list only if it is currently empty.*

---

#### `template <typename T> List &append(T value)`

**Signature**

```cpp
template <typename T> List &append(T value) {
  append_impl(value);
  return *this;
}
```

**Synopsis**
Appends a value to the end. Returns this list for chaining.

**Parameters**

* `value` – Value to add to the end of the list. Must be a supported type (such as `bool`, `int`, `float`, `const char *`, `String`, `List`, `Map`, or `Set`).

**Return value**

* Reference to `*this`, allowing chains like `list.append(1).append(2)`.

**In Depth**

`append()` adds a new element after the current last element. It can increase the list’s length by one. If internal capacity is exceeded, the list automatically grows.

Because the method is templated, you can pass values of different types. Internally, the appropriate overload of `append_impl()` is chosen.

**Example**

```cpp
List values;
values.append(1)
      .append("two")
      .append(3.0f);
// values holds [1, "two", 3.0f]
```

*This example appends multiple heterogeneous values using method chaining.*

---

#### `template <typename T> List &prepend(T value)`

**Signature**

```cpp
template <typename T> List &prepend(T value) {
  prepend_impl(value);
  return *this;
}
```

**Synopsis**
Prepends a value to the beginning. Returns this list for chaining.

**Parameters**

* `value` – Value to insert at the front.

**Return value**

* Reference to `*this`.

**In Depth**

`prepend()` inserts a value at index `0` and shifts existing elements to higher indices. This is useful when building lists from the front or when you need to quickly add a header element.

**Example**

```cpp
List values(2, 3);
values.prepend(1);
// values holds [1, 2, 3]
```

*This example adds an element at the beginning of the list.*

---

#### `template <typename T> List &insert(int index, T value)`

**Signature**

```cpp
template <typename T> List &insert(int index, T value) {
  insert_impl(index, value);
  return *this;
}
```

**Synopsis**
Inserts a value at index. Returns this list for chaining.

**Parameters**

* `index` – Position at which to insert. Negative indices count from the end.
* `value` – Value to insert.

**Return value**

* Reference to `*this`.

**In Depth**

`insert()` adds a new element at the specified **element index**, shifting existing elements at that index and beyond to the right. The index rules are:

* `0` – insert at the beginning.
* `length()` – insert at the end (equivalent to `append()`).
* Negative values – count from the end (`-1` is just before the last element, etc.), with behavior depending on how indices are mapped internally.

You should ensure that `index` is within the allowed range (including valid negative indices); out-of-range indices result in undefined or implementation-specific behavior.

**Example**

```cpp
List values(1, 3);
values.insert(1, 2);
// values holds [1, 2, 3]
```

*This example inserts a new element into the middle of the list.*

---

#### `template <typename T> List &set(int index, T value)`

**Signature**

```cpp
template <typename T> List &set(int index, T value) {
  set_impl(index, value);
  return *this;
}
```

**Synopsis**
Sets the value at index. Returns this list for chaining.

**Parameters**

* `index` – Element index to overwrite. Negative indices count from the end.
* `value` – New value to store at that position.

**Return value**

* Reference to `*this`.

**In Depth**

`set()` replaces the value at the given index without changing the list’s length or the positions of other elements. Indexing rules (including negative indices) are the same as for `insert()` and `at()`.

You should ensure that the index refers to an existing element (i.e., `-length() <= index < length()`).

**Example**

```cpp
List values("red", "green", "blue");
values.set(1, "yellow");
// values holds ["red", "yellow", "blue"]
```

*This example overwrites a single element in the list.*

---

#### `List &remove(int index)`

**Signature**

```cpp
List &remove(int index);
```

**Synopsis**
Removes the element at index. Returns this list for chaining.

**Parameters**

* `index` – Element index to remove. Negative indices count from the end.

**Return value**

* Reference to `*this`.

**In Depth**

`remove()` deletes the element at the specified index and shifts subsequent elements one position to the left. The list’s length decreases by one.

Behavior is undefined if the index is out of range. You can guard with `length()` or use `isEmpty()` when working with negative indices like `-1`.

**Example**

```cpp
List values("a", "b", "c");
values.remove(1);
// values holds ["a", "c"]
```

*This example removes the middle element from a three-element list.*

---

#### `List &clear()`

**Signature**

```cpp
List &clear();
```

**Synopsis**
Removes all elements. Returns this list for chaining.

**Parameters**

* *(none)*

**Return value**

* Reference to `*this`.

**In Depth**

`clear()` empties the list completely:

* `length()` becomes `0`.
* `isEmpty()` becomes `true`.

Capacity may or may not be reduced; implementation details are hidden.

**Example**

```cpp
List values(1, 2, 3);
values.clear();
// values is now empty
```

*This example clears all elements from the list.*

---

#### `List &reverse()`

**Signature**

```cpp
List &reverse();
```

**Synopsis**
Reverses element order in place. Returns this list for chaining.

**Parameters**

* *(none)*

**Return value**

* Reference to `*this`.

**In Depth**

`reverse()` reorders the list so that the first element becomes the last, the second becomes the second-to-last, and so on. Length and element types do not change; only their positions do.

**Example**

```cpp
List values(1, 2, 3);
values.reverse();
// values holds [3, 2, 1]
```

*This example reverses the order of elements.*

---

#### `List &sort(bool ascending = true)`

**Signature**

```cpp
List &sort(bool ascending = true);
```

**Synopsis**
Sorts elements. Returns this list for chaining.

**Parameters**

* `ascending` – If `true` (default), sort in ascending order; if `false`, sort in descending order.

**Return value**

* Reference to `*this`.

**In Depth**

`sort()` reorders elements according to an internal comparison. Because `List` can store mixed types, sorting behavior for lists with different value types is implementation-dependent. For predictable results:

* Ensure all elements are of the **same type** (for example, all `int` or all `String`).
* Avoid mixing numbers, strings, and containers in the same list when sorting.

**Example**

```cpp
List numbers(3, 1, 2);
numbers.sort();
// numbers holds [1, 2, 3]
```

*This example sorts a purely numeric list in ascending order.*

---

#### `List &concat(const List &other)`

**Signature**

```cpp
List &concat(const List &other);
```

**Synopsis**
Appends all elements from another list. Returns this list for chaining.

**Parameters**

* `other` – List whose elements will be appended to the end of this list.

**Return value**

* Reference to `*this`.

**In Depth**

`concat()` iterates over `other` and appends each element in order. The original `other` is not modified. The resulting list’s length becomes `length() + other.length()`.

**Example**

```cpp
List a(1, 2);
List b(3, 4);

a.concat(b);
// a holds [1, 2, 3, 4]
// b is unchanged
```

*This example concatenates all elements from one list onto another.*

---

#### `List &concat(const Set &set)`

**Signature**

```cpp
List &concat(const Set &set);
```

**Synopsis**
Appends all values from a set. Returns this list for chaining.

**Parameters**

* `set` – `Set` whose values will be appended.

**Return value**

* Reference to `*this`.

**In Depth**

This overload behaves like `concat(const List&)`, but reads elements from a `Set`. Because sets are unordered, the order in which values are appended is implementation-specific.

Duplicates are not introduced by the set itself (each value appears at most once), but appending to an existing list may still produce duplicates overall.

**Example**

```cpp
Set tags("red", "green");
List allTags("blue");
allTags.concat(tags);
// allTags now holds "blue" plus the set's values, order unspecified
```

*This example combines a list and a set into a single list.*

---

#### `template <typename T> T pop()`

**Signature**

```cpp
template <typename T> T pop();
```

**Synopsis**
Removes and returns the element at index.

**Parameters**

* *(none in the signature; the index is implied)*

**Return value**

* Element removed from the list, converted to type `T`.

**In Depth**

Although the synopsis says “at index”, the signature does not take an explicit index parameter. The intended behavior is to remove and return an element, typically from the end or another fixed position chosen by the implementation. You must request a type `T` that matches the stored value’s type.

Important considerations:

* If the list is empty, behavior is undefined; check `isEmpty()` first.
* If the stored element’s type does not match `T`, the result is undefined or may not be meaningful.

**Example**

```cpp
List values(1, 2, 3);
int last = values.pop<int>();
// last holds 3 (implementation-dependent)
// values now holds [1, 2]
```

*This example removes and returns an element from the list, then uses the modified list.*

---

#### `template <typename T> T at(int index) const`

**Signature**

```cpp
template <typename T> T at(int index) const;
```

**Synopsis**
Returns the element at index.

**Parameters**

* `index` – Element index to access. Negative indices count from the end.

**Return value**

* Value at the given index, as type `T`.

**In Depth**

`at<T>()` returns a copy of the element at the specified index, converted to the requested type `T`. You must:

1. Ensure `index` is within bounds (`-length() <= index < length()`).
2. Ensure the element at that index has a type compatible with `T`, which you can check using `typeAt(index)`.

If the type does not match, the behavior is undefined or may produce a meaningless value.

**Example**

```cpp
List values("Alice", "Bob");
String name = values.at<String>(0);  // "Alice"
```

*This example retrieves a string element by index using `at<String>()`.*

---

#### `template <typename T> T operator[](int index) const`

**Signature**

```cpp
template <typename T> T operator[](int index) const;
```

**Synopsis**
Returns the element at index.

**Parameters**

* `index` – Element index to access. Negative indices count from the end.

**Return value**

* Value at the given index, as type `T`.

**In Depth**

This operator is a more idiomatic way to access elements with bracket syntax:

```cpp
T value = list[index];
```

However, because `List` stores mixed types, you **must** specify the template parameter explicitly in code that expects a particular type (for example, `list.at<String>(i)` is usually clearer than `list<String>[i]`).

Indexing and type-matching rules are the same as for `at<T>()`.

**Example**

```cpp
List values("Alice", "Bob");
String second = values.operator[]<String>(1);  // "Bob"
```

*This example demonstrates element access through the bracket-like operator.*

---

#### `ValueType typeAt(int index) const`

**Signature**

```cpp
ValueType typeAt(int index) const;
```

**Synopsis**
Returns the type of the element at index.

**Parameters**

* `index` – Element index to inspect. Negative indices count from the end.

**Return value**

* A `ValueType` enum describing the stored type (for example, `TYPE_INT`, `TYPE_STRING`, etc.).

**In Depth**

`typeAt()` allows you to safely determine which type is stored at a given index before retrieving it via `at<T>()`, `pop<T>()`, or `operator[]<T>()`. If the index is out of bounds, the result is undefined; you should check ranges first.

**Example**

```cpp
List values(1, "two", 3.0f);
ValueType t0 = values.typeAt(0);  // TYPE_INT (typically)
ValueType t1 = values.typeAt(1);  // TYPE_STRING
ValueType t2 = values.typeAt(2);  // TYPE_FLOAT
```

*This example inspects the types of several elements in a mixed list.*

---

#### `List slice(int start, int end) const`

**Signature**

```cpp
List slice(int start, int end) const;
```

**Synopsis**
Returns a new list with elements from start to end.

**Parameters**

* `start` – Index of the first element to include. Negative indices count from the end.
* `end` – Index **after** the last element to include (exclusive). Negative indices count from the end.

**Return value**

* New `List` containing the elements in the range `[start, end)`.

**In Depth**

`slice()` creates a shallow copy of a subrange of the list. For example:

* `slice(0, length())` returns a copy of the entire list.
* `slice(1, 3)` returns elements at indices `1` and `2`.

Indices work the same as in `at()` and `set()`. You must ensure `start` and `end` describe a valid subrange.

**Example**

```cpp
List values(10, 20, 30, 40);
List middle = values.slice(1, 3);
// middle holds [20, 30]
```

*This example extracts a sublist from the middle of a larger list.*

---

#### `List duplicate() const`

**Signature**

```cpp
List duplicate() const;
```

**Synopsis**
Returns a copy of this list.

**Parameters**

* *(none)*

**Return value**

* New `List` containing the same elements as the original.

**In Depth**

`duplicate()` provides an explicit way to obtain a copy of the list. It is equivalent in spirit to the copy constructor but may be clearer when used in method chains:

```cpp
List copy = original.duplicate().reverse();
```

**Example**

```cpp
List original(1, 2, 3);
List copy = original.duplicate();
// copy initially holds [1, 2, 3]
```

*This example duplicates a list to work with a separate copy.*

---

#### `template <typename T> int find(T value) const`

**Signature**

```cpp
template <typename T> int find(T value) const;
```

**Synopsis**
Returns the index of the first occurrence of value, or -1.

**Parameters**

* `value` – Value to search for. The type must be comparable with the stored elements.

**Return value**

* Index of the first matching element, or `-1` if not found.

**In Depth**

`find()` scans the list from the beginning and looks for an element equal to `value`. Equality is based on the type-specific comparison internal to the list. For consistent results:

* Pass `value` with the same type as elements you expect to match.
* Avoid searching for complex nested containers unless you know how they are compared.

**Example**

```cpp
List values("apple", "banana", "apple");
int firstApple = values.find("apple");  // 0
int missing = values.find("pear");      // -1
```

*This example finds the first occurrence of a string in the list.*

---

#### `template <typename T> bool contains(T value) const`

**Signature**

```cpp
template <typename T> bool contains(T value) const;
```

**Synopsis**
Returns true if the list contains the value.

**Parameters**

* `value` – Value to search for.

**Return value**

* `true` if `find(value)` returns a non-negative index; `false` otherwise.

**In Depth**

`contains()` is a convenience wrapper around `find()` that returns a boolean. It is ideal for quick membership tests.

**Example**

```cpp
List values(1, 2, 3);
bool hasTwo = values.contains(2);  // true
bool hasFive = values.contains(5); // false
```

*This example checks whether certain values are present in the list.*

---

#### `String toCSVString() const`

**Signature**

```cpp
String toCSVString() const;
```

**Synopsis**
Converts a list of lists to a CSV string.

**Parameters**

* *(none)*

**Return value**

* A `String` containing a CSV representation of the list.

**In Depth**

`toCSVString()` expects the current list to represent **rows** of a table, where each element is itself a `List` representing the columns in that row:

* Each inner list corresponds to one CSV row.
* Elements in each inner list become cell values in that row.

The function handles common CSV requirements like separating values with commas and ending rows with newlines. Exact quoting and escaping rules are implementation-specific.

**Example**

```cpp
List row1("Alice", 30);
List row2("Bob", 25);

List table;
table.append(row1).append(row2);

String csv = table.toCSVString();
// csv might be:
// Alice,30
// Bob,25
```

*This example converts a list-of-lists table into a simple CSV string.*

---

#### `static List FromCSVString(const String &csv)`

**Signature**

```cpp
static List FromCSVString(const String &csv);
```

**Synopsis**
Creates a list of lists from a CSV string.

**Parameters**

* `csv` – CSV-formatted text to parse.

**Return value**

* A `List` where each element is an inner `List` representing one row, and each inner element is a cell value (typically `String`).

**In Depth**

`FromCSVString()` is the inverse of `toCSVString()`:

* Input: a CSV text, potentially with multiple lines.
* Output: a list of rows, each row a list of cell values.

It allows you to reconstruct structured tabular data from textual CSV content.

**Example**

```cpp
String csv("Alice,30\nBob,25\n");
List table = List::FromCSVString(csv);

// table[0] is ["Alice", "30"]
// table[1] is ["Bob", "25"]
```

*This example parses CSV text into a list-of-lists representation.*

---

#### `String toJSONString() const`

**Signature**

```cpp
String toJSONString() const;
```

**Synopsis**
Converts this list to a JSON array string.

**Parameters**

* *(none)*

**Return value**

* A `String` containing a JSON array representation of the list.

**In Depth**

`toJSONString()` serializes the list to a JSON array string. Each element is converted to JSON according to its type:

* Numbers become JSON numbers.
* Booleans become `true`/`false`.
* Strings become quoted JSON strings with appropriate escaping.
* Nested `List`, `Map`, and `Set` elements become nested arrays or objects.

This is useful for logging, configuration, and communication with JSON-based APIs.

**Example**

```cpp
List values;
values.append("Alice").append(30).append(true);

String json = values.toJSONString();
// e.g., ["Alice",30,true]
```

*This example converts a heterogeneous list into a JSON array string.*

---

#### `static List FromJSONString(const String &json)`

**Signature**

```cpp
static List FromJSONString(const String &json);
```

**Synopsis**
Creates a list from a JSON array string.

**Parameters**

* `json` – JSON array text to parse.

**Return value**

* A `List` representing the parsed JSON array.

**In Depth**

`FromJSONString()` parses a JSON array string and constructs a `List` where:

* Each JSON element becomes a corresponding value in the list.
* JSON nested arrays and objects become `List` and `Map` values, respectively.

If the input JSON is not a valid array, the resulting list may be empty or reflect an error state; you should validate the input if needed.

**Example**

```cpp
String json("[\"Alice\",30,true]");
List values = List::FromJSONString(json);

// values[0] == "Alice"
// values[1] == "30" or integer 30 (implementation-dependent representation)
// values[2] == true
```

*This example parses JSON array text into a `List` suitable for further processing.*

---

#### `bool compare(const List &other) const`

**Signature**

```cpp
bool compare(const List &other) const;
```

**Synopsis**
Returns true if this list equals the other list.

**Parameters**

* `other` – List to compare against.

**Return value**

* `true` if both lists have the same length and corresponding elements are equal; `false` otherwise.

**In Depth**

`compare()` performs an element-wise comparison:

* First, it compares lengths.
* Then, it compares each element (including type and value).

It is functionally equivalent to `operator==` and is primarily a named alternative for code that prefers explicit method calls.

**Example**

```cpp
List a(1, 2, 3);
List b(1, 2, 3);
List c(1, 2);

bool ab = a.compare(b);  // true
bool ac = a.compare(c);  // false
```

*This example compares two lists for equality using `compare()`.*

---

#### `bool operator==(const List &other) const`

**Signature**

```cpp
bool operator==(const List &other) const;
```

**Synopsis**
Returns true if this list equals the other list.

**Parameters**

* `other` – List to compare against.

**Return value**

* `true` if lists are equal; `false` otherwise.

**In Depth**

This operator is the idiomatic way to test list equality using `==`. It delegates to the same logic as `compare()`, comparing lengths and elements.

**Example**

```cpp
List a(1, 2, 3);
List b(1, 2, 3);
List c(3, 2, 1);

bool sameOrder = (a == b); // true
bool diffOrder = (a == c); // false
```

*This example shows equality tests based on both value and order.*

---

#### `bool operator!=(const List &other) const`

**Signature**

```cpp
bool operator!=(const List &other) const;
```

**Synopsis**
Returns true if this list does not equal the other list.

**Parameters**

* `other` – List to compare against.

**Return value**

* `true` if lists are not equal; `false` if they are equal.

**In Depth**

This is simply the logical negation of `operator==`.

**Example**

```cpp
List a(1, 2, 3);
List b(1, 2);

if (a != b) {
  // lists differ in length or content
}
```

*This example uses the inequality operator to detect differences.*

---

#### `template <typename T> List operator+(T value) const`

**Signature**

```cpp
template <typename T> List operator+(T value) const {
  List result = duplicate();
  result.append(value);
  return result;
}
```

**Synopsis**
Returns a new list with the value appended.

**Parameters**

* `value` – Value to append to a copy of the current list.

**Return value**

* New `List` containing all original elements followed by `value`.

**In Depth**

This operator provides a functional-style way to build new lists without mutating the original:

* It duplicates the current list.
* It appends `value` to the duplicate.
* It returns the new list.

This is useful when you want to preserve the original list and work with an extended version.

**Example**

```cpp
List base(1, 2);
List extended = base + 3;
// base still holds [1, 2]
// extended holds [1, 2, 3]
```

*This example uses the `+` operator to create a new list with one additional element, leaving the original unchanged.*
