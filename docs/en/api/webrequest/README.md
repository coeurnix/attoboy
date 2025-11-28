# `attoboy::WebRequest` User Guide

## Overview

The `attoboy::WebRequest` class is the primary way to perform HTTP requests in the attoboy library. It wraps Windows’ underlying networking APIs (WinHTTP) and gives you a high-level, RAII-friendly interface for:

* Sending HTTP **GET** and **POST** requests.
* Sending JSON, binary, or string request bodies.
* Using other HTTP methods such as **PUT**, **PATCH**, or **DELETE**.
* Managing query parameters and headers.
* Downloading files directly to disk.

At a high level:

* You create a `WebRequest` with a URL (plus optional query parameters and headers).
* You call one of the `doGet`, `doPost`, or `doRequest` functions with an optional timeout.
* You receive a `WebResponse` object, which you inspect via methods like `succeeded()`, `getStatusCode()`, `asString()`, or `asJson()`.

All examples below assume:

```cpp
using namespace attoboy;
```

### HTTP Basics (for New Web Programmers)

If you are new to web programming, it helps to understand a few core ideas that `WebRequest` exposes.

#### URL

A URL is a string that tells the client *where* to send the request. For example:

* `https://api.example.com/users`
* `https://example.com/search?q=hello`

A URL usually has:

* **Scheme**: `http` or `https`.
* **Host**: `api.example.com`.
* **Path**: `/users`.
* **Query string**: e.g., `?q=hello`, containing key–value pairs.

`WebRequest` takes a `String url` and optional `Map` of query parameters. Those parameters are automatically appended to the URL as a query string.

#### HTTP Methods

The **method** describes what you want to do:

* **GET** – Retrieve data. No request body is normally sent.
* **POST** – Send data to a server (e.g., JSON, form data, or binary).
* **PUT** / **PATCH** – Update existing data.
* **DELETE** – Remove data.

`WebRequest` provides convenience methods for `GET` and several `POST` variants, plus a general-purpose `doRequest()` for custom methods.

#### Headers

HTTP **headers** are key–value pairs that describe metadata about the request:

* Examples: `Content-Type`, `Authorization`, `Accept`, `User-Agent`.
* In `WebRequest`, you pass headers as a `Map` pointer when constructing the object.

#### Request Body

Some methods (especially `POST`, `PUT`, `PATCH`) include a **body**:

* Raw binary data (`Buffer`).
* Text (`String`).
* JSON data (`Map` or `List`, which `WebRequest` converts to JSON for you).

`WebRequest` offers overloaded `doPost()` functions for different body types.

#### Response, Status Code, and Errors

The server’s answer is the **HTTP response**, which contains:

* **Status code** – e.g., `200` for success, `404` for “Not Found”.
* **Headers** – Metadata from the server.
* **Body** – Data returned (HTML, JSON, binary, etc.).

The library represents this as a `WebResponse`:

* `response.succeeded()` checks if the status code is 2xx.
* `response.getStatusCode()` returns the code.
* `response.asString()`, `response.asBuffer()`, or `response.asJson()` give you the body.

#### Timeouts and Blocking Behavior

All `WebRequest` operations are **blocking**:

* Calls like `doGet()` and `doPost()` will not return until:

  * The request completes successfully,
  * It fails (e.g., network error), or
  * The **timeout** is reached.

Timeouts are specified in **milliseconds**:

* `timeout = -1` (the default) means *no timeout* (wait as long as needed).
* `timeout = 10'000` means “wait up to 10 seconds for the operation to complete”.

#### Single-use Requests

A single `WebRequest` instance is intended to perform **one** request:

* After you call `doGet`, `doPost`, or `doRequest`, `hasCompleted()` will return `true`.
* You should treat that request as **single-use** and create a new `WebRequest` object for the next call.

---

## Reference

Each entry below covers one public constructor, method, or static function of `attoboy::WebRequest` with:

* **Signature** – exact declaration from the header.
* **Synopsis** – original one-line Doxygen comment.
* **Parameters** and **Return value**.
* **In Depth** – details, caveats, and an example.

---

### Constructors, Copy, Assignment, and Destruction

#### `WebRequest(const String &url, const Map *params = nullptr, const Map *headers = nullptr)`

**Signature**

```cpp
WebRequest(const String &url, const Map *params = nullptr,
           const Map *headers = nullptr);
```

