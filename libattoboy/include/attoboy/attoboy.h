#ifndef ATTOBOY_H
#define ATTOBOY_H

// Entry point for the application (bypasses CRT startup)
extern "C" void atto_main();

// Character type aliases and macros based on UNICODE configuration
#ifdef UNICODE
using ATTO_CHAR = wchar_t;
#define ATTO_TEXT(x) L##x
#else
using ATTO_CHAR = char;
#define ATTO_TEXT(x) x
#endif

namespace attoboy {

/// Type identifier for values stored in containers.
enum ValueType {
  TYPE_NULL = 0, ///< Null or uninitialized value
  TYPE_INVALID,  ///< Invalid or error state
  TYPE_BOOL,     ///< Boolean value (true/false)
  TYPE_INT,      ///< Integer value (32-bit)
  TYPE_FLOAT,    ///< Floating-point value (single precision)
  TYPE_STRING,   ///< String value (attoboy::String)
  TYPE_LIST,     ///< List value (attoboy::List)
  TYPE_MAP,      ///< Map value (attoboy::Map)
  TYPE_SET,      ///< Set value (attoboy::Set)
  TYPE_UNKNOWN   ///< Unknown or unrecognized type
};

class StringImpl;
class ListImpl;
class MapImpl;
class SetImpl;
class DateTimeImpl;
class BufferImpl;
class ArgumentsImpl;
class ThreadImpl;
class MutexImpl;
class PathImpl;
class FileImpl;

// Forward declarations
class List;
class Map;
class Set;

/// Immutable string class with batteries-included functionality.
class String {
public:
  /// Creates an empty string.
  String();

  /// Creates a string from a C-style string.
  String(const ATTO_CHAR *str);

  /// Creates a string from a boolean ("true" or "false").
  String(bool value);

  /// Creates a string from an integer.
  String(int value);

  /// Creates a string from a long integer.
  String(long long value);

  /// Creates a string from a floating-point number.
  String(float value);

  /// Creates a JSON string from a list (e.g., ["value1",2,true]).
  String(const List &list);

  /// Creates a JSON string from a map (e.g., {"key":"value"}).
  String(const Map &map);

  /// Creates a JSON string from a set (e.g., [1,2,3]).
  String(const Set &set);

  /// Creates a string by concatenating multiple arguments.
  /// Each argument is converted to a String and concatenated in order.
  /// Requires at least 2 arguments.
  template <typename T, typename U, typename... Args>
  String(const T &first, const U &second, const Args &...rest)
      : String(String(first) + String(second) + String(rest...)) {}

  /// Copies another string.
  String(const String &other);

  /// Destroys the string and releases memory.
  ~String();

  /// Assigns another string to this string.
  String &operator=(const String &other);

  /// Returns the number of characters in the string.
  int length() const;

  /// Returns true if the string is empty.
  bool isEmpty() const;

  /// Returns a pointer to the underlying C-style string.
  const ATTO_CHAR *c_str() const;

  /// Returns the character at index as a single-character string.
  /// Negative indices count from the end. Returns empty string if out of
  /// bounds.
  String at(int index) const;

  /// Returns a substring from start (inclusive) to end (exclusive).
  /// Negative indices count from the end. If end is -1, extracts to the end.
  String substring(int start, int end = -1) const;

  /// Returns a copy of this string.
  String duplicate() const;

  /// Returns true if this string contains the substring.
  bool contains(const String &substring) const;

  /// Returns true if this string starts with the substring.
  bool startsWith(const String &substring) const;

  /// Returns true if this string ends with the substring.
  bool endsWith(const String &substring) const;

  /// Returns the number of non-overlapping occurrences of substring.
  int count(const String &substring) const;

  /// Returns true if the string is a valid integer or decimal number.
  bool isNumber() const;

  /// Converts the string to an integer. Returns 0 if invalid.
  int toInteger() const;

  /// Converts the string to a float. Returns 0.0 if invalid.
  float toFloat() const;

  /// Converts the string to a boolean.
  /// Returns true for "true", "t", "1", "yes", "on" (case-insensitive).
  bool toBool() const;

  /// Returns a new string with substring appended to the end.
  String append(const String &substring) const;

  /// Returns a new string with substring prepended to the beginning.
  String prepend(const String &substring) const;

  /// Returns a new string with substring inserted at index.
  /// Negative indices count from the end.
  String insert(int index, const String &substring) const;

  /// Returns a new string with characters from start to end removed.
  /// Negative indices count from the end.
  String remove(int start, int end) const;

  /// Returns a new string with all occurrences of target replaced.
  String replace(const String &target, const String &replacement) const;

  /// Returns a new string with leading and trailing whitespace removed.
  String trim() const;

  /// Returns a new string with all characters converted to uppercase.
  String upper() const;

  /// Returns a new string with all characters converted to lowercase.
  String lower() const;

  /// Returns a new string with characters in reverse order.
  String reverse() const;

  /// Returns a new string with the content repeated count times.
  String repeat(int count) const;

  /// Returns true if this string equals the other string.
  bool equals(const String &other) const;

  /// Compares this string with another lexicographically.
  /// Returns negative if this < other, 0 if equal, positive if this > other.
  int compare(const String &other) const;

  /// Returns true if this string equals the other string.
  bool operator==(const String &other) const;

  /// Returns true if this string does not equal the other string.
  bool operator!=(const String &other) const;

  /// Returns the concatenation of this string and other.
  String operator+(const String &other) const;

  /// Returns a hash code for this string.
  int hash() const;

  /// Splits this string into a list by line breaks.
  /// Handles both Windows (\r\n) and Unix (\n) line endings.
  List lines() const;

  /// Joins all elements of list using this string as separator.
  /// Non-string elements are converted to strings automatically.
  String join(const List &list) const;

  /// Splits this string by separator into a list.
  /// The max parameter controls maximum splits (default 1).
  /// Returns up to max+2 parts (e.g., max=1 allows 1 split, yielding 2 parts).
  List split(const String &separator, int max = 1) const;

  /// Splits this string by whitespace into a list.
  /// Consecutive whitespace is treated as a single separator.
  List split() const;

