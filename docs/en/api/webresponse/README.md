# WebResponse

## Overview

The `attoboy::WebResponse` class represents the result of an HTTP request made with `attoboy::WebRequest`. It provides a convenient, RAII-friendly way to inspect:

* Whether the request **succeeded** (2xx status codes).
* The final **URL** after any redirects.
* The HTTP **status code** and human-readable **reason phrase**.
* All **response headers** as an `attoboy::Map`.
* The **body** as:

  * A string (`String`),
  * Raw bytes (`Buffer`),
  * Or a parsed JSON object (`Map`).

You typically never construct a `WebResponse` directly. Instead:

1. You create a `WebRequest`.
2. You call one of its methods like `doGet()` or `doPost()`.
3. You receive a `WebResponse` from that method.

Example pattern:

```cpp
using namespace attoboy;

WebRequest request("https://api.example.com/info");
WebResponse response = request.doGet();

if (response.succeeded()) {
  String body = response.asString();
  // Use the response body...
}
```

`WebResponse` is designed to be:

* **Easy to inspect** – high-level methods for status, headers, and body.
* **Integrated with attoboy types** – uses `String`, `Map`, and `Buffer` for content and metadata.
* **Safe to copy** – copying a `WebResponse` shares the underlying implementation while keeping a simple value-like interface.

### Anatomy of an HTTP Response

If you are new to web programming, it helps to think of an HTTP response as having three main parts:

1. **Status line**
   Contains:

   * HTTP version (e.g., `HTTP/1.1`),
   * **Status code** (e.g., `200`, `404`),
   * **Reason phrase** (e.g., `"OK"`, `"Not Found"`).
     In `WebResponse`, this is represented by:
   * `getStatusCode()`
   * `getStatusReason()`
   * `succeeded()` (which checks whether the code is in the 200–299 range).

2. **Headers**
   A set of key–value pairs describing the response:

   * Examples: `Content-Type`, `Content-Length`, `Set-Cookie`.
     In `WebResponse`, headers are accessible via:
   * `getResponseHeaders()` returning a `Map`.

3. **Body**
   The actual payload:

   * HTML, JSON, binary data (images, PDF, etc.).
     In `WebResponse`, you can access it via:
   * `asString()` for text (UTF-8),
   * `asBuffer()` for raw bytes,
   * `asJson()` when you expect a JSON object.

### Typical Usage Flow

A common workflow:

1. Make a request:

   ```cpp
   WebRequest req("https://api.example.com/user/42");
   WebResponse res = req.doGet();
   ```

2. Check overall status:

   ```cpp
   if (!res.succeeded()) {
     int code = res.getStatusCode();
     String reason = res.getStatusReason();
     // Handle error based on code/reason
   }
   ```

3. Read headers if needed:

   ```cpp
   Map headers = res.getResponseHeaders();
   String contentType = headers.get("Content-Type", String());
   ```

4. Access the body in the most appropriate form:

   * JSON API: `const Map json = res.asJson();`
   * Text: `String body = res.asString();`
   * Binary: `Buffer data = res.asBuffer();`

---

## Reference

Each entry below covers one public constructor, assignment operator, or method of `attoboy::WebResponse` with:

* **Signature** – exact declaration from the header.
* **Synopsis** – original Doxygen one-line comment.
* **Parameters** and **Return value**.
* **In Depth** – additional explanation and a short example.

All examples assume:

```cpp
using namespace attoboy;
```

---

### Constructors, Copy, Assignment, and Destruction

#### `WebResponse(const WebResponse &other)`

**Signature**

```cpp
WebResponse(const WebResponse &other);
```

**Synopsis**
Creates a copy (shares the underlying response).

**Parameters**

* `other` – Existing `WebResponse` to copy.

**Return value**

* *(constructor; not applicable)*

**In Depth**

The copy constructor creates a new `WebResponse` that **shares** the same underlying implementation as `other`. This is a shallow copy:

