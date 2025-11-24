#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <attoboy/attoboy.h>
#include "test_functions.h"

#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef GetUserName  // Avoid conflict with attoboy::GetUserName

using namespace attoboy;

// Global tracking of tested functions
namespace TestFramework {
    struct FunctionRegistry {
        const ATTO_CHAR* name;
        bool tested;
    };

    // Initialize registry with all functions
    static FunctionRegistry g_functions[FUNCTION_COUNT] = {
        #define X(name) { ATTO_TEXT(#name), false },
        REGISTER_ALL_FUNCTIONS
        #undef X
    };

    // Mark a function as tested
    inline void MarkTested(const ATTO_CHAR* name) {
        for (int i = 0; i < FUNCTION_COUNT; ++i) {
            // Simple string comparison
            const ATTO_CHAR* a = g_functions[i].name;
            const ATTO_CHAR* b = name;
            bool match = true;
            while (*a && *b) {
                if (*a != *b) {
                    match = false;
                    break;
                }
                ++a;
                ++b;
            }
            if (match && *a == *b) {
                g_functions[i].tested = true;
                return;
            }
        }
    }

    // Calculate and display coverage
    inline void DisplayCoverage() {
        int tested_count = 0;
        List untested;

        for (int i = 0; i < FUNCTION_COUNT; ++i) {
            if (g_functions[i].tested) {
                tested_count++;
            } else {
                untested.append(String(g_functions[i].name));
            }
        }

        float percentage = (static_cast<float>(tested_count) / FUNCTION_COUNT) * 100.0f;

        Log(ATTO_TEXT(""));
        Log(ATTO_TEXT("=== Test Coverage Report ==="));
        String coverage_msg(
            ATTO_TEXT("Coverage: "),
            tested_count,
            ATTO_TEXT("/"),
            FUNCTION_COUNT,
            ATTO_TEXT(" functions ("),
            percentage,
            ATTO_TEXT("%)"));
        Log(coverage_msg);

        if (untested.length() > 0) {
            String untested_list = ATTO_TEXT(" - Untested: ");
            for (int i = 0; i < untested.length(); ++i) {
                untested_list = untested_list + untested.at<String>(i);
                if (i < untested.length() - 1) {
                    untested_list = untested_list + String(ATTO_TEXT(", "));
                }
            }
            Log(untested_list);
        } else {
            Log(ATTO_TEXT(" - All functions tested!"));
        }
        Log(ATTO_TEXT("============================"));
        Log(ATTO_TEXT(""));
    }

    // Write coverage data in machine-readable format for aggregation
    inline void WriteCoverageData(const String& test_name) {
        // Build list of tested functions
        List tested_funcs;
        for (int i = 0; i < FUNCTION_COUNT; ++i) {
            if (g_functions[i].tested) {
                tested_funcs.append(String(g_functions[i].name));
            }
        }

        // Create coverage data file
        String filename = String(test_name, ATTO_TEXT("_coverage.txt"));

        // Format: TESTED_COUNT TOTAL_COUNT function1,function2,function3
        String data = String(tested_funcs.length(), ATTO_TEXT(" "), FUNCTION_COUNT);
        if (tested_funcs.length() > 0) {
            data = data + String(ATTO_TEXT(" "));
            for (int i = 0; i < tested_funcs.length(); ++i) {
                data = data + tested_funcs.at<String>(i);
                if (i < tested_funcs.length() - 1) {
                    data = data + String(ATTO_TEXT(","));
                }
            }
        }
        data = data + String(ATTO_TEXT("\n"));

        // Write coverage file - now with automatic flushing
        Path coverage_path(filename);
        coverage_path.writeFromString(data);
    }
}

// Macro to register that a function has been tested
#define REGISTER_TESTED(function_name) \
    TestFramework::MarkTested(ATTO_TEXT(#function_name))

// Test assertion macros
#define ASSERT(condition) \
    do { \
        if (!(condition)) { \
            LogError(ATTO_TEXT("ASSERTION FAILED: "), ATTO_TEXT(#condition), \
                     ATTO_TEXT(" at "), __FILE__, ATTO_TEXT(":"), __LINE__); \
            Exit(1); \
        } \
    } while(0)

#define ASSERT_EQ(actual, expected) \
    do { \
        auto _actual = (actual); \
        auto _expected = (expected); \
        if (_actual != _expected) { \
            LogError(ATTO_TEXT("ASSERTION FAILED: "), ATTO_TEXT(#actual), \
                     ATTO_TEXT(" == "), ATTO_TEXT(#expected)); \
            LogError(ATTO_TEXT("  Expected: "), _expected); \
            LogError(ATTO_TEXT("  Actual:   "), _actual); \
            LogError(ATTO_TEXT("  at "), __FILE__, ATTO_TEXT(":"), __LINE__); \
            Exit(1); \
        } \
    } while(0)

#define ASSERT_NE(actual, expected) \
    do { \
        auto _actual = (actual); \
        auto _expected = (expected); \
        if (_actual == _expected) { \
            LogError(ATTO_TEXT("ASSERTION FAILED: "), ATTO_TEXT(#actual), \
                     ATTO_TEXT(" != "), ATTO_TEXT(#expected)); \
            LogError(ATTO_TEXT("  Both values: "), _actual); \
            LogError(ATTO_TEXT("  at "), __FILE__, ATTO_TEXT(":"), __LINE__); \
            Exit(1); \
        } \
    } while(0)

#define ASSERT_TRUE(condition) ASSERT(condition)
#define ASSERT_FALSE(condition) ASSERT(!(condition))

// Python validation macro (graceful fallback when Python unavailable)
#ifdef PYTHON_VALIDATION_AVAILABLE
    #define PYTHON_EXPECTED(function_name, ...) \
        // TODO: Implement Python validation
#else
    #define PYTHON_EXPECTED(function_name, ...)
#endif

#endif // TEST_FRAMEWORK_H