  /// Formats this string using list elements as replacements.
  /// Replaces {0}, {1}, etc. with corresponding list elements.
  String format(const List &list) const;

  /// Formats this string using map values as replacements.
  /// Replaces {key} with the corresponding value from the map.
  String format(const Map &map) const;

private:
  StringImpl *impl;
};

/// Dynamic array that stores mixed types of values.
class List {
public:
  /// Creates an empty list.
  List();

  /// Creates an empty list with the specified initial capacity.
  List(int capacity);

  /// Creates a list with the specified values.
  template <typename... Args> List(const Args &...args) : List() {
    variadic_append(args...);
  }

  /// Copies another list.
  List(const List &other);

  /// Creates a list from a set.
  List(const Set &set);

  /// Destroys the list and releases memory.
  ~List();

  /// Assigns another list to this list.
  List &operator=(const List &other);

  /// Returns the number of elements in the list.
  int length() const;

  /// Returns true if the list is empty.
  bool isEmpty() const;

  /// Appends a value to the end of the list.
  /// Returns a reference to this list for chaining.
  template <typename T> List &append(T value) {
    append_impl(value);
    return *this;
  }

  /// Prepends a value to the beginning of the list.
  /// Returns a reference to this list for chaining.
  template <typename T> List &prepend(T value) {
    prepend_impl(value);
    return *this;
  }

  /// Inserts a value at the specified index.
  /// If index is beyond the end, appends. If index < 0, prepends.
  /// Returns a reference to this list for chaining.
  template <typename T> List &insert(int index, T value) {
    insert_impl(index, value);
    return *this;
  }

  /// Removes and returns the value at index.
  /// Index is clamped to valid range. Shifts remaining elements.
  template <typename T> T pop();

  /// Returns the value at index.
  /// Index is clamped to valid range [0, length-1].
  template <typename T> T at(int index) const;

  /// Returns the value at index using [] operator.
  /// Index is clamped to valid range [0, length-1].
  template <typename T> T operator[](int index) const;

  /// Sets the value at index.
  /// Index is clamped to valid range. If list is empty, appends the value.
  /// Returns a reference to this list for chaining.
  template <typename T> List &set(int index, T value) {
    set_impl(index, value);
    return *this;
  }

  /// Removes the element at index and shifts remaining elements.
  /// Index is clamped to valid range.
  /// Returns a reference to this list for chaining.
  List &remove(int index);

  /// Removes all elements from the list.
  /// Returns a reference to this list for chaining.
  List &clear();

  /// Returns the index of the first occurrence of value, or -1 if not found.
  /// Uses numeric coercion (e.g., int 5 equals float 5.0).
  template <typename T> int find(T value) const;

  /// Returns true if the list contains value.
  /// Uses numeric coercion (e.g., int 5 equals float 5.0).
  template <typename T> bool contains(T value) const;

  /// Reverses the order of elements in place.
  /// Returns a reference to this list for chaining.
  List &reverse();

  /// Sorts the list in ascending or descending order.
  /// Uses best-effort cross-type comparison with numeric coercion.
  /// Returns a reference to this list for chaining.
  List &sort(bool ascending = true);

  /// Returns the type of the value at index.
  /// Returns TYPE_INVALID if index is out of bounds.
  ValueType typeAt(int index) const;

  /// Returns a new list containing elements from start to end.
  /// Negative indices count from the end. Indices are clamped.
  List slice(int start, int end) const;

  /// Appends all elements from another list to this list.
  /// Returns a reference to this list for chaining.
  List &concat(const List &other);

  /// Appends all values from a set to this list.
  /// Returns a reference to this list for chaining.
  List &concat(const Set &set);

  /// Returns a copy of this list.
  List duplicate() const;

  /// Returns true if this list equals the other list.
  /// Lists are equal if they have the same types and values in order.
  bool compare(const List &other) const;

  /// Returns true if this list equals the other list.
  bool operator==(const List &other) const;

  /// Returns true if this list does not equal the other list.
  bool operator!=(const List &other) const;

  /// Returns a new list with the value appended.
  template <typename T> List operator+(T value) const {
    List result = duplicate();
    result.append(value);
    return result;
  }

private:
  ListImpl *impl;

  // Implementation methods for different types
  void append_impl(bool value);
  void append_impl(int value);
  void append_impl(float value);
  void append_impl(const ATTO_CHAR *value);
  void append_impl(const String &value);
  void append_impl(const List &value);
  void append_impl(const Map &value);
  void append_impl(const Set &value);

  void prepend_impl(bool value);
  void prepend_impl(int value);
  void prepend_impl(float value);
  void prepend_impl(const ATTO_CHAR *value);
  void prepend_impl(const String &value);
  void prepend_impl(const List &value);
  void prepend_impl(const Map &value);
  void prepend_impl(const Set &value);

  void insert_impl(int index, bool value);
  void insert_impl(int index, int value);
  void insert_impl(int index, float value);
  void insert_impl(int index, const ATTO_CHAR *value);
  void insert_impl(int index, const String &value);
  void insert_impl(int index, const List &value);
  void insert_impl(int index, const Map &value);
  void insert_impl(int index, const Set &value);

  void set_impl(int index, bool value);
  void set_impl(int index, int value);
  void set_impl(int index, float value);
  void set_impl(int index, const ATTO_CHAR *value);
  void set_impl(int index, const String &value);
  void set_impl(int index, const List &value);
  void set_impl(int index, const Map &value);
  void set_impl(int index, const Set &value);

  // Variadic helper for constructor
  template <typename T, typename... Args>
  void variadic_append(const T &first, const Args &...rest) {
    append(first);
    variadic_append(rest...);
  }

  void variadic_append() {} // Base case
};

/// Key-value map that stores mixed types.
/// Keys must be unique. Insertion order is not guaranteed.
class Map {
public:
  /// Creates an empty map.
  Map();

  /// Creates an empty map with the specified initial capacity.
  Map(int capacity);

  /// Creates a map with alternating key-value pairs.
  /// If argument count is odd, the last key gets a null value.
  template <typename... Args> Map(const Args &...args) : Map() {
    variadic_put_pairs(args...);
  }

  /// Copies another map.
  Map(const Map &other);

  /// Destroys the map and releases memory.
  ~Map();