* Status code, headers, and body are shared internally.

Because `WebResponse` is logically immutable from the user’s perspective, sharing is safe and efficient.

**Example**

```cpp
WebRequest request("https://example.com/info");
WebResponse original = request.doGet();

// Create a copy that shares the same underlying response
WebResponse copy(original);

int codeOriginal = original.getStatusCode();
int codeCopy     = copy.getStatusCode();   // same code
```

*This example shows copying a response and accessing shared status information.*

---

#### `~WebResponse()`

**Signature**

```cpp
~WebResponse();
```

**Synopsis**
Destroys the response and frees resources.

**Parameters**

* *(none)*

**Return value**

* *(destructor; not applicable)*

**In Depth**

The destructor releases any underlying resources associated with the response, such as:

* Internal network handles.
* Buffers used to store the body.

**Example**

```cpp
WebResponse response = WebRequest("https://example.com").doGet();
{
  WebResponse innerCopy(response);
  // Use innerCopy here...
} // innerCopy is destroyed; response is still valid

// response can still be used here.
String body = response.asString();
```

*This example demonstrates that copies of a response can be safely created and destroyed while the original remains valid.*

---

#### `WebResponse &operator=(const WebResponse &other)`

**Signature**

```cpp
WebResponse &operator=(const WebResponse &other);
```

**Synopsis**
Assigns another response (shares the underlying response).

**Parameters**

* `other` – `WebResponse` to assign from.

**Return value**

* Reference to `*this`, enabling assignment chaining.

**In Depth**

The copy assignment operator:

* Releases the current response’s resources.
* Makes this object share the underlying response data with `other`.

After assignment:

* Both `*this` and `other` refer to the same response.
* Methods like `getStatusCode()` and `asString()` will return the same values from both objects.

Self-assignment is safe and handled by the implementation.

**Example**

```cpp
WebResponse a = WebRequest("https://example.com/a").doGet();
WebResponse b = WebRequest("https://example.com/b").doGet();

// Reassign b to share the same response as a
b = a;

if (b.succeeded()) {
  String body = b.asString();  // same body as a.asString()
}
```

*This example reassigns one response to share another response’s data.*

---

### Status and Metadata

#### `bool succeeded() const`

**Signature**

```cpp
bool succeeded() const;
```

**Synopsis**
Returns true if status code is 200-299.

**Parameters**

* *(none)*

**Return value**

* `true` if the HTTP status code is in the range `200` through `299` (inclusive).
* `false` otherwise.

**In Depth**

`succeeded()` provides a simple, high-level way to check whether the request was considered successful by HTTP conventions:

* 2xx codes represent success:

  * `200 OK`
  * `201 Created`
  * `204 No Content`, etc.

For any non-2xx status (including redirects, client errors, and server errors):

* `succeeded()` returns `false`.
* You can still inspect the code and body using `getStatusCode()` and `asString()`/`asBuffer()`.

Remember that network-level issues (such as a timeout or connection failure) may produce a response where `getStatusCode()` is `0` or another sentinel, depending on implementation. You should treat `!succeeded()` as “the operation did not produce a successful HTTP 2xx result.”

**Example**

```cpp
WebRequest request("https://api.example.com/data");
WebResponse response = request.doGet(5000);

if (response.succeeded()) {
  String body = response.asString();
  // Process successful response body
} else {
  int code = response.getStatusCode();
  String reason = response.getStatusReason();
  // Handle non-2xx status or network error
}
```

*This example checks for a successful response before processing the body.*

---

#### `String getUrl() const`

**Signature**

```cpp
String getUrl() const;
```

**Synopsis**
Returns the final URL after redirects.

**Parameters**

* *(none)*

**Return value**

* `String` containing the final URL that produced this response, including any redirects that were followed.

**In Depth**

`getUrl()` is particularly useful when:

* You start with a URL that may redirect (for example, from `http` to `https` or to a different path).
* You want to know the **final** URL after any redirects handled by the underlying HTTP client.

