import sys
from pathlib import Path

# Read all test functions from coverage files
coverage_dir = "build/libattoboy"
coverage_files = list(Path(coverage_dir).glob("*_coverage.txt"))

all_tested = set()
total_functions = None

for coverage_file in coverage_files:
    with open(coverage_file, 'rb') as f:
        raw_data = f.read()
    
    if raw_data.startswith(b'\xff\xfe') or (len(raw_data) > 1 and raw_data[1] == 0):
        line = raw_data.decode('utf-16le').strip()
    else:
        line = raw_data.decode('utf-8').strip()
    
    if not line:
        continue
    
    parts = line.split(' ', 2)
    total_functions = int(parts[1])
    
    if len(parts) > 2 and parts[2]:
        tested_funcs = parts[2].split(',')
        all_tested.update(tested_funcs)

# Get all functions from test_functions.h
with open('libattoboy/tests/test_functions.h', 'r') as f:
    content = f.read()

# Extract function names from X() macros
import re
all_functions = set(re.findall(r'X\((\w+)\)', content))

# Find untested
untested = sorted(all_functions - all_tested)

print(f"Total functions: {len(all_functions)}")
print(f"Tested: {len(all_tested)}")
print(f"Untested: {len(untested)}")
print()
print("Untested functions:")
for func in untested:
    print(f"  - {func}")