  /// Assigns another map to this map.
  Map &operator=(const Map &other);

  /// Returns the number of key-value pairs in the map.
  int length() const;

  /// Returns true if the map is empty.
  bool isEmpty() const;

  /// Returns the value for key, or defaultValue if not found.
  template <typename K, typename V> V get(K key, V defaultValue = V()) const;

  /// Returns the value for key using [] operator.
  /// Returns a null-type value if key is not found.
  template <typename K, typename V> V operator[](K key) const;

  /// Sets key to value. If key exists, updates its value.
  /// Returns a reference to this map for chaining.
  template <typename K, typename V> Map &put(K key, V value);

  /// Removes the key-value pair with the specified key.
  /// Returns a reference to this map for chaining.
  template <typename K> Map &remove(K key);

  /// Removes all key-value pairs from the map.
  /// Returns a reference to this map for chaining.
  Map &clear();

  /// Returns the first key with the specified value.
  /// Returns a null-type value if not found.
  template <typename K, typename V> K findValue(V value) const;

  /// Returns true if the map contains key.
  template <typename K> bool hasKey(K key) const;

  /// Returns the type of the value for key.
  /// Returns TYPE_INVALID if key is not found.
  template <typename K> ValueType typeAt(K key) const;

  /// Adds all key-value pairs from other to this map.
  /// If a key exists in both, uses the value from other.
  /// Returns a reference to this map for chaining.
  Map &merge(const Map &other);

  /// Returns a copy of this map.
  Map duplicate() const;

  /// Returns a list of all keys in the map.
  List keys() const;

  /// Returns a list of all values in the map.
  List values() const;

  /// Returns true if this map equals the other map.
  /// Maps are equal if they have the same keys and values.
  bool compare(const Map &other) const;

  /// Returns true if this map equals the other map.
  bool operator==(const Map &other) const;

  /// Returns true if this map does not equal the other map.
  bool operator!=(const Map &other) const;

private:
  MapImpl *impl;

  // Implementation methods for different types
  void put_impl(bool key, bool value);
  void put_impl(bool key, int value);
  void put_impl(bool key, float value);
  void put_impl(bool key, const ATTO_CHAR *value);
  void put_impl(bool key, const String &value);
  void put_impl(bool key, const List &value);
  void put_impl(bool key, const Map &value);
  void put_impl(bool key, const Set &value);

  void put_impl(int key, bool value);
  void put_impl(int key, int value);
  void put_impl(int key, float value);
  void put_impl(int key, const ATTO_CHAR *value);
  void put_impl(int key, const String &value);
  void put_impl(int key, const List &value);
  void put_impl(int key, const Map &value);
  void put_impl(int key, const Set &value);

  void put_impl(float key, bool value);
  void put_impl(float key, int value);
  void put_impl(float key, float value);
  void put_impl(float key, const ATTO_CHAR *value);
  void put_impl(float key, const String &value);
  void put_impl(float key, const List &value);
  void put_impl(float key, const Map &value);
  void put_impl(float key, const Set &value);

  void put_impl(const ATTO_CHAR *key, bool value);
  void put_impl(const ATTO_CHAR *key, int value);
  void put_impl(const ATTO_CHAR *key, float value);
  void put_impl(const ATTO_CHAR *key, const ATTO_CHAR *value);
  void put_impl(const ATTO_CHAR *key, const String &value);
  void put_impl(const ATTO_CHAR *key, const List &value);
  void put_impl(const ATTO_CHAR *key, const Map &value);
  void put_impl(const ATTO_CHAR *key, const Set &value);

  void put_impl(const String &key, bool value);
  void put_impl(const String &key, int value);
  void put_impl(const String &key, float value);
  void put_impl(const String &key, const ATTO_CHAR *value);
  void put_impl(const String &key, const String &value);
  void put_impl(const String &key, const List &value);
  void put_impl(const String &key, const Map &value);
  void put_impl(const String &key, const Set &value);

  void remove_impl(bool key);
  void remove_impl(int key);
  void remove_impl(float key);
  void remove_impl(const ATTO_CHAR *key);
  void remove_impl(const String &key);

  // Variadic helper for constructor - handles alternating key-value pairs
  template <typename K, typename V, typename... Args>
  void variadic_put_pairs(const K &key, const V &value, const Args &...rest) {
    put(key, value);
    variadic_put_pairs(rest...);
  }

  // Handle odd number of arguments - last key gets null value
  template <typename K> void variadic_put_pairs(const K &key) {
    put_impl(key, false); // Put with null/false value
  }

  void variadic_put_pairs() {} // Base case
};

// Template implementations for Map
template <typename K, typename V> inline Map &Map::put(K key, V value) {
  put_impl(key, value);
  return *this;
}

template <typename K> inline Map &Map::remove(K key) {
  remove_impl(key);
  return *this;
}

/// Set that stores unique values of mixed types.
/// Values are unique (no duplicates). Insertion order is not guaranteed.
class Set {
public:
  /// Creates an empty set.
  Set();

  /// Creates an empty set with the specified initial capacity.
  Set(int capacity);

  /// Creates a set with the specified values.
  template <typename... Args> Set(const Args &...args) : Set() {
    variadic_put(args...);
  }

  /// Copies another set.
  Set(const Set &other);

  /// Creates a set from a list, removing duplicates.
  Set(const List &list);

  /// Destroys the set and releases memory.
  ~Set();

  /// Assigns another set to this set.
  Set &operator=(const Set &other);

  /// Returns the number of unique values in the set.
  int length() const;

  /// Returns true if the set is empty.
  bool isEmpty() const;

  /// Adds a value to the set. Duplicates are ignored.
  /// Returns a reference to this set for chaining.
  template <typename T> Set &put(T value) {
    put_impl(value);
    return *this;
  }

  /// Returns true if the set contains value.
  template <typename T> bool contains(T value) const;

  /// Removes value from the set if it exists.
  /// Returns a reference to this set for chaining.
  template <typename T> Set &remove(T value) {
    remove_impl(value);
    return *this;
  }

  /// Removes all values from the set.
  /// Returns a reference to this set for chaining.
  Set &clear();