For example, if you request `http://example.com` and the server redirects to `https://www.example.com/home`, `getUrl()` will give you the latter.

This is especially helpful when:

* Downloading resources that move over time.
* Debugging redirect chains.

**Example**

```cpp
WebRequest request("http://example.com"); // may redirect to https
WebResponse response = request.doGet();

String finalUrl = response.getUrl();
// e.g., "https://www.example.com/"
```

*This example shows retrieving the final URL after potential redirects.*

---

#### `int getStatusCode() const`

**Signature**

```cpp
int getStatusCode() const;
```

**Synopsis**
Returns the HTTP status code (e.g., 200, 404).

**Parameters**

* *(none)*

**Return value**

* Integer HTTP status code returned by the server, such as `200`, `404`, or `500`.

**In Depth**

The **status code** is the primary indicator of how the server processed the request:

* **2xx** – Success (`200 OK`, `201 Created`, `204 No Content`, etc.).
* **3xx** – Redirects.
* **4xx** – Client errors (`400 Bad Request`, `401 Unauthorized`, `404 Not Found`, etc.).
* **5xx** – Server errors (`500 Internal Server Error`, etc.).

You can use `getStatusCode()` to:

* Implement detailed error handling.
* Distinguish between different types of failures even when `succeeded()` is `false`.

For network errors, the code might be `0` or another implementation-defined value. Use this in combination with `succeeded()` and possibly the body for diagnostics.

**Example**

```cpp
WebResponse response = WebRequest("https://example.com/missing").doGet();

int code = response.getStatusCode();  // e.g., 404
if (code == 404) {
  // specific handling for "Not Found"
}
```

*This example checks for a specific HTTP status code.*

---

#### `String getStatusReason() const`

**Signature**

```cpp
String getStatusReason() const;
```

**Synopsis**
Returns the HTTP status reason (e.g., "OK", "Not Found").

**Parameters**

* *(none)*

**Return value**

* `String` containing the reason phrase associated with the status code, such as `"OK"`, `"Created"`, or `"Not Found"`.

**In Depth**

The **reason phrase** is a short human-readable description that accompanies the status code. It is primarily useful for:

* Logging and debugging.
* Informative messages shown to users or developers.

The reason phrase is not typically used for logic decisions; those should be based on the numeric code. However, it can provide helpful context when diagnosing issues.

**Example**

```cpp
WebResponse response = WebRequest("https://example.com/missing").doGet();

int code = response.getStatusCode();
String reason = response.getStatusReason();
// e.g., code == 404, reason == "Not Found"
```

*This example retrieves both the numeric code and human-readable reason phrase.*

---

#### `Map getResponseHeaders() const`

**Signature**

```cpp
Map getResponseHeaders() const;
```

**Synopsis**
Returns a map of response headers.

**Parameters**

* *(none)*

**Return value**

* `Map` containing all response headers.

  * Keys: header names as `String`s.
  * Values: header values as `String`s (or types convertible to `String`).

**In Depth**

`getResponseHeaders()` lets you inspect server-provided metadata:

* `Content-Type`
* `Content-Length`
* `Date`
* `Server`
* Custom headers (e.g., `X-Request-ID`, `X-RateLimit-Limit`)

Considerations:

* HTTP header names are case-insensitive, but the actual case stored in the map is implementation-dependent. You should usually look up keys using the exact case you observe in typical responses or normalize them yourself if needed.
* The returned `Map` is a **copy**. Modifying it does not change the original response.

Common usage:

* Validate content type before parsing JSON.
* Inspect caching or rate-limiting headers.
* Debug issues with server configuration.

**Example**

```cpp
WebResponse response = WebRequest("https://api.example.com/data").doGet();

// Inspect headers
Map headers = response.getResponseHeaders();

String contentType = headers.get("Content-Type", String());
String requestId   = headers.get("X-Request-ID", String());

// Use header values for logging or conditional logic
```

