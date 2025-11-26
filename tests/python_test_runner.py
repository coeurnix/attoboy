#!/usr/bin/env python3
"""
Python test runner for validating attoboy test executables.
Supports exit code validation, output validation, and other external checks.
"""

import sys
import subprocess
import os
from pathlib import Path

class TestResult:
    """Result of a test validation."""
    def __init__(self, passed, message=""):
        self.passed = passed
        self.message = message

    def __bool__(self):
        return self.passed


def run_executable(exe_path, expected_exit_code=None, check_output=None, timeout=10):
    """
    Run an executable and validate its behavior.

    Args:
        exe_path: Path to the executable
        expected_exit_code: If specified, validate the exit code matches
        check_output: If specified, a function that receives stdout and returns TestResult
        timeout: Timeout in seconds

    Returns:
        TestResult indicating success/failure
    """
    exe_path = Path(exe_path)
    if not exe_path.exists():
        return TestResult(False, f"Executable not found: {exe_path}")

    try:
        # Run the executable and capture output
        result = subprocess.run(
            [str(exe_path)],
            capture_output=True,
            text=True,
            timeout=timeout,
            encoding='utf-8',
            errors='replace'
        )

        # Validate exit code if specified
        if expected_exit_code is not None:
            if result.returncode != expected_exit_code:
                return TestResult(
                    False,
                    f"Exit code mismatch: expected {expected_exit_code}, got {result.returncode}"
                )

        # Validate output if specified
        if check_output is not None:
            output_result = check_output(result.stdout)
            if not output_result:
                return output_result

        return TestResult(True, "All validations passed")

    except subprocess.TimeoutExpired:
        return TestResult(False, f"Test timed out after {timeout} seconds")
    except Exception as e:
        return TestResult(False, f"Error running test: {e}")


def check_file_contains(file_path, expected_content):
    """
    Check if a file contains expected content.

    Args:
        file_path: Path to the file to check
        expected_content: String or list of strings to search for

    Returns:
        TestResult indicating success/failure
    """
    file_path = Path(file_path)
    if not file_path.exists():
        return TestResult(False, f"File not found: {file_path}")

    try:
        with open(file_path, 'r', encoding='utf-8', errors='replace') as f:
            content = f.read()

        if isinstance(expected_content, str):
            expected_content = [expected_content]

        for expected in expected_content:
            if expected not in content:
                return TestResult(False, f"Expected content not found: '{expected}'")

        return TestResult(True, f"All expected content found in {file_path.name}")

    except Exception as e:
        return TestResult(False, f"Error reading file: {e}")


def main():
    """Main entry point for the test runner."""
    if len(sys.argv) < 2:
        print("Usage: python_test_runner.py <test_name> [args...]")
        print()
        print("Available tests:")
        print("  exit <exe_path> <expected_code>")
        print("  console <exe_path> <log_file> <expected_content...>")
        return 1

    test_name = sys.argv[1]

    # Test: Exit code validation
    if test_name == "exit":
        if len(sys.argv) < 4:
            print("Usage: python_test_runner.py exit <exe_path> <expected_code>")
            return 1

        exe_path = sys.argv[2]
        expected_code = int(sys.argv[3])

        print(f"Testing exit code of {Path(exe_path).name}...")
        result = run_executable(exe_path, expected_exit_code=expected_code)

        if result:
            print(f"[PASS] {result.message}")
            return 0
        else:
            print(f"[FAIL] {result.message}")
            return 1

    # Test: Console output validation
    elif test_name == "console":
        if len(sys.argv) < 5:
            print("Usage: python_test_runner.py console <exe_path> <log_file> <expected_content...>")
            return 1

        exe_path = sys.argv[2]
        log_file = sys.argv[3]
        expected_content = sys.argv[4:]

        print(f"Testing console output of {Path(exe_path).name}...")

        # Run the executable
        result = run_executable(exe_path, expected_exit_code=0)
        if not result:
            print(f"[FAIL] {result.message}")
            return 1

        # Check the log file for expected content
        result = check_file_contains(log_file, expected_content)
        if result:
            print(f"[PASS] {result.message}")
            return 0
        else:
            print(f"[FAIL] {result.message}")
            return 1

    else:
        print(f"Unknown test: {test_name}")
        return 1


if __name__ == "__main__":
    sys.exit(main())
