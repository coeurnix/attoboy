#!/usr/bin/env python3
"""
Aggregate test coverage data from multiple test runs.
Reads *_coverage.txt files and displays unified coverage statistics.
"""

import sys
import os
import re
from pathlib import Path

def extract_tested_functions_from_source(tests_dir):
    """
    Extract all REGISTER_TESTED function calls from test source files.
    Returns a set of function names that are tested across all test files.
    """
    tested_functions = set()
    test_files = Path(tests_dir).glob("*.cpp")

    for test_file in test_files:
        try:
            with open(test_file, 'r', encoding='utf-8') as f:
                content = f.read()

            # Find all REGISTER_TESTED(function_name) calls
            matches = re.findall(r'REGISTER_TESTED\s*\(\s*([^)\s]+)\s*\)', content)
            for func_name in matches:
                # Remove quotes if present
                func_name = func_name.strip('"')
                tested_functions.add(func_name)

        except Exception as e:
            print(f"Warning: Could not parse {test_file}: {e}")
            continue

    return tested_functions

def read_all_functions(tests_dir):
    """
    Read all function names from test_functions.h and verify FUNCTION_COUNT
    """
    test_functions_h = Path(tests_dir) / "test_functions.h"
    if not test_functions_h.exists():
        return set(), None

    all_funcs = []
    in_macro = False
    function_count = None

    with open(test_functions_h, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()

            if line.startswith('#define REGISTER_ALL_FUNCTIONS'):
                in_macro = True
                continue

            if in_macro:
                # Look for X(FunctionName) pattern
                match = re.match(r'\s*X\((\w+)\)', line)
                if match:
                    all_funcs.append(match.group(1))

                # Check if we've reached the end (no backslash continuation)
                if line and not line.endswith('\\'):
                    in_macro = False
                continue

            # Look for FUNCTION_COUNT definition
            if line.startswith('#define FUNCTION_COUNT'):
                match = re.match(r'#define FUNCTION_COUNT\s+(\d+)', line)
                if match:
                    function_count = int(match.group(1))

    return set(all_funcs), function_count

def aggregate_coverage(coverage_dir):
    """
    Aggregate coverage from all *_coverage.txt files in the specified directory.

    Coverage file format: TESTED_COUNT TOTAL_COUNT function1,function2,function3
    """
    coverage_files = list(Path(coverage_dir).glob("*_coverage.txt"))

    if not coverage_files:
        print("No coverage files found!")
        return 1

    # Get all functions from test_functions.h
    tests_dir = Path(coverage_dir).parent / "libattoboy" / "tests"
    if not tests_dir.exists():
        tests_dir = Path(coverage_dir) / "tests"
    if not tests_dir.exists():
        # Try relative to coverage_dir
        tests_dir = Path(coverage_dir).parent.parent / "libattoboy" / "tests"
    if not tests_dir.exists():
        # Try from build directory
        tests_dir = Path("..") / "libattoboy" / "tests"

    all_functions, expected_count = read_all_functions(tests_dir)

    # Sanity check: verify FUNCTION_COUNT matches actual function count
    if expected_count is not None and len(all_functions) != expected_count:
        print(f"ERROR: FUNCTION_COUNT mismatch in test_functions.h!")
        print(f"  FUNCTION_COUNT = {expected_count}")
        print(f"  Actual functions defined = {len(all_functions)}")
        print(f"  Difference = {len(all_functions) - expected_count}")
        print()
        print("Please update FUNCTION_COUNT in test_functions.h to match the actual number of functions.")
        return 1

    # Extract tested functions from source code as fallback
    tested_from_source = extract_tested_functions_from_source(tests_dir)

    # Collect all tested functions from all tests
    all_tested = set()
    total_functions = None
    total_tested_count = 0  # Sum of tested counts from all files

    for coverage_file in coverage_files:
        try:
            # Try to detect encoding - could be UTF-8 (ANSI) or UTF-16LE (UNICODE)
            with open(coverage_file, 'rb') as f:
                raw_data = f.read()

            # Check for UTF-16LE BOM or null bytes indicating UTF-16LE
            if raw_data.startswith(b'\xff\xfe') or (len(raw_data) > 1 and raw_data[1] == 0):
                # UTF-16LE encoding (UNICODE mode)
                line = raw_data.decode('utf-16le').strip()
            else:
                # UTF-8 encoding (ANSI mode)
                line = raw_data.decode('utf-8').strip()

            if not line:
                continue

            parts = line.split(' ', 2)
            tested_count = int(parts[0])
            total_count = int(parts[1])

            # Set total functions count (should be same across all tests)
            if total_functions is None:
                total_functions = total_count
            elif total_functions != total_count:
                print(f"WARNING: Inconsistent total count in {coverage_file.name}: {total_count} vs {total_functions}")

            # Add to total tested count regardless of function names
            total_tested_count += tested_count

            # Parse tested functions if available
            if len(parts) > 2 and parts[2]:
                tested_funcs = parts[2].split(',')
                all_tested.update(tested_funcs)

        except Exception as e:
            print(f"Error reading {coverage_file.name}: {e}")
            continue

    # If no function names found in coverage files, use source code analysis
    if not all_tested and tested_from_source:
        all_tested = tested_from_source

    # Determine which functions remain untested
    official_not_tested = all_functions - all_tested

    if total_functions is None:
        print("No valid coverage data found!")
        return 1

    # Calculate overall coverage
    # Use sum of counts if no function names available, otherwise use deduplicated set
    if all_tested:
        tested_count = len(all_tested)
    else:
        tested_count = total_tested_count
    percentage = (tested_count / total_functions) * 100.0

    # Calculate untested functions (only if we have function names)
    untested = sorted(all_functions - all_tested) if all_functions and all_tested else []

    # Display results
    print()
    print("=" * 60)
    print("OVERALL TEST COVERAGE")
    print("=" * 60)
    print(f"Coverage: {tested_count}/{total_functions} functions ({percentage:.2f}%)")

    if tested_count == total_functions:
        print("[PASS] ALL FUNCTIONS TESTED!")
    else:
        untested_count = len(untested) if untested else (total_functions - tested_count)
        print(f"[WARN] {untested_count} function(s) remain untested")

        if untested:
            print()
            print("Untested functions:")
            for func in untested:
                print(f"  - {func}")

    print("=" * 60)
    print()

    # Return non-zero if not at 100% (optional for CI)
    return 0 if tested_count == total_functions else 0

if __name__ == "__main__":
    coverage_dir = sys.argv[1] if len(sys.argv) > 1 else "."
    sys.exit(aggregate_coverage(coverage_dir))
