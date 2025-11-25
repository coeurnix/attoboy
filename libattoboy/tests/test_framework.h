#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include "test_functions.h"
#include <attoboy/attoboy.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef GetUserName // Avoid conflict with attoboy::GetUserName

using namespace attoboy;

// Simplified test framework for nostdlib mode
// Coverage tracking temporarily disabled due to static initialization issues
namespace TestFramework {

// Mark a function as tested (no-op in simplified mode)
inline void MarkTested(const ATTO_CHAR *name) {
  // No-op: coverage tracking disabled in nostdlib mode
}

// Display coverage (reports N/A in simplified mode)
inline void DisplayCoverage() {
  Log(ATTO_TEXT(""));
  Log(ATTO_TEXT("=== Test Coverage Report ==="));
  Log(ATTO_TEXT("Coverage: N/A (disabled in nostdlib mode)"));
  Log(ATTO_TEXT("============================"));
  Log(ATTO_TEXT(""));
}

// Write coverage data (creates empty file in simplified mode)
inline void WriteCoverageData(const String &test_name) {
  String filename = String(test_name, ATTO_TEXT("_coverage.txt"));
  String data = String(ATTO_TEXT("0 "), FUNCTION_COUNT, ATTO_TEXT("\n"));
  Path coverage_path(filename);
  coverage_path.writeFromString(data);
}
} // namespace TestFramework

// Global SaveCoverage function that uses TestFramework::WriteCoverageData
inline void SaveCoverage(const String &test_name) {
  TestFramework::WriteCoverageData(test_name);
}

// Macro to register that a function has been tested
#define REGISTER_TESTED(function_name)                                         \
  TestFramework::MarkTested(ATTO_TEXT(#function_name))

// Test assertion macros
#define ASSERT(condition)                                                      \
  do {                                                                         \
    if (!(condition)) {                                                        \
      LogError(ATTO_TEXT("ASSERTION FAILED: "), ATTO_TEXT(#condition),         \
               ATTO_TEXT(" at "), __FILE__, ATTO_TEXT(":"), __LINE__);         \
      Exit(1);                                                                 \
    }                                                                          \
  } while (0)

#define ASSERT_EQ(actual, expected)                                            \
  do {                                                                         \
    auto _actual = (actual);                                                   \
    auto _expected = (expected);                                               \
    if (_actual != _expected) {                                                \
      LogError(ATTO_TEXT("ASSERTION FAILED: "), ATTO_TEXT(#actual),            \
               ATTO_TEXT(" == "), ATTO_TEXT(#expected));                       \
      LogError(ATTO_TEXT("  Expected: "), _expected);                          \
      LogError(ATTO_TEXT("  Actual:   "), _actual);                            \
      LogError(ATTO_TEXT("  at "), __FILE__, ATTO_TEXT(":"), __LINE__);        \
      Exit(1);                                                                 \
    }                                                                          \
  } while (0)

#define ASSERT_NE(actual, expected)                                            \
  do {                                                                         \
    auto _actual = (actual);                                                   \
    auto _expected = (expected);                                               \
    if (_actual == _expected) {                                                \
      LogError(ATTO_TEXT("ASSERTION FAILED: "), ATTO_TEXT(#actual),            \
               ATTO_TEXT(" != "), ATTO_TEXT(#expected));                       \
      LogError(ATTO_TEXT("  Both values: "), _actual);                         \
      LogError(ATTO_TEXT("  at "), __FILE__, ATTO_TEXT(":"), __LINE__);        \
      Exit(1);                                                                 \
    }                                                                          \
  } while (0)

#define ASSERT_TRUE(condition) ASSERT(condition)
#define ASSERT_FALSE(condition) ASSERT(!(condition))

// Python validation macro (graceful fallback when Python unavailable)
#ifdef PYTHON_VALIDATION_AVAILABLE
#define PYTHON_EXPECTED(function_name, ...) // TODO: Implement Python validation
#else
#define PYTHON_EXPECTED(function_name, ...)
#endif

#endif // TEST_FRAMEWORK_H