**Synopsis**
Creates a request. params become query string; headers are sent with
request.

**Parameters**

* `url`
  Base URL for the request, including scheme (`http` or `https`), host, and path.
  Example: `"https://api.example.com/users"`.

* `params` *(optional)*
  Pointer to a `Map` whose key–value pairs will be appended to the URL as
  query string parameters. If `nullptr`, no additional query parameters are added.

* `headers` *(optional)*
  Pointer to a `Map` of headers to send with the request. Keys and values are converted
  to strings. If `nullptr`, no custom headers are added.

**Return value**

* *(constructor; not applicable)*

**In Depth**

This constructor sets up all the configuration needed for one HTTP request:

* It stores the URL.
* It takes an optional `Map` of **query parameters** and converts them into
  `?key=value&key2=value2` on the URL.
* It takes an optional `Map` of **headers** and adds them to every request method you call
  on this `WebRequest` instance.

Typical parameter usage:

* **Parameters map** (`params`):
  Keys and values are usually strings (or types that `String` can be constructed from). For example,
  `params.put("q", "hello")` becomes `?q=hello`. Multiple keys are joined with `&`.

* **Headers map** (`headers`):
  Also represented as a `Map`. For example,
  `headers.put("Authorization", "Bearer TOKEN")`.

You do **not** send the request at construction time. The actual network operation happens when you call `doGet`, `doPost`, or `doRequest`.

**Example**

```cpp
String url("https://api.example.com/search");

// Build query parameters: ?q=attoboy&page=1
Map params;
params.put("q", "attoboy").put("page", 1);

// Build headers: Authorization: Bearer XYZ
Map headers;
headers.put("Authorization", "Bearer XYZ");

WebRequest request(url, &params, &headers);
WebResponse response = request.doGet(5000);  // 5-second timeout

if (response.succeeded()) {
  String body = response.asString();
  // ... use response body ...
}
```

*This example constructs a GET request with query parameters and a custom header, then executes it with a timeout.*

---

#### `WebRequest(const WebRequest &other)`

**Signature**

```cpp
WebRequest(const WebRequest &other);
```

**Synopsis**
Creates a copy (shares the underlying request).

**Parameters**

* `other` – Existing `WebRequest` to copy.

**Return value**

* *(constructor; not applicable)*

**In Depth**

The copy constructor creates a new `WebRequest` object that **shares** the internal implementation with `other`. This is a **shallow** copy:

* Configuration (URL, params, headers) is effectively shared.
* Because requests are single-use, you should be careful not to treat this as a way
  to run the **same** request twice. Once any shared instance completes a request, all
  copies will report `hasCompleted() == true`.

The copy constructor is mostly useful when passing `WebRequest` between functions without transferring ownership of the underlying implementation.

**Example**

```cpp
String url("https://example.com/info");
WebRequest base(url);

// Copy the request handle
WebRequest copy(base);

// Either instance can be used to perform the one allowed request.
// After one of them calls doGet/doPost/doRequest, both will reflect completion.
WebResponse response = copy.doGet();
```

*This example copies a `WebRequest`, but the underlying request configuration is shared.*

---

#### `~WebRequest()`

**Signature**

```cpp
~WebRequest();
```

**Synopsis**
Destroys the request and frees resources.

**Parameters**

* *(none)*

**Return value**

* *(destructor; not applicable)*

**In Depth**

The destructor releases any underlying resources associated with the request, such as:

* Internal WinHTTP handles.
* Allocated memory for configuration and state.

You do not need to call any explicit “close” method; simply allow `WebRequest` to go out of scope, and RAII will clean up resources.

Destroying a `WebRequest` does **not** cancel an already completed operation. Requests are synchronous and complete before the method returns.

**Example**

```cpp
{
  WebRequest req("https://example.com/ping");
  WebResponse res = req.doGet();
  // ... process response ...
} // req is destroyed here; underlying resources are freed.
```

*This example demonstrates automatic resource cleanup when a `WebRequest` goes out of scope.*

---

#### `WebRequest &operator=(const WebRequest &other)`

**Signature**

```cpp
WebRequest &operator=(const WebRequest &other);
```

**Synopsis**
Assigns another request (shares the underlying request).

**Parameters**

* `other` – `WebRequest` to assign from.

**Return value**

* Reference to `*this`, allowing assignment chaining.

**In Depth**

The copy assignment operator:

