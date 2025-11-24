#include "test_framework.h"

// This test is designed to be run by Python's python_test_runner.py
// It tests the Exit() function by exiting with a specific code (42)
// Python validates that the process actually exits with code 42

void atto_main() {
    // Enable file logging
    EnableLoggingToFile(ATTO_TEXT("test_exit.log"));

    Log(ATTO_TEXT("=== Testing Exit Function ==="));
    Log(ATTO_TEXT("This test will exit with code 42"));
    Log(ATTO_TEXT("Python will validate the exit code"));

    // Mark Exit as tested
    REGISTER_TESTED(Exit);

    // Write coverage data before exiting
    TestFramework::WriteCoverageData(ATTO_TEXT("test_exit"));

    // Exit with code 42 for Python validation
    Exit(42);

    // This code should never be reached
    Log(ATTO_TEXT("ERROR: Code after Exit() was reached!"));
}
