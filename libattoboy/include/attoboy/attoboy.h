#ifndef ATTOBOY_H
#define ATTOBOY_H

// Entry point for the application (bypasses CRT startup)
extern "C" void atto_main();

namespace attoboy {

/// Represents the type of a value stored in a container.
enum ValueType {
  TYPE_NULL = 0,  ///< Null or uninitialized value
  TYPE_INVALID,   ///< Invalid or error state
  TYPE_BOOL,      ///< Boolean value (true/false)
  TYPE_INT,       ///< Integer value (32-bit)
  TYPE_LONG_LONG, ///< Long integer value (64-bit)
  TYPE_DOUBLE,    ///< Floating-point value (double precision)
  TYPE_STRING,    ///< String value (attoboy::String)
  TYPE_LIST,      ///< List value (attoboy::List)
  TYPE_MAP,       ///< Map value (attoboy::Map)
  TYPE_SET,       ///< Set value (attoboy::Set)
  TYPE_UNKNOWN    ///< Unknown or unrecognized type
};

class StringImpl;
class ListImpl;
class MapImpl;
class SetImpl;
class DateTimeImpl;

// Forward declarations
class List;
class Map;
class Set;

/// An easy-to-use, batteries-included mutable string class
class String {
public:
  /// Creates an empty string with length 0.
  String();

  /// Creates a string from a C-style string, converting ANSI to Unicode.
  String(const char *str);

  /// Creates a string from a wide character string.
  String(const wchar_t *str);

  /// Creates a deep copy of another string.
  String(const String &other);

  /// Destroys the string and releases allocated memory.
  ~String();

  /// Assigns another string to this string (deep copy).
  String &operator=(const String &other);

  /// Creates a string representation of a boolean ("true" or "false").
  String(bool val);

  /// Creates a string representation of an integer.
  String(int val);

  /// Creates a string representation of a long integer.
  String(long long val);

  /// Creates a string representation of a floating-point number.
  String(double val);

  /// Creates a JSON-style string from a list (e.g., ["value1",2,true]).
  /// Handles all value types intelligently with proper JSON escaping.
  String(const List &list);

  /// Creates a JSON-style string from a map (e.g., {"key1":"value1","key2":2}).
  /// Converts keys to strings if needed and handles values intelligently with
  /// proper JSON escaping.
  String(const Map &map);

  /// Creates a JSON-style string from a set (e.g., [1,2,3]).
  /// Converts the set to a list and creates a JSON array representation.
  String(const Set &set);

  /// Returns the number of characters in the string.
  int length() const;

  /// Returns true if the string has a length of 0.
  bool isEmpty() const;

  /// Returns a new string containing the single character at `index`.
  /// Negative indices count from the end. Returns empty string if out of
  /// bounds.
  String at(int index) const;

  /// Returns a new string containing characters from `start` (inclusive) to
  /// `end` (exclusive). Negative indices count from the end of the string. If
  /// `end` is -1 (default), extracts to the end of the string.
  String substring(int start, int end = -1) const;

  /// Returns a deep copy of this string.
  String duplicate() const;

  /// Returns true if this string contains the specified `substring`.
  bool contains(const String &substring) const;

  /// Returns true if this string starts with the specified `substring`.
  bool startsWith(const String &substring) const;

  /// Returns true if this string ends with the specified `substring`.
  bool endsWith(const String &substring) const;

  /// Returns the number of non-overlapping occurrences of `substring`.
  int count(const String &substring) const;

  /// Returns true if the string represents a valid integer or decimal number.
  bool isNumber() const;

  /// Parses the string as a long integer. Returns 0 if invalid.
  long long toInteger() const;

  /// Parses the string as a floating-point number. Returns 0.0 if invalid.
  double toDouble() const;

  /// Returns true if string is "true", "t", "1", "yes", or "on"
  /// (case-insensitive). Otherwise returns false.
  bool toBool() const;

  /// Appends `substring` to the end of this string in-place.
  /// Returns a reference to this string for chaining.
  String &append(const String &substring);

  /// Prepends `substring` to the beginning of this string in-place.
  /// Returns a reference to this string for chaining.
  String &prepend(const String &substring);

  /// Inserts `substring` at the specified `index` in-place.
  /// Negative indices count from the end.
  /// Returns a reference to this string for chaining.
  String &insert(int index, const String &substring);

  /// Removes characters from `start` (inclusive) to `end` (exclusive) in-place.
  /// Negative indices count from the end.
  /// Returns a reference to this string for chaining.
  String &remove(int start, int end);

