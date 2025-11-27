#ifndef ATTOBOY_H
#define ATTOBOY_H

//==============================================================================
// libattoboy - Batteries-Included C++ for Tiny Windows Executables
//==============================================================================
//
// A lightweight C++17 library for building extremely small 32-bit Windows
// utilities. Designed for simplicity and minimal binary size, libattoboy
// provides high-level abstractions for strings, collections, files, networking,
// threading, AI, and TUI—all without the STL, CRT, or libc. Uses UTF-8
// encoding, RAII patterns, and thread-safe designs throughout.
//
// Quick Start:
//   - Define atto_main() as your entry point (replaces main())
//   - Use String, List, Map, Set for data handling
//   - Use Path/File for filesystem and network I/O
//   - Use WebRequest for HTTP, AI for LLM integration
//
// Build Notes:
//   - No exceptions, RTTI, or virtual functions
//   - Links only against Windows API
//   - Compatible with MSVC and MinGW
//
//==============================================================================

/// Application entry point. Define this instead of main().
extern "C" void atto_main();

namespace attoboy {

//------------------------------------------------------------------------------
// Forward Declarations
//------------------------------------------------------------------------------

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
class SubprocessImpl;
class RegistryImpl;
class WebRequestImpl;
class WebResponseImpl;
class AIImpl;
class EmbeddingImpl;
class ConversationImpl;
class ConsoleImpl;

class List;
class Map;
class Set;
class WebResponse;
class AI;
class Embedding;
class Conversation;

//------------------------------------------------------------------------------
// Core Types
//------------------------------------------------------------------------------

/// Type tags for values in containers (List, Map, Set).
enum ValueType {
  /// Null or uninitialized value.
  TYPE_NULL = 0,
  /// Invalid or error state.
  TYPE_INVALID,
  /// Boolean value.
  TYPE_BOOL,
  /// 32-bit integer value.
  TYPE_INT,
  /// Single-precision floating-point value.
  TYPE_FLOAT,
  /// String value (attoboy::String).
  TYPE_STRING,
  /// List value (attoboy::List).
  TYPE_LIST,
  /// Map value (attoboy::Map).
  TYPE_MAP,
  /// Set value (attoboy::Set).
  TYPE_SET,
  /// Unrecognized type.
  TYPE_UNKNOWN
};

/// Immutable UTF-8 string with rich text manipulation.
/// Supports automatic conversion from bool, int, float, and collections.
/// All character-based operations respect UTF-8 codepoint boundaries.
class String {
public:
  /// Creates an empty string.
  String();
  /// Creates a string from a null-terminated C string.
  String(const char *str);
  /// Creates a string from a boolean ("true" or "false").
  String(bool value);
  /// Creates a string from an integer.
  String(int value);
  /// Creates a string from a 64-bit integer.
  String(long long value);
  /// Creates a string from a float.
  String(float value);
  /// Creates a JSON array string from a list (e.g., [1,"a",true]).
  String(const List &list);
  /// Creates a JSON object string from a map (e.g., {"k":"v"}).
  String(const Map &map);
  /// Creates a JSON array string from a set.
  String(const Set &set);

  /// Concatenates multiple values into one string.
  template <typename T, typename U, typename... Args>
  String(const T &first, const U &second, const Args &...rest)
      : String(String(first) + String(second) + String(rest...)) {}

  /// Creates a copy of another string.
  String(const String &other);
  /// Destroys the string and frees memory.
  ~String();

  /// Creates a string from raw bytes with specified length (does not require
  /// null termination).
  static String FromCStr(const char *data, int size);

  /// Assigns another string to this string.
  String &operator=(const String &other);

  /// Returns the number of UTF-8 characters in the string.
  int length() const;
  /// Returns the number of bytes in the string.
  int byteLength() const;
  /// Returns true if the string is empty.
  bool isEmpty() const;
  /// Returns a pointer to the null-terminated C string.
  const char *c_str() const;
  /// Returns the character at index as a string. Negative indices count from
  /// end.
  String at(int index) const;
  /// Returns a single byte at index as a string. Negative indices count from
  /// end.
  String byteAt(int index) const;

  /// Returns a substring from start to end (exclusive). Negative indices count
  /// from end.
  String substring(int start, int end = -1) const;
  /// Returns a byte-based substring from start to end (exclusive).
  String byteSubstring(int start, int end = -1) const;
  /// Returns a copy of this string.
  String duplicate() const;

  /// Returns true if this string contains the substring.
  bool contains(const String &sub) const;
  /// Returns true if this string starts with the substring.
  bool startsWith(const String &sub) const;
  /// Returns true if this string ends with the substring.
  bool endsWith(const String &sub) const;
  /// Returns the number of non-overlapping occurrences of substring.
  int count(const String &sub) const;
  /// Returns the character index of substring, or -1 if not found.
  int getPositionOf(const String &sub, int start = 0) const;

  /// Returns true if the string is a valid integer or decimal number.
  bool isNumber() const;
  /// Converts the string to an integer. Returns 0 if invalid.
  int toInteger() const;
  /// Converts the string to a float. Returns 0.0 if invalid.
  float toFloat() const;
  /// Returns true for "true", "yes", "1", "on" (case-insensitive).
  bool toBool() const;

  /// Returns a new string with the substring appended.
  String append(const String &s) const;
  /// Returns a new string with the substring prepended.
  String prepend(const String &s) const;
  /// Returns a new string with the substring inserted at index.
  String insert(int index, const String &s) const;
  /// Returns a new string with characters from start to end removed.
  String remove(int start, int end) const;
  /// Returns a new string with all occurrences of target replaced.
  String replace(const String &target, const String &replacement) const;
  /// Returns a new string with leading and trailing whitespace removed.
  String trim() const;
  /// Returns a new string with all characters uppercase.
  String upper() const;
  /// Returns a new string with all characters lowercase.
  String lower() const;
  /// Returns a new string with characters in reverse order.
  String reverse() const;
  /// Returns a new string with content repeated count times.
  String repeat(int count) const;

  /// Returns true if this string equals the other string.
  bool equals(const String &other) const;
  /// Compares lexicographically. Returns <0, 0, or >0.
  int compare(const String &other) const;
  /// Returns true if this string equals the other string.
  bool operator==(const String &other) const;
  /// Returns true if this string does not equal the other string.
  bool operator!=(const String &other) const;
  /// Returns the concatenation of this string and other.
  String operator+(const String &other) const;
  /// Returns a hash code for this string.
  int hash() const;

  /// Splits this string by newlines into a list.
  List lines() const;
  /// Joins list elements using this string as separator.
  String join(const List &list) const;
  /// Splits by separator into a list. max controls maximum splits.
  List split(const String &sep, int max = 1) const;
  /// Splits by whitespace into a list.
  List split() const;

  /// Replaces {0}, {1}, etc. with list elements.
  String format(const List &list) const;
  /// Replaces {key} placeholders with map values.
  String format(const Map &map) const;

private:
  StringImpl *impl;
};

/// String concatenation operator for natural syntax: "Hello " + myString
inline String operator+(const char *lhs, const String &rhs) {
  return String(lhs) + rhs;
}

/// Dynamic array storing mixed types (bool, int, float, String, List, Map,
/// Set). Elements are accessed by index; negative indices count from end.
class List {
public:
  /// Creates an empty list.
  List();
  /// Creates an empty list with reserved capacity.
  List(int capacity);

  /// Creates a list with the given values: List(1, "two", 3.0f)
  template <typename... Args> List(const Args &...args) : List() {
    variadic_append(args...);
  }

  /// Creates a copy of another list.
  List(const List &other);
  /// Creates a list from a set.
  List(const Set &set);
  /// Destroys the list and frees memory.
  ~List();
  /// Assigns another list to this list.
  List &operator=(const List &other);

  /// Returns the number of elements in the list.
  int length() const;
  /// Returns true if the list is empty.
  bool isEmpty() const;

