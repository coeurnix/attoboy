#ifndef TEST_FUNCTIONS_H
#define TEST_FUNCTIONS_H

// This file contains a registry of all functions from attoboy.h
// Each function is registered here for coverage tracking purposes

#define REGISTER_ALL_FUNCTIONS \
    X(Exit) \
    X(Sleep) \
    X(GetEnv) \
    X(SetEnv) \
    X(Alloc) \
    X(Realloc) \
    X(Free) \
    X(GetUserName) \
    X(GetUserDisplayName) \
    X(GetProcessId) \
    X(EnableLoggingToFile) \
    X(EnableLoggingToConsole) \
    X(Log) \
    X(LogDebug) \
    X(LogInfo) \
    X(LogWarning) \
    X(LogError)

// Count of all registered functions
#define FUNCTION_COUNT 17

#endif // TEST_FUNCTIONS_H
