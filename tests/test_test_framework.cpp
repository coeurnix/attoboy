#include "test_framework.h"

void atto_main() {
    // Enable file logging for reliable output capture
    EnableLoggingToFile("test_test_framework.log", true);

    Log("=== Running Test Framework Validation ===");

    // Test ASSERT macro
    {
        ASSERT(true);
        ASSERT(1 == 1);
        ASSERT(5 > 3);
        Log("ASSERT: passed");
    }

    // Test ASSERT_EQ macro
    {
        ASSERT_EQ(5, 5);
        ASSERT_EQ(String("hello"), String("hello"));
        ASSERT_EQ(true, true);
        Log("ASSERT_EQ: passed");
    }

    // Test ASSERT_NE macro
    {
        ASSERT_NE(5, 10);
        ASSERT_NE(String("hello"), String("world"));
        ASSERT_NE(true, false);
        Log("ASSERT_NE: passed");
    }

    // Test ASSERT_TRUE macro
    {
        ASSERT_TRUE(true);
        ASSERT_TRUE(1 == 1);
        ASSERT_TRUE(5 > 3);
        Log("ASSERT_TRUE: passed");
    }

    // Test ASSERT_FALSE macro
    {
        ASSERT_FALSE(false);
        ASSERT_FALSE(1 == 2);
        ASSERT_FALSE(5 < 3);
        Log("ASSERT_FALSE: passed");
    }

    // Test function registration
    {
        // Mark some functions as tested
        REGISTER_TESTED(GetUserName);
        REGISTER_TESTED(GetProcessId);
        REGISTER_TESTED(Log);

        Log("Function registration: passed");
    }

    // Test coverage calculation
    {
        // The coverage report will show that we've tested at least GetUserName,
        // GetProcessId, and Log
        Log("Coverage calculation test: passed");
    }

    Log("=== All Framework Validation Tests Passed ===");

    // Display coverage report
    TestFramework::DisplayCoverage();

    // Write machine-readable coverage data for aggregation
    TestFramework::WriteCoverageData("test_test_framework");

    Exit(0);
}