*This example reads some common headers from the response.*

---

### Body Access

#### `Map asJson() const`

**Signature**

```cpp
Map asJson() const;
```

**Synopsis**
Parses body as JSON object. Returns empty Map if not a JSON object.

**Parameters**

* *(none)*

**Return value**

* `Map` representing the JSON object parsed from the body, or an empty `Map` if:

  * The body is empty,
  * The body is not valid JSON, or
  * The JSON is not a JSON **object** (for example, if it is a bare array or a primitive).

**In Depth**

`asJson()` is a convenience function for JSON-based APIs:

* It attempts to parse the response body as JSON.
* If parsing succeeds and the top-level value is a JSON object (`{ ... }`), it returns a `Map` representing that object.
* If parsing fails or the top-level value is not an object (for example, `[...]`), it returns an empty `Map`.

Important details:

* The returned `Map` is a value that you own; you can store it as needed.
* Use `isEmpty()` to check if the JSON parsing succeeded.
* The `Map` contents can be accessed using `Map` methods like `get`, `hasKey`, `keys`, etc.

If you expect a JSON **array** at the top level, you should instead parse the body manually (for example, using `List::FromJSONString(response.asString())`), since `asJson()` is specifically for JSON objects.

**Example**

```cpp
WebResponse response = WebRequest("https://api.example.com/user/42").doGet();

if (!response.succeeded()) {
  // handle error...
} else {
  Map json = response.asJson();
  if (!json.isEmpty()) {
    // Extract fields with defaults
    String name = json.get("name", String("Unknown"));
    int age     = json.get("age", 0);

    // Use parsed data...
  } else {
    // Body was not a JSON object
    String rawBody = response.asString();
    // handle unexpected format...
  }
}
```

*This example parses the response body as a JSON object and safely accesses fields using defaults.*

---

#### `String asString() const`

**Signature**

```cpp
String asString() const;
```

**Synopsis**
Returns the body as a string.

**Parameters**

* *(none)*

**Return value**

* `String` containing the response body interpreted as UTF-8 text.

**In Depth**

`asString()` is the primary way to access textual responses:

* It decodes the body bytes as UTF-8 and returns a `String`.
* It is suitable for:

  * JSON,
  * HTML,
  * plain text,
  * or any other UTF-8-compatible content.

If the response is binary (e.g., images, PDFs, or compressed data), you should prefer `asBuffer()` instead. Calling `asString()` on binary data may yield unreadable text or invalid characters.

The returned `String` is a copy; it remains valid even after the `WebResponse` is destroyed.

**Example**

```cpp
WebResponse response = WebRequest("https://example.com/hello.txt").doGet();

if (response.succeeded()) {
  String text = response.asString();
  // text now holds the entire body as UTF-8
}
```

*This example reads a plain text file from a server as a string.*

---

#### `Buffer asBuffer() const`

**Signature**

```cpp
Buffer asBuffer() const;
```

**Synopsis**
Returns the body as a buffer.

**Parameters**

* *(none)*

**Return value**

* `Buffer` containing the raw bytes of the response body.

**In Depth**

`asBuffer()` is the low-level counterpart to `asString()`:

* It returns the body as binary data, without any decoding or assumptions about encoding.
* It is ideal for:

  * Downloading images.
  * Handling binary protocols.
  * Working with compressed or encrypted data.

Because the returned `Buffer` is a copy, you can safely modify it, slice it, or store it beyond the lifetime of the `WebResponse`.

You can combine `asBuffer()` with other `Buffer` methods:

* `length()`
* `toBase64()`
* `compress()` / `decompress()`
* `crypt()` for encryption/decryption

**Example**

```cpp
WebResponse response = WebRequest("https://example.com/image.png").doGet();

if (response.succeeded()) {
  Buffer data = response.asBuffer();

  int size = data.length();
  // You could write 'data' to disk with Path/File, or process it further.
}
```

*This example retrieves a binary image as a buffer of raw bytes.*
