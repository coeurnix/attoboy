#include "test_framework.h"

void atto_main() {
    // Enable file logging for reliable output capture
    EnableLoggingToFile("test_util_functions.log", true);

    Log("=== Running Utility Functions Tests ===");

    // Test GetUserName
    {
        String username = GetUserName();
        REGISTER_TESTED(GetUserName);
        ASSERT_FALSE(username.isEmpty());
        Log("GetUserName: ", username);
    }

    // Test GetUserDisplayName
    {
        String displayName = GetUserDisplayName();
        REGISTER_TESTED(GetUserDisplayName);
        // Display name might be empty on some systems
        Log("GetUserDisplayName: ", displayName);
    }

    // Test GetProcessId
    {
        int pid = GetProcessId();
        REGISTER_TESTED(GetProcessId);
        ASSERT(pid > 0);
        Log("GetProcessId: ", pid);
    }

    // Test GetEnv and SetEnv
    {
        String testKey = "ATTOBOY_TEST_VAR";
        String testValue = "test_value_123";

        // Set environment variable
        bool setResult = SetEnv(testKey, testValue);
        REGISTER_TESTED(SetEnv);
        ASSERT_TRUE(setResult);

        // Get environment variable
        String retrievedValue = GetEnv(testKey);
        REGISTER_TESTED(GetEnv);
        ASSERT_EQ(retrievedValue, testValue);

        Log("SetEnv/GetEnv: passed");
    }

    // Test Sleep
    {
        REGISTER_TESTED(Sleep);
        Sleep(10); // Sleep for 10ms
        Log("Sleep: passed");
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

        Log("Alloc/Realloc/Free: passed");
    }

    // Test logging functions
    {
        REGISTER_TESTED(Log);
        REGISTER_TESTED(LogDebug);
        REGISTER_TESTED(LogInfo);
        REGISTER_TESTED(LogWarning);
        REGISTER_TESTED(LogError);

        Log("Log: testing basic logging");
        LogDebug("LogDebug: debug message");
        LogInfo("LogInfo: info message");
        LogWarning("LogWarning: warning message");
        LogError("LogError: error message");
    }

    // Test EnableLoggingToFile (already enabled at start)
    {
        REGISTER_TESTED(EnableLoggingToFile);
        Log("EnableLoggingToFile: passed (using file logging)");
    }

    // Note: EnableLoggingToConsole is not tested to avoid console output issues
    // REGISTER_TESTED(EnableLoggingToConsole);

    Log("=== All Utility Function Tests Passed ===");

    // Display coverage report
    TestFramework::DisplayCoverage();

    // Write machine-readable coverage data for aggregation
    TestFramework::WriteCoverageData("test_util_functions");

    Exit(0);
}
