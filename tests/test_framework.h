#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include "test_functions.h"
#include <attoboy/attoboy.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef GetUserName // Avoid conflict with attoboy::GetUserName

using namespace attoboy;

// Test framework with dynamic coverage tracking
// Uses attoboy's Set to avoid static initialization issues in nostdlib mode
namespace TestFramework {

static Set *tested_functions = nullptr;
static int test_count = 0;

// Mark a function as tested
inline void MarkTested(const char *name) {
  if (!tested_functions) {
    tested_functions = new Set();
  }
  tested_functions->put(String(name));
}

// Display coverage
inline void DisplayCoverage() {
  Log("");
  Log("=== Test Coverage Report ===");
  int tested_count = tested_functions ? tested_functions->length() : 0;
  Log("Coverage: ", tested_count, " / ", FUNCTION_COUNT);
  Log("============================");
  Log("");
}

// Write coverage data
inline void WriteCoverageData(const String &test_name) {
  int tested_count = tested_functions ? tested_functions->length() : 0;

  String data = String(tested_count) + " " + String(FUNCTION_COUNT) + "\n";

  String filename = test_name + "_coverage.txt";
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
  TestFramework::MarkTested(#function_name)

// Test assertion macros
#define ASSERT(condition)                                                      \
  do {                                                                         \
    if (!(condition)) {                                                        \
      LogError("ASSERTION FAILED: ", #condition,         \
               " at ", __FILE__, ":", __LINE__);         \
      Exit(1);                                                                 \
    }                                                                          \
  } while (0)

#define ASSERT_EQ(actual, expected)                                            \
  do {                                                                         \
    auto _actual = (actual);                                                   \
    auto _expected = (expected);                                               \
    if (_actual != _expected) {                                                \
      LogError("ASSERTION FAILED: ", #actual,            \
               " == ", #expected);                       \
      LogError("  Expected: ", _expected);                          \
      LogError("  Actual:   ", _actual);                            \
      LogError("  at ", __FILE__, ":", __LINE__);        \
      Exit(1);                                                                 \
    }                                                                          \
  } while (0)

#define ASSERT_NE(actual, expected)                                            \
  do {                                                                         \
    auto _actual = (actual);                                                   \
    auto _expected = (expected);                                               \
    if (_actual == _expected) {                                                \
      LogError("ASSERTION FAILED: ", #actual,            \
               " != ", #expected);                       \
      LogError("  Both values: ", _actual);                         \
      LogError("  at ", __FILE__, ":", __LINE__);        \
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
