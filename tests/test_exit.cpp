#include "test_framework.h"

// This test is designed to be run by Python's python_test_runner.py
// It tests the Exit() function by exiting with a specific code (42)
// Python validates that the process actually exits with code 42

void atto_main() {
    // Enable file logging
    EnableLoggingToFile("test_exit.log", true);

    Log("=== Testing Exit Function ===");
    Log("This test will exit with code 42");
    Log("Python will validate the exit code");

    // Mark Exit as tested
    REGISTER_TESTED(Exit);

    // Write coverage data before exiting
    TestFramework::WriteCoverageData("test_exit");

    // Exit with code 42 for Python validation
    Exit(42);

    // This code should never be reached
    Log("ERROR: Code after Exit() was reached!");
}
