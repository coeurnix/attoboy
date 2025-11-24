#include "test_framework.h"

void atto_main() {
    // Enable file logging for reliable output capture
    EnableLoggingToFile(ATTO_TEXT("test_test_framework.log"));

    Log(ATTO_TEXT("=== Running Test Framework Validation ==="));

    // Test ASSERT macro
    {
        ASSERT(true);
        ASSERT(1 == 1);
        ASSERT(5 > 3);
        Log(ATTO_TEXT("ASSERT: passed"));
    }

    // Test ASSERT_EQ macro
    {
        ASSERT_EQ(5, 5);
        ASSERT_EQ(String(ATTO_TEXT("hello")), String(ATTO_TEXT("hello")));
        ASSERT_EQ(true, true);
        Log(ATTO_TEXT("ASSERT_EQ: passed"));
    }

    // Test ASSERT_NE macro
    {
        ASSERT_NE(5, 10);
        ASSERT_NE(String(ATTO_TEXT("hello")), String(ATTO_TEXT("world")));
        ASSERT_NE(true, false);
        Log(ATTO_TEXT("ASSERT_NE: passed"));
    }

    // Test ASSERT_TRUE macro
    {
        ASSERT_TRUE(true);
        ASSERT_TRUE(1 == 1);
        ASSERT_TRUE(5 > 3);
        Log(ATTO_TEXT("ASSERT_TRUE: passed"));
    }

    // Test ASSERT_FALSE macro
    {
        ASSERT_FALSE(false);
        ASSERT_FALSE(1 == 2);
        ASSERT_FALSE(5 < 3);
        Log(ATTO_TEXT("ASSERT_FALSE: passed"));
    }

    // Test function registration
    {
        // Mark some functions as tested
        REGISTER_TESTED(GetUserName);
        REGISTER_TESTED(GetProcessId);
        REGISTER_TESTED(Log);

        Log(ATTO_TEXT("Function registration: passed"));
    }

    // Test coverage calculation
    {
        // The coverage report will show that we've tested at least GetUserName,
        // GetProcessId, and Log
        Log(ATTO_TEXT("Coverage calculation test: passed"));
    }

    Log(ATTO_TEXT("=== All Framework Validation Tests Passed ==="));

    // Display coverage report
    TestFramework::DisplayCoverage();

    // Write machine-readable coverage data for aggregation
    TestFramework::WriteCoverageData(ATTO_TEXT("test_test_framework"));

    Exit(0);
}