  /// Replaces all occurrences of `target` with `replacement` in-place.
  /// Returns a reference to this string for chaining.
  String &replace(const String &target, const String &replacement);

  /// Removes all leading and trailing whitespace characters in-place.
  /// Returns a reference to this string for chaining.
  String &trim();

  /// Converts all characters in this string to uppercase in-place.
  /// Returns a reference to this string for chaining.
  String &upper();

  /// Converts all characters in this string to lowercase in-place.
  /// Returns a reference to this string for chaining.
  String &lower();

  /// Reverses the order of characters in this string in-place.
  /// Returns a reference to this string for chaining.
  String &reverse();

  /// Repeats the string content `count` times in-place.
  /// Returns a reference to this string for chaining.
  String &repeat(int count);

  /// Returns true if this string is equal to the other string.
  bool equals(const String &other) const;

  /// Compares this string with another lexicographically.
  /// Returns negative if this < other, 0 if equal, positive if this > other.
  int compare(const String &other) const;

  /// Returns true if this string is equal to the other string.
  bool operator==(const String &other) const;

  /// Returns true if this string is not equal to the other string.
  bool operator!=(const String &other) const;

  /// Returns a hash code for this string.
  int hash() const;

  /// Splits this string into a list of strings by line breaks.
  /// Handles both Windows (\\r\\n) and Unix (\\n) line endings.
  List lines() const;

  /// Joins all elements of the list using this string as the separator.
  /// Non-string elements are converted to strings automatically.
  String join(const List &list) const;

  /// Splits this string by the separator into a list of strings.
  /// The max parameter controls the maximum number of splits (default 1).
  /// Returns up to max+2 parts (e.g., max=1 allows 1 split, yielding 2 parts).
  List split(const String &sep, int max = 1) const;

  /// Splits this string by whitespace into a list of strings.
  /// Consecutive whitespace is treated as a single separator.
  List split() const;

  /// Formats this string using list elements as substitutions.
  /// Replaces {0}, {1}, etc. with corresponding list elements.
  String format(const List &list) const;

  /// Formats this string using map values as substitutions.
  /// Replaces {key} with the corresponding value from the map.
  String format(const Map &map) const;

  /// Returns a pointer to the underlying null-terminated wide string.
  /// Returns an empty string literal if the string is empty.
  const wchar_t *c_str() const;

private:
  StringImpl *impl;
};

/// A dynamic array container that stores heterogeneous typed values.
/// Supports storing bools, integers, doubles, strings, and nested lists.
/// All operations are thread-safe and handle errors gracefully without
/// crashing.
class List {
public:
  /// Creates an empty list with initial capacity of 8 elements.
  List();

  /// Creates an empty list with the specified initial capacity.
  List(int capacity);

  /// Creates a deep copy of another list.
  List(const List &other);

  /// Creates a list from a set, preserving all values.
  List(const Set &set);

  /// Destroys the list and releases allocated memory.
  ~List();

  /// Assigns another list to this list (deep copy).
  List &operator=(const List &other);

  /// Returns the number of elements in the list.
  int length() const;

  /// Returns true if the list has no elements.
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

  /// Removes and returns the value at the specified index.
  /// Index is clamped to valid range. If list is empty, returns null-type
  /// value. Shifts remaining elements down.
  template <typename T> T pop();

  /// Returns the value at the specified index.
  /// Index is clamped to valid range [0, length-1].
  /// If list is empty, returns default-constructed value.
  template <typename T> T at(int index) const;

  /// Returns the value at the specified index using [] operator.
  /// Index is clamped to valid range [0, length-1].
  /// If list is empty, returns default-constructed value.
  template <typename T> T operator[](int index) const;

  /// Sets the value at the specified index.
  /// Index is clamped to valid range. If list is empty, appends the value.
  /// Returns a reference to this list for chaining.
  template <typename T> List &set(int index, T value) {
    set_impl(index, value);
    return *this;
  }

  /// Removes the element at the specified index and shifts remaining elements.
  /// Index is clamped to valid range. If list is empty, does nothing.
  /// Returns a reference to this list for chaining.
  List &remove(int index);

  /// Removes all elements from the list.
  /// Returns a reference to this list for chaining.
  List &clear();

  /// Finds the first occurrence of a value in the list.
  /// Uses numeric coercion for comparison (e.g., int 5 equals double 5.0).
  /// Returns the index of the value, or -1 if not found.
  template <typename T> int find(T value) const;

  /// Returns true if the list contains the specified value.
  /// Uses numeric coercion for comparison (e.g., int 5 equals double 5.0).
  template <typename T> bool contains(T value) const;