  /// Adds all values from other to this set (union operation).
  /// Duplicates are automatically ignored.
  /// Returns a reference to this set for chaining.
  Set &setUnion(const Set &other);

  /// Keeps only values that exist in both this set and other (intersection).
  /// Returns a reference to this set for chaining.
  Set &intersect(const Set &other);

  /// Removes all values that exist in other from this set (difference).
  /// Returns a reference to this set for chaining.
  Set &subtract(const Set &other);

  /// Returns a copy of this set.
  Set duplicate() const;

  /// Returns a list containing all values from this set.
  List toList() const;

  /// Returns true if this set equals the other set.
  /// Sets are equal if they have the same values (order doesn't matter).
  bool compare(const Set &other) const;

  /// Returns true if this set equals the other set.
  bool operator==(const Set &other) const;

  /// Returns true if this set does not equal the other set.
  bool operator!=(const Set &other) const;

  /// Returns a new set with the value added.
  template <typename T> Set operator+(T value) const {
    Set result = duplicate();
    result.put(value);
    return result;
  }

private:
  SetImpl *impl;

  // Implementation methods for different types
  void put_impl(bool value);
  void put_impl(int value);
  void put_impl(float value);
  void put_impl(const ATTO_CHAR *value);
  void put_impl(const String &value);
  void put_impl(const List &value);
  void put_impl(const Map &value);
  void put_impl(const Set &value);

  void remove_impl(bool value);
  void remove_impl(int value);
  void remove_impl(float value);
  void remove_impl(const ATTO_CHAR *value);
  void remove_impl(const String &value);

  // Variadic helper for constructor
  template <typename T, typename... Args>
  void variadic_put(const T &first, const Args &...rest) {
    put(first);
    variadic_put(rest...);
  }

  void variadic_put() {} // Base case
};

/// Date and time with millisecond precision.
class DateTime {
public:
  /// Creates a DateTime representing the current date and time.
  DateTime();

  /// Creates a DateTime from milliseconds since Unix epoch (Jan 1, 1970).
  DateTime(long long millisSinceEpoch);

  /// Creates a DateTime from an ISO-8601 string (YYYY-MM-DDTHH:MM:SS.fffZ).
  DateTime(const String &iso8601);

  /// Copies another DateTime.
  DateTime(const DateTime &other);

  /// Destroys the DateTime and releases memory.
  ~DateTime();

  /// Assigns another DateTime to this one.
  DateTime &operator=(const DateTime &other);

  /// Adds milliseconds to this DateTime (negative to subtract).
  /// Returns a reference to this DateTime for chaining.
  DateTime &add(long long milliseconds);

  /// Returns the difference in milliseconds between this and other.
  /// Positive if this is later, negative if this is earlier.
  long long diff(const DateTime &other) const;

  /// Compares this DateTime with other.
  /// Returns negative if this < other, 0 if equal, positive if this > other.
  int compare(const DateTime &other) const;

  /// Returns true if this DateTime equals other.
  bool equals(const DateTime &other) const;

  /// Returns true if this DateTime equals other.
  bool operator==(const DateTime &other) const;

  /// Returns true if this DateTime does not equal other.
  bool operator!=(const DateTime &other) const;

  /// Returns milliseconds since Unix epoch (Jan 1, 1970).
  long long timestamp() const;

  /// Returns an ISO-8601 formatted string (YYYY-MM-DDTHH:MM:SS.fffZ).
  String toString() const;

private:
  DateTimeImpl *impl;
};

/// Mutable byte buffer for storing and manipulating binary data.
class Buffer {
public:
  /// Creates an empty buffer.
  Buffer();

  /// Creates an empty buffer with the specified initial capacity.
  Buffer(int capacity);

  /// Creates a buffer from a String, copying its byte data.
  Buffer(const String &str);

  /// Creates a buffer by copying bytes from a pointer.
  Buffer(const unsigned char *ptr, int size);

  /// Copies another buffer.
  Buffer(const Buffer &other);

  /// Destroys the buffer and releases memory.
  ~Buffer();

  /// Assigns another buffer to this buffer.
  Buffer &operator=(const Buffer &other);

  /// Returns the number of bytes in the buffer.
  int length() const;

  /// Returns true if the buffer is empty.
  bool isEmpty() const;

  /// Returns a pointer to the buffer's data and its size.
  /// For empty buffers, returns nullptr and sets len to 0.
  const unsigned char *c_ptr(int *len) const;

  /// Removes all bytes from the buffer.
  /// Returns a reference to this buffer for chaining.
  Buffer &clear();

  /// Returns true if this buffer equals other.
  bool compare(const Buffer &other) const;

  /// Appends a string's byte data to the end.
  /// Returns a reference to this buffer for chaining.
  Buffer &append(const String &str);

  /// Appends another buffer's contents to the end.
  /// Returns a reference to this buffer for chaining.
  Buffer &append(const Buffer &other);

  /// Appends bytes from a pointer to the end.
  /// Returns a reference to this buffer for chaining.
  Buffer &append(const unsigned char *ptr, int size);

  /// Prepends a string's byte data to the beginning.
  /// Returns a reference to this buffer for chaining.
  Buffer &prepend(const String &str);

  /// Prepends another buffer's contents to the beginning.
  /// Returns a reference to this buffer for chaining.
  Buffer &prepend(const Buffer &other);

  /// Prepends bytes from a pointer to the beginning.
  /// Returns a reference to this buffer for chaining.
  Buffer &prepend(const unsigned char *ptr, int size);

  /// Inserts a string's byte data at index.
  /// Returns a reference to this buffer for chaining.
  Buffer &insert(int index, const String &str);

  /// Inserts another buffer's contents at index.
  /// Returns a reference to this buffer for chaining.
  Buffer &insert(int index, const Buffer &other);

  /// Inserts bytes from a pointer at index.
  /// Returns a reference to this buffer for chaining.
  Buffer &insert(int index, const unsigned char *ptr, int size);

  /// Returns a new buffer containing bytes from start to end.
  /// If end is -1, slices to the end of the buffer.
  Buffer slice(int start, int end = -1) const;

  /// Removes bytes from start to end.
  /// If end is -1, removes to the end of the buffer.
  /// Returns a reference to this buffer for chaining.
  Buffer &remove(int start, int end = -1);

