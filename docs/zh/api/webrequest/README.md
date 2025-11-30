# WebRequest

## 概览

`attoboy::WebRequest` 类是 attoboy 库中执行 HTTP 请求的主要方式。它封装了 Windows 底层的网络 API（WinHTTP），并为你提供一个高层、适合 RAII 的接口，用于：

* 发送 HTTP **GET** 和 **POST** 请求。
* 发送 JSON、二进制或字符串类型的请求体。
* 使用其他 HTTP 方法，例如 **PUT**、**PATCH** 或 **DELETE**。
* 管理查询参数和请求头。
* 将文件直接下载到磁盘。

从整体使用流程来看：

* 你通过 URL（加上可选的查询参数和请求头）创建一个 `WebRequest`。
* 你调用 `doGet`、`doPost` 或 `doRequest` 之一，并可选地指定超时时间。
* 你会得到一个 `WebResponse` 对象，通过 `succeeded()`、`getStatusCode()`、`asString()` 或 `asJson()` 等方法进行检查。

下面的所有示例都假设：

```cpp
using namespace attoboy;
```

### HTTP 基础知识（面向 Web 新手）

如果你是 Web 编程新手，理解 `WebRequest` 所暴露的一些核心概念会有帮助。

#### URL

URL 是一个字符串，用来告诉客户端*将请求发送到哪里*。例如：

* `https://api.example.com/users`
* `https://example.com/search?q=hello`

一个 URL 通常包含：

* **Scheme（协议）**：`http` 或 `https`。
* **Host（主机名）**：`api.example.com`。
* **Path（路径）**：`/users`。
* **Query string（查询字符串）**：例如 `?q=hello`，里面包含键值对。

`WebRequest` 接收一个 `String url` 和一个可选的查询参数 `Map`。这些参数会自动附加到 URL 上作为查询字符串。

#### HTTP 方法

HTTP **方法**描述了你想做什么：

* **GET** – 获取数据。通常不发送请求体。
* **POST** – 向服务器发送数据（例如 JSON、表单数据或二进制数据）。
* **PUT** / **PATCH** – 更新已有数据。
* **DELETE** – 删除数据。

`WebRequest` 提供了 GET 和若干 POST 变体的便捷方法，并通过通用的 `doRequest()` 支持自定义方法。

#### 请求头（Headers）

HTTP **请求头**是描述请求元数据的键值对：

* 例如：`Content-Type`、`Authorization`、`Accept`、`User-Agent`。
* 在 `WebRequest` 中，你在构造对象时通过一个 `Map` 指针传入请求头。

#### 请求体（Request Body）

某些方法（尤其是 `POST`、`PUT`、`PATCH`）会包含一个 **请求体**：

* 原始二进制数据（`Buffer`）。
* 文本（`String`）。
* JSON 数据（`Map` 或 `List`，`WebRequest` 会帮你转换为 JSON）。

`WebRequest` 为不同的请求体类型提供了重载的 `doPost()` 函数。

#### 响应、状态码与错误

服务器返回的内容就是 **HTTP 响应**，其中包含：

* **状态码** – 例如 `200` 表示成功，`404` 表示 “Not Found”。
* **响应头** – 服务器端的元数据。
* **响应体** – 返回的数据（HTML、JSON、二进制等）。

在库中，这由 `WebResponse` 表示：

* `response.succeeded()` 用来检查状态码是否是 2xx。
* `response.getStatusCode()` 返回状态码。
* `response.asString()`、`response.asBuffer()` 或 `response.asJson()` 用来获取响应体。

#### 超时与阻塞行为

所有 `WebRequest` 操作都是**阻塞式**的：

* 像 `doGet()` 和 `doPost()` 这类调用在以下情况之一发生前都不会返回：

  * 请求成功完成，
  * 请求失败（例如网络错误），或
  * 达到指定的**超时时间**。

超时以**毫秒**为单位指定：