* Releases the current object’s resources.
* Makes the left-hand side share the underlying request implementation with `other`.

As with the copy constructor, this produces **shared** state. If one of the shared instances performs a request, all of them will see `hasCompleted()` as true afterwards.

This is useful when you want to reassign a variable to refer to an existing `WebRequest`.

**Example**

```cpp
WebRequest a("https://example.com/a");
WebRequest b("https://example.com/b");

b = a; // Now b shares the underlying request with a

WebResponse r = b.doGet();
// a.hasCompleted() is now also true.
```

*This example shows reassigning a `WebRequest` so that two variables share the same underlying request.*

---

### HTTP Operations

#### `WebResponse doGet(int timeout = -1)`

**Signature**

```cpp
WebResponse doGet(int timeout = -1);
```

**Synopsis**
Performs an HTTP GET request. timeout in ms (-1 = infinite).

**Parameters**

* `timeout` *(optional)*
  Maximum time to wait for the operation, in milliseconds:

  * `-1` (default): no timeout (wait indefinitely).
  * Any non-negative integer: maximum number of milliseconds before the library treats the request as timed out.

**Return value**

* `WebResponse` representing the server’s response.
  Use `response.succeeded()`, `response.getStatusCode()`, and other `WebResponse` methods to inspect it.

**In Depth**

`doGet()` sends an HTTP **GET** request to the URL you specified in the constructor, with:

* Any query parameters from the `params` map added to the URL.
* Any headers from the `headers` map included in the request.

The method:

* Blocks until the request completes or the timeout is reached.
* Returns a `WebResponse`, even in error conditions. For example:

  * Network errors and timeouts typically produce a response where `succeeded()` is `false`, or where body/content is empty.
  * HTTP non-2xx status codes (e.g., `404`, `500`) are still represented as `WebResponse` objects; `succeeded()` will be `false`, but `getStatusCode()` will reflect the actual code.

Once `doGet()` returns:

* `hasCompleted()` will report `true`.
* You should not call other request methods (`doPost()`, `doRequest()`) on the same `WebRequest` instance.

**Example**

```cpp
String url("https://api.example.com/status");
WebRequest request(url);

// Wait up to 10 seconds
WebResponse response = request.doGet(10 * 1000);

if (response.succeeded()) {
  String body = response.asString();
  // e.g., parse JSON or inspect text
} else {
  int status = response.getStatusCode();
  // handle error based on status or lack of response
}
```

*This example performs a GET request with a 10-second timeout and inspects the result.*

---

#### `WebResponse doPost(int timeout = -1)`

**Signature**

```cpp
WebResponse doPost(int timeout = -1);
```

**Synopsis**
Performs an HTTP POST request with no body.

**Parameters**

* `timeout` *(optional)*
  Maximum time to wait for the operation, in milliseconds (`-1` for no timeout).

**Return value**

* `WebResponse` representing the server’s response.

**In Depth**

`doPost()` sends an HTTP **POST** request with:

* No explicit request body.
* Any query parameters from `params` appended to the URL.
* Any headers from `headers` included in the request.

This is useful for endpoints that treat a bare POST (with parameters in the query string or in headers) as a trigger or command rather than as data submission.

The same completion rules and single-use semantics as `doGet()` apply.

**Example**

```cpp
String url("https://api.example.com/trigger-job");

// Add ?job=cleanup to the URL
Map params;
params.put("job", "cleanup");

WebRequest request(url, &params, nullptr);
WebResponse response = request.doPost(5000);

if (!response.succeeded()) {
  // handle non-2xx or timeout
}
```

*This example sends a POST request with no body but with query parameters.*

---

#### `WebResponse doPost(const Map &json, int timeout = -1)`

**Signature**

```cpp
WebResponse doPost(const Map &json, int timeout = -1);
```

**Synopsis**
Performs an HTTP POST with JSON body (Map).

**Parameters**

* `json`
  Map representing a JSON object. Keys and values are converted into JSON object fields.

* `timeout` *(optional)*
  Maximum time to wait for the operation, in milliseconds (`-1` for no timeout).

**Return value**

* `WebResponse` representing the server’s response.

**In Depth**

This overload is a **JSON helper**:

* The `json` map is serialized into a JSON **object** string, such as:

  * `{"name":"Alice","age":30}`.