  /// Reverses the order of elements in the list in-place.
  /// Returns a reference to this list for chaining.
  List &reverse();

  /// Sorts the list in ascending or descending order.
  /// Uses best-effort cross-type comparison with numeric coercion.
  /// Returns a reference to this list for chaining.
  List &sort(bool ascending = true);

  /// Returns the type of the value at the specified index.
  /// Returns TYPE_INVALID if index is out of bounds.
  ValueType typeAt(int index) const;

  /// Returns a new list containing elements from start (inclusive) to end
  /// (exclusive). Negative indices count from the end. Indices are clamped to
  /// valid range.
  List slice(int start, int end) const;

  /// Appends all elements from another list to the end of this list.
  /// Returns a reference to this list for chaining.
  List &concat(const List &other);

  /// Appends all values from a set to the end of this list.
  /// Returns a reference to this list for chaining.
  List &concat(const Set &set);

  /// Returns a shallow copy of this list
  List duplicate() const;

private:
  ListImpl *impl;

  // Implementation methods for different types
  void append_impl(bool value);
  void append_impl(int value);
  void append_impl(long long value);
  void append_impl(double value);
  void append_impl(const char *value);
  void append_impl(const wchar_t *value);
  void append_impl(const String &value);
  void append_impl(const List &value);
  void append_impl(const Map &value);
  void append_impl(const Set &value);

  void prepend_impl(bool value);
  void prepend_impl(int value);
  void prepend_impl(long long value);
  void prepend_impl(double value);
  void prepend_impl(const char *value);
  void prepend_impl(const wchar_t *value);
  void prepend_impl(const String &value);
  void prepend_impl(const List &value);
  void prepend_impl(const Map &value);
  void prepend_impl(const Set &value);

  void insert_impl(int index, bool value);
  void insert_impl(int index, int value);
  void insert_impl(int index, long long value);
  void insert_impl(int index, double value);
  void insert_impl(int index, const char *value);
  void insert_impl(int index, const wchar_t *value);
  void insert_impl(int index, const String &value);
  void insert_impl(int index, const List &value);
  void insert_impl(int index, const Map &value);
  void insert_impl(int index, const Set &value);

  void set_impl(int index, bool value);
  void set_impl(int index, int value);
  void set_impl(int index, long long value);
  void set_impl(int index, double value);
  void set_impl(int index, const char *value);
  void set_impl(int index, const wchar_t *value);
  void set_impl(int index, const String &value);
  void set_impl(int index, const List &value);
  void set_impl(int index, const Map &value);
  void set_impl(int index, const Set &value);
};

/// A key-value map container that stores heterogeneous typed keys and values.
/// Keys must be unique (exclusive map). Insertion order is not guaranteed.
/// All operations are thread-safe and handle errors gracefully without
/// crashing.
class Map {
public:
  /// Creates an empty map with initial capacity of 8 key-value pairs.
  Map();

  /// Creates an empty map with the specified initial capacity.
  Map(int capacity);

  /// Creates a deep copy of another map.
  Map(const Map &other);

  /// Destroys the map and releases allocated memory.
  ~Map();

  /// Assigns another map to this map (deep copy).
  Map &operator=(const Map &other);

  /// Returns the number of key-value pairs in the map.
  int length() const;

  /// Returns true if the map has no key-value pairs.
  bool isEmpty() const;

  /// Returns the value associated with the key, or default value if not found.
  /// The default value is a null-type value if not specified.
  template <typename K, typename V> V get(K key, V defaultValue = V()) const;

  /// Returns the value associated with the key using [] operator.
  /// Returns a null-type value if the key is not found.
  template <typename K, typename V> V operator[](K key) const;

  /// Associates the key with the value. If key exists, updates its value.
  /// Returns a reference to this map for chaining.
  template <typename K, typename V> Map &put(K key, V value);

  /// Removes the key-value pair with the specified key.
  /// Returns a reference to this map for chaining.
  template <typename K> Map &remove(K key);

  /// Removes all key-value pairs from the map.
  /// Returns a reference to this map for chaining.
  Map &clear();

  /// Finds the first key associated with the specified value.
  /// Returns a null-type value if the value is not found.
  template <typename K, typename V> K findValue(V value) const;

  /// Returns true if the map contains the specified key.
  template <typename K> bool hasKey(K key) const;

  /// Returns the type of the value associated with the key.
  /// Returns TYPE_INVALID if the key is not found.
  template <typename K> ValueType typeAt(K key) const;

  /// Adds all key-value pairs from another map to this map.
  /// If a key exists in both maps, the value from the other map is used.
  /// Returns a reference to this map for chaining.
  Map &merge(const Map &other);