* `timeout = -1`（默认）表示*不超时*（一直等待直到完成）。
* `timeout = 10'000` 表示“最多等待 10 秒钟以完成操作”。

#### 单次使用的请求

一个 `WebRequest` 实例设计用于执行**一次**请求：

* 在你调用 `doGet`、`doPost` 或 `doRequest` 之后，`hasCompleted()` 会返回 `true`。
* 你应当把该请求视作**一次性**，下一次调用应创建一个新的 `WebRequest` 对象。

---

## 参考

下面每一节分别介绍 `attoboy::WebRequest` 的一个公开构造函数、成员方法或静态函数，包括：

* **Signature（声明）** – 头文件中的精确声明。
* **Synopsis（概要）** – 原始的一行 Doxygen 注释。
* **Parameters（参数）**和 **Return value（返回值）**。
* **In Depth（深入说明）** – 细节、注意事项与示例。

---

#### `WebRequest(const String &url, const Map &params = Map(), const Map &headers = Map())`

**Signature**

```cpp
WebRequest(const String &url, const Map &params = Map(),
           const Map &headers = Map());
```

**Synopsis**
Creates a request. params become query string; headers are sent with
request.

**Parameters**

* `url`  
  请求的基础 URL，包括协议（`http` 或 `https`）、主机名和路径。  
  示例：`"https://api.example.com/users"`。

* `params` *(可选)*  
  `Map`，其键值对会作为查询参数附加到 URL 上。若为空，则不会添加额外查询参数。

* `headers` *(可选)*  
  要随请求一起发送的头部 `Map`。键和值会被转换为字符串。若为空，则不会添加自定义请求头。

**Return value**

* *(构造函数；无返回值)*

**In Depth**

此构造函数完成一次 HTTP 请求所需的全部配置：

* 保存 URL。
* 接收一个可选的 **查询参数** `Map`，并将其转换为 URL 中的
  `?key=value&key2=value2`。
* 接收一个可选的 **请求头** `Map`，并将其附加到你在该 `WebRequest` 实例上调用的每一个请求方法。

典型参数用法：

* **参数映射表**（`params`）：  
  键和值通常是字符串（或可用来构造 `String` 的类型）。例如，
  `params.put("q", "hello")` 会变成 `?q=hello`。多个键会用 `&` 连接。

* **请求头映射表**（`headers`）：  
  同样是 `Map` 表示。例如，
  `headers.put("Authorization", "Bearer TOKEN")`。

构造时**不会**发送请求。真正的网络操作发生在你调用 `doGet`、`doPost` 或 `doRequest` 时。

**Example**

```cpp
String url("https://api.example.com/search");

// Build query parameters: ?q=attoboy&page=1
Map params;
params.put("q", "attoboy").put("page", 1);

// Build headers: Authorization: Bearer XYZ
Map headers;
headers.put("Authorization", "Bearer XYZ");

WebRequest request(url, params, headers);
WebResponse response = request.doGet(5000);  // 5-second timeout

if (response.succeeded()) {
  String body = response.asString();
  // ... use response body ...
}
```

*本示例构造了一个带查询参数和自定义请求头的 GET 请求，并在指定超时下执行。*

---

#### `WebRequest(const WebRequest &other)`

**Signature**

```cpp
WebRequest(const WebRequest &other);
```

**Synopsis**
Creates a copy (shares the underlying request).

**Parameters**

* `other` – 要拷贝的现有 `WebRequest`。

**Return value**

* *(构造函数；无返回值)*

**In Depth**

拷贝构造函数会创建一个新的 `WebRequest` 对象，并与 `other` **共享**内部实现。这是一次**浅拷贝**：

* 配置（URL、参数、请求头）实际上是共享的。
* 由于请求是一次性使用的，因此不要把它当作执行**同一请求两次**的手段。一旦任意一个共享实例完成请求，所有拷贝的 `hasCompleted()`
  都会返回 `true`。