  /// Reverses the order of bytes in place.
  /// Returns a reference to this buffer for chaining.
  Buffer &reverse();

  /// Shrinks the buffer's capacity to match its length, freeing unused memory.
  /// Returns a reference to this buffer for chaining.
  Buffer &trim();

  /// Returns a compressed version of this buffer.
  /// Uses XPRESS Huffman compression.
  Buffer compress() const;

  /// Returns a decompressed version of this buffer.
  /// Buffer must contain data compressed with compress().
  Buffer decompress() const;

  /// Encrypts or decrypts the buffer using ChaCha20 stream cipher.
  /// The operation is symmetric: applying twice with the same key and nonce
  /// restores the original data.
  ///
  /// The key must be at least 32 bytes (256 bits).
  /// The nonce must be at least 12 bytes (96 bits).
  /// Returns an empty buffer if key or nonce is too short.
  Buffer crypt(const String &key, const String &nonce) const;
  Buffer crypt(const String &key, const Buffer &nonce) const;
  Buffer crypt(const Buffer &key, const String &nonce) const;
  Buffer crypt(const Buffer &key, const Buffer &nonce) const;

  /// Converts the buffer to a Base64-encoded string.
  String toBase64() const;

  /// Creates a buffer from a Base64-encoded string.
  /// Returns an empty buffer if the input is not valid Base64.
  static Buffer fromBase64(const String &base64String);

  /// Converts the buffer's bytes to a String.
  String toString() const;

  /// Returns a hash code for this buffer.
  int hash() const;

  /// Returns true if this buffer equals other.
  bool operator==(const Buffer &other) const;

  /// Returns true if this buffer does not equal other.
  bool operator!=(const Buffer &other) const;

  /// Returns a new buffer with the string's bytes appended.
  Buffer operator+(const String &str) const;

  /// Returns a new buffer with the other buffer's bytes appended.
  Buffer operator+(const Buffer &other) const;

private:
  BufferImpl *impl;
};

/// Command-line argument parser with support for flags, named parameters, and
/// positional parameters.
class Arguments {
public:
  /// Creates an Arguments parser and captures the command-line arguments.
  Arguments();

  /// Copies another Arguments parser.
  Arguments(const Arguments &other);

  /// Destroys the Arguments parser and releases memory.
  ~Arguments();

  /// Assigns another Arguments parser to this one.
  Arguments &operator=(const Arguments &other);

  /// Adds a flag argument that is set to true when present.
  /// Flags can be specified as -name or --longName.
  /// They can also be explicitly set: -name=true, -name=false, etc.
  /// Returns a reference to this Arguments for chaining.
  Arguments &addFlag(const String &name, const String &description = String(),
                     bool defaultValue = false,
                     const String &longName = String());

  /// Adds a named parameter that requires a value.
  /// Parameters can be specified as -name=value, --longName=value,
  /// -name value, or --longName value.
  /// Returns a reference to this Arguments for chaining.
  Arguments &addParameter(const String &name,
                          const String &description = String(),
                          const String &defaultValue = String(),
                          const String &longName = String());

  /// Adds a positional parameter at the next position.
  /// Positional parameters are filled from non-flag, non-named arguments
  /// in the order they are added.
  /// Returns a reference to this Arguments for chaining.
  Arguments &addPositionalParameter(const String &name,
                                    const String &description = String());

  /// Sets the help text to display when -h or --help is used.
  /// Returns a reference to this Arguments for chaining.
  Arguments &setHelp(const String &helpText);

  /// Marks an argument as required.
  /// If a required argument is missing, parsing fails and help is displayed.
  /// Returns a reference to this Arguments for chaining.
  Arguments &requireArgument(const String &name);

  /// Returns the value of the named argument, or empty string if not set.
  /// Works for flags (returns "true" or "false"), parameters, and positional
  /// parameters.
  String getArgument(const String &name) const;

  /// Returns true if the argument has been added and was set by the user or
  /// has a default value.
  bool hasArgument(const String &name) const;

  /// Parses the command-line arguments and returns a Map of names to values.
  /// If help is requested (-h or --help) or required arguments are missing,
  /// prints help text (unless suppressHelp is true) and returns an empty Map.
  Map parseArguments(bool suppressHelp = false);

private:
  ArgumentsImpl *impl;
};

/// Lightweight thread for running functions in separate threads.
class Thread {
public:
  /// Creates a thread that runs the specified function with an optional
  /// argument. The function receives a void* argument and returns void*.
  /// The thread starts running immediately.
  Thread(void *(*func)(void *), void *arg = nullptr);

  /// Copies another thread (shares the same underlying thread).
  Thread(const Thread &other);

  /// Destroys the thread handle but does not terminate the thread.
  /// Use await() or cancel() to ensure completion before destruction.
  ~Thread();

  /// Assigns another thread to this thread (shares the same underlying thread).
  Thread &operator=(const Thread &other);

  /// Waits for the thread to complete and returns its return value.
  /// Blocks until the thread finishes execution.
  /// Returns nullptr if already awaited or cancelled.
  void *await();

  /// Terminates the thread forcefully.
  /// This is a hard termination and may leave resources in an inconsistent
  /// state. Use with caution - prefer allowing threads to complete naturally.
  void cancel();

  /// Returns true if the thread is currently running.
  /// Returns false if completed, awaited, or cancelled.
  bool isRunning() const;

  /// Returns true if this thread equals the other thread.
  bool equals(const Thread &other) const;

  /// Returns true if this thread equals the other thread.
  bool operator==(const Thread &other) const;

  /// Returns true if this thread does not equal the other thread.
  bool operator!=(const Thread &other) const;

private:
  ThreadImpl *impl;
};

/// Mutex for protecting shared resources across threads.
class Mutex {
public:
  /// Creates a mutex.
  Mutex();

  /// Copies another mutex (shares the same underlying mutex).
  Mutex(const Mutex &other);

  /// Destroys the mutex handle.
  ~Mutex();

  /// Assigns another mutex to this mutex (shares the same underlying mutex).
  Mutex &operator=(const Mutex &other);

  /// Locks the mutex, blocking until it becomes available.
  void lock();