  /// Appends a value to the end. Returns this list for chaining.
  template <typename T> List &append(T value) {
    append_impl(value);
    return *this;
  }
  /// Prepends a value to the beginning. Returns this list for chaining.
  template <typename T> List &prepend(T value) {
    prepend_impl(value);
    return *this;
  }
  /// Inserts a value at index. Returns this list for chaining.
  template <typename T> List &insert(int index, T value) {
    insert_impl(index, value);
    return *this;
  }
  /// Sets the value at index. Returns this list for chaining.
  template <typename T> List &set(int index, T value) {
    set_impl(index, value);
    return *this;
  }
  /// Removes the element at index. Returns this list for chaining.
  List &remove(int index);
  /// Removes all elements. Returns this list for chaining.
  List &clear();
  /// Reverses element order in place. Returns this list for chaining.
  List &reverse();
  /// Sorts elements. Returns this list for chaining.
  List &sort(bool ascending = true);
  /// Appends all elements from another list. Returns this list for chaining.
  List &concat(const List &other);
  /// Appends all values from a set. Returns this list for chaining.
  List &concat(const Set &set);

  /// Removes and returns the element at index.
  template <typename T> T pop();
  /// Returns the element at index.
  template <typename T> T at(int index) const;
  /// Returns the element at index.
  template <typename T> T operator[](int index) const;
  /// Returns the type of the element at index.
  ValueType typeAt(int index) const;
  /// Returns a new list with elements from start to end.
  List slice(int start, int end) const;
  /// Returns a copy of this list.
  List duplicate() const;

  /// Returns the index of the first occurrence of value, or -1.
  template <typename T> int find(T value) const;
  /// Returns true if the list contains the value.
  template <typename T> bool contains(T value) const;

  /// Converts a list of lists to a CSV string.
  String toCSVString() const;
  /// Creates a list of lists from a CSV string.
  static List FromCSVString(const String &csv);
  /// Converts this list to a JSON array string.
  String toJSONString() const;
  /// Creates a list from a JSON array string.
  static List FromJSONString(const String &json);

  /// Returns true if this list equals the other list.
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
  void append_impl(bool value);
  void append_impl(int value);
  void append_impl(float value);
  void append_impl(const char *value);
  void append_impl(const String &value);
  void append_impl(const List &value);
  void append_impl(const Map &value);
  void append_impl(const Set &value);
  void prepend_impl(bool value);
  void prepend_impl(int value);
  void prepend_impl(float value);
  void prepend_impl(const char *value);
  void prepend_impl(const String &value);
  void prepend_impl(const List &value);
  void prepend_impl(const Map &value);
  void prepend_impl(const Set &value);
  void insert_impl(int index, bool value);
  void insert_impl(int index, int value);
  void insert_impl(int index, float value);
  void insert_impl(int index, const char *value);
  void insert_impl(int index, const String &value);
  void insert_impl(int index, const List &value);
  void insert_impl(int index, const Map &value);
  void insert_impl(int index, const Set &value);
  void set_impl(int index, bool value);
  void set_impl(int index, int value);
  void set_impl(int index, float value);
  void set_impl(int index, const char *value);
  void set_impl(int index, const String &value);
  void set_impl(int index, const List &value);
  void set_impl(int index, const Map &value);
  void set_impl(int index, const Set &value);
  template <typename T, typename... Args>
  void variadic_append(const T &first, const Args &...rest) {
    append(first);
    variadic_append(rest...);
  }
  void variadic_append() {}
};

/// Key-value map with mixed types for both keys and values.
/// Keys must be unique. Order is not guaranteed.
class Map {
public:
  /// Creates an empty map.
  Map();
  /// Creates an empty map with reserved capacity.
  Map(int capacity);

  /// Creates a map with key-value pairs: Map("name", "Alice", "age", 30)
  template <typename... Args> Map(const Args &...args) : Map() {
    variadic_put_pairs(args...);
  }

  /// Creates a copy of another map.
  Map(const Map &other);
  /// Destroys the map and frees memory.
  ~Map();
  /// Assigns another map to this map.
  Map &operator=(const Map &other);

  /// Returns the number of key-value pairs.
  int length() const;
  /// Returns true if the map is empty.
  bool isEmpty() const;

  /// Returns the value for key, or defaultValue if not found.
  template <typename K, typename V> V get(K key, V defaultValue = V()) const;
  /// Returns the value for key, or null-type if not found.
  template <typename K, typename V> V operator[](K key) const;
  /// Returns true if the map contains the key.
  template <typename K> bool hasKey(K key) const;
  /// Returns the type of the value for key, or TYPE_INVALID if not found.
  template <typename K> ValueType typeAt(K key) const;
  /// Returns the first key with the given value, or null-type if not found.
  template <typename K, typename V> K findValue(V value) const;
  /// Returns a list of all keys.
  List keys() const;
  /// Returns a list of all values.
  List values() const;

  /// Sets key to value. Returns this map for chaining.
  template <typename K, typename V> Map &put(K key, V value);
  /// Removes the key-value pair. Returns this map for chaining.
  template <typename K> Map &remove(K key);
  /// Removes all key-value pairs. Returns this map for chaining.
  Map &clear();
  /// Merges another map into this one. Returns this map for chaining.
  Map &merge(const Map &other);
  /// Returns a copy of this map.
  Map duplicate() const;

  /// Converts this map to a JSON object string.
  String toJSONString() const;
  /// Creates a map from a JSON object string.
  static Map FromJSONString(const String &json);

  /// Returns true if this map equals the other map.
  bool compare(const Map &other) const;
  /// Returns true if this map equals the other map.
  bool operator==(const Map &other) const;
  /// Returns true if this map does not equal the other map.
  bool operator!=(const Map &other) const;

private:
  MapImpl *impl;
  void put_impl(bool key, bool value);
  void put_impl(bool key, int value);
  void put_impl(bool key, float value);
  void put_impl(bool key, const char *value);
  void put_impl(bool key, const String &value);
  void put_impl(bool key, const List &value);
  void put_impl(bool key, const Map &value);
  void put_impl(bool key, const Set &value);
  void put_impl(int key, bool value);
  void put_impl(int key, int value);
  void put_impl(int key, float value);
  void put_impl(int key, const char *value);
  void put_impl(int key, const String &value);
  void put_impl(int key, const List &value);
  void put_impl(int key, const Map &value);
  void put_impl(int key, const Set &value);
  void put_impl(float key, bool value);
  void put_impl(float key, int value);
  void put_impl(float key, float value);
  void put_impl(float key, const char *value);
  void put_impl(float key, const String &value);
  void put_impl(float key, const List &value);
  void put_impl(float key, const Map &value);
  void put_impl(float key, const Set &value);
  void put_impl(const char *key, bool value);
  void put_impl(const char *key, int value);
  void put_impl(const char *key, float value);
  void put_impl(const char *key, const char *value);
  void put_impl(const char *key, const String &value);
  void put_impl(const char *key, const List &value);
  void put_impl(const char *key, const Map &value);
  void put_impl(const char *key, const Set &value);
  void put_impl(const String &key, bool value);
  void put_impl(const String &key, int value);
  void put_impl(const String &key, float value);
  void put_impl(const String &key, const char *value);
  void put_impl(const String &key, const String &value);
  void put_impl(const String &key, const List &value);
  void put_impl(const String &key, const Map &value);
  void put_impl(const String &key, const Set &value);
  void remove_impl(bool key);
  void remove_impl(int key);
  void remove_impl(float key);
  void remove_impl(const char *key);
  void remove_impl(const String &key);
  template <typename K, typename V, typename... Args>
  void variadic_put_pairs(const K &key, const V &value, const Args &...rest) {
    put(key, value);
    variadic_put_pairs(rest...);
  }
  template <typename K> void variadic_put_pairs(const K &key) {
    put_impl(key, false);
  }
  void variadic_put_pairs() {}
};

template <typename K, typename V> inline Map &Map::put(K key, V value) {
  put_impl(key, value);
  return *this;
}
template <typename K> inline Map &Map::remove(K key) {
  remove_impl(key);
  return *this;
}

/// Unordered collection of unique mixed-type values.
/// Duplicates are silently ignored. Order is not guaranteed.
class Set {
public:
  /// Creates an empty set.
  Set();
  /// Creates an empty set with reserved capacity.
  Set(int capacity);