  /// Returns a deep copy of this map.
  Map duplicate() const;

  /// Returns a copy of all keys in the map as a List.
  List keys() const;

  /// Returns a copy of all values in the map as a List.
  List values() const;

private:
  MapImpl *impl;

  // Implementation methods for different types
  void put_impl(bool key, bool value);
  void put_impl(bool key, int value);
  void put_impl(bool key, long long value);
  void put_impl(bool key, double value);
  void put_impl(bool key, const char *value);
  void put_impl(bool key, const wchar_t *value);
  void put_impl(bool key, const String &value);
  void put_impl(bool key, const List &value);
  void put_impl(bool key, const Map &value);
  void put_impl(bool key, const Set &value);

  void put_impl(int key, bool value);
  void put_impl(int key, int value);
  void put_impl(int key, long long value);
  void put_impl(int key, double value);
  void put_impl(int key, const char *value);
  void put_impl(int key, const wchar_t *value);
  void put_impl(int key, const String &value);
  void put_impl(int key, const List &value);
  void put_impl(int key, const Map &value);
  void put_impl(int key, const Set &value);

  void put_impl(long long key, bool value);
  void put_impl(long long key, int value);
  void put_impl(long long key, long long value);
  void put_impl(long long key, double value);
  void put_impl(long long key, const char *value);
  void put_impl(long long key, const wchar_t *value);
  void put_impl(long long key, const String &value);
  void put_impl(long long key, const List &value);
  void put_impl(long long key, const Map &value);
  void put_impl(long long key, const Set &value);

  void put_impl(double key, bool value);
  void put_impl(double key, int value);
  void put_impl(double key, long long value);
  void put_impl(double key, double value);
  void put_impl(double key, const char *value);
  void put_impl(double key, const wchar_t *value);
  void put_impl(double key, const String &value);
  void put_impl(double key, const List &value);
  void put_impl(double key, const Map &value);
  void put_impl(double key, const Set &value);

  void put_impl(const char *key, bool value);
  void put_impl(const char *key, int value);
  void put_impl(const char *key, long long value);
  void put_impl(const char *key, double value);
  void put_impl(const char *key, const char *value);
  void put_impl(const char *key, const wchar_t *value);
  void put_impl(const char *key, const String &value);
  void put_impl(const char *key, const List &value);
  void put_impl(const char *key, const Map &value);
  void put_impl(const char *key, const Set &value);

  void put_impl(const wchar_t *key, bool value);
  void put_impl(const wchar_t *key, int value);
  void put_impl(const wchar_t *key, long long value);
  void put_impl(const wchar_t *key, double value);
  void put_impl(const wchar_t *key, const char *value);
  void put_impl(const wchar_t *key, const wchar_t *value);
  void put_impl(const wchar_t *key, const String &value);
  void put_impl(const wchar_t *key, const List &value);
  void put_impl(const wchar_t *key, const Map &value);
  void put_impl(const wchar_t *key, const Set &value);

  void put_impl(const String &key, bool value);
  void put_impl(const String &key, int value);
  void put_impl(const String &key, long long value);
  void put_impl(const String &key, double value);
  void put_impl(const String &key, const char *value);
  void put_impl(const String &key, const wchar_t *value);
  void put_impl(const String &key, const String &value);
  void put_impl(const String &key, const List &value);
  void put_impl(const String &key, const Map &value);
  void put_impl(const String &key, const Set &value);

  void remove_impl(bool key);
  void remove_impl(int key);
  void remove_impl(long long key);
  void remove_impl(double key);
  void remove_impl(const char *key);
  void remove_impl(const wchar_t *key);
  void remove_impl(const String &key);
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

/// A set container that stores unique heterogeneous typed values.
/// Values are exclusive (no duplicates). Insertion order is not guaranteed.
/// All operations are thread-safe and handle errors gracefully without
/// crashing.
class Set {
public:
  /// Creates an empty set with initial capacity of 8 elements.
  Set();

  /// Creates an empty set with the specified initial capacity.
  Set(int capacity);

  /// Creates a deep copy of another set.
  Set(const Set &other);

  /// Creates a set from a list, removing duplicate values.
  Set(const List &list);

  /// Destroys the set and releases allocated memory.
  ~Set();

  /// Assigns another set to this set (deep copy).
  Set &operator=(const Set &other);

  /// Returns the number of unique values in the set.
  int length() const;

  /// Returns true if the set has no values.
  bool isEmpty() const;

