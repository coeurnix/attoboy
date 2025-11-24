# libattoboy CTest Framework

This directory contains the CTest-based testing framework for libattoboy with automatic function coverage tracking.

## Overview

The testing system uses a CTest-based framework that:

- Uses `enable_testing()` and `add_test()` for proper CTest integration
- Automatically tracks coverage of all functions from `attoboy.h`
- Provides assertion macros for writing tests
- Displays coverage reports showing tested vs. untested functions

## Framework Components

### test_functions.h
Registry of all functions from `attoboy.h`. Currently tracks 17 system/utility functions:
- Exit, Sleep
- GetEnv, SetEnv
- Alloc, Realloc, Free
- GetUserName, GetUserDisplayName, GetProcessId
- EnableLoggingToFile, EnableLoggingToConsole
- Log, LogDebug, LogInfo, LogWarning, LogError

### test_framework.h
Provides:
- `REGISTER_TESTED(function_name)` - Mark a function as tested
- `TestFramework::DisplayCoverage()` - Show coverage report
- `TestFramework::WriteCoverageData(test_name)` - Write machine-readable coverage for aggregation
- `ASSERT(condition)` - Basic assertion
- `ASSERT_EQ(actual, expected)` - Equality assertion
- `ASSERT_NE(actual, expected)` - Inequality assertion
- `ASSERT_TRUE(condition)` - Boolean true assertion
- `ASSERT_FALSE(condition)` - Boolean false assertion

### python_test_runner.py
Python-cooperative test runner for validating test executables externally:
- `exit <exe> <expected_code>` - Validates process exit code
- `console <exe> <log_file> <expected_content...>` - Validates log file contains expected strings
- Extensible for future validation patterns (timing, resource usage, etc.)

## Running Tests

**Recommended**: Build and run tests with aggregated coverage display:
```bash
cmake --build build --config Release
cmake --build build --target coverage
```

Output:
```
============================================================
OVERALL TEST COVERAGE
============================================================
Coverage: 15/17 functions (88.24%)
[WARN] 2 function(s) remain untested
============================================================
```

Alternative: Run CTest directly without coverage aggregation:
```bash
cd build
ctest --output-on-failure
```

Run a specific test:
```bash
ctest -R test_util_functions --output-on-failure
```

Run tests in verbose mode:
```bash
ctest -V
```

## Example Tests

- **test_util_functions.cpp**: Tests system utility functions (GetUserName, GetEnv, etc.) with coverage tracking
- **test_test_framework.cpp**: Validates the test framework itself (assertion macros, coverage tracking)
- **test_exit.cpp**: Python-cooperative test validating `Exit()` returns correct exit code (42)
- **test_logging_console.cpp**: Python-cooperative test validating `EnableLoggingToConsole()` behavior

## Writing New Tests

1. Create a new `.cpp` file in `libattoboy/tests/`
2. Include `"test_framework.h"`
3. Implement `void atto_main()` as your test entry point
4. Use assertion macros to validate behavior
5. Call `REGISTER_TESTED(function_name)` for each function you test
6. Optionally call `TestFramework::DisplayCoverage()` to show coverage

Example:
```cpp
#include "test_framework.h"

void atto_main() {
    // Enable file logging for reliable output capture
    EnableLoggingToFile("my_test.log");

    Log("Testing MyFunction");

    String result = MyFunction();
    REGISTER_TESTED(MyFunction);

    ASSERT_FALSE(result.isEmpty());
    ASSERT_EQ(result, String("expected"));

    TestFramework::DisplayCoverage();

    // Write machine-readable coverage data for aggregation
    TestFramework::WriteCoverageData("my_test");

    Exit(0);
}
```

**Important**:
- Always use `EnableLoggingToFile()` at the start of your test to ensure output is captured reliably
- Always call `WriteCoverageData()` at the end to contribute to overall coverage statistics
- The log file and coverage file will be created in the working directory (usually `build/libattoboy/`)

View test output:
```bash
cat build/libattoboy/my_test.log
```

## Python-Cooperative Testing Pattern

Some functions require external validation (e.g., exit codes, console output). Use Python-cooperative tests:

**Example: Testing Exit() with exit code validation**
```cpp
void atto_main() {
    EnableLoggingToFile("test_exit.log");
    REGISTER_TESTED(Exit);
    TestFramework::WriteCoverageData("test_exit");
    Exit(42);  // Python validates exit code is 42
}
```

**CMakeLists.txt integration:**
```cmake
if(test_name STREQUAL "test_exit" AND PYTHON_EXECUTABLE)
    add_test(NAME ${test_name}
        COMMAND ${PYTHON_EXECUTABLE}
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/python_test_runner.py"
            exit
            "${CMAKE_BINARY_DIR}/tests/${test_name}${CMAKE_EXECUTABLE_SUFFIX}"
            42
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/libattoboy"
    )
endif()
```

**Available Python validators:**
- `exit <exe> <code>` - Validates exit code
- `console <exe> <log> <text...>` - Validates log file content

This pattern allows testing functions that:
- Terminate the process (Exit)
- Produce side effects difficult to capture internally
- Require timing or resource validation
- Need cross-platform validation against known-good implementations

## Coverage Goals

**Current Status: 17/17 functions (100.00%) ✓**

The system tracks all utility functions from `attoboy.h`. As you expand coverage:
- Mark each tested function with `REGISTER_TESTED()`
- Call `WriteCoverageData()` to contribute to overall statistics
- Run `cmake --build build --target coverage` to see progress
- Coverage reports show: "X/Y functions (Z%) - Untested: [list]"

## Next Steps

1. Expand `test_functions.h` to include all class methods and functions from `attoboy.h`
2. Create comprehensive tests for each class (String, List, Map, Set, etc.)
3. Implement Python validation system for cross-checking results
4. Achieve 100% function coverage