  /// Creates a set with the given values: Set(1, 2, 3)
  template <typename... Args> Set(const Args &...args) : Set() {
    variadic_put(args...);
  }

  /// Creates a copy of another set.
  Set(const Set &other);
  /// Creates a set from a list, removing duplicates.
  Set(const List &list);
  /// Destroys the set and frees memory.
  ~Set();
  /// Assigns another set to this set.
  Set &operator=(const Set &other);

  /// Returns the number of values in the set.
  int length() const;
  /// Returns true if the set is empty.
  bool isEmpty() const;

  /// Adds a value to the set. Duplicates are ignored. Returns this set for
  /// chaining.
  template <typename T> Set &put(T value) {
    put_impl(value);
    return *this;
  }
  /// Removes a value from the set. Returns this set for chaining.
  template <typename T> Set &remove(T value) {
    remove_impl(value);
    return *this;
  }
  /// Removes all values. Returns this set for chaining.
  Set &clear();

  /// Adds all values from another set (union). Returns this set for chaining.
  Set &setUnion(const Set &other);
  /// Keeps only values in both sets (intersection). Returns this set for
  /// chaining.
  Set &intersect(const Set &other);
  /// Removes all values in other (difference). Returns this set for chaining.
  Set &subtract(const Set &other);

  /// Returns true if the set contains the value.
  template <typename T> bool contains(T value) const;
  /// Returns a copy of this set.
  Set duplicate() const;
  /// Returns a list of all values in the set.
  List toList() const;

  /// Converts this set to a JSON array string.
  String toJSONString() const;
  /// Creates a set from a JSON array string.
  static Set FromJSONString(const String &json);

  /// Returns true if this set equals the other set.
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
  void put_impl(bool value);
  void put_impl(int value);
  void put_impl(float value);
  void put_impl(const char *value);
  void put_impl(const String &value);
  void put_impl(const List &value);
  void put_impl(const Map &value);
  void put_impl(const Set &value);
  void remove_impl(bool value);
  void remove_impl(int value);
  void remove_impl(float value);
  void remove_impl(const char *value);
  void remove_impl(const String &value);
  template <typename T, typename... Args>
  void variadic_put(const T &first, const Args &...rest) {
    put(first);
    variadic_put(rest...);
  }
  void variadic_put() {}
};

//------------------------------------------------------------------------------
// Utility Types
//------------------------------------------------------------------------------

/// Date and time with millisecond precision (UTC).
class DateTime {
public:
  /// Creates a DateTime representing the current time.
  DateTime();
  /// Creates a DateTime from milliseconds since Unix epoch (Jan 1, 1970).
  DateTime(long long millisSinceEpoch);
  /// Creates a DateTime from an ISO-8601 string (YYYY-MM-DDTHH:MM:SS.fffZ).
  DateTime(const String &iso8601);
  /// Creates a copy of another DateTime.
  DateTime(const DateTime &other);
  /// Destroys the DateTime and frees memory.
  ~DateTime();
  /// Assigns another DateTime to this DateTime.
  DateTime &operator=(const DateTime &other);

  /// Adds milliseconds (negative to subtract). Returns this DateTime for
  /// chaining.
  DateTime &add(long long milliseconds);
  /// Returns the difference in milliseconds (positive if this is later).
  long long diff(const DateTime &other) const;
  /// Compares with another DateTime. Returns <0, 0, or >0.
  int compare(const DateTime &other) const;
  /// Returns true if this DateTime equals the other.
  bool equals(const DateTime &other) const;
  /// Returns true if this DateTime equals the other.
  bool operator==(const DateTime &other) const;
  /// Returns true if this DateTime does not equal the other.
  bool operator!=(const DateTime &other) const;

  /// Returns milliseconds since Unix epoch (Jan 1, 1970).
  long long timestamp() const;
  /// Returns an ISO-8601 formatted string.
  String toString() const;

private:
  DateTimeImpl *impl;
};

/// Mutable byte buffer for binary data.
/// Supports compression (LZ4) and encryption (ChaCha20).
class Buffer {
public:
  /// Creates an empty buffer.
  Buffer();
  /// Creates an empty buffer with reserved capacity.
  Buffer(int capacity);
  /// Creates a buffer by copying a string's bytes.
  Buffer(const String &str);
  /// Creates a buffer by copying bytes from a pointer.
  Buffer(const unsigned char *ptr, int size);
  /// Creates a copy of another buffer.
  Buffer(const Buffer &other);
  /// Destroys the buffer and frees memory.
  ~Buffer();
  /// Assigns another buffer to this buffer.
  Buffer &operator=(const Buffer &other);

  /// Returns the number of bytes in the buffer.
  int length() const;
  /// Returns true if the buffer is empty.
  bool isEmpty() const;
  /// Returns a pointer to the data and sets len to the size.
  const unsigned char *c_ptr(int *len) const;
  /// Removes all bytes. Returns this buffer for chaining.
  Buffer &clear();

  /// Appends a string's bytes. Returns this buffer for chaining.
  Buffer &append(const String &str);
  /// Appends another buffer's bytes. Returns this buffer for chaining.
  Buffer &append(const Buffer &other);
  /// Appends bytes from a pointer. Returns this buffer for chaining.
  Buffer &append(const unsigned char *ptr, int size);
  /// Prepends a string's bytes. Returns this buffer for chaining.
  Buffer &prepend(const String &str);
  /// Prepends another buffer's bytes. Returns this buffer for chaining.
  Buffer &prepend(const Buffer &other);
  /// Prepends bytes from a pointer. Returns this buffer for chaining.
  Buffer &prepend(const unsigned char *ptr, int size);
  /// Inserts a string's bytes at index. Returns this buffer for chaining.
  Buffer &insert(int index, const String &str);
  /// Inserts another buffer's bytes at index. Returns this buffer for chaining.
  Buffer &insert(int index, const Buffer &other);
  /// Inserts bytes from a pointer at index. Returns this buffer for chaining.
  Buffer &insert(int index, const unsigned char *ptr, int size);
  /// Removes bytes from start to end. Returns this buffer for chaining.
  Buffer &remove(int start, int end = -1);
  /// Reverses byte order in place. Returns this buffer for chaining.
  Buffer &reverse();
  /// Shrinks capacity to match length. Returns this buffer for chaining.
  Buffer &trim();

  /// Returns a new buffer with bytes from start to end.
  Buffer slice(int start, int end = -1) const;

  /// Returns an LZ4-compressed version of this buffer.
  Buffer compress() const;
  /// Returns a decompressed version of this buffer.
  Buffer decompress() const;

  /// Encrypts/decrypts using ChaCha20 (symmetric). Key ≥32 bytes, nonce ≥12
  /// bytes.
  Buffer crypt(const String &key, const String &nonce) const;
  /// Encrypts/decrypts using ChaCha20 (symmetric). Key ≥32 bytes, nonce ≥12
  /// bytes.
  Buffer crypt(const String &key, const Buffer &nonce) const;
  /// Encrypts/decrypts using ChaCha20 (symmetric). Key ≥32 bytes, nonce ≥12
  /// bytes.
  Buffer crypt(const Buffer &key, const String &nonce) const;
  /// Encrypts/decrypts using ChaCha20 (symmetric). Key ≥32 bytes, nonce ≥12
  /// bytes.
  Buffer crypt(const Buffer &key, const Buffer &nonce) const;