  /// Adds one or more values to the set. Duplicates are ignored.
  /// Returns a reference to this set for chaining.
  template <typename T> Set &put(T value) {
    put_impl(value);
    return *this;
  }

  /// Returns true if the set contains the specified value.
  template <typename T> bool contains(T value) const;

  /// Removes the specified value from the set if it exists.
  /// Returns a reference to this set for chaining.
  template <typename T> Set &remove(T value) {
    remove_impl(value);
    return *this;
  }

  /// Removes all values from the set.
  /// Returns a reference to this set for chaining.
  Set &clear();

  /// Adds all values from another set to this set (union operation).
  /// Duplicates are automatically ignored.
  /// Returns a reference to this set for chaining.
  Set &setUnion(const Set &other);

  /// Keeps only values that exist in both this set and the other set.
  /// Removes all values not in the intersection.
  /// Returns a reference to this set for chaining.
  Set &intersect(const Set &other);

  /// Removes all values that exist in the other set from this set.
  /// Returns a reference to this set for chaining.
  Set &subtract(const Set &other);

  /// Returns a deep copy of this set.
  Set duplicate() const;

  /// Returns a List containing all values from this set.
  List toList() const;

private:
  SetImpl *impl;

  // Implementation methods for different types
  void put_impl(bool value);
  void put_impl(int value);
  void put_impl(long long value);
  void put_impl(double value);
  void put_impl(const char *value);
  void put_impl(const wchar_t *value);
  void put_impl(const String &value);
  void put_impl(const List &value);
  void put_impl(const Map &value);
  void put_impl(const Set &value);

  void remove_impl(bool value);
  void remove_impl(int value);
  void remove_impl(long long value);
  void remove_impl(double value);
  void remove_impl(const char *value);
  void remove_impl(const wchar_t *value);
  void remove_impl(const String &value);
};

/// A date and time class using Windows FileTime internally.
/// Supports operations on dates/times with millisecond precision.
class DateTime {
public:
  /// Creates a DateTime representing the current date and time.
  DateTime();

  /// Creates a DateTime from milliseconds since Unix epoch (Jan 1, 1970).
  DateTime(long long millisSinceEpoch);

  /// Creates a DateTime from an ISO-8601 formatted string (YYYY-MM-DDTHH:MM:SS.fffZ).
  DateTime(const String &iso8601);

  /// Creates a deep copy of another DateTime.
  DateTime(const DateTime &other);

  /// Destroys the DateTime and releases allocated memory.
  ~DateTime();

  /// Assigns another DateTime to this one.
  DateTime &operator=(const DateTime &other);

  /// Adds (or subtracts if negative) milliseconds to this DateTime.
  /// Returns a reference to this DateTime for chaining.
  DateTime &add(long long milliseconds);

  /// Returns the difference in milliseconds between this and another DateTime.
  /// Positive if this is later than other, negative otherwise.
  long long diff(const DateTime &other) const;

  /// Compares this DateTime with another.
  /// Returns negative if this < other, 0 if equal, positive if this > other.
  int compare(const DateTime &other) const;

  /// Returns true if this DateTime equals the other DateTime.
  bool equals(const DateTime &other) const;

  /// Returns true if this DateTime equals the other DateTime.
  bool operator==(const DateTime &other) const;

  /// Returns true if this DateTime does not equal the other DateTime.
  bool operator!=(const DateTime &other) const;

  /// Returns this DateTime as milliseconds since Unix epoch (Jan 1, 1970).
  long long timestamp() const;

  /// Returns this DateTime as an ISO-8601 formatted string (YYYY-MM-DDTHH:MM:SS.fffZ).
  String toString() const;

private:
  DateTimeImpl *impl;
};

/// Terminates the process immediately with the specified exit code.
void Exit(int exitCode);

/// Returns the value of an environment variable, or empty string if not set.
String GetEnv(const String &name);

/// Sets an environment variable. Returns true on success, false on failure.
bool SetEnv(const String &name, const String &value);

// Logging Level Configuration:
// Define one of the following to set the logging level:
// - ATTOBOY_LOG_DEBUG_ENABLE: Enables Debug, Info, Warning, Error
// - ATTOBOY_LOG_INFO_ENABLE: Enables Info, Warning, Error (default if none
// set)
// - ATTOBOY_LOG_WARNING_ENABLE: Enables Warning, Error
// - ATTOBOY_LOG_ERROR_ENABLE: Enables Error only
// - ATTOBOY_LOG_DISABLE: Disables all logging
//
// If no level is defined, INFO level is used by default.

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

} // namespace attoboy

#endif // ATTOBOY_H
