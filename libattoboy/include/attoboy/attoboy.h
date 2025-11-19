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
  TYPE_UNKNOWN    ///< Unknown or unrecognized type
};

class StringImpl;

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

  /// Creates a string representation of a boolean ("true" or "false").
  String(bool val);

  /// Creates a string representation of an integer.
  String(int val);

  /// Creates a string representation of a long integer.
  String(long long val);

  /// Creates a string representation of a floating-point number.
  String(double val);

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

  /// Returns a pointer to the underlying null-terminated wide string.
  /// Returns an empty string literal if the string is empty.
  const wchar_t *c_str() const;

private:
  StringImpl *impl;
};

class ListImpl;

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

  /// Destroys the list and releases allocated memory.
  ~List();

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

  void prepend_impl(bool value);
  void prepend_impl(int value);
  void prepend_impl(long long value);
  void prepend_impl(double value);
  void prepend_impl(const char *value);
  void prepend_impl(const wchar_t *value);
  void prepend_impl(const String &value);
  void prepend_impl(const List &value);

  void insert_impl(int index, bool value);
  void insert_impl(int index, int value);
  void insert_impl(int index, long long value);
  void insert_impl(int index, double value);
  void insert_impl(int index, const char *value);
  void insert_impl(int index, const wchar_t *value);
  void insert_impl(int index, const String &value);
  void insert_impl(int index, const List &value);

  void set_impl(int index, bool value);
  void set_impl(int index, int value);
  void set_impl(int index, long long value);
  void set_impl(int index, double value);
  void set_impl(int index, const char *value);
  void set_impl(int index, const wchar_t *value);
  void set_impl(int index, const String &value);
  void set_impl(int index, const List &value);
};

/// Terminates the process immediately with the specified exit code.
void Exit(int exitCode);

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
