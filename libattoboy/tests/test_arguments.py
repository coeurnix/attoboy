#!/usr/bin/env python3
"""
Comprehensive test suite for the Arguments class.
Tests argument parsing, required arguments, help display, and edge cases.
"""

import subprocess
import sys
import os
from pathlib import Path

# Color codes for terminal output
GREEN = '\033[92m'
RED = '\033[91m'
YELLOW = '\033[93m'
RESET = '\033[0m'

class TestRunner:
    def __init__(self, test_exe_path):
        self.test_exe = test_exe_path
        self.passed = 0
        self.failed = 0
        self.tests_run = 0

    def run_test(self, name, args, expected_exit_code=0, should_contain=None, should_not_contain=None):
        """Run a single test case"""
        self.tests_run += 1
        print(f"  Running: {name}...", end=" ")

        try:
            # Run the test executable with the provided arguments
            result = subprocess.run(
                [self.test_exe] + args,
                capture_output=True,
                text=True,
                timeout=5
            )

            # Check exit code
            if result.returncode != expected_exit_code:
                print(f"{RED}FAIL{RESET}")
                print(f"    Expected exit code {expected_exit_code}, got {result.returncode}")
                if result.stdout:
                    print(f"    stdout: {result.stdout}")
                if result.stderr:
                    print(f"    stderr: {result.stderr}")
                self.failed += 1
                return False

            # Check output contains expected strings
            if should_contain:
                for expected in should_contain:
                    if expected not in result.stdout and expected not in result.stderr:
                        print(f"{RED}FAIL{RESET}")
                        print(f"    Expected output to contain: '{expected}'")
                        print(f"    stdout: {result.stdout}")
                        print(f"    stderr: {result.stderr}")
                        self.failed += 1
                        return False

            # Check output doesn't contain unexpected strings
            if should_not_contain:
                for unexpected in should_not_contain:
                    if unexpected in result.stdout or unexpected in result.stderr:
                        print(f"{RED}FAIL{RESET}")
                        print(f"    Expected output NOT to contain: '{unexpected}'")
                        print(f"    stdout: {result.stdout}")
                        print(f"    stderr: {result.stderr}")
                        self.failed += 1
                        return False

            print(f"{GREEN}PASS{RESET}")
            self.passed += 1
            return True

        except subprocess.TimeoutExpired:
            print(f"{RED}TIMEOUT{RESET}")
            print(f"    Test timed out after 5 seconds")
            self.failed += 1
            return False
        except Exception as e:
            print(f"{RED}ERROR{RESET}")
            print(f"    Exception: {e}")
            self.failed += 1
            return False

    def print_summary(self):
        """Print test summary"""
        print("\n" + "="*60)
        print(f"Test Results: {self.passed}/{self.tests_run} passed")
        if self.failed > 0:
            print(f"{RED}{self.failed} tests failed{RESET}")
            return 1
        else:
            print(f"{GREEN}All tests passed!{RESET}")
            return 0


def main():
    # Find the test executable
    script_dir = Path(__file__).parent
    build_dir = script_dir.parent.parent / "build" / "tests"

    test_exe = build_dir / "python_test_arguments_runner.exe"

    if not test_exe.exists():
        print(f"{RED}Error: Test executable not found at {test_exe}{RESET}")
        return 1

    print(f"Running Arguments tests with: {test_exe}")
    print("="*60)

    runner = TestRunner(str(test_exe))

    # Test 1: Basic flag parsing
    print("\n[1] Basic Flag Tests")
    runner.run_test("Flag not provided (default false)", ["-r=x"], 0)
    runner.run_test("Flag with -v", ["-v", "-r=x"], 0)
    runner.run_test("Flag with --verbose", ["--verbose", "-r=x"], 0)
    runner.run_test("Flag with -v=true", ["-v=true", "-r=x"], 0)
    runner.run_test("Flag with -v=false", ["-v=false", "-r=x"], 0)
    runner.run_test("Flag with -v=1", ["-v=1", "-r=x"], 0)
    runner.run_test("Flag with -v=0", ["-v=0", "-r=x"], 0)
    runner.run_test("Flag with -v=on", ["-v=on", "-r=x"], 0)
    runner.run_test("Flag with -v=off", ["-v=off", "-r=x"], 0)

    # Test 2: Parameter parsing
    print("\n[2] Parameter Tests")
    runner.run_test("Parameter with -o=test.txt", ["-o=test.txt", "-r=x"], 0)
    runner.run_test("Parameter with --output=test.txt", ["--output=test.txt", "-r=x"], 0)
    runner.run_test("Parameter with default value", ["-r=x"], 0)
    runner.run_test("Parameter with empty value -o=", ["-o=", "-r=x"], 0)
    runner.run_test("Parameter with spaces -o='my file.txt'", ["-o=my file.txt", "-r=x"], 0)

    # Test 3: Positional parameters
    print("\n[3] Positional Parameter Tests")
    runner.run_test("Positional parameter provided", ["-r=x", "input.txt"], 0)
    runner.run_test("Multiple positional parameters", ["-r=x", "input.txt", "output.txt"], 0)
    runner.run_test("Positional with flags", ["-v", "-r=x", "input.txt"], 0)
    runner.run_test("Mixed order: flag, param, positional", ["-v", "-o=out.txt", "-r=x", "input.txt"], 0)

    # Test 4: Required arguments
    print("\n[4] Required Argument Tests")
    runner.run_test("Required parameter missing", [], expected_exit_code=1,
                   should_contain=["Options:", "-r"])
    runner.run_test("Required parameter provided", ["-r=value"], 0)

    # Test 5: Help display
    print("\n[5] Help Display Tests")
    runner.run_test("Help with -h", ["-h"], expected_exit_code=1,
                   should_contain=["Options:", "-v"])
    runner.run_test("Help with --help", ["--help"], expected_exit_code=1,
                   should_contain=["Options:", "--verbose"])
    runner.run_test("Help shows descriptions", ["-h"], expected_exit_code=1,
                   should_contain=["Enable verbose mode"])

    # Test 7: Edge cases
    print("\n[7] Edge Case Tests")
    runner.run_test("Unknown flag ignored", ["-x", "-r=x"], 0)
    runner.run_test("Unknown long flag ignored", ["--unknown", "-r=x"], 0)
    runner.run_test("Double dash without value", ["--", "-r=x"], 0)
    runner.run_test("Single dash alone", ["-", "-r=x"], 0)
    runner.run_test("Multiple flags combined", ["-v", "-d", "-x", "-r=x"], 0)

    # Print summary
    return runner.print_summary()


if __name__ == "__main__":
    sys.exit(main())