拷贝构造函数主要用于在函数之间传递 `WebRequest`，而不转移底层实现的所有权。

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

*本示例拷贝了一个 `WebRequest`，但底层请求配置是共享的。*

---

#### `~WebRequest()`

**Signature**

```cpp
~WebRequest();
```

**Synopsis**
Destroys the request and frees resources.

**Parameters**

* *(无)*

**Return value**

* *(析构函数；无返回值)*

**In Depth**

析构函数会释放与此请求关联的任意底层资源，例如：

* 内部 WinHTTP 句柄。
* 为配置和状态分配的内存。

你无需显式调用“关闭”方法；让 `WebRequest` 离开作用域即可，由 RAII 负责清理资源。

销毁一个 `WebRequest` **不会**取消已经完成的操作。请求是同步的，并且在方法返回前就已经完成。

**Example**

```cpp
{
  WebRequest req("https://example.com/ping");
  WebResponse res = req.doGet();
  // ... process response ...
} // req is destroyed here; underlying resources are freed.
```

*本示例演示了当 `WebRequest` 离开作用域时自动清理资源。*

---

#### `WebRequest &operator=(const WebRequest &other)`

**Signature**

```cpp
WebRequest &operator=(const WebRequest &other);
```

**Synopsis**
Assigns another request (shares the underlying request).

**Parameters**

* `other` – 赋值来源的 `WebRequest`。

**Return value**

* 当前对象的引用 `*this`，以支持链式赋值。

**In Depth**

拷贝赋值运算符会：

* 释放当前对象的资源。
* 让左值与 `other` 共享底层请求实现。

与拷贝构造函数一样，这会产生**共享**状态。如果其中一个共享实例执行了请求，之后所有实例的 `hasCompleted()` 都会返回 `true`。

当你希望让某个变量重新指向一个已有的 `WebRequest` 时，这会很有用。

**Example**

```cpp
WebRequest a("https://example.com/a");
WebRequest b("https://example.com/b");

b = a; // Now b shares the underlying request with a

WebResponse r = b.doGet();
// a.hasCompleted() is now also true.
```

*本示例展示了如何通过赋值让两个变量共享同一个底层请求。*

---

#### `WebResponse doGet(int timeout = -1)`

**Signature**

```cpp
WebResponse doGet(int timeout = -1);
```

**Synopsis**
Performs an HTTP GET request. timeout in ms (-1 = infinite).

**Parameters**

* `timeout` *(可选)*  
  操作的最大等待时间（毫秒）：

  * `-1`（默认）：无超时（无限等待）。
  * 任意非负整数：在指定毫秒数后，库会将请求视为超时。

**Return value**

* 表示服务器响应的 `WebResponse`。  
  使用 `response.succeeded()`、`response.getStatusCode()` 及其他 `WebResponse` 方法进行检查。

**In Depth**

`doGet()` 会向构造函数中指定的 URL 发送一个 HTTP **GET** 请求，并附带：

* 来自 `params` 映射表的查询参数（追加到 URL）。
* 来自 `headers` 映射表的请求头。

该方法：

* 会阻塞直到请求完成或超时。
* 即使在错误情况下也会返回一个 `WebResponse`。例如：

  * 网络错误和超时通常会得到一个 `succeeded()` 为 `false` 的响应，或者其内容为空。
  * HTTP 非 2xx 状态码（如 `404`、`500`）同样会被表示为 `WebResponse` 对象；`succeeded()` 为 `false`，但 `getStatusCode()` 中会包含真实状态码。

一旦 `doGet()` 返回：

* `hasCompleted()` 会返回 `true`。
* 你不应在同一个 `WebRequest` 实例上再调用其他请求方法（`doPost()`、`doRequest()`）。

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

*本示例执行了一个带 10 秒超时的 GET 请求，并对结果进行了检查。*

---

#### `WebResponse doPost(int timeout = -1)`

**Signature**

```cpp
WebResponse doPost(int timeout = -1);
```

