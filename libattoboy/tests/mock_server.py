#!/usr/bin/env python3
"""
Mock HTTP server for testing libattoboy WebRequest/WebResponse and AI functionality.
Provides fast, deterministic testing without network dependencies.
Includes realistic error scenarios: timeouts, 500 errors, rate limits, auth failures.
"""

import json
import sys
import time
from http.server import HTTPServer, BaseHTTPRequestHandler
from urllib.parse import urlparse, parse_qs

class MockHTTPHandler(BaseHTTPRequestHandler):
    """Handler for mock HTTP requests supporting WebRequest and AI testing."""

    # Suppress default logging for cleaner test output
    def log_message(self, format, *args):
        if '--verbose' in sys.argv:
            super().log_message(format, *args)

    def _send_response(self, status_code, body, content_type='application/json'):
        """Send HTTP response with given status, body, and content type."""
        self.send_response(status_code)
        self.send_header('Content-Type', content_type)
        self.send_header('Content-Length', len(body))
        self.end_headers()
        self.wfile.write(body)

    def _send_json(self, status_code, data):
        """Send JSON response."""
        body = json.dumps(data).encode('utf-8')
        self._send_response(status_code, body, 'application/json')

    def do_GET(self):
        """Handle GET requests."""
        path = urlparse(self.path).path

        if path == '/test/hello':
            self._send_json(200, {"message": "Hello from mock server!"})

        elif path == '/test/status/404':
            self._send_json(404, {"error": "Not Found"})

        elif path == '/test/status/500':
            self._send_json(500, {"error": "Internal Server Error"})

        elif path == '/test/status/429':
            # Rate limit error
            self._send_json(429, {"error": "Rate limit exceeded", "retry_after": 60})

        elif path == '/test/status/401':
            # Auth error
            self._send_json(401, {"error": "Invalid authentication"})

        elif path == '/test/timeout':
            # Simulate timeout - delay 30 seconds (client should timeout first)
            time.sleep(30)
            self._send_json(200, {"message": "This should have timed out"})

        elif path == '/test/slow':
            # Slow response - 2 seconds
            time.sleep(2)
            self._send_json(200, {"message": "Slow response"})

        elif path == '/test/large':
            # Large response for testing
            data = {"data": "x" * 10000}
            self._send_json(200, data)

        else:
            self._send_json(404, {"error": "Unknown endpoint"})

    def do_POST(self):
        """Handle POST requests."""
        path = urlparse(self.path).path

        # Read request body
        content_length = int(self.headers.get('Content-Length', 0))
        body = self.rfile.read(content_length)

        try:
            request_data = json.loads(body) if body else {}
        except json.JSONDecodeError:
            self._send_json(400, {"error": "Invalid JSON"})
            return

        # AI Chat Completions endpoint
        if path == '/v1/chat/completions':
            # Check for authentication
            auth_header = self.headers.get('Authorization', '')
            if not auth_header.startswith('Bearer '):
                self._send_json(401, {"error": {"message": "Missing or invalid authentication", "type": "invalid_request_error"}})
                return

            model = request_data.get('model', 'gpt-5-mini')
            messages = request_data.get('messages', [])
            max_tokens = request_data.get('max_tokens', 100)

            # Extract user message
            user_content = "test"
            for msg in messages:
                if msg.get('role') == 'user':
                    user_content = msg.get('content', 'test')
                    break

            # Simulate error scenarios based on message content
            if "error" in user_content.lower():
                self._send_json(500, {"error": {"message": "Internal server error", "type": "server_error"}})
                return
            elif "timeout" in user_content.lower():
                time.sleep(30)  # Force timeout
                return
            elif "rate_limit" in user_content.lower():
                self._send_json(429, {"error": {"message": "Rate limit exceeded", "type": "rate_limit_error"}})
                return

            # Generate mock response
            response = {
                "id": "chatcmpl-mock123",
                "object": "chat.completion",
                "created": 1234567890,
                "model": model,
                "choices": [{
                    "index": 0,
                    "message": {
                        "role": "assistant",
                        "content": f"Mock response to: {user_content}",
                        "refusal": None
                    },
                    "finish_reason": "stop"
                }],
                "usage": {
                    "prompt_tokens": 10,
                    "completion_tokens": 15,
                    "total_tokens": 25
                }
            }

            self._send_json(200, response)

        # AI Embeddings endpoint
        elif path == '/v1/embeddings':
            # Check for authentication
            auth_header = self.headers.get('Authorization', '')
            if not auth_header.startswith('Bearer '):
                self._send_json(401, {"error": {"message": "Missing or invalid authentication", "type": "invalid_request_error"}})
                return

            model = request_data.get('model', 'text-embedding-3-small')
            input_text = request_data.get('input', '')
            dimensions = request_data.get('dimensions', 1536)

            # Simulate error scenarios based on input text
            if isinstance(input_text, str):
                if "error" in input_text.lower():
                    self._send_json(500, {"error": {"message": "Internal server error", "type": "server_error"}})
                    return
                elif "timeout" in input_text.lower():
                    time.sleep(30)  # Force timeout
                    return

            # Generate mock embedding (all 0.1 for simplicity)
            embedding_data = [0.1] * dimensions

            response = {
                "object": "list",
                "data": [{
                    "object": "embedding",
                    "embedding": embedding_data,
                    "index": 0
                }],
                "model": model,
                "usage": {
                    "prompt_tokens": 5,
                    "total_tokens": 5
                }
            }

            self._send_json(200, response)

        # Echo endpoint (returns what was sent)
        elif path == '/test/echo':
            self._send_json(200, request_data)

        # Test endpoint
        elif path == '/test/post':
            self._send_json(200, {"received": "ok", "data": request_data})

        else:
            self._send_json(404, {"error": "Unknown endpoint"})


def run_server(port=8080):
    """Run the mock HTTP server on specified port."""
    server_address = ('', port)
    httpd = HTTPServer(server_address, MockHTTPHandler)
    print(f"Mock HTTP server running on http://localhost:{port}")
    print("Press Ctrl+C to stop")
    print("Endpoints:")
    print("  GET  /test/hello")
    print("  GET  /test/status/404")
    print("  GET  /test/status/500")
    print("  GET  /test/large")
    print("  POST /test/echo")
    print("  POST /test/post")
    print("  POST /v1/chat/completions  (OpenAI-compatible)")
    print("  POST /v1/embeddings        (OpenAI-compatible)")
    print()

    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\nShutting down mock server...")
        httpd.shutdown()


if __name__ == '__main__':
    port = 8080
    if len(sys.argv) > 1 and sys.argv[1].isdigit():
        port = int(sys.argv[1])
    run_server(port)