* The serialized JSON is sent as the **request body**.
* The implementation is designed to treat this as a JSON POST. Typically,
  the request will include an appropriate `Content-Type` header
  (implementation-dependent), or you can specify one explicitly in the `headers`
  map when constructing the `WebRequest`.

Use this when the remote API expects a JSON object in the POST body.

**Example**

```cpp
String url("https://api.example.com/users");

Map headers;
headers.put("Authorization", "Bearer TOKEN");

Map payload;
payload.put("name", "Alice").put("age", 30);

WebRequest request(url, nullptr, &headers);
WebResponse response = request.doPost(payload, 10000);

if (response.succeeded()) {
  String body = response.asString();
  // server might return created user or status message
}
```

*This example POSTs a JSON object to create or update a user.*

---

#### `WebResponse doPost(const List &json, int timeout = -1)`

**Signature**

```cpp
WebResponse doPost(const List &json, int timeout = -1);
```

**Synopsis**
Performs an HTTP POST with JSON body (List).

**Parameters**

* `json`
  List representing a JSON array. Elements are converted into JSON array entries.

* `timeout` *(optional)*
  Maximum time to wait for the operation, in milliseconds (`-1` for no timeout).

**Return value**

* `WebResponse` representing the server’s response.

**In Depth**

This overload is the array counterpart to `doPost(const Map&)`:

* The `List` is serialized into a JSON **array**, such as:

  * `[1,2,3]`
  * `["one","two","three"]`
  * `[{"id":1},{"id":2}]` if you store `Map` values inside.

This is useful when an API expects a list of values in the request body, for example when uploading batches of events or IDs.

**Example**

```cpp
String url("https://api.example.com/events/batch");

List events;
events.append("start").append("stop").append("pause");

WebRequest request(url);
WebResponse response = request.doPost(events, 8000);

if (response.succeeded()) {
  // batch accepted
}
```

*This example sends a JSON array of events in a single POST.*

---

#### `WebResponse doPost(const Buffer &body, int timeout = -1)`

**Signature**

```cpp
WebResponse doPost(const Buffer &body, int timeout = -1);
```

**Synopsis**
Performs an HTTP POST with binary body.

**Parameters**

* `body`
  `Buffer` containing the raw bytes to send as the request body.

* `timeout` *(optional)*
  Maximum time to wait for the operation, in milliseconds (`-1` for no timeout).

**Return value**

* `WebResponse` representing the server’s response.

**In Depth**

This overload lets you send arbitrary **binary** data:

* It does not impose any particular encoding.
* You should set appropriate headers (such as `Content-Type` or `Content-Length`)
  via the `headers` map when you construct the `WebRequest`.

Typical use cases:

* Uploading files.
* Sending compressed or encrypted payloads.
* Using custom non-JSON protocols.

**Example**

```cpp
String url("https://example.com/upload/blob");

// Build a binary buffer (for example, an in-memory file)
Buffer data;
data.append(String("BINARY-DATA"));  // placeholder

Map headers;
headers.put("Content-Type", "application/octet-stream");

WebRequest request(url, nullptr, &headers);
WebResponse response = request.doPost(data, 15000);

if (!response.succeeded()) {
  // handle failure
}
```

*This example sends binary data with a generic binary content type.*

---

#### `WebResponse doPost(const String &body, int timeout = -1)`

**Signature**

```cpp
WebResponse doPost(const String &body, int timeout = -1);
```

**Synopsis**
Performs an HTTP POST with string body.

**Parameters**

* `body`
  `String` containing the text to send as the request body. It may be JSON, plain text, URL-encoded form data, or any other textual protocol.

* `timeout` *(optional)*
  Maximum time to wait for the operation, in milliseconds (`-1` for no timeout).

**Return value**

* `WebResponse` representing the server’s response.

**In Depth**

This overload is for sending **text-based** payloads when you want full control over the body:

* If you are sending JSON, you can build the JSON string yourself (for example using `String` and `format`) and pass it here.
* If you are sending form data such as `key=value&other=value`, you can construct that string manually and send it as the body.

As always, set the appropriate `Content-Type` header in the `headers` map when constructing the `WebRequest`.

**Example**

```cpp
String url("https://example.com/api/raw");
Map headers;
headers.put("Content-Type", "text/plain; charset=utf-8");

String body("Hello from attoboy!");

WebRequest request(url, nullptr, &headers);
WebResponse response = request.doPost(body, 5000);

if (response.succeeded()) {
  String reply = response.asString();
  // use server's response
}
```

