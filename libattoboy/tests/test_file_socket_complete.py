#!/usr/bin/env python3
import socket
import subprocess
import sys
import threading
import time

def run_server():
    s = socket.socket()
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    try:
        s.bind(('127.0.0.1', 12357))
        s.listen(1)
        print("[Server] Ready on port 12357", flush=True)

        s.settimeout(10)
        c, addr = s.accept()
        print(f"[Server] Connection from {addr}", flush=True)

        data = c.recv(1024)
        print(f"[Server] Received: {data!r}", flush=True)

        response = b"Server received: " + data
        c.sendall(response)
        print(f"[Server] Sent: {response!r}", flush=True)

        time.sleep(0.3)
        c.close()
    except Exception as e:
        print(f"[Server] Error: {e}", flush=True)
    finally:
        s.close()

# Start server
server_thread = threading.Thread(target=run_server, daemon=True)
server_thread.start()

time.sleep(0.5)

# Run client test
print("[Client] Running test...", flush=True)
result = subprocess.run(
    [r'..\..\build\tests\test_file_socket_complete.exe'],
    capture_output=True,
    text=True
)

print("\n[Client] Output:")
print(result.stdout)
if result.stderr:
    print("[Client] Errors:")
    print(result.stderr)

server_thread.join(timeout=2)

print(f"\n[Result] Exit code: {result.returncode}")
sys.exit(result.returncode)