  /// Converts the buffer to a Base64-encoded string.
  String toBase64() const;
  /// Creates a buffer from a Base64-encoded string.
  static Buffer fromBase64(const String &base64);
  /// Converts the buffer's bytes to a string.
  String toString() const;

  /// Returns true if this buffer equals the other.
  bool compare(const Buffer &other) const;
  /// Returns a hash code for this buffer.
  int hash() const;
  /// Returns true if this buffer equals the other.
  bool operator==(const Buffer &other) const;
  /// Returns true if this buffer does not equal the other.
  bool operator!=(const Buffer &other) const;
  /// Returns a new buffer with the string's bytes appended.
  Buffer operator+(const String &str) const;
  /// Returns a new buffer with the other buffer's bytes appended.
  Buffer operator+(const Buffer &other) const;

private:
  BufferImpl *impl;
};

//------------------------------------------------------------------------------
// Command-Line Parsing
//------------------------------------------------------------------------------

/// Command-line argument parser with flags, parameters, and positionals.
/// Automatically handles -h/--help.
class Arguments {
public:
  /// Creates an Arguments parser and captures command-line arguments.
  Arguments();
  /// Creates a copy of another Arguments parser.
  Arguments(const Arguments &other);
  /// Destroys the Arguments parser and frees memory.
  ~Arguments();
  /// Assigns another Arguments parser to this one.
  Arguments &operator=(const Arguments &other);

  /// Adds a boolean flag (-name, --longName, or -name=true/false).
  Arguments &addFlag(const String &name, const String &description = String(),
                     bool defaultValue = false,
                     const String &longName = String());
  /// Adds a named parameter (-name=value or -name value).
  Arguments &addParameter(const String &name,
                          const String &description = String(),
                          const String &defaultValue = String(),
                          const String &longName = String());
  /// Adds a positional parameter (filled in order from remaining args).
  Arguments &addPositionalParameter(const String &name,
                                    const String &description = String());
  /// Sets custom help text for -h/--help.
  Arguments &setHelp(const String &helpText);
  /// Marks an argument as required.
  Arguments &requireArgument(const String &name);

  /// Returns the value of the argument (flags return "true" or "false").
  String getArgument(const String &name) const;
  /// Returns true if the argument was set or has a default value.
  bool hasArgument(const String &name) const;

  /// Parses arguments and returns a Map. Empty if help shown or required
  /// missing.
  Map parseArguments(bool suppressHelp = false);

private:
  ArgumentsImpl *impl;
};

//------------------------------------------------------------------------------
// Filesystem
//------------------------------------------------------------------------------

/// Immutable filesystem path with metadata and convenience operations.
class Path {
  friend class File;
  friend SubprocessImpl *CreateSubprocessImpl(const Path &, const List &, bool,
                                              bool);

public:
  /// Creates a path from a string.
  Path(const String &pathStr);
  /// Creates a copy of another path.
  Path(const Path &other);
  /// Destroys the path and frees memory.
  ~Path();
  /// Assigns another path to this path.
  Path &operator=(const Path &other);

  /// Returns true if the path exists on the filesystem.
  bool exists() const;
  /// Returns true if the path is a regular file.
  bool isRegularFile() const;
  /// Returns true if the path is a directory.
  bool isDirectory() const;
  /// Returns true if the path is a named pipe.
  bool isNamedPipe() const;
  /// Returns true if the path is a symbolic link.
  bool isSymbolicLink() const;
  /// Returns true if the path is a device or other special file.
  bool isOther() const;

  /// Returns the file size in bytes (0 if unavailable).
  long long getSize() const;
  /// Returns the creation time.
  DateTime getCreatedOn() const;
  /// Returns the last modification time.
  DateTime getModifiedOn() const;
  /// Returns the last access time.
  DateTime getAccessedOn() const;
  /// Returns true if the file is read-only.
  bool isReadOnly() const;
  /// Sets the read-only attribute. Returns true on success.
  bool setReadOnly(bool isReadOnly = true) const;
  /// Returns true if the file is hidden.
  bool isHidden() const;
  /// Sets the hidden attribute. Returns true on success.
  bool setHidden(bool isHidden = true) const;

  /// Moves this path to the destination. Returns true on success.
  bool moveTo(const Path &dest) const;
  /// Copies this path to the destination. Returns true on success.
  bool copyTo(const Path &dest) const;
  /// Deletes the file. Returns true on success.
  bool deleteFile() const;
  /// Removes the directory. Returns true on success.
  bool removeDirectory(bool deleteIfNotEmpty = false) const;
  /// Creates the directory. Returns true on success.
  bool makeDirectory(bool createParents = true) const;

  /// Returns the symbolic link target, or empty path if not a link.
  Path getSymbolicLinkTarget() const;
  /// Creates or updates a symbolic link to the target. Returns true on success.
  bool setSymbolicLinkTarget(const Path &target) const;

  /// Returns the filename (last path component).
  String getName() const;
  /// Returns the parent directory path.
  Path getParentDirectory() const;
  /// Returns the file extension without dot (e.g., "txt").
  String getExtension() const;
  /// Returns the filename without extension.
  String getStem() const;
  /// Returns true if the file has the given extension (case-insensitive).
  bool hasExtension(const String &ext) const;
  /// Returns the path as a string.
  String toString() const;

  /// Returns true if this path equals the other (case-insensitive).
  bool equals(const Path &other) const;
  /// Returns true if this path equals the other (case-insensitive).
  bool operator==(const Path &other) const;
  /// Returns true if this path does not equal the other.
  bool operator!=(const Path &other) const;
  /// Returns true if this path is within the given directory.
  bool isWithin(const Path &dir) const;

  /// Returns a list of child path strings.
  List listChildren(bool recursive = false) const;

  /// Reads the entire file as a string.
  String readToString() const;
  /// Reads the entire file as a buffer.
  Buffer readToBuffer() const;
  /// Writes a string to the file. Returns true on success.
  bool writeFromString(const String &str) const;
  /// Writes a buffer to the file. Returns true on success.
  bool writeFromBuffer(const Buffer &buf) const;
  /// Appends a string to the file. Returns true on success.
  bool appendFromString(const String &str) const;
  /// Appends a buffer to the file. Returns true on success.
  bool appendFromBuffer(const Buffer &buf) const;

  /// Changes the current working directory. Returns true on success.
  static bool ChangeCurrentDirectory(const Path &path);
  /// Returns a list of volume drive letters (e.g., "C:", "D:").
  static List ListVolumes();
  /// Creates a temporary file and returns its path.
  static Path CreateTemporaryFile(const String &prefix = String());
  /// Creates a temporary directory and returns its path.
  static Path CreateTemporaryDirectory(const String &prefix = String());
  /// Returns the user's home directory path.
  static Path GetHomeDirectory();
  /// Returns the user's documents directory path.
  static Path GetDocumentsDirectory();
  /// Returns the roaming application data directory path.
  static Path GetRoamingAppDirectory();
  /// Returns the local application data directory path.
  static Path GetLocalAppDirectory();

private:
  PathImpl *impl;
};

/// Stream-based I/O for files, named pipes, and TCP sockets.
class File {
public:
  /// Opens a file at the given path for reading and writing.
  File(const Path &path);
  /// Opens a TCP socket connection to host:port.
  File(const String &host, int port);
  /// Creates a listening server socket on the given port.
  File(int port);
  /// Creates a copy (shares the underlying handle).
  File(const File &other);
  /// Closes the file/socket and frees resources.
  ~File();
  /// Assigns another file (shares the underlying handle).
  File &operator=(const File &other);

  /// Returns the file path, or nullptr for sockets.
  const char *getPath() const;
  /// Returns the socket host, or nullptr for files.
  const char *getHost() const;
  /// Returns the socket port, or -1 for files.
  int getPort() const;