*This example sends a plain text payload as the POST body.*

---

#### `WebResponse doRequest(const String &method, const Buffer &body, int timeout = -1)`

**Signature**

```cpp
WebResponse doRequest(const String &method, const Buffer &body,
                      int timeout = -1);
```

**Synopsis**
Performs a custom HTTP method (PUT, PATCH, DELETE, etc.).

**Parameters**

* `method`
  HTTP method to use. Typical values: `"PUT"`, `"PATCH"`, `"DELETE"`, `"OPTIONS"`, etc. It should be an uppercase string, but the server may accept other forms.

* `body`
  `Buffer` containing the raw request body bytes. For methods like `DELETE` that often have no body, you can pass an empty buffer.

* `timeout` *(optional)*
  Maximum time to wait for the operation, in milliseconds (`-1` for no timeout).

**Return value**

* `WebResponse` representing the server’s response.

**In Depth**

`doRequest()` is the most flexible operation:

* You can send **any HTTP method** supported by the server.
* You supply the raw body bytes, giving you complete control over the payload.

Common patterns:

* **PUT** with JSON body:

  * Serialize your JSON to a `String`, then wrap it in a `Buffer`:

    * `Buffer body(jsonString);`
* **PATCH** with partial updates.
* **DELETE** with or without a small body, depending on the API.

Remember to set headers such as `Content-Type` using the `headers` map passed to the constructor.

**Example**

```cpp
String url("https://api.example.com/users/42");
Map headers;
headers.put("Content-Type", "application/json");

// Build JSON body as a string, then convert to a buffer
String jsonBody("{\"active\":false}");
Buffer body(jsonBody);

WebRequest request(url, nullptr, &headers);

// Perform a PATCH request with JSON body
WebResponse response = request.doRequest("PATCH", body, 10000);

if (response.succeeded()) {
  // user deactivated successfully
}
```

*This example sends a custom PATCH request with a JSON body.*

---

### Inspecting Request Configuration and State

#### `String getUrl() const`

**Signature**

```cpp
String getUrl() const;
```

**Synopsis**
Returns the request URL.

**Parameters**

* *(none)*

**Return value**

* `String` holding the full URL for this request, including any query string derived from the parameters map.

**In Depth**

`getUrl()` returns the effective URL stored in the `WebRequest`. After construction:

* It typically includes:

  * The base URL you provided.
  * Any query parameters that were added from the `params` map.

You can call this before or after the request to confirm that your parameters were applied as expected.

**Example**

```cpp
Map params;
params.put("q", "attoboy");

WebRequest request("https://example.com/search", &params, nullptr);
String fullUrl = request.getUrl();
// e.g., "https://example.com/search?q=attoboy"
```

*This example inspects the final URL that `WebRequest` will use for the request.*

---

#### `Map getParams() const`

**Signature**

```cpp
Map getParams() const;
```

**Synopsis**
Returns the query parameters map.

**Parameters**

* *(none)*

**Return value**

* `Map` containing the query parameters associated with this request.

**In Depth**

`getParams()` returns the `Map` of query parameters that the `WebRequest` is using. This allows you to:

* Debug or log which parameters are attached to the request.
* Duplicate them into another request.
* Inspect or modify them (by creating a new `WebRequest` with the returned map, since this method returns a `Map` by value).

The returned `Map` is not a live view; modifying it does **not** change the original `WebRequest`. To change parameters, create a new `WebRequest` with the updated map.

**Example**

```cpp
Map params;
params.put("page", 2).put("limit", 50);

WebRequest request("https://example.com/users", &params, nullptr);

Map copy = request.getParams();
// copy can be logged, inspected, or reused when constructing a new request
```

*This example fetches the parameters used in a request for inspection or reuse.*

---

#### `Map getHeaders() const`

**Signature**

```cpp
Map getHeaders() const;
```

**Synopsis**
Returns the headers map.

**Parameters**

* *(none)*

**Return value**

* `Map` containing the headers associated with this request.

**In Depth**

`getHeaders()` returns the `Map` of custom headers that were provided at construction time. This is useful for:

* Debugging header configuration.
* Copying headers into another request.
* Logging authentication or content-type settings (carefully, without exposing secrets in logs).

As with `getParams()`, modifying the returned `Map` does **not** affect the existing `WebRequest`. To change headers, construct a new `WebRequest` with an updated headers map.