**Synopsis**
Performs an HTTP POST request with no body.

**Parameters**

* `timeout` *(可选)*  
  操作的最大等待时间（毫秒）（`-1` 表示不超时）。

**Return value**

* 表示服务器响应的 `WebResponse`。

**In Depth**

`doPost()` 会发送一个 HTTP **POST** 请求，特性如下：

* 不带显式请求体。
* 将 `params` 中的查询参数追加到 URL。
* 将 `headers` 中的请求头包含进请求。

这对某些仅根据裸 POST（通过查询字符串或请求头传参）触发操作的接口很有用，而非通过请求体提交数据。

其完成规则与一次性语义与 `doGet()` 相同。

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

*本示例发送了一个不带请求体但带查询参数的 POST 请求。*

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
  表示 JSON 对象的 `Map`。键和值会被转换为 JSON 对象字段。

* `timeout` *(可选)*  
  操作的最大等待时间（毫秒）（`-1` 表示不超时）。

**Return value**

* 表示服务器响应的 `WebResponse`。

**In Depth**

此重载是一个 **JSON 辅助方法**：

* `json` 映射表会被序列化为一个 JSON **对象**字符串，例如：

  * `{"name":"Alice","age":30}`。
* 序列化后的 JSON 会作为**请求体**发送。
* 实现会将其视作 JSON POST。通常，请求会包含合适的 `Content-Type` 请求头（取决于实现），或者你也可以在构造 `WebRequest` 时通过 `headers`
  映射表显式设置。

当远程 API 期望在 POST 请求体中接收 JSON 对象时应使用此重载。

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

*本示例通过 POST 一个 JSON 对象来创建或更新用户。*

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
  表示 JSON 数组的 `List`。其中元素会被转换为 JSON 数组项。

* `timeout` *(可选)*  
  操作的最大等待时间（毫秒）（`-1` 表示不超时）。

**Return value**

* 表示服务器响应的 `WebResponse`。

**In Depth**

此重载是 `doPost(const Map&)` 的数组对应版本：

* `List` 会被序列化为一个 JSON **数组**，例如：

  * `[1,2,3]`
  * `["one","two","three"]`
  * 如果其中存储的是 `Map` 值，则为 `[{"id":1},{"id":2}]`。

当某个 API 期望在请求体中接收值列表时（例如批量上传事件或 ID），非常适用。

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

*本示例在一次 POST 请求中发送了一个 JSON 事件数组。*

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
  包含要作为请求体发送的原始字节的 `Buffer`。

* `timeout` *(可选)*  
  操作的最大等待时间（毫秒）（`-1` 表示不超时）。

**Return value**

* 表示服务器响应的 `WebResponse`。

**In Depth**

此重载允许你发送任意**二进制**数据：

* 它不对编码做任何强制要求。
* 你应在构造 `WebRequest` 时，使用 `headers` 映射表设置合适的请求头（例如 `Content-Type` 或 `Content-Length`）。

典型用例：

* 上传文件。
* 发送压缩或加密 payload。
* 使用自定义的非 JSON 协议。

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

*本示例发送了带泛型二进制内容类型的二进制数据。*

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
  包含要作为请求体发送文本的 `String`。可以是 JSON、纯文本、URL 编码表单数据或其他任意文本协议。

* `timeout` *(可选)*  
  操作的最大等待时间（毫秒）（`-1` 表示不超时）。

**Return value**

* 表示服务器响应的 `WebResponse`。

**In Depth**

此重载用于发送**基于文本**的 payload，并由你完全控制请求体内容：

* 若要发送 JSON，可以自己构造 JSON 字符串（例如通过 `String` 和 `format`），然后在此传入。
* 若要发送表单数据（如 `key=value&other=value`），也可以手动构造该字符串并作为请求体发送。

同样，应在构造 `WebRequest` 时通过 `headers` 映射表设置合适的 `Content-Type` 请求头。

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