  /// Returns true if the file/socket was opened successfully.
  bool isValid() const;
  /// Returns true if the file/socket is currently open.
  bool isOpen() const;
  /// Closes the file/socket.
  void close();

  /// Reads all available data into a buffer.
  Buffer readAllToBuffer();
  /// Reads up to count bytes into a buffer.
  Buffer readToBuffer(int count);
  /// Reads all available data into a string.
  String readAllToString();
  /// Reads up to count bytes into a string.
  String readToString(int count);
  /// Returns true if data is available to read.
  bool hasAvailable() const;
  /// Returns the number of bytes available to read.
  int getAvailableCount() const;

  /// Writes a buffer. Returns bytes written, or -1 on error.
  int write(const Buffer &buf);
  /// Writes a string. Returns bytes written, or -1 on error.
  int write(const String &str);
  /// Writes up to count bytes from a buffer. Returns bytes written, or -1 on
  /// error.
  int writeUpTo(const Buffer &buf, int count = -1);
  /// Writes up to count bytes from a string. Returns bytes written, or -1 on
  /// error.
  int writeUpTo(const String &str, int count = -1);
  /// Flushes buffered data. Returns true on success.
  bool flush();

  /// Sets the read/write position (files only). Returns true on success.
  bool setPosition(long long pos);
  /// Returns the current position, or -1 if not supported.
  long long getPosition() const;
  /// Returns the file size, or -1 if not supported.
  long long getEndPosition() const;
  /// Returns true if at end of file.
  bool isAtEnd() const;

  /// Returns true if this is a regular file.
  bool isRegularFile() const;
  /// Returns true if this is a TCP socket.
  bool isSocket() const;
  /// Returns true if this is a server socket.
  bool isServerSocket() const;
  /// Returns true if this is a named pipe.
  bool isNamedPipe() const;

  /// Accepts a client connection on a server socket.
  File accept();

  /// Returns true if this file equals the other.
  bool equals(const File &other) const;
  /// Returns true if this file equals the other.
  bool operator==(const File &other) const;
  /// Returns true if this file does not equal the other.
  bool operator!=(const File &other) const;

private:
  FileImpl *impl;
};

//------------------------------------------------------------------------------
// Concurrency
//------------------------------------------------------------------------------

/// Lightweight thread.
class Thread {
public:
  /// Creates and starts a thread running the given function.
  Thread(void *(*func)(void *), void *arg = nullptr);
  /// Creates a copy (shares the underlying thread).
  Thread(const Thread &other);
  /// Destroys the handle (does not terminate the thread).
  ~Thread();
  /// Assigns another thread (shares the underlying thread).
  Thread &operator=(const Thread &other);

  /// Waits for the thread to complete and returns its result.
  void *await();
  /// Forcefully terminates the thread (use sparingly).
  void cancel();
  /// Returns true if the thread is still running.
  bool isRunning() const;

  /// Returns true if this thread equals the other.
  bool equals(const Thread &other) const;
  /// Returns true if this thread equals the other.
  bool operator==(const Thread &other) const;
  /// Returns true if this thread does not equal the other.
  bool operator!=(const Thread &other) const;

private:
  ThreadImpl *impl;
};

/// Mutex for thread synchronization.
class Mutex {
public:
  /// Creates a mutex.
  Mutex();
  /// Creates a copy (shares the underlying mutex).
  Mutex(const Mutex &other);
  /// Destroys the mutex handle.
  ~Mutex();
  /// Assigns another mutex (shares the underlying mutex).
  Mutex &operator=(const Mutex &other);

  /// Locks the mutex, blocking until available.
  void lock();
  /// Unlocks the mutex.
  void unlock();
  /// Tries to lock without blocking. Returns true if acquired.
  bool tryLock();

private:
  MutexImpl *impl;
};

//------------------------------------------------------------------------------
// Process Management
//------------------------------------------------------------------------------

/// Execute external programs with optional I/O streaming.
class Subprocess {
public:
  /// Starts a process without waiting (fire and forget).
  template <typename... Args>
  static void Start(const Path &executable, const Args &...args) {
    Start_impl(executable, List(String(args)...));
  }

  /// Runs a process and waits for completion. Returns exit code.
  template <typename... Args>
  static int Run(const Path &executable, const Args &...args) {
    return Run_impl(executable, List(String(args)...));
  }

  /// Runs a process and captures stdout+stderr. Returns output as string.
  template <typename... Args>
  static String Capture(const Path &executable, const Args &...args) {
    return Capture_impl(executable, List(String(args)...));
  }

  /// Creates a subprocess with streaming I/O support.
  template <typename... Args>
  Subprocess(const Path &executable, const Args &...args)
      : Subprocess(executable, List(String(args)...)) {}

  /// Creates a copy (shares the underlying process).
  Subprocess(const Subprocess &other);
  /// Destroys the handle (does not terminate the process).
  ~Subprocess();
  /// Assigns another subprocess (shares the underlying process).
  Subprocess &operator=(const Subprocess &other);

  /// Returns true if the process was created successfully.
  bool isValid() const;
  /// Returns true if the process is still running.
  bool isRunning() const;
  /// Waits for completion and returns exit code (-1 on error).
  int wait();
  /// Forcefully terminates the process. Returns true on success.
  bool terminate();
  /// Returns the process ID (-1 on error).
  int getProcessId() const;

  /// Returns true if output data is available to read.
  bool hasAvailable() const;
  /// Reads available output into a buffer.
  Buffer readToBuffer();
  /// Reads available output into a string.
  String readToString();
  /// Writes a buffer to stdin. Returns bytes written, or -1 on error.
  int write(const Buffer &buf);
  /// Writes a string to stdin. Returns bytes written, or -1 on error.
  int write(const String &str);

private:
  SubprocessImpl *impl;
  Subprocess(const Path &executable, const List &arguments);
  static void Start_impl(const Path &executable, const List &arguments);
  static int Run_impl(const Path &executable, const List &arguments);
  static String Capture_impl(const Path &executable, const List &arguments);
};

//------------------------------------------------------------------------------
// System Integration
//------------------------------------------------------------------------------

/// Windows registry access.
class Registry {
public:
  /// Opens a registry key (e.g., "HKEY_CURRENT_USER\\Software\\MyApp").
  Registry(const String &key);
  /// Creates a copy (shares the underlying key).
  Registry(const Registry &other);
  /// Closes the key and frees resources.
  ~Registry();
  /// Assigns another registry (shares the underlying key).
  Registry &operator=(const Registry &other);

  /// Returns true if the key exists.
  bool exists() const;
  /// Creates the key if it doesn't exist. Returns true on success.
  bool create();
  /// Removes the key. Returns true on success.
  bool remove(bool isRecursive = true);

  /// Returns true if the value exists (empty name = default value).
  bool valueExists(const String &name = String()) const;
  /// Returns the string value, or nullptr if not found.
  const char *getStringValue(const String &name = String()) const;
  /// Returns the binary value, or nullptr if not found.
  const unsigned char *getBinaryValue(const String &name = String()) const;
  /// Returns the integer value (0 if not found).
  unsigned int getIntegerValue(const String &name = String()) const;
  /// Sets a string value. Returns true on success.
  bool setStringValue(const String &name, const String &str);
  /// Sets a binary value. Returns true on success.
  bool setBinaryValue(const String &name, const Buffer &buf);
  /// Sets an integer value. Returns true on success.
  bool setIntegerValue(const String &name, unsigned int num);
  /// Deletes a value. Returns true on success.
  bool deleteValue(const String &name = String());

  /// Returns a list of all value names.
  List listValueNames() const;
  /// Returns a list of all subkey names.
  List listSubkeys() const;

private:
  RegistryImpl *impl;
};

//------------------------------------------------------------------------------
// Networking
//------------------------------------------------------------------------------

/// HTTP response (returned by WebRequest).
class WebResponse {
  friend class WebRequest;

public:
  /// Creates a copy (shares the underlying response).
  WebResponse(const WebResponse &other);
  /// Destroys the response and frees resources.
  ~WebResponse();
  /// Assigns another response (shares the underlying response).
  WebResponse &operator=(const WebResponse &other);

