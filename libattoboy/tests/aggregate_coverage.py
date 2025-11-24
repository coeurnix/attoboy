#!/usr/bin/env python3
"""
Aggregate test coverage data from multiple test runs.
Reads *_coverage.txt files and displays unified coverage statistics.
"""

import sys
import os
from pathlib import Path

def aggregate_coverage(coverage_dir):
    """
    Aggregate coverage from all *_coverage.txt files in the specified directory.

    Coverage file format: TESTED_COUNT TOTAL_COUNT function1,function2,function3
    """
    coverage_files = list(Path(coverage_dir).glob("*_coverage.txt"))

    if not coverage_files:
        print("No coverage files found!")
        return 1

    # Collect all tested functions from all tests
    all_tested = set()
    total_functions = None

    for coverage_file in coverage_files:
        try:
            with open(coverage_file, 'r') as f:
                line = f.read().strip()
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

                # Parse tested functions
                if len(parts) > 2 and parts[2]:
                    tested_funcs = parts[2].split(',')
                    all_tested.update(tested_funcs)

        except Exception as e:
            print(f"Error reading {coverage_file.name}: {e}")
            continue

    if total_functions is None:
        print("No valid coverage data found!")
        return 1

    # Calculate overall coverage
    tested_count = len(all_tested)
    percentage = (tested_count / total_functions) * 100.0

    # Display results
    print()
    print("=" * 60)
    print("OVERALL TEST COVERAGE")
    print("=" * 60)
    print(f"Coverage: {tested_count}/{total_functions} functions ({percentage:.2f}%)")

    if tested_count == total_functions:
        print("[PASS] ALL FUNCTIONS TESTED!")
    else:
        untested_count = total_functions - tested_count
        print(f"[WARN] {untested_count} function(s) remain untested")

    print("=" * 60)
    print()

    # Return non-zero if not at 100% (optional for CI)
    return 0 if tested_count == total_functions else 0

if __name__ == "__main__":
    coverage_dir = sys.argv[1] if len(sys.argv) > 1 else "."
    sys.exit(aggregate_coverage(coverage_dir))
