cmake_minimum_required(VERSION 3.15)
project(attoboy VERSION 0.1.0 LANGUAGES C CXX)

# ------------------------------------------------------------------------------
# Build Options
# ------------------------------------------------------------------------------
option(ATTO_BUILD_TESTS "Build test executables" ON)
option(ATTO_BUILD_EXAMPLES "Build example executables" ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------------------------------------------------------
# Enable testing at the root level for CTest
# ------------------------------------------------------------------------------
if(ATTO_BUILD_TESTS)
    enable_testing()
endif()

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


# Prevent Windows headers from defining min/max macros that conflict with our functions
add_compile_definitions(NOMINMAX)

# ------------------------------------------------------------------------------
# Compiler & Linker Flags (Size Optimization, No STL, 32-bit, Stripped)
# ------------------------------------------------------------------------------
if(MSVC)
    # MSVC Compiler Options
    string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REPLACE "/EHa"  "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    add_compile_options(
        /utf-8            # Source files are UTF-8 encoded
        /O1               # Optimize for size (minimize space)
        /Ob2              # Inline expansion level 2 (any suitable)
        /Os               # Favor small code
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
        /STACK:1048576     # Set stack size to 1MB
        /SUBSYSTEM:CONSOLE # Console application subsystem
        /NODEFAULTLIB      # Ignore all default libraries (CRT, STL, etc.)
        /OPT:REF           # Remove unreferenced functions and data
        /OPT:ICF           # Enable identical COMDAT folding (merge duplicate code)
        /ENTRY:atto_main   # Set entry point to atto_main (bypasses CRT startup)
        /DEBUG:NONE        # Do not generate debug info (reduces binary size)
    )

    set(SYSTEM_LIBS kernel32.lib user32.lib ws2_32.lib advapi32.lib crypt32.lib secur32.lib shell32.lib winhttp.lib)

else()
    # GCC/Clang Compiler Options
    add_compile_options(
        -m32                              # Generate 32-bit x86 code
        -finput-charset=UTF-8             # Source files are UTF-8 encoded
        -fexec-charset=UTF-8              # String literals are UTF-8 encoded
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
        -Wl,--stack,1048576          # Set stack size to 1MB
    )

    set(SYSTEM_LIBS -lkernel32 -luser32 -lws2_32 -ladvapi32 -lcrypt32 -lsecur32 -lshell32 -lwinhttp)
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
# Distribution Target
# ------------------------------------------------------------------------------
if(MSVC)
    set(LIB_PATH lib/Release/attoboy.lib)
else()
    set(LIB_PATH lib/libattoboy.a)
endif()

add_custom_target(dist ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/dist/include/attoboy"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/dist/lib"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/include/attoboy/attoboy.h" "${CMAKE_BINARY_DIR}/dist/include/attoboy/"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/${LIB_PATH}" "${CMAKE_BINARY_DIR}/dist/lib/"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/scripts/attobuild.bat" "${CMAKE_BINARY_DIR}/dist/"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/dist/docs"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/docs/index.html" "${CMAKE_BINARY_DIR}/dist/docs"
    DEPENDS attoboy
    COMMENT "Creating distribution package in build/dist/"
)