  /// Returns true if status code is 200-299.
  bool succeeded() const;
  /// Returns the final URL after redirects.
  String getUrl() const;
  /// Returns the HTTP status code (e.g., 200, 404).
  int getStatusCode() const;
  /// Returns the HTTP status reason (e.g., "OK", "Not Found").
  String getStatusReason() const;
  /// Returns a map of response headers.
  Map getResponseHeaders() const;

  /// Parses body as JSON object. Returns nullptr if not a JSON object.
  const Map *asJson() const;
  /// Returns the body as a string.
  String asString() const;
  /// Returns the body as a buffer.
  Buffer asBuffer() const;

private:
  WebResponseImpl *impl;
  WebResponse();
};

/// HTTP client using WinHTTP.
class WebRequest {
public:
  /// Creates a request. params become query string; headers are sent with
  /// request.
  WebRequest(const String &url, const Map *params = nullptr,
             const Map *headers = nullptr);
  /// Creates a copy (shares the underlying request).
  WebRequest(const WebRequest &other);
  /// Destroys the request and frees resources.
  ~WebRequest();
  /// Assigns another request (shares the underlying request).
  WebRequest &operator=(const WebRequest &other);

  /// Performs an HTTP GET request. timeout in ms (-1 = infinite).
  WebResponse doGet(int timeout = -1);
  /// Performs an HTTP POST request with no body.
  WebResponse doPost(int timeout = -1);
  /// Performs an HTTP POST with JSON body (Map).
  WebResponse doPost(const Map &json, int timeout = -1);
  /// Performs an HTTP POST with JSON body (List).
  WebResponse doPost(const List &json, int timeout = -1);
  /// Performs an HTTP POST with binary body.
  WebResponse doPost(const Buffer &body, int timeout = -1);
  /// Performs an HTTP POST with string body.
  WebResponse doPost(const String &body, int timeout = -1);
  /// Performs a custom HTTP method (PUT, PATCH, DELETE, etc.).
  WebResponse doRequest(const String &method, const Buffer &body,
                        int timeout = -1);

  /// Returns the request URL.
  String getUrl() const;
  /// Returns the query parameters map.
  Map getParams() const;
  /// Returns the headers map.
  Map getHeaders() const;
  /// Returns true if a request has been completed (can only be done once).
  bool hasCompleted() const;

  /// Downloads a file from URL to disk. Returns true on success.
  static bool Download(const String &url, const String &savePath,
                       const Map *params = nullptr,
                       const Map *headers = nullptr, bool overwrite = true,
                       int timeout = -1);

private:
  WebRequestImpl *impl;
};

//------------------------------------------------------------------------------
// AI Integration
//------------------------------------------------------------------------------

/// Embedding vector for semantic similarity comparisons.
class Embedding {
  friend class AI;

public:
  /// Creates a copy (shares the underlying data).
  Embedding(const Embedding &other);
  /// Destroys the embedding and frees resources.
  ~Embedding();
  /// Assigns another embedding (shares the underlying data).
  Embedding &operator=(const Embedding &other);

  /// Computes cosine similarity with another embedding. Returns [-1, 1].
  float compare(const Embedding &other) const;
  /// Returns a pointer to the raw float array.
  const float *getRawArray() const;
  /// Returns the number of dimensions.
  int getDimensions() const;

private:
  EmbeddingImpl *impl;
  Embedding();
};

/// Multi-turn conversation with message history.
class Conversation {
  friend class AI;

public:
  /// Creates a copy (shares the underlying conversation).
  Conversation(const Conversation &other);
  /// Destroys the conversation and frees resources.
  ~Conversation();
  /// Assigns another conversation (shares the underlying conversation).
  Conversation &operator=(const Conversation &other);

  /// Sends a message and returns the response. Check isEmpty() on error.
  String ask(const String &prompt, int timeout = -1);

  /// Returns the conversation history (even=user, odd=assistant).
  List getConversationList() const;
  /// Replaces the conversation history. Returns true on success.
  bool setConversationList(const List &list);

  /// Creates a copy of this conversation for branching.
  Conversation duplicate() const;
  /// Returns the AI instance managing this conversation.
  AI getAI() const;

  /// Returns the number of prompt tokens used.
  int getPromptTokensUsed() const;
  /// Returns the number of response tokens used.
  int getResponseTokensUsed() const;
  /// Returns the total tokens used.
  int getTotalTokensUsed() const;

private:
  ConversationImpl *impl;
  Conversation();
};

/// OpenAI-compatible API client for chat completions and embeddings.
class AI {
  friend class Conversation;

public:
  /// Creates an AI client with base URL, API key, and model name.
  AI(const String &baseUrl, const String &apiKey, const String &model);
  /// Creates a copy (shares the underlying configuration).
  AI(const AI &other);
  /// Destroys the AI client and frees resources.
  ~AI();
  /// Assigns another AI client (shares the underlying configuration).
  AI &operator=(const AI &other);

  /// Sets the model name. Returns this AI for chaining.
  AI &setModel(const String &model);
  /// Sets the system prompt (nullptr to clear). Returns this AI for chaining.
  AI &setSystemPrompt(const String *prompt);
  /// Sets max response tokens (-1 for model default). Returns this AI for
  /// chaining.
  AI &setMaxTokens(int max = -1);
  /// Enables/disables JSON response mode. Returns this AI for chaining.
  AI &setJsonMode(bool isJsonMode = false);

  /// Returns the current model name.
  String getModel() const;
  /// Returns the system prompt. Check isEmpty() to see if set.
  String getSystemPrompt() const;
  /// Returns the base URL.
  String getBaseUrl() const;
  /// Returns the API key.
  String getAPIKey() const;

  /// Returns cumulative prompt tokens used.
  int getPromptTokensUsed() const;
  /// Returns cumulative response tokens used.
  int getResponseTokensUsed() const;
  /// Returns cumulative total tokens used.
  int getTotalTokensUsed() const;
  /// Resets all token counters to zero.
  void resetTokenTracking();
  /// Returns the finish reason from the last call (e.g., "stop", "length").
  String getFinishReason() const;

  /// Sends a single prompt and returns the response. Check isEmpty() on error.
  String ask(const String &prompt, int timeout = -1);
  /// Creates an embedding vector. Check getDimensions() == 0 on error.
  Embedding createEmbedding(const String &str, int dimensions = -1,
                            int timeout = -1);
  /// Creates a new multi-turn conversation.
  Conversation createConversation();

private:
  AIImpl *impl;
};

//------------------------------------------------------------------------------
// Console UI
//------------------------------------------------------------------------------

/// Console colors (classic 16-color palette, Windows 7 compatible).
enum ConsoleColor {
  /// Black (0).
  CON_BLACK = 0,
  /// Dark blue.
  CON_DARK_BLUE,
  /// Dark green.
  CON_DARK_GREEN,
  /// Dark cyan.
  CON_DARK_CYAN,
  /// Dark red.
  CON_DARK_RED,
  /// Dark magenta.
  CON_DARK_MAGENTA,
  /// Dark yellow (brown).
  CON_DARK_YELLOW,
  /// Gray (light gray).
  CON_GRAY,
  /// Dark gray.
  CON_DARK_GRAY,
  /// Bright blue.
  CON_BLUE,
  /// Bright green.
  CON_GREEN,
  /// Bright cyan.
  CON_CYAN,
  /// Bright red.
  CON_RED,
  /// Bright magenta.
  CON_MAGENTA,
  /// Bright yellow.
  CON_YELLOW,
  /// White.
  CON_WHITE
};

/// Text alignment modes for formatted console output.
enum ConsoleAlign {
  /// Left-aligned (default).
  CON_ALIGN_LEFT = 0,
  /// Center-aligned.
  CON_ALIGN_CENTER,
  /// Right-aligned.
  CON_ALIGN_RIGHT,
  /// Justified (stretched to fill width).
  CON_ALIGN_JUSTIFY
};

/// Configuration for interactive console input.
/// All fields have sensible defaults; set only what you need.
struct ConsoleInput {
  /// Tab completion candidates (nullptr for no completion).
  const List *completions;
  /// Command history buffer (read/write, nullptr for no history).
  /// History is automatically updated with each input.
  List *history;
  /// Mask input with asterisks for password entry.
  bool password;
  /// Allow multi-line input (Shift+Enter or Ctrl+Enter inserts newline).
  bool multiline;
  /// Prompt shown for continuation lines in multiline mode.
  String continuation;

