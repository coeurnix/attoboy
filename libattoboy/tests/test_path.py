#!/usr/bin/env python3
"""
Comprehensive test suite for the Path class.
Tests file I/O, path operations, attributes, and directory management.
"""

import subprocess
import sys
import os
import tempfile
import shutil
from pathlib import Path as PyPath

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
        self.temp_dir = None

    def setup(self):
        """Set up temporary directory for testing"""
        self.temp_dir = tempfile.mkdtemp(prefix="attoboy_path_test_")
        print(f"Using temporary directory: {self.temp_dir}")

    def cleanup(self):
        """Clean up temporary directory"""
        if self.temp_dir and os.path.exists(self.temp_dir):
            try:
                shutil.rmtree(self.temp_dir)
            except:
                pass

    def run_test(self, name, args, expected_exit_code=0, should_contain=None, should_not_contain=None):
        """Run a single test case"""
        self.tests_run += 1
        print(f"  Running: {name}...", end=" ")

        try:
            result = subprocess.run(
                [self.test_exe] + args,
                capture_output=True,
                text=True,
                timeout=5
            )

            if result.returncode != expected_exit_code:
                print(f"{RED}FAIL{RESET}")
                print(f"    Expected exit code {expected_exit_code}, got {result.returncode}")
                if result.stdout:
                    print(f"    stdout: {result.stdout}")
                if result.stderr:
                    print(f"    stderr: {result.stderr}")
                self.failed += 1
                return False

            if should_contain:
                for expected in should_contain:
                    if expected not in result.stdout and expected not in result.stderr:
                        print(f"{RED}FAIL{RESET}")
                        print(f"    Expected output to contain: '{expected}'")
                        print(f"    stdout: {result.stdout}")
                        print(f"    stderr: {result.stderr}")
                        self.failed += 1
                        return False

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

    def get_temp_path(self, filename):
        """Get a temporary file path"""
        return os.path.join(self.temp_dir, filename)

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
    script_dir = PyPath(__file__).parent
    build_dir = script_dir.parent.parent / "build" / "tests"

    test_exe = build_dir / "python_test_path_runner.exe"

    if not test_exe.exists():
        print(f"{RED}Error: Test executable not found at {test_exe}{RESET}")
        return 1

    print(f"Running Path tests with: {test_exe}")
    print("="*60)

    runner = TestRunner(str(test_exe))
    runner.setup()

    try:
        # Test 1: Temporary file/directory creation
        print("\n[1] Temporary File/Directory Creation Tests")
        runner.run_test("Create temporary file", ["-test=create_temp_file"], 0)
        runner.run_test("Create temporary directory", ["-test=create_temp_dir"], 0)

        # Test 2: File I/O operations
        print("\n[2] File I/O Tests")
        test_file = runner.get_temp_path("test.txt")
        runner.run_test("Write and read file",
                       ["-test=write_read", f"-path1={test_file}", "-content=Hello World"],
                       0, should_contain=["Hello World"])

        test_file2 = runner.get_temp_path("test2.txt")
        runner.run_test("Append to file",
                       ["-test=append", f"-path1={test_file2}", "-content=Content"],
                       0, should_contain=["Initial Content"])

        # Test 3: Path existence checks
        print("\n[3] Path Existence Tests")
        runner.run_test("File exists", ["-test=exists", f"-path1={test_file}"], 0)
        runner.run_test("File doesn't exist",
                       ["-test=not_exists", f"-path1={runner.get_temp_path('nonexistent.txt')}"], 0)

        # Test 4: Path type checks
        print("\n[4] Path Type Tests")
        runner.run_test("Is regular file", ["-test=is_file", f"-path1={test_file}"], 0)
        runner.run_test("Is directory", ["-test=is_directory", f"-path1={runner.temp_dir}"], 0)

        # Test 5: Path queries
        print("\n[5] Path Query Tests")
        runner.run_test("Get file size", ["-test=get_size", f"-path1={test_file}"], 0)
        runner.run_test("Get name", ["-test=get_name", f"-path1={test_file}"], 0, should_contain=["test.txt"])
        runner.run_test("Get extension", ["-test=get_extension", f"-path1={test_file}"], 0, should_contain=["txt"])
        runner.run_test("Get stem", ["-test=get_stem", f"-path1={test_file}"], 0, should_contain=["test"])
        runner.run_test("Has extension .txt", ["-test=has_extension", f"-path1={test_file}", "-path2=txt"], 0)

        # Test with different extensions
        pdf_file = runner.get_temp_path("document.pdf")
        open(pdf_file, 'w').close()
        runner.run_test("Get extension .pdf", ["-test=get_extension", f"-path1={pdf_file}"], 0, should_contain=["pdf"])
        runner.run_test("Get stem for document.pdf", ["-test=get_stem", f"-path1={pdf_file}"], 0, should_contain=["document"])
        runner.run_test("Has extension .pdf", ["-test=has_extension", f"-path1={pdf_file}", "-path2=pdf"], 0)

        # Test 6: Path comparison
        print("\n[6] Path Comparison Tests")
        test_path1 = runner.get_temp_path("file.txt")
        test_path2 = runner.get_temp_path("FILE.TXT")
        test_path3 = runner.get_temp_path("other.txt")
        runner.run_test("Paths equal (case-insensitive)",
                       ["-test=equals", f"-path1={test_path1}", f"-path2={test_path2}"], 0)
        runner.run_test("Paths not equal",
                       ["-test=not_equals", f"-path1={test_path1}", f"-path2={test_path3}"], 0)

        # Test 7: File operations
        print("\n[7] File Operations Tests")
        source_file = runner.get_temp_path("source.txt")
        with open(source_file, 'w') as f:
            f.write("Test content")

        copy_dest = runner.get_temp_path("copy.txt")
        runner.run_test("Copy file", ["-test=copy", f"-path1={source_file}", f"-path2={copy_dest}"], 0)
        runner.run_test("Copied file exists", ["-test=exists", f"-path1={copy_dest}"], 0)

        move_src = runner.get_temp_path("move_src.txt")
        move_dest = runner.get_temp_path("move_dest.txt")
        with open(move_src, 'w') as f:
            f.write("Move test")
        runner.run_test("Move file", ["-test=move", f"-path1={move_src}", f"-path2={move_dest}"], 0)
        runner.run_test("Moved file exists at destination", ["-test=exists", f"-path1={move_dest}"], 0)
        runner.run_test("Source file doesn't exist after move",
                       ["-test=not_exists", f"-path1={move_src}"], 0)

        delete_file = runner.get_temp_path("delete.txt")
        with open(delete_file, 'w') as f:
            f.write("Delete me")
        runner.run_test("Delete file", ["-test=delete_file", f"-path1={delete_file}"], 0)
        runner.run_test("Deleted file doesn't exist", ["-test=not_exists", f"-path1={delete_file}"], 0)

        # Test 8: Directory operations
        print("\n[8] Directory Operations Tests")
        new_dir = runner.get_temp_path("newdir")
        runner.run_test("Create directory", ["-test=make_directory", f"-path1={new_dir}"], 0)
        runner.run_test("Created directory exists", ["-test=exists", f"-path1={new_dir}"], 0)
        runner.run_test("Created directory is directory", ["-test=is_directory", f"-path1={new_dir}"], 0)

        deep_dir = runner.get_temp_path("deep/nested/dir")
        runner.run_test("Create nested directories", ["-test=make_directory", f"-path1={deep_dir}"], 0)
        runner.run_test("Deep directory exists", ["-test=exists", f"-path1={deep_dir}"], 0)

        empty_dir = runner.get_temp_path("emptydir")
        os.makedirs(empty_dir)
        runner.run_test("Remove empty directory", ["-test=remove_directory", f"-path1={empty_dir}"], 0)
        runner.run_test("Removed directory doesn't exist", ["-test=not_exists", f"-path1={empty_dir}"], 0)

        nonempty_dir = runner.get_temp_path("nonemptydir")
        os.makedirs(nonempty_dir)
        with open(os.path.join(nonempty_dir, "file.txt"), 'w') as f:
            f.write("content")
        runner.run_test("Remove non-empty directory recursively",
                       ["-test=remove_directory_recursive", f"-path1={nonempty_dir}"], 0)
        runner.run_test("Recursively removed directory doesn't exist",
                       ["-test=not_exists", f"-path1={nonempty_dir}"], 0)

        # Test 9: File attributes
        print("\n[9] File Attribute Tests")
        attr_file = runner.get_temp_path("attr.txt")
        with open(attr_file, 'w') as f:
            f.write("Attributes test")

        runner.run_test("Set readonly true", ["-test=set_readonly", f"-path1={attr_file}", "-attr=true"], 0)
        runner.run_test("File is readonly", ["-test=is_readonly", f"-path1={attr_file}"], 0)
        runner.run_test("Set readonly false", ["-test=set_readonly", f"-path1={attr_file}", "-attr=false"], 0)

        runner.run_test("Set hidden true", ["-test=set_hidden", f"-path1={attr_file}", "-attr=true"], 0)
        runner.run_test("File is hidden", ["-test=is_hidden", f"-path1={attr_file}"], 0)
        runner.run_test("Set hidden false", ["-test=set_hidden", f"-path1={attr_file}", "-attr=false"], 0)

        # Test 10: List children
        print("\n[10] List Children Tests")
        list_dir = runner.get_temp_path("listdir")
        os.makedirs(list_dir)
        for i in range(3):
            with open(os.path.join(list_dir, f"file{i}.txt"), 'w') as f:
                f.write(f"File {i}")

        runner.run_test("List children (non-recursive)",
                       ["-test=list_children", f"-path1={list_dir}"], 0, should_contain=["3"])

        subdir = os.path.join(list_dir, "subdir")
        os.makedirs(subdir)
        with open(os.path.join(subdir, "nested.txt"), 'w') as f:
            f.write("nested")

        runner.run_test("List children (recursive)",
                       ["-test=list_children_recursive", f"-path1={list_dir}"], 0)

        # Test 11: Static methods
        print("\n[11] Static Method Tests")
        runner.run_test("List volumes", ["-test=list_volumes"], 0)

        # Test 12: Path relationships
        print("\n[12] Path Relationship Tests")
        parent_dir = runner.get_temp_path("parent")
        child_path = os.path.join(parent_dir, "child", "file.txt")
        os.makedirs(os.path.dirname(child_path), exist_ok=True)
        with open(child_path, 'w') as f:
            f.write("child file")

        runner.run_test("Path is within parent",
                       ["-test=is_within", f"-path1={child_path}", f"-path2={parent_dir}"], 0)

        # Test 13: Edge cases
        print("\n[13] Edge Case Tests")
        # Path with no extension
        no_ext = runner.get_temp_path("noextension")
        with open(no_ext, 'w') as f:
            f.write("no extension")
        runner.run_test("Get extension (no extension)", ["-test=get_extension", f"-path1={no_ext}"], 0)
        runner.run_test("Get stem (no extension)", ["-test=get_stem", f"-path1={no_ext}"], 0,
                       should_contain=["noextension"])

        # Path with multiple dots
        multi_dot = runner.get_temp_path("file.backup.txt")
        with open(multi_dot, 'w') as f:
            f.write("multi dot")
        runner.run_test("Get extension (multiple dots)", ["-test=get_extension", f"-path1={multi_dot}"], 0,
                       should_contain=["txt"])
        runner.run_test("Get stem (multiple dots)", ["-test=get_stem", f"-path1={multi_dot}"], 0,
                       should_contain=["file.backup"])

        # Print summary
        return runner.print_summary()

    finally:
        runner.cleanup()


if __name__ == "__main__":
    sys.exit(main())
