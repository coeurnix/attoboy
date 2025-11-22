#!/usr/bin/env python3
import socket
import subprocess
import sys
import time
import threading

def run_server(port):
    """Simple TCP server that echoes messages"""
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind(('127.0.0.1', port))
    server.listen(1)
    server.settimeout(5)

    try:
        conn, addr = server.accept()
        # Receive data
        data = conn.recv(1024)
        # Send response
        conn.sendall(b"Server received: " + data)
        time.sleep(0.1)
        conn.close()
    except socket.timeout:
        print("Server timeout waiting for connection")
    finally:
        server.close()

def main():
    port = 12345

    # Start server in background thread
    server_thread = threading.Thread(target=run_server, args=(port,))
    server_thread.daemon = True
    server_thread.start()

    # Give server time to start
    time.sleep(0.5)

    # Run the test executable
    result = subprocess.run(['../../build/tests/test_file_socket_runner.exe', str(port)],
                          capture_output=True, text=True)

    print(result.stdout)
    if result.stderr:
        print(result.stderr, file=sys.stderr)

    # Wait for server thread
    server_thread.join(timeout=2)

    sys.exit(result.returncode)

if __name__ == '__main__':
    main()
