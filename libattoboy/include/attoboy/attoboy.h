#ifndef ATTOBOY_H
#define ATTOBOY_H

// Entry point for the application (bypasses CRT startup)
extern "C" void atto_main();

namespace attoboy {

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

/// Terminates the process immediately with the specified exit code.
void Exit(int exitCode);

// Logging Level Configuration:
// Define one of the following to set the logging level:
// - ATTOBOY_LOG_DEBUG_ENABLE: Enables Debug, Info, Warning, Error
// - ATTOBOY_LOG_INFO_ENABLE: Enables Info, Warning, Error (default if none set)
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
/// Enabled when ATTOBOY_LOG_DEBUG_ENABLE or ATTOBOY_LOG_INFO_ENABLE is defined.
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