  /// Creates default input options (no completion, no history, single-line,
  /// visible).
  ConsoleInput();
  /// Creates a copy of another ConsoleInput.
  ConsoleInput(const ConsoleInput &other);
  /// Destroys the ConsoleInput.
  ~ConsoleInput();
  /// Assigns another ConsoleInput to this one.
  ConsoleInput &operator=(const ConsoleInput &other);
};

/// Interactive console UI with colors, formatted output, REPL-style input,
/// animated progress bars, and interactive menus.
///
/// Features:
/// - Beautiful 16-color output with sensible defaults
/// - Automatic handling of console resize (UI elements reflow cleanly)
/// - Rich input with history, tab completion, multiline, and password mode
/// - Word-wrapping and text alignment helpers
/// - Smooth animated progress bars that resize gracefully
/// - Keyboard and mouse-driven menus with highlighting
/// - Non-blocking global hotkey support
///
/// Uses RAII to manage console state; original settings are restored on
/// destruction. Only one Console instance should exist at a time.
class Console {
public:
  /// Initializes the console for interactive use.
  /// Enables mouse input, sets up resize handling, and configures UTF-8 output.
  Console();
  /// Restores the original console state and settings.
  ~Console();
  /// Console cannot be copied.
  Console(const Console &) = delete;
  /// Console cannot be assigned.
  Console &operator=(const Console &) = delete;

  //-- Properties --

  /// Returns the console width in characters.
  int width() const;
  /// Returns the console height in characters.
  int height() const;

  //-- Basic Output --

  /// Prints text with optional foreground and background colors.
  Console &print(const String &text, ConsoleColor fg = CON_WHITE,
                 ConsoleColor bg = CON_BLACK);
  /// Prints text followed by a newline.
  Console &println(const String &text = "", ConsoleColor fg = CON_WHITE,
                   ConsoleColor bg = CON_BLACK);
  /// Clears the entire console screen.
  Console &clear();

  //-- Formatted Output --

  /// Prints text aligned within the specified width.
  /// If width is -1, uses the current console width.
  Console &printAligned(const String &text, int width,
                        ConsoleAlign align = CON_ALIGN_LEFT,
                        ConsoleColor fg = CON_WHITE,
                        ConsoleColor bg = CON_BLACK);
  /// Prints text with automatic word wrapping.
  /// If width is -1, uses the current console width.
  Console &printWrapped(const String &text, int width = -1,
                        ConsoleColor fg = CON_WHITE,
                        ConsoleColor bg = CON_BLACK);

  //-- Cursor Control --

  /// Moves the cursor to the specified position (0-based coordinates).
  Console &setCursor(int x, int y);
  /// Shows the text cursor.
  Console &showCursor();
  /// Hides the text cursor.
  Console &hideCursor();

  //-- Input --

  /// Reads a line of input with optional prompt and advanced options.
  /// Supports up/down arrow history, tab completion with hints, and more.
  /// Returns empty string if input is cancelled (Ctrl+C).
  String input(const String &prompt = "",
               const ConsoleInput *options = nullptr);
  /// Reads a single key press (blocking).
  /// Returns a descriptive name: "A", "Enter", "Space", "Ctrl+C", "Alt+X",
  /// "F1", etc.
  String readKey();

  //-- Progress Bar --

  /// Shows or updates an animated progress bar at the bottom of the console.
  /// percent: 0.0 to 1.0 for determinate, or -1.0 for indeterminate spinner.
  /// The bar animates smoothly between values and resizes with the console.
  Console &progress(float percent, const String &label = "");
  /// Hides the progress bar and restores the line.
  Console &progressHide();

  //-- Interactive Menu --

  /// Shows an interactive menu and returns the selected index (0-based).
  /// Returns -1 if cancelled (Escape key).
  /// Supports arrow keys, number keys (1-9), mouse hover/click, and
  /// first-letter jump.
  int menu(const List &options, const String &title = "");

  //-- Global Hotkeys --

  /// Registers a non-blocking global hotkey callback.
  /// Key format examples: "Ctrl+Q", "Alt+X", "Shift+F1", "F12", "Ctrl+Shift+S".
  /// The callback is invoked asynchronously when the key is pressed.
  Console &onHotkey(const String &key, void (*callback)(void *),
                    void *arg = nullptr);
  /// Unregisters a previously registered hotkey.
  Console &offHotkey(const String &key);

  //-- Static Text Utilities --

  /// Returns text padded and aligned to the specified width.
  /// Useful for building formatted output or tables.
  static String Align(const String &text, int width,
                      ConsoleAlign align = CON_ALIGN_LEFT);
  /// Returns text word-wrapped to fit the specified width.
  /// Preserves existing line breaks and wraps at word boundaries.
  static String Wrap(const String &text, int width);

private:
  ConsoleImpl *impl;
};

//------------------------------------------------------------------------------
// Math Utilities
//------------------------------------------------------------------------------

/// Mathematical functions and constants (all static, no state).
class Math {
public:
  /// Pi (π ≈ 3.14159).
  static constexpr float PI = 3.14159265358979323846f;
  /// Euler's number (e ≈ 2.71828).
  static constexpr float E = 2.71828182845904523536f;
  /// Tau (τ = 2π ≈ 6.28318).
  static constexpr float TAU = 6.28318530717958647693f;
  /// Square root of 2 (√2 ≈ 1.41421).
  static constexpr float SQRT_2 = 1.41421356237309504880f;
  /// Positive infinity.
  static const float INF;
  /// Negative infinity.
  static const float NEG_INF;
  /// Not-a-number.
  static const float NAN;

  /// Returns a random integer.
  static int random() noexcept;
  /// Returns a random float in [0.0, 1.0).
  static float randomFloat() noexcept;
  /// Returns a random integer in [start, end).
  static int randomRange(int start, int end) noexcept;
  /// Returns a random boolean.
  static bool randomBool() noexcept;
  /// Returns a random element from the list.
  template <typename T> static T randomChoice(const List &list) noexcept;

  /// Returns the absolute value of x.
  static int abs(int x) noexcept;
  /// Returns the absolute value of x.
  static float abs(float x) noexcept;
  /// Returns the smaller of a and b.
  static int min(int a, int b) noexcept;
  /// Returns the smaller of a and b.
  static float min(float a, float b) noexcept;
  /// Returns the larger of a and b.
  static int max(int a, int b) noexcept;
  /// Returns the larger of a and b.
  static float max(float a, float b) noexcept;
  /// Returns x clamped to [minVal, maxVal].
  static int clamp(int x, int minVal, int maxVal) noexcept;
  /// Returns x clamped to [minVal, maxVal].
  static float clamp(float x, float minVal, float maxVal) noexcept;
  /// Returns -1, 0, or 1 based on the sign of x.
  static int sign(int x) noexcept;
  /// Returns -1.0, 0.0, or 1.0 based on the sign of x.
  static float sign(float x) noexcept;
  /// Returns true if x is even.
  static bool isEven(int x) noexcept;
  /// Returns true if x is odd.
  static bool isOdd(int x) noexcept;
  /// Returns true if x is a power of two.
  static bool isPowerOfTwo(int x) noexcept;