# ------------------------------------------------------------------------------
# Tests (Built as Exes in build/tests/ with CTest)
# ------------------------------------------------------------------------------
if(ATTO_BUILD_TESTS)
    message(STATUS "Building tests with CTest integration...")

    file(GLOB TEST_SOURCES "tests/*.cpp")

    # Find Python for cooperative tests
    find_program(PYTHON_EXECUTABLE
        NAMES python3 python py
        DOC "Python interpreter for cooperative tests"
    )

    # Add test executables matching the current build configuration
    foreach(test_src ${TEST_SOURCES})
        get_filename_component(test_name ${test_src} NAME_WE)

        # Build the executable
        add_executable(${test_name} ${test_src})
        if(NOT MSVC)
            target_compile_options(${test_name} PRIVATE -fno-exceptions -fno-rtti)
        endif()
        target_link_libraries(${test_name} PRIVATE attoboy ${SYSTEM_LIBS})
        target_include_directories(${test_name} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/tests")
        set_target_properties(${test_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
            OUTPUT_NAME "${test_name}"
        )

        # Determine how to run the test
        if(test_name STREQUAL "test_exit" AND PYTHON_EXECUTABLE)
            # Python-cooperative test: validate exit code
            add_test(NAME ${test_name}
                COMMAND ${PYTHON_EXECUTABLE}
                    "${CMAKE_CURRENT_SOURCE_DIR}/tests/python_test_runner.py"
                    exit
                    "$<TARGET_FILE:${test_name}>"
                    42
                WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
            )
        elseif(test_name STREQUAL "test_logging_console" AND PYTHON_EXECUTABLE)
            # Python-cooperative test: validate console logging behavior
            add_test(NAME ${test_name}
                COMMAND ${PYTHON_EXECUTABLE}
                    "${CMAKE_CURRENT_SOURCE_DIR}/tests/python_test_runner.py"
                    console
                    "$<TARGET_FILE:${test_name}>"
                    "${CMAKE_BINARY_DIR}/test_logging_console_result.log"
                    "EnableLoggingToConsole test completed successfully"
                WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
            )
        else()
            # Standard CTest: run executable directly
            add_test(NAME ${test_name} COMMAND ${test_name})

            # Label tests that require live API access
            if(test_name MATCHES "^test_(ai|webrequest)_(comprehensive|simple|verbose|minimal|live)$")
                set_tests_properties(${test_name} PROPERTIES LABELS "live_api")
            endif()
        endif()
    endforeach()

    message(STATUS "CTest enabled (UTF-8 mode) - use 'ctest --output-on-failure' to run tests")

    # Add custom 'coverage' target that runs CTest and shows aggregated coverage
    if(PYTHON_EXECUTABLE)
        add_custom_target(coverage
            COMMAND ${PYTHON_EXECUTABLE} -c "import pathlib; [f.unlink() for f in pathlib.Path('${CMAKE_BINARY_DIR}').glob('*_coverage.txt')]"
            COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/tests/run_tests_with_mock.py"
                ${CMAKE_CTEST_COMMAND} --build-config Release --output-on-failure --label-exclude live_api
            COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/tests/aggregate_coverage.py" "${CMAKE_BINARY_DIR}"
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Running tests with mock server (excluding live API tests) and aggregating coverage..."
            USES_TERMINAL
        )
        add_custom_target(coverage_live
            COMMAND ${PYTHON_EXECUTABLE} -c "import pathlib; [f.unlink() for f in pathlib.Path('${CMAKE_BINARY_DIR}').glob('*_coverage.txt')]"
            COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/tests/run_tests_with_mock.py"
                ${CMAKE_CTEST_COMMAND} --build-config Release --output-on-failure
            COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/tests/aggregate_coverage.py" "${CMAKE_BINARY_DIR}"
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Running ALL tests with mock server (including live API) and aggregating coverage..."
            USES_TERMINAL
        )
        message(STATUS "Custom 'coverage' target enabled - use 'cmake --build build --target coverage'")
        message(STATUS "  (Automatically starts/stops mock server for AI/WebRequest tests)")
        message(STATUS "Custom 'coverage_live' target for live API tests - use 'cmake --build build --target coverage_live'")
        message(STATUS "  (Requires OPENAI_API_KEY environment variable)")
    else()
        message(STATUS "Python not found - coverage aggregation will be skipped")
    endif()
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

        # Special handling for GUI example
        if(ex_name STREQUAL "example_gui_chat")
            if(MSVC)
                target_link_options(ex_${ex_name} PRIVATE /SUBSYSTEM:WINDOWS)
                target_link_libraries(ex_${ex_name} PRIVATE comctl32.lib gdi32.lib)
            else()
                target_link_options(ex_${ex_name} PRIVATE -mwindows)
                target_link_libraries(ex_${ex_name} PRIVATE -lcomctl32 -lgdi32)
            endif()
        endif()

        # Special handling for DirectX example (requires CRT and standard libraries)
        if(ex_name STREQUAL "example_directx")
            if(MSVC)
                # Override /NODEFAULTLIB to allow CRT linking
                target_link_options(ex_${ex_name} PRIVATE
                    /SUBSYSTEM:CONSOLE
                    /ENTRY:mainCRTStartup
                )
                # Remove /NODEFAULTLIB for this target
                get_target_property(LINK_OPTIONS ex_${ex_name} LINK_OPTIONS)
                list(REMOVE_ITEM LINK_OPTIONS "/NODEFAULTLIB")
                list(REMOVE_ITEM LINK_OPTIONS "/ENTRY:atto_main")
                set_target_properties(ex_${ex_name} PROPERTIES LINK_OPTIONS "${LINK_OPTIONS}")
                # Add DirectX libraries
                target_link_libraries(ex_${ex_name} PRIVATE d3d11.lib d3dcompiler.lib dxgi.lib)
            else()
                # Override -nostdlib to allow CRT linking
                target_link_options(ex_${ex_name} PRIVATE -mconsole)
                # Remove -nostdlib and custom entry point for this target
                get_target_property(LINK_OPTIONS ex_${ex_name} LINK_OPTIONS)
                list(REMOVE_ITEM LINK_OPTIONS "-nostdlib")
                list(REMOVE_ITEM LINK_OPTIONS "-Wl,--entry=_atto_main")
                list(REMOVE_ITEM LINK_OPTIONS "-Wl,--defsym,main=_atto_main")
                set_target_properties(ex_${ex_name} PROPERTIES LINK_OPTIONS "${LINK_OPTIONS}")
                # Add DirectX libraries and standard libs
                target_link_libraries(ex_${ex_name} PRIVATE -ld3d11 -ld3dcompiler -ldxgi)
            endif()
        endif()
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
message(STATUS "Encoding:          UTF-8")
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