  /// Unlocks the mutex, allowing other threads to acquire it.
  void unlock();

  /// Tries to lock the mutex without blocking.
  /// Returns true if the lock was acquired, false otherwise.
  bool tryLock();

private:
  MutexImpl *impl;
};

/// Immutable path to a file or I/O device on the system.
class Path {
  friend class File;

public:
  /// Creates a path from a string.
  Path(const String &pathStr);

  /// Copies another path.
  Path(const Path &other);

  /// Destroys the path and releases memory.
  ~Path();

  /// Assigns another path to this path.
  Path &operator=(const Path &other);

  /// Returns true if the path exists on the filesystem.
  bool exists() const;

  /// Returns true if the path points to a regular file.
  bool isRegularFile() const;

  /// Returns true if the path points to a directory.
  bool isDirectory() const;

  /// Returns true if the path points to a named pipe.
  bool isNamedPipe() const;

  /// Returns true if the path points to a symbolic link.
  bool isSymbolicLink() const;

  /// Returns true if the path points to another type of device.
  /// This includes physical disks, mailslots, and other special files.
  bool isOther() const;

  /// Returns the size of the file in bytes.
  /// Returns 0 if the path doesn't exist or size is unavailable.
  long long getSize() const;

  /// Returns the creation time of the file or directory.
  DateTime getCreatedOn() const;

  /// Returns the last modification time of the file or directory.
  DateTime getModifiedOn() const;

  /// Returns the last access time of the file or directory.
  DateTime getAccessedOn() const;

  /// Returns true if the file or directory is read-only.
  bool isReadOnly() const;

  /// Sets the read-only attribute of the file or directory.
  /// Returns true if successful.
  bool setReadOnly(bool isReadOnly = true) const;

  /// Returns true if the file or directory is hidden.
  bool isHidden() const;

  /// Sets the hidden attribute of the file or directory.
  /// Returns true if successful.
  bool setHidden(bool isHidden = true) const;

  /// Moves this path to the destination path.
  /// Returns true if successful.
  bool moveTo(const Path &path) const;

  /// Copies this path to the destination path.
  /// Returns true if successful.
  bool copyTo(const Path &path) const;

  /// Deletes the file at this path.
  /// Returns true if successful.
  bool deleteFile() const;

  /// Removes the directory at this path.
  /// If deleteIfNotEmpty is true, recursively deletes directory contents.
  /// Returns true if successful.
  bool removeDirectory(bool deleteIfNotEmpty = false) const;

  /// Creates a directory at this path.
  /// If createParents is true, creates all parent directories as needed.
  /// Returns true if successful.
  bool makeDirectory(bool createParents = true) const;

  /// Returns the target path if this is a symbolic link.
  /// Returns a path to empty string if not a symbolic link.
  Path getSymbolicLinkTarget() const;

  /// Creates or updates this path as a symbolic link to the target.
  /// Returns true if successful.
  bool setSymbolicLinkTarget(const Path &target) const;

  /// Returns the name of the file or directory (last component of path).
  /// Returns empty string if path doesn't exist or has no name component.
  String getName() const;

  /// Returns true if this path equals the other path (case-insensitive).
  bool equals(const Path &other) const;

  /// Returns true if this path equals the other path (case-insensitive).
  bool operator==(const Path &other) const;

  /// Returns true if this path does not equal the other path.
  bool operator!=(const Path &other) const;

  /// Returns true if this path is within the specified directory path.
  bool isWithin(const Path &path) const;

  /// Returns a list of child path strings.
  /// If recursive is true, walks all subdirectories.
  List listChildren(bool recursive = false) const;

  /// Returns the parent directory of this path.
  /// Returns a path to empty string if no parent exists.
  Path getParentDirectory() const;

  /// Returns the file extension (e.g., "txt" for "file.txt").
  /// Returns empty string if no extension.
  String getExtension() const;

  /// Returns true if this path has the specified extension (case-insensitive).
  bool hasExtension(const String &ext) const;

  /// Returns the filename without extension (e.g., "file" for "file.txt").
  String getStem() const;

  /// Reads the entire file contents as a String.
  /// Returns empty string if the file doesn't exist or can't be read.
  String readToString() const;

  /// Reads the entire file contents as a Buffer.
  /// Returns empty buffer if the file doesn't exist or can't be read.
  Buffer readToBuffer() const;

  /// Writes the string to the file, replacing existing contents.
  /// Returns true if successful.
  bool writeFromString(const String &str) const;

  /// Writes the buffer to the file, replacing existing contents.
  /// Returns true if successful.
  bool writeFromBuffer(const Buffer &buf) const;

  /// Appends the string to the end of the file.
  /// Returns true if successful.
  bool appendFromString(const String &str) const;

  /// Appends the buffer to the end of the file.
  /// Returns true if successful.
  bool appendFromBuffer(const Buffer &buf) const;

  /// Changes the current working directory to the specified path.
  /// Returns true if successful.
  static bool ChangeCurrentDirectory(const Path &path);

  /// Returns a list of all volume drive letters (e.g., "C:", "D:").
  static List ListVolumes();

  /// Creates a temporary file with an optional prefix.
  /// Returns the path to the created file.
  static Path CreateTemporaryFile(const String &prefix = String());

  /// Creates a temporary directory with an optional prefix.
  /// Returns the path to the created directory.
  static Path CreateTemporaryDirectory(const String &prefix = String());

private:
  PathImpl *impl;
};

/// Stream-based file and socket I/O class.
/// Supports regular files, named pipes, and TCP sockets.
class File {
public:
  /// Opens a file at the specified path.
  /// The file is opened for reading and writing.
  File(const Path &path);

  /// Opens a TCP socket connection to host at port.
  File(const String &host, int port);

  /// Copies another file (shares the same underlying handle).
  File(const File &other);

  /// Closes the file or socket and releases resources.
  ~File();

  /// Assigns another file to this file (shares the same underlying handle).
  File &operator=(const File &other);

  /// Returns the path of the file, or nullptr if this is a socket.
  const ATTO_CHAR *getPath() const;

  /// Returns the host of the socket, or nullptr if this is not a socket.
  const ATTO_CHAR *getHost() const;

  /// Returns the port of the socket, or -1 if this is not a socket.
  int getPort() const;

