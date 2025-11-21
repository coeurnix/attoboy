cmake_minimum_required(VERSION 3.15)
project(attoboy VERSION 0.0.1 LANGUAGES CXX)

# ------------------------------------------------------------------------------
# Build Options
# ------------------------------------------------------------------------------
option(ATTO_BUILD_TESTS "Build test executables" ON)
option(ATTO_BUILD_EXAMPLES "Build example executables" ON)
option(ATTO_UNICODE "Enable Unicode (wide character) support" OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------------------------------------------------------
# Global Configuration
# ------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Force Release build to ensure minimal size
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()
set(CMAKE_CONFIGURATION_TYPES "Release" CACHE STRING "" FORCE)

# Output directories - everything goes to build directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Enable Unicode (Wide Character Support) for Windows API if option is enabled
if(ATTO_UNICODE)
    add_compile_definitions(UNICODE _UNICODE)
    message(STATUS "Unicode support: ENABLED")
else()
    message(STATUS "Unicode support: DISABLED (ANSI mode)")
endif()

# ------------------------------------------------------------------------------
# Compiler & Linker Flags (Size Optimization, No STL, 32-bit, Stripped)
# ------------------------------------------------------------------------------
if(MSVC)
    # MSVC Compiler Options
    add_compile_options(
        /O1               # Optimize for size (minimize space)
        /Ob2              # Inline expansion level 2 (any suitable)
        /Os               # Favor small code
        /GL               # Whole program optimization (enables cross-module optimizations)
        /Gy               # Enable function-level linking (allows linker to remove unused functions)
        /GS-              # Disable buffer security check (removes stack canary overhead)
        /GR-              # Disable RTTI (Run-Time Type Information)
        /EHs-c-           # Disable C++ exceptions (both synchronous and asynchronous)
        /wd4530           # Suppress warning C4530 about missing exception handling
        /Gs9999999        # Disable stack probes (avoids _chkstk dependency for small stacks)
    )
    
    # MSVC Linker Options
    add_link_options(
        /MACHINE:X86       # Target 32-bit x86 architecture
        /SUBSYSTEM:CONSOLE # Console application subsystem
        /NODEFAULTLIB      # Ignore all default libraries (CRT, STL, etc.)
        /OPT:REF           # Remove unreferenced functions and data
        /OPT:ICF           # Enable identical COMDAT folding (merge duplicate code)
        /LTCG              # Link-time code generation (works with /GL)
        /ENTRY:atto_main   # Set entry point to atto_main (bypasses CRT startup)
        /DEBUG:NONE        # Do not generate debug info (reduces binary size)
    )
    
    set(SYSTEM_LIBS kernel32.lib ws2_32.lib advapi32.lib crypt32.lib)
    
else()
    # GCC/Clang Compiler Options
    add_compile_options(
        -m32                              # Generate 32-bit x86 code
        -Os                               # Optimize for size (balance of -O2 with size focus)
        -ffunction-sections               # Place each function in its own section (enables --gc-sections)
        -fdata-sections                   # Place each data item in its own section (enables --gc-sections)
        -fno-exceptions                   # Disable C++ exception handling support
        -fno-rtti                         # Disable Run-Time Type Information
        -fno-unwind-tables                # Don't generate static unwind tables (used for exceptions)
        -fno-asynchronous-unwind-tables   # Don't generate async unwind tables (used for debugging/exceptions)
    )
    
    # GCC/Clang Linker Options
    add_link_options(
        -m32                         # Link as 32-bit binary
        -s                           # Strip all symbols from binary (removes symbol table and debug info)
        -Wl,--gc-sections            # Garbage collect unused sections (removes unreferenced code/data)
        -nostdlib                    # Do not use standard system startup files or libraries
        -Wl,--entry=_atto_main       # Set entry point to atto_main (bypasses CRT startup)
        -Wl,--defsym,main=_atto_main # makes "main()" still work
    )

    set(SYSTEM_LIBS -lkernel32 -lws2_32 -ladvapi32 -lcrypt32)
endif()

# ------------------------------------------------------------------------------
# Build-Time Banner
# ------------------------------------------------------------------------------
add_custom_target(print_banner ALL
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "=========================================="
    COMMAND ${CMAKE_COMMAND} -E echo "        _   _        _                 "
    COMMAND ${CMAKE_COMMAND} -E echo "   __ _| |_| |_ ___ | |__   ___  _   _ "
    COMMAND ${CMAKE_COMMAND} -E echo "  / _\` | __| __/ _ \\| '_ \\ / _ \\| | | |"
    COMMAND ${CMAKE_COMMAND} -E echo " | (_| | |_| || (_) | |_) | (_) | |_| |"
    COMMAND ${CMAKE_COMMAND} -E echo "  \\__,_|\\__|\\__\\___/|_.__/ \\___/ \\__, |"
    COMMAND ${CMAKE_COMMAND} -E echo "                                 |___/ "
    COMMAND ${CMAKE_COMMAND} -E echo "   v${PROJECT_VERSION} by coeurnix"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "                  ..^____/"
    COMMAND ${CMAKE_COMMAND} -E echo "                 \`-. ___ )"
    COMMAND ${CMAKE_COMMAND} -E echo "                   ||  ||"
    COMMAND ${CMAKE_COMMAND} -E echo "=========================================="
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMENT "Displaying Banner"
)

# ------------------------------------------------------------------------------
# Core Library Target
# ------------------------------------------------------------------------------
message(STATUS "Building library sources...")
file(GLOB_RECURSE LIB_SOURCES "src/*.cpp")
add_library(attoboy STATIC ${LIB_SOURCES})
add_dependencies(attoboy print_banner)

# Public include directory for consumers of the library
target_include_directories(attoboy PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Copy headers to build directory for easy distribution
add_custom_command(TARGET attoboy POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_BINARY_DIR}/include"
    COMMENT "Copying headers to build/include/"
)

# ------------------------------------------------------------------------------
# Tests (Built as Exes in build/tests/)
# ------------------------------------------------------------------------------
if(ATTO_BUILD_TESTS)
    message(STATUS "Building tests...")
    file(GLOB TEST_SOURCES "tests/*.cpp")

    # List to keep track of test targets for the 'check' dependency
    set(TEST_TARGETS "")

    foreach(test_src ${TEST_SOURCES})
        get_filename_component(test_name ${test_src} NAME_WE)
        add_executable(${test_name} ${test_src})
        target_link_libraries(${test_name} PRIVATE attoboy ${SYSTEM_LIBS})
        set_target_properties(${test_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
            OUTPUT_NAME "${test_name}"
        )
        list(APPEND TEST_TARGETS ${test_name})
    endforeach()

    # Add a target to build and run all tests
    set(CHECK_SCRIPT_FILE "${CMAKE_BINARY_DIR}/run_all_tests.cmake")

    # Generate a single script that runs all tests and fails the build if any test fails
    set(SCRIPT_CONTENT "
SET(ANY_FAILED 0)
")
    foreach(test_target ${TEST_TARGETS})
        set(EXE_FILE "${CMAKE_BINARY_DIR}/tests/${test_target}${CMAKE_EXECUTABLE_SUFFIX}")
        set(SCRIPT_CONTENT "${SCRIPT_CONTENT}
EXECUTE_PROCESS(
    COMMAND cmd.exe /c \"${EXE_FILE}\"
    RESULT_VARIABLE RESULT
)
IF(NOT RESULT EQUAL 0)
    SET(ANY_FAILED 1)
    MESSAGE(\"Running ${test_target} failed! Please run ${EXE_FILE} to see detailed error output.\")
ELSE()
    MESSAGE(\"Running ${test_target}... passed.\")
ENDIF()
")
    endforeach()
    set(SCRIPT_CONTENT "${SCRIPT_CONTENT}
IF(ANY_FAILED)
    MESSAGE(FATAL_ERROR \"Some tests failed.\")
ENDIF()
")

    file(WRITE "${CHECK_SCRIPT_FILE}" "${SCRIPT_CONTENT}")

    add_custom_target(check
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMAND ${CMAKE_COMMAND} -P "${CHECK_SCRIPT_FILE}"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Building and running all tests..."
    )
    add_dependencies(check ${TEST_TARGETS})
endif()

# ------------------------------------------------------------------------------
# Examples (Built as Exes in build/examples/)
# ------------------------------------------------------------------------------
if(ATTO_BUILD_EXAMPLES)
    message(STATUS "Building examples...")
    file(GLOB EXAMPLE_SOURCES "examples/*.cpp")
    foreach(ex_src ${EXAMPLE_SOURCES})
        get_filename_component(ex_name ${ex_src} NAME_WE)
        add_executable(ex_${ex_name} ${ex_src})
        target_link_libraries(ex_${ex_name} PRIVATE attoboy ${SYSTEM_LIBS})
        set_target_properties(ex_${ex_name} PROPERTIES 
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples"
            OUTPUT_NAME "${ex_name}"
        )
    endforeach()
endif()

# ------------------------------------------------------------------------------
# Install Targets (Optional - for system-wide installation)
# ------------------------------------------------------------------------------
install(TARGETS attoboy
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)


# ------------------------------------------------------------------------------
# Build Configuration Summary
# ------------------------------------------------------------------------------
message(STATUS "========================================")
message(STATUS "attoboy Build Configuration")
message(STATUS "========================================")
message(STATUS "Version:           ${PROJECT_VERSION}")
message(STATUS "Build Type:        ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard:      ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler:          ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Unicode Mode:      ${ATTO_UNICODE}")
message(STATUS "Build Tests:       ${ATTO_BUILD_TESTS}")
message(STATUS "Build Examples:    ${ATTO_BUILD_EXAMPLES}")
message(STATUS "")
message(STATUS "Output Directories:")
message(STATUS "  Library:         ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
message(STATUS "  Headers:         ${CMAKE_BINARY_DIR}/include")
message(STATUS "  Tests:           ${CMAKE_BINARY_DIR}/tests")
message(STATUS "  Examples:        ${CMAKE_BINARY_DIR}/examples")
message(STATUS "========================================")
message(STATUS "")