  /// Returns the largest integer ≤ x.
  static float floor(float x) noexcept;
  /// Returns the smallest integer ≥ x.
  static float ceil(float x) noexcept;
  /// Returns x with fractional part removed (toward zero).
  static float trunc(float x) noexcept;
  /// Returns x rounded to the nearest integer.
  static float round(float x) noexcept;

  /// Returns 2^x.
  static float exp2(float x) noexcept;
  /// Returns log₂(x).
  static float log2(float x) noexcept;
  /// Returns the natural logarithm ln(x).
  static float log(float x) noexcept;
  /// Returns log₁₀(x).
  static float log10(float x) noexcept;
  /// Returns e^x.
  static float exp(float x) noexcept;
  /// Returns x^y.
  static float pow(float x, float y) noexcept;
  /// Returns 2^x (integer version).
  static int pow2(int x) noexcept;

  /// Returns the square root of x.
  static float sqrt(float x) noexcept;
  /// Returns the sine of x (radians).
  static float sin(float x) noexcept;
  /// Returns the cosine of x (radians).
  static float cos(float x) noexcept;
  /// Returns the tangent of x (radians).
  static float tan(float x) noexcept;
  /// Returns the arctangent of y/x (radians), using signs for quadrant.
  static float atan2(float y, float x) noexcept;
  /// Returns the arctangent of x (radians).
  static float atan(float x) noexcept;
  /// Returns the arcsine of x (radians).
  static float asin(float x) noexcept;
  /// Returns the hyperbolic cosine of x.
  static float cosh(float x) noexcept;

  /// Returns the floating-point remainder of x/y.
  static float mod(float x, float y) noexcept;
  /// Returns linear interpolation between a and b by t.
  static float lerp(float a, float b, float t) noexcept;
  /// Returns 0 if x < edge, otherwise 1.
  static float step(float edge, float x) noexcept;
  /// Converts degrees to radians.
  static float degToRad(float degrees) noexcept;
  /// Converts radians to degrees.
  static float radToDeg(float radians) noexcept;
  /// Returns true if x is finite (not infinity or NaN).
  static bool isFinite(float x) noexcept;
  /// Returns true if x is NaN.
  static bool isNaN(float x) noexcept;

  /// Returns the sum of a + b (64-bit integers).
  static long long Add64(long long a, long long b) noexcept;
  /// Returns the difference of a - b (64-bit integers).
  static long long Sub64(long long a, long long b) noexcept;
  /// Returns the quotient of a / b (64-bit integers).
  static long long Div64(long long a, long long b) noexcept;
  /// Returns the remainder of a / b (64-bit integers).
  static long long Mod64(long long a, long long b) noexcept;
};

template <typename T> inline T Math::randomChoice(const List &list) noexcept {
  if (list.isEmpty())
    return T();
  return list.at<T>(randomRange(0, list.length()));
}

//------------------------------------------------------------------------------
// System Functions
//------------------------------------------------------------------------------

/// Terminates the process with the given exit code.
void Exit(int exitCode);
/// Pauses the current thread for the given milliseconds.
void Sleep(int milliseconds);
/// Returns the value of an environment variable, or empty string if not set.
String GetEnv(const String &name);
/// Sets an environment variable. Returns true on success.
bool SetEnv(const String &name, const String &value);

/// Allocates memory. Returns nullptr on failure.
void *Alloc(int size);
/// Reallocates memory. Returns nullptr on failure.
void *Realloc(void *ptr, int size);
/// Frees allocated memory (null-safe).
void Free(void *ptr);

/// Returns the current user's login name.
String GetUserName();
/// Returns the current user's display name.
String GetUserDisplayName();
/// Returns the current process ID.
int GetProcessId();

//------------------------------------------------------------------------------
// Logging
//------------------------------------------------------------------------------

/// Enables logging to a file. Mutually exclusive with console logging.
void EnableLoggingToFile(const String &path, bool truncate = false);
/// Enables logging to console (default). Mutually exclusive with file logging.
void EnableLoggingToConsole();

// Log level selection (define ONE before including this header):
//   ATTOBOY_LOG_DEBUG_ENABLE   - Debug, Info, Warning, Error
//   ATTOBOY_LOG_INFO_ENABLE    - Info, Warning, Error (default)
//   ATTOBOY_LOG_WARNING_ENABLE - Warning, Error
//   ATTOBOY_LOG_ERROR_ENABLE   - Error only
//   ATTOBOY_LOG_DISABLE        - No logging

#if !defined(ATTOBOY_LOG_DISABLE)
#if !defined(ATTOBOY_LOG_DEBUG_ENABLE) && !defined(ATTOBOY_LOG_INFO_ENABLE) && \
    !defined(ATTOBOY_LOG_WARNING_ENABLE) && !defined(ATTOBOY_LOG_ERROR_ENABLE)
#define ATTOBOY_LOG_INFO_ENABLE
#endif
#endif

namespace internal {
void LogImpl(const String *args, int count, const String &prefix);
}

/// Prints arguments to the log destination followed by a newline.
template <typename... Args> void Log(const Args &...args) {
#if !defined(ATTOBOY_LOG_DISABLE)
  constexpr int count = sizeof...(Args);
  if constexpr (count > 0) {
    String strings[] = {String(args)...};
    internal::LogImpl(strings, count, String());
  } else {
    internal::LogImpl(nullptr, 0, String());
  }
#endif
}

//// Prints a debug message: "DEBUG YYYY-MM-DDTHH:MM:SS.fffZ: ..."
template <typename... Args> void LogDebug(const Args &...args) {
#if defined(ATTOBOY_LOG_DEBUG_ENABLE)
  constexpr int count = sizeof...(Args);
  if constexpr (count > 0) {
    String strings[] = {String(args)...};
    internal::LogImpl(strings, count, String("DEBUG "));
  } else {
    internal::LogImpl(nullptr, 0, String("DEBUG "));
  }
#endif
}

//// Prints an info message: "INFO YYYY-MM-DDTHH:MM:SS.fffZ: ..."
template <typename... Args> void LogInfo(const Args &...args) {
#if defined(ATTOBOY_LOG_DEBUG_ENABLE) || defined(ATTOBOY_LOG_INFO_ENABLE)
  constexpr int count = sizeof...(Args);
  if constexpr (count > 0) {
    String strings[] = {String(args)...};
    internal::LogImpl(strings, count, String("INFO "));
  } else {
    internal::LogImpl(nullptr, 0, String("INFO "));
  }
#endif
}

//// Prints a warning message: "WARNING YYYY-MM-DDTHH:MM:SS.fffZ: ..."
template <typename... Args> void LogWarning(const Args &...args) {
#if defined(ATTOBOY_LOG_DEBUG_ENABLE) || defined(ATTOBOY_LOG_INFO_ENABLE) ||   \
    defined(ATTOBOY_LOG_WARNING_ENABLE)
  constexpr int count = sizeof...(Args);
  if constexpr (count > 0) {
    String strings[] = {String(args)...};
    internal::LogImpl(strings, count, String("WARNING "));
  } else {
    internal::LogImpl(nullptr, 0, String("WARNING "));
  }
#endif
}

//// Prints an error message: "ERROR YYYY-MM-DDTHH:MM:SS.fffZ: ..."
template <typename... Args> void LogError(const Args &...args) {
#if !defined(ATTOBOY_LOG_DISABLE)
  constexpr int count = sizeof...(Args);
  if constexpr (count > 0) {
    String strings[] = {String(args)...};
    internal::LogImpl(strings, count, String("ERROR "));
  } else {
    internal::LogImpl(nullptr, 0, String("ERROR "));
  }
#endif
}

} // namespace attoboy

#endif // ATTOBOY_H