  /// Returns true if the file or socket was created/opened successfully.
  bool isValid() const;

  /// Returns true if the file or socket is currently open.
  bool isOpen() const;

  /// Closes the file or socket.
  void close();

  /// Reads all available data into a buffer.
  /// Returns an empty buffer on error or if no data is available.
  Buffer readAllToBuffer();

  /// Reads up to count bytes into a buffer.
  /// Returns an empty buffer on error or if no data is available.
  Buffer readToBuffer(int count);

  /// Reads all available data into a string.
  /// Returns an empty string on error or if no data is available.
  String readAllToString();

  /// Reads up to count bytes into a string.
  /// Returns an empty string on error or if no data is available.
  String readToString(int count);

  /// Returns true if data is available to read.
  bool hasAvailable() const;

  /// Returns the number of bytes available to read.
  /// Returns 0 if no data is available or on error.
  int getAvailableCount() const;

  /// Writes the buffer contents to the file or socket.
  /// Returns the number of bytes actually written, or -1 on error.
  int write(const Buffer &buf);

  /// Writes the string to the file or socket.
  /// Returns the number of bytes actually written, or -1 on error.
  int write(const String &str);

  /// Writes up to count bytes from the buffer.
  /// If count is -1, writes the entire buffer.
  /// Returns the number of bytes actually written, or -1 on error.
  int writeUpTo(const Buffer &buf, int count = -1);

  /// Writes up to count bytes from the string.
  /// If count is -1, writes the entire string.
  /// Returns the number of bytes actually written, or -1 on error.
  int writeUpTo(const String &str, int count = -1);

  /// Flushes any buffered data to the file or socket.
  /// Returns true on success.
  bool flush();

  /// Sets the read/write position for regular files.
  /// Returns true on success, false if not supported or on error.
  bool setPosition(long long pos);

  /// Returns the current read/write position for regular files.
  /// Returns -1 if not supported or on error.
  long long getPosition() const;

  /// Returns the end position (file size) for regular files.
  /// Returns -1 if not supported or on error.
  long long getEndPosition() const;

  /// Returns true if at the end of the file.
  /// Always returns false for sockets and pipes.
  bool isAtEnd() const;

  /// Returns true if this is a regular file.
  bool isRegularFile() const;

  /// Returns true if this is a TCP socket.
  bool isSocket() const;

  /// Returns true if this is a named pipe.
  bool isNamedPipe() const;

  /// Returns true if this file equals the other file.
  bool equals(const File &other) const;

  /// Returns true if this file equals the other file.
  bool operator==(const File &other) const;

  /// Returns true if this file does not equal the other file.
  bool operator!=(const File &other) const;

private:
  FileImpl *impl;
};

/// Mathematical functions and constants.
class Math {
public:
  // Constants
  static constexpr float PI = 3.14159265358979323846f;     ///< π
  static constexpr float E = 2.71828182845904523536f;      ///< Euler's number
  static constexpr float TAU = 6.28318530717958647693f;    ///< τ = 2π
  static constexpr float SQRT_2 = 1.41421356237309504880f; ///< √2
  static const float INF;                                  ///< +∞
  static const float NEG_INF;                              ///< -∞
  static const float NAN;                                  ///< Not-a-number

  // Random number generation

  /// Returns a random integer.
  static int random() noexcept;

  /// Returns a random float between 0.0 (inclusive) and 1.0 (exclusive).
  static float randomFloat() noexcept;

  /// Returns a random integer between start (inclusive) and end (exclusive).
  /// If start >= end, returns start.
  static int randomRange(int start, int end) noexcept;

  /// Returns a random boolean value (true or false).
  static bool randomBool() noexcept;

  /// Returns a random element from a list.
  /// Returns a null-type value if the list is empty.
  template <typename T> static T randomChoice(const List &list) noexcept;

  // Basic arithmetic

  /// Returns the absolute value of x.
  static int abs(int x) noexcept;

  /// Returns the absolute value of x.
  static float abs(float x) noexcept;

  /// Returns the minimum of a and b.
  static int min(int a, int b) noexcept;

  /// Returns the minimum of a and b.
  static float min(float a, float b) noexcept;

  /// Returns the maximum of a and b.
  static int max(int a, int b) noexcept;

  /// Returns the maximum of a and b.
  static float max(float a, float b) noexcept;

  /// Returns x clamped to [minVal, maxVal].
  static int clamp(int x, int minVal, int maxVal) noexcept;

  /// Returns x clamped to [minVal, maxVal].
  static float clamp(float x, float minVal, float maxVal) noexcept;

  /// Returns the sign of x: -1 if negative, 0 if zero, 1 if positive.
  static int sign(int x) noexcept;

  /// Returns the sign of x: -1.0f if negative, 0.0f if zero, 1.0f if positive.
  static float sign(float x) noexcept;

  /// Returns true if x is even.
  static bool isEven(int x) noexcept;

  /// Returns true if x is odd.
  static bool isOdd(int x) noexcept;

  /// Returns true if x is a power of two.
  static bool isPowerOfTwo(int x) noexcept;

  // Rounding

  /// Returns the largest integer less than or equal to x.
  static float floor(float x) noexcept;

  /// Returns the smallest integer greater than or equal to x.
  static float ceil(float x) noexcept;

  /// Returns x rounded toward zero (fractional part removed).
  static float trunc(float x) noexcept;

  /// Returns x rounded to the nearest integer (0.5 rounds up).
  static float round(float x) noexcept;

  // Exponential and logarithmic

  /// Returns 2 raised to the power of x.
  static float exp2(float x) noexcept;

  /// Returns the base-2 logarithm of x.
  static float log2(float x) noexcept;

  /// Returns the natural logarithm (base e) of x.
  static float log(float x) noexcept;

  /// Returns the base-10 logarithm of x.
  static float log10(float x) noexcept;

  /// Returns the natural exponential e^x.
  static float exp(float x) noexcept;

  /// Returns x raised to the power of y.
  static float pow(float x, float y) noexcept;

  /// Returns 2 raised to the power of x (integer version).
  static int pow2(int x) noexcept;

  // Trigonometric

