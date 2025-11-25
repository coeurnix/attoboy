#include "test_framework.h"

void atto_main() {
    // Enable file logging for reliable output capture
    EnableLoggingToFile(ATTO_TEXT("test_util_functions.log"), true);

    Log(ATTO_TEXT("=== Running Utility Functions Tests ==="));

    // Test GetUserName
    {
        String username = GetUserName();
        REGISTER_TESTED(GetUserName);
        ASSERT_FALSE(username.isEmpty());
        Log(ATTO_TEXT("GetUserName: "), username);
    }

    // Test GetUserDisplayName
    {
        String displayName = GetUserDisplayName();
        REGISTER_TESTED(GetUserDisplayName);
        // Display name might be empty on some systems
        Log(ATTO_TEXT("GetUserDisplayName: "), displayName);
    }

    // Test GetProcessId
    {
        int pid = GetProcessId();
        REGISTER_TESTED(GetProcessId);
        ASSERT(pid > 0);
        Log(ATTO_TEXT("GetProcessId: "), pid);
    }

    // Test GetEnv and SetEnv
    {
        String testKey = ATTO_TEXT("ATTOBOY_TEST_VAR");
        String testValue = ATTO_TEXT("test_value_123");

        // Set environment variable
        bool setResult = SetEnv(testKey, testValue);
        REGISTER_TESTED(SetEnv);
        ASSERT_TRUE(setResult);

        // Get environment variable
        String retrievedValue = GetEnv(testKey);
        REGISTER_TESTED(GetEnv);
        ASSERT_EQ(retrievedValue, testValue);

        Log(ATTO_TEXT("SetEnv/GetEnv: passed"));
    }

    // Test Sleep
    {
        REGISTER_TESTED(Sleep);
        Sleep(10); // Sleep for 10ms
        Log(ATTO_TEXT("Sleep: passed"));
    }

    // Test memory allocation functions
    {
        // Test Alloc
        int size = 1024;
        void* ptr = Alloc(size);
        REGISTER_TESTED(Alloc);
        ASSERT(ptr != nullptr);

        // Write some data to verify it's valid
        for (int i = 0; i < size; ++i) {
            ((unsigned char*)ptr)[i] = (unsigned char)(i % 256);
        }

        // Test Realloc
        int newSize = 2048;
        void* newPtr = Realloc(ptr, newSize);
        REGISTER_TESTED(Realloc);
        ASSERT(newPtr != nullptr);

        // Verify original data is preserved
        for (int i = 0; i < size; ++i) {
            ASSERT_EQ(((unsigned char*)newPtr)[i], (unsigned char)(i % 256));
        }

        // Test Free
        Free(newPtr);
        REGISTER_TESTED(Free);

        Log(ATTO_TEXT("Alloc/Realloc/Free: passed"));
    }

    // Test logging functions
    {
        REGISTER_TESTED(Log);
        REGISTER_TESTED(LogDebug);
        REGISTER_TESTED(LogInfo);
        REGISTER_TESTED(LogWarning);
        REGISTER_TESTED(LogError);

        Log(ATTO_TEXT("Log: testing basic logging"));
        LogDebug(ATTO_TEXT("LogDebug: debug message"));
        LogInfo(ATTO_TEXT("LogInfo: info message"));
        LogWarning(ATTO_TEXT("LogWarning: warning message"));
        LogError(ATTO_TEXT("LogError: error message"));
    }

    // Test EnableLoggingToFile (already enabled at start)
    {
        REGISTER_TESTED(EnableLoggingToFile);
        Log(ATTO_TEXT("EnableLoggingToFile: passed (using file logging)"));
    }

    // Note: EnableLoggingToConsole is not tested to avoid console output issues
    // REGISTER_TESTED(EnableLoggingToConsole);

    Log(ATTO_TEXT("=== All Utility Function Tests Passed ==="));

    // Display coverage report
    TestFramework::DisplayCoverage();

    // Write machine-readable coverage data for aggregation
    TestFramework::WriteCoverageData(ATTO_TEXT("test_util_functions"));

    Exit(0);
}