**Example**

```cpp
Map headers;
headers.put("Authorization", "Bearer TOKEN")
       .put("Accept", "application/json");

WebRequest request("https://example.com/data", nullptr, &headers);

Map copy = request.getHeaders();
// copy["Authorization"], copy["Accept"], etc.
```

*This example retrieves the headers used for a request for introspection or reuse.*

---

#### `bool hasCompleted() const`

**Signature**

```cpp
bool hasCompleted() const;
```

**Synopsis**
Returns true if a request has been completed (can only be done once).

**Parameters**

* *(none)*

**Return value**

* `true` if this `WebRequest` has already run a request method (`doGet`, `doPost`, or `doRequest`) and that operation has finished.
* `false` if no request has been executed yet.

**In Depth**

`hasCompleted()` reflects the **single-use** nature of `WebRequest`:

* Once a call to `doGet`, any `doPost` overload, or `doRequest` finishes (successfully or with an error), `hasCompleted()` will return `true`.
* After completion, you should **not** call another request method on the same instance. Instead, create a new `WebRequest`.

This function is especially useful when you have shared or copied `WebRequest` objects and want to know whether any of them have already issued a request.

**Example**

```cpp
WebRequest req("https://example.com/ping");

if (!req.hasCompleted()) {
  WebResponse res = req.doGet();
}

if (req.hasCompleted()) {
  // A request was performed (successfully or not); this instance is “used”
}
```

*This example checks whether a `WebRequest` has already been used before calling `doGet()`.*

---

### Static Helper

#### `static bool Download(const String &url, const String &savePath, const Map *params = nullptr, const Map *headers = nullptr, bool overwrite = true, int timeout = -1)`

**Signature**

```cpp
static bool Download(const String &url, const String &savePath,
                     const Map *params = nullptr,
                     const Map *headers = nullptr, bool overwrite = true,
                     int timeout = -1);
```

**Synopsis**
Downloads a file from URL to disk. Returns true on success.

**Parameters**

* `url`
  URL of the resource to download. This is equivalent to the `url` passed to the `WebRequest` constructor.

* `savePath`
  Path (as a `String`) to the file on disk where the downloaded data should be written. The path should be valid for the current user and system.

* `params` *(optional)*
  Pointer to a `Map` of query parameters to append to the URL. If `nullptr`, no extra parameters are added.

* `headers` *(optional)*
  Pointer to a `Map` of headers to send with the request. If `nullptr`, no custom headers are added.

* `overwrite` *(optional)*
  If `true` (default), an existing file at `savePath` may be overwritten. If `false`, existing files should cause the function to fail (implementation dependent).

* `timeout` *(optional)*
  Maximum time to wait for the entire download, in milliseconds (`-1` for no timeout).

**Return value**

* `true` if the download completed successfully and the file was written to `savePath`.
* `false` if there was an error (network failure, timeout, permission issue, etc.).

**In Depth**

`Download()` is a **convenience** function that wraps a common pattern:

1. Build a `WebRequest` for the given `url`, `params`, and `headers`.
2. Perform a request (typically `GET`).
3. Write the response body directly to disk at `savePath`.

It simplifies download tasks so you do not have to:

* Manually call `doGet()`.
* Extract the body as a `Buffer` or `String`.
* Open a `File` and write the data yourself.

Behavior notes:

* If `overwrite` is `false` and the target file already exists, the download should fail and return `false` (you should treat this as “file already present”).
* You are responsible for ensuring that the directory for `savePath` exists and that you have permission to write to it.
* This function is blocking and subject to the given timeout.

**Example**

```cpp
String url("https://example.com/files/report.pdf");

// Build query parameters: ?version=latest
Map params;
params.put("version", "latest");

// Set a custom header
Map headers;
headers.put("User-Agent", "attoboy-downloader");

// Decide where to save the file
Path destPath = Path::GetDocumentsDirectory()
                    .getParentDirectory()
                    .getParentDirectory(); // example chaining (if desired)
// In practice, build a proper path string:
String savePath("C:\\Users\\User\\Documents\\report.pdf");

bool ok = WebRequest::Download(url, savePath, &params, &headers,
                               true,   // overwrite if exists
                               30000); // 30-second timeout

if (!ok) {
  // handle download failure
}
```

*This example downloads a remote PDF to a specific path, with query parameters, a custom header, overwrite enabled, and a 30-second timeout.*