*本示例发送了一个纯文本 payload 作为 POST 请求体。*

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
  要使用的 HTTP 方法。典型值如：`"PUT"`、`"PATCH"`、`"DELETE"`、`"OPTIONS"` 等。通常应为大写字符串，但服务器可能也接受其他形式。

* `body`  
  包含原始请求体字节的 `Buffer`。对于像 `DELETE` 这样通常没有请求体的方法，可以传入一个空缓冲区。

* `timeout` *(可选)*  
  操作的最大等待时间（毫秒）（`-1` 表示不超时）。

**Return value**

* 表示服务器响应的 `WebResponse`。

**In Depth**

`doRequest()` 是最灵活的操作：

* 你可以发送服务器支持的**任意 HTTP 方法**。
* 你提供原始请求体字节，从而完全控制 payload。

常见模式：

* 带 JSON 请求体的 **PUT**：

  * 将 JSON 序列化为 `String`，再包装到 `Buffer` 中：

    * `Buffer body(jsonString);`
* 表示部分更新的 **PATCH**。
* 可带或不带小型请求体的 **DELETE**，具体取决于 API。

记得通过构造函数中的 `headers` 映射表设置如 `Content-Type` 等请求头。

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

*本示例发送了一个带 JSON 请求体的自定义 PATCH 请求。*

---

#### `String getUrl() const`

**Signature**

```cpp
String getUrl() const;
```

**Synopsis**
Returns the request URL.

**Parameters**

* *(无)*

**Return value**

* `String`，其中包含该请求的完整 URL，包括从参数映射表生成的任何查询字符串。

**In Depth**

`getUrl()` 返回 `WebRequest` 中存储的有效 URL。在构造之后：

* 它通常包含：

  * 你提供的基础 URL。
  * 从 `params` 映射表中添加的任何查询参数。

你可以在请求前或请求后调用此方法，以确认参数是否按预期应用。

**Example**

```cpp
Map params;
params.put("q", "attoboy");

WebRequest request("https://example.com/search", &params, nullptr);
String fullUrl = request.getUrl();
// e.g., "https://example.com/search?q=attoboy"
```

*本示例检查了 `WebRequest` 用于请求的最终 URL。*

---

#### `Map getParams() const`

**Signature**

```cpp
Map getParams() const;
```

**Synopsis**
Returns the query parameters map.

**Parameters**

* *(无)*

**Return value**

* 包含与此请求关联的查询参数的 `Map`。

**In Depth**

`getParams()` 返回 `WebRequest` 正在使用的查询参数 `Map`。这让你可以：

* 调试或记录请求附带了哪些参数。
* 将这些参数复制到另一个请求中。
* 对其进行检查或修改（通过使用返回的映射表构造新的 `WebRequest`，因为该方法是按值返回 `Map`）。

返回的 `Map` 不是实时视图；修改它**不会**改变原有 `WebRequest`。如需修改参数，请使用更新后的映射表创建一个新的 `WebRequest`。

**Example**

```cpp
Map params;
params.put("page", 2).put("limit", 50);

WebRequest request("https://example.com/users", &params, nullptr);

Map copy = request.getParams();
// copy can be logged, inspected, or reused when constructing a new request
```

*本示例获取了请求中使用的参数，以便检查或复用。*

---

#### `Map getHeaders() const`

**Signature**

```cpp
Map getHeaders() const;
```

**Synopsis**
Returns the headers map.

**Parameters**

* *(无)*

**Return value**

* 包含与此请求关联的请求头的 `Map`。

**In Depth**

`getHeaders()` 返回构造时提供的自定义请求头 `Map`。这在以下场景很有用：

* 调试请求头配置。
* 将请求头复制到另一个请求。
* 记录认证信息或内容类型设置（注意在日志中不要泄露机密）。

与 `getParams()` 一样，修改返回的 `Map` **不会**影响已存在的 `WebRequest`。如需更改请求头，请使用更新后的请求头映射表构造新的 `WebRequest`。