  /// Returns the square root of x. Returns 0 if x is negative.
  static float sqrt(float x) noexcept;

  /// Returns the sine of x (x in radians).
  static float sin(float x) noexcept;

  /// Returns the cosine of x (x in radians).
  static float cos(float x) noexcept;

  /// Returns the tangent of x (x in radians).
  static float tan(float x) noexcept;

  /// Returns the arctangent of y/x in radians, using signs for quadrant.
  static float atan2(float y, float x) noexcept;

  /// Returns the arctangent of x in radians.
  static float atan(float x) noexcept;

  /// Returns the arcsine of x in radians.
  static float asin(float x) noexcept;

  /// Returns the hyperbolic cosine of x.
  static float cosh(float x) noexcept;

  // Utility

  /// Returns the floating-point remainder of x / y.
  static float mod(float x, float y) noexcept;

  /// Returns linear interpolation between a and b by t (0.0 to 1.0).
  static float lerp(float a, float b, float t) noexcept;

  /// Returns 0.0f if x < edge, otherwise 1.0f.
  static float step(float edge, float x) noexcept;

  /// Converts degrees to radians.
  static float degToRad(float degrees) noexcept;

  /// Converts radians to degrees.
  static float radToDeg(float radians) noexcept;

  /// Returns true if x is finite (not infinite or NaN).
  static bool isFinite(float x) noexcept;

  /// Returns true if x is NaN (not a number).
  static bool isNaN(float x) noexcept;
};

// System functions

/// Terminates the process immediately with the specified exit code.
void Exit(int exitCode);

/// Sleeps the current thread for the specified milliseconds.
void Sleep(int milliseconds);

/// Returns the value of an environment variable, or empty string if not set.
String GetEnv(const String &name);

/// Sets an environment variable. Returns true on success.
bool SetEnv(const String &name, const String &value);

/// Allocates a block of memory of the specified size in bytes.
/// Returns a pointer to the memory, or nullptr on failure.
void *Alloc(int size);

/// Reallocates a block of memory to a new size.
/// Returns a pointer to the memory, or nullptr on failure.
/// If ptr is nullptr, behaves like Alloc(). If size is 0, frees the memory.
void *Realloc(void *ptr, int size);

/// Frees a previously allocated block of memory.
/// Does nothing if ptr is nullptr.
void Free(void *ptr);

/// Returns the current user's login name.
String GetUserName();

/// Returns the current user's full display name.
String GetUserDisplayName();

/// Returns the current process ID.
int GetProcessId();

// Logging functions
//
// Logging levels (define one to set the level):
// - ATTOBOY_LOG_DEBUG_ENABLE: Enables Debug, Info, Warning, Error
// - ATTOBOY_LOG_INFO_ENABLE: Enables Info, Warning, Error (default)
// - ATTOBOY_LOG_WARNING_ENABLE: Enables Warning, Error
// - ATTOBOY_LOG_ERROR_ENABLE: Enables Error only
// - ATTOBOY_LOG_DISABLE: Disables all logging

#if !defined(ATTOBOY_LOG_DISABLE)
#if !defined(ATTOBOY_LOG_DEBUG_ENABLE) && !defined(ATTOBOY_LOG_INFO_ENABLE) && \
    !defined(ATTOBOY_LOG_WARNING_ENABLE) && !defined(ATTOBOY_LOG_ERROR_ENABLE)
#define ATTOBOY_LOG_INFO_ENABLE
#endif
#endif

namespace internal {
void LogImpl(const String *args, int count, const String &prefix);
}

/// Prints all arguments to the console followed by a newline.
/// Arguments are converted to String if not already.
template <typename... Args> void Log(const Args &...args) {
#if !defined(ATTOBOY_LOG_DISABLE)
  String strings[] = {String(args)...};
  internal::LogImpl(strings, sizeof...(Args), String());
#endif
}

/// Prints debug message with timestamp prefix: "DEBUG HH:MM:SS: ".
/// Only enabled when ATTOBOY_LOG_DEBUG_ENABLE is defined.
template <typename... Args> void LogDebug(const Args &...args) {
#if defined(ATTOBOY_LOG_DEBUG_ENABLE)
  String strings[] = {String(args)...};
  internal::LogImpl(strings, sizeof...(Args), String("DEBUG "));
#endif
}

/// Prints info message with timestamp prefix: "INFO HH:MM:SS: ".
/// Enabled when ATTOBOY_LOG_DEBUG_ENABLE or ATTOBOY_LOG_INFO_ENABLE is
/// defined.
template <typename... Args> void LogInfo(const Args &...args) {
#if defined(ATTOBOY_LOG_DEBUG_ENABLE) || defined(ATTOBOY_LOG_INFO_ENABLE)
  String strings[] = {String(args)...};
  internal::LogImpl(strings, sizeof...(Args), String("INFO "));
#endif
}

/// Prints warning message with timestamp prefix: "WARNING HH:MM:SS: ".
/// Enabled when ATTOBOY_LOG_DEBUG_ENABLE, ATTOBOY_LOG_INFO_ENABLE, or
/// ATTOBOY_LOG_WARNING_ENABLE is defined.
template <typename... Args> void LogWarning(const Args &...args) {
#if defined(ATTOBOY_LOG_DEBUG_ENABLE) || defined(ATTOBOY_LOG_INFO_ENABLE) ||   \
    defined(ATTOBOY_LOG_WARNING_ENABLE)
  String strings[] = {String(args)...};
  internal::LogImpl(strings, sizeof...(Args), String("WARNING "));
#endif
}

/// Prints error message with timestamp prefix: "ERROR HH:MM:SS: ".
/// Enabled unless ATTOBOY_LOG_DISABLE is defined.
template <typename... Args> void LogError(const Args &...args) {
#if !defined(ATTOBOY_LOG_DISABLE)
  String strings[] = {String(args)...};
  internal::LogImpl(strings, sizeof...(Args), String("ERROR "));
#endif
}

// Template implementation for Math::randomChoice
template <typename T> inline T Math::randomChoice(const List &list) noexcept {
  if (list.isEmpty()) {
    return T();
  }
  int index = randomRange(0, list.length());
  return list.at<T>(index);
}

} // namespace attoboy

#endif // ATTOBOY_H
