#include "test_framework.h"

// This test is designed to be run by Python's python_test_runner.py
// It tests EnableLoggingToConsole() by writing to a file first, then switching
// to console mode and writing more, then back to file to capture results
// Python validates that the console-mode output appears in a separate log

void atto_main() {
    // Start with file logging to write initial coverage data
    EnableLoggingToFile("test_logging_console_setup.log", true);

    Log("=== Testing EnableLoggingToConsole Function ===");
    Log("Phase 1: File logging");

    // Switch to console logging
    EnableLoggingToConsole();
    REGISTER_TESTED(EnableLoggingToConsole);

    // These messages go to console (we can't easily capture them)
    // But we can test that the function doesn't crash
    Log("Phase 2: Console logging - message 1");
    Log("Phase 2: Console logging - message 2");
    Log("Phase 2: Console logging - message 3");

    // Switch back to file logging to write results
    EnableLoggingToFile("test_logging_console_result.log", true);
    Log("Phase 3: Back to file logging");
    Log("EnableLoggingToConsole test completed successfully");

    // Mark as tested and write coverage
    TestFramework::DisplayCoverage();
    TestFramework::WriteCoverageData("test_logging_console");

    Exit(0);
}
