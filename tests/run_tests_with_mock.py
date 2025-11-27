#!/usr/bin/env python3
"""
Test runner that manages mock server lifecycle.
Starts mock server before tests, runs tests, then stops server (even on failure).
"""

import subprocess
import sys
import time
import os
import signal

def main():
    if len(sys.argv) < 2:
        print("Usage: run_tests_with_mock.py <ctest_command> [ctest_args...]")
        sys.exit(1)

    # Get ctest command and arguments
    ctest_args = sys.argv[1:]

    # Start mock server
    mock_server_script = os.path.join(os.path.dirname(__file__), "mock_server.py")
    print(f"Starting mock server on port 8469...")

    try:
        # Start server in background
        server_process = subprocess.Popen(
            [sys.executable, "-u", mock_server_script, "8469"],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            creationflags=subprocess.CREATE_NEW_PROCESS_GROUP if sys.platform == 'win32' else 0
        )

        # Wait a moment for server to start
        time.sleep(1)

        # Check if server started successfully
        if server_process.poll() is not None:
            print("ERROR: Mock server failed to start")
            sys.exit(1)

        print("Mock server started successfully")
        print(f"Running tests: {' '.join(ctest_args)}")
        print("-" * 60)

        # Run CTest
        result = subprocess.run(ctest_args)
        exit_code = result.returncode

        print("-" * 60)
        print(f"Tests completed with exit code: {exit_code}")

    except Exception as e:
        print(f"ERROR: {e}")
        exit_code = 1

    finally:
        # Stop mock server
        print("Stopping mock server...")
        try:
            if sys.platform == 'win32':
                # Windows: send Ctrl+Break to process group
                server_process.send_signal(signal.CTRL_BREAK_EVENT)
            else:
                server_process.terminate()

            server_process.wait(timeout=5)
            print("Mock server stopped")
        except Exception as e:
            print(f"Warning: Error stopping mock server: {e}")
            try:
                server_process.kill()
            except:
                pass

    sys.exit(exit_code)

if __name__ == '__main__':
    main()