**Example**

```cpp
Map headers;
headers.put("Authorization", "Bearer TOKEN")
       .put("Accept", "application/json");

WebRequest request("https://example.com/data", nullptr, &headers);

Map copy = request.getHeaders();
// copy["Authorization"], copy["Accept"], etc.
```

*本示例获取了请求中使用的请求头，以便自省或复用。*

---

#### `bool hasCompleted() const`

**Signature**

```cpp
bool hasCompleted() const;
```

**Synopsis**
Returns true if a request has been completed (can only be done once).

**Parameters**

* *(无)*

**Return value**

* 若该 `WebRequest` 已经执行过任意请求方法（`doGet`、`doPost` 或 `doRequest`）且该操作已完成，则为 `true`。
* 若尚未执行任何请求，则为 `false`。

**In Depth**

`hasCompleted()` 反映了 `WebRequest` 的**一次性**特性：

* 一旦对 `doGet`、任意 `doPost` 重载或 `doRequest` 的调用结束（无论成功还是失败），`hasCompleted()` 都会返回 `true`。
* 完成之后，不应在同一实例上再次调用请求方法。应创建新的 `WebRequest`。

当你拥有共享或拷贝出来的多个 `WebRequest` 对象并想知道它们中是否已有某个发起过请求时，此函数尤其有用。

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

*本示例在调用 `doGet()` 前检查 `WebRequest` 是否已经被使用过。*

---

#### `static bool Download(const String &url, const String &savePath, const Map &params = Map(), const Map &headers = Map(), bool overwrite = true, int timeout = -1)`

**Signature**

```cpp
static bool Download(const String &url, const String &savePath,
                     const Map &params = Map(),
                     const Map &headers = Map(), bool overwrite = true,
                     int timeout = -1);
```

**Synopsis**
Downloads a file from URL to disk. Returns true on success.

**Parameters**

* `url`  
  要下载的资源的 URL。等价于传给 `WebRequest` 构造函数的 `url`。

* `savePath`  
  磁盘上文件路径（`String`），下载的数据会写入该路径。路径应对当前用户和系统是有效的。

* `params` *(可选)*  
  要追加到 URL 的查询参数 `Map`。若为空，则不会添加额外参数。

* `headers` *(可选)*  
  要随请求一起发送的请求头 `Map`。若为空，则不会添加自定义请求头。

* `overwrite` *(可选)*  
  若为 `true`（默认），则允许覆盖 `savePath` 上已存在的同名文件。若为 `false`，则现有文件应导致函数失败（具体取决于实现）。

* `timeout` *(可选)*  
  整个下载操作的最大等待时间（毫秒）（`-1` 表示不超时）。

**Return value**

* 若下载成功完成且文件已写入 `savePath`，则为 `true`。
* 若发生错误（网络失败、超时、权限问题等），则为 `false`。

**In Depth**

`Download()` 是一个**便捷函数**，封装了一个常见流程：

1. 使用给定的 `url`、`params` 和 `headers` 构造一个 `WebRequest`。
2. 执行请求（通常是 `GET`）。
3. 将响应体直接写入磁盘上的 `savePath`。

它简化了下载任务，使你无需：

* 手动调用 `doGet()`。
* 将响应体提取为 `Buffer` 或 `String`。
* 自己打开 `File` 并写入数据。

行为说明：

* 如果 `overwrite` 为 `false` 且目标文件已存在，下载应失败并返回 `false`（你可以视为“文件已存在”）。
* 你有责任确保 `savePath` 所在目录已存在，并且你拥有写入权限。
* 该函数是阻塞式的，并遵守指定的超时时间。

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

bool ok = WebRequest::Download(url, savePath, params, headers,
                               true,   // overwrite if exists
                               30000); // 30-second timeout

if (!ok) {
  // handle download failure
}
```

*本示例将远程 PDF 下载到指定路径，并使用查询参数、自定义请求头、启用覆盖以及 30 秒超时。*