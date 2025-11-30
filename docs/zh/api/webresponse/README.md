# WebResponse

## 概览

`attoboy::WebResponse` 类表示通过 `attoboy::WebRequest` 发起的 HTTP 请求的结果。它以一种方便、适合 RAII 的方式提供对以下内容的检查：

* 请求是否**成功**（2xx 状态码）。
* 经过重定向后的最终 **URL**。
* HTTP **状态码**以及可读的 **原因短语**。
* 所有 **响应头**，以 `attoboy::Map` 形式呈现。
* **响应体**，可表示为：

  * 字符串（`String`），
  * 原始字节（`Buffer`），
  * 或解析后的 JSON 对象（`Map`）。

通常你不会直接构造一个 `WebResponse`。典型流程是：

1. 创建一个 `WebRequest`。
2. 调用其方法，如 `doGet()` 或 `doPost()`。
3. 由该方法返回一个 `WebResponse`。

典型用法示例：

```cpp
using namespace attoboy;

WebRequest request("https://api.example.com/info");
WebResponse response = request.doGet();

if (response.succeeded()) {
  String body = response.asString();
  // 使用响应体...
}
```

`WebResponse` 的设计目标是：

* **易于检查**——提供状态、头部与响应体的高层接口。
* **与 attoboy 类型集成良好**——使用 `String`、`Map` 和 `Buffer` 表示内容及元数据。
* **可安全拷贝**——拷贝 `WebResponse` 会共享其底层实现，同时保持简单的值语义接口。

### HTTP 响应结构

如果你对 Web 编程还不熟悉，可以把 HTTP 响应看作由三部分组成：

1. **状态行**  
   包含：

   * HTTP 版本（例如 `HTTP/1.1`），
   * **状态码**（例如 `200`、`404`），
   * **原因短语**（例如 `"OK"`、`"Not Found"`）。
     在 `WebResponse` 中，由以下方法表示：
   * `getStatusCode()`
   * `getStatusReason()`
   * `succeeded()`（检查状态码是否在 200–299 范围内）。

2. **响应头**  
   一组描述响应的键值对：

   * 示例：`Content-Type`、`Content-Length`、`Set-Cookie`。  
     在 `WebResponse` 中，可以通过以下方式访问响应头：
   * `getResponseHeaders()`，返回一个 `Map`。

3. **响应体**  
   实际的负载内容：

   * HTML、JSON、二进制数据（图像、PDF 等）。  
     在 `WebResponse` 中，可以通过以下方式访问：
   * `asString()` 获取文本（UTF-8），
   * `asBuffer()` 获取原始字节，
   * `asJson()` 当你期望响应体是一个 JSON 对象时。

### 典型使用流程

常见的工作流程：

1. 发起请求：

   ```cpp
   WebRequest req("https://api.example.com/user/42");
   WebResponse res = req.doGet();
   ```

2. 检查整体状态：

   ```cpp
   if (!res.succeeded()) {
     int code = res.getStatusCode();
     String reason = res.getStatusReason();
     // 根据状态码 / 原因处理错误
   }
   ```

3. 如有需要，读取响应头：

   ```cpp
   Map headers = res.getResponseHeaders();
   String contentType = headers.get("Content-Type", String());
   ```

4. 以最合适的形式访问响应体：

   * JSON API：`const Map json = res.asJson();`
   * 文本：`String body = res.asString();`
   * 二进制：`Buffer data = res.asBuffer();`

---

## 参考

下面的每一条目都涵盖了 `attoboy::WebResponse` 的一个公有构造函数、赋值运算符或方法，包括：

* **Signature** – 头文件中的精确声明。
* **Synopsis** – 原始 Doxygen 一行注释。
* **Parameters** 和 **Return value**。
* **In Depth** – 额外说明和简短示例。

所有示例都假设：

```cpp
using namespace attoboy;
```

---

#### `WebResponse(const WebResponse &other)`

**Signature**

```cpp
WebResponse(const WebResponse &other);
```

**Synopsis**  
创建一个拷贝（共享底层响应）。

**Parameters**

* `other` – 要拷贝的现有 `WebResponse`。

**Return value**

* *（构造函数；不适用）*

**In Depth**

拷贝构造函数会创建一个新的 `WebResponse`，该对象与 `other` **共享**相同的底层实现。这是浅拷贝：

* 状态码、响应头和响应体在内部是共享的。

由于从用户视角看 `WebResponse` 在逻辑上是不可变的，因此这种共享既安全又高效。

**Example**

```cpp
WebRequest request("https://example.com/info");
WebResponse original = request.doGet();

// 创建一个共享同一底层响应的拷贝
WebResponse copy(original);

int codeOriginal = original.getStatusCode();
int codeCopy     = copy.getStatusCode();   // 相同的状态码
```

*此示例展示了拷贝一个响应并访问共享状态信息。*

---

#### `~WebResponse()`

**Signature**

```cpp
~WebResponse();
```

**Synopsis**  
销毁响应并释放资源。

**Parameters**

* *（无）*

**Return value**

* *（析构函数；不适用）*

**In Depth**

析构函数会释放与该响应相关的所有底层资源，例如：

* 内部网络句柄。
* 用于存储响应体的缓冲区。

**Example**

```cpp
WebResponse response = WebRequest("https://example.com").doGet();
{
  WebResponse innerCopy(response);
  // 在此使用 innerCopy...
} // innerCopy 被销毁；response 仍然有效

// response 在此仍然可以使用。
String body = response.asString();
```

*此示例说明了可以安全地创建和销毁响应的拷贝，而原始响应对象仍然保持有效。*

---

#### `WebResponse &operator=(const WebResponse &other)`

**Signature**

```cpp
WebResponse &operator=(const WebResponse &other);
```

**Synopsis**  
赋值为另一个响应（共享底层响应）。

**Parameters**

* `other` – 要赋值自的 `WebResponse`。

**Return value**

* 对 `*this` 的引用，以支持赋值链式调用。

**In Depth**

拷贝赋值运算符会：

* 释放当前响应所持有的资源。
* 让此对象与 `other` 共享底层响应数据。

赋值之后：

* `*this` 和 `other` 都指向同一个响应。
* 诸如 `getStatusCode()` 和 `asString()` 等方法，在两个对象上都会返回相同的值。

自我赋值是安全的，由实现负责处理。

**Example**

```cpp
WebResponse a = WebRequest("https://example.com/a").doGet();
WebResponse b = WebRequest("https://example.com/b").doGet();

// 将 b 重新赋值，使其与 a 共享相同的响应
b = a;

if (b.succeeded()) {
  String body = b.asString();  // 与 a.asString() 相同的响应体
}
```

*此示例展示了将一个响应重新赋值为共享另一个响应的数据。*

---

#### `bool succeeded() const`

**Signature**

```cpp
bool succeeded() const;
```

**Synopsis**  
如果状态码在 200–299 范围内，则返回 true。

**Parameters**

* *（无）*

**Return value**

* 当 HTTP 状态码在 `200` 到 `299`（含）之间时返回 `true`。
* 否则返回 `false`。

**In Depth**

`succeeded()` 提供了一个简单的高层方法来判断请求是否按照 HTTP 约定被视作成功：

* 2xx 状态码代表成功：

  * `200 OK`
  * `201 Created`
  * `204 No Content` 等。

对于任何非 2xx 状态（包括重定向、客户端错误和服务器错误）：

* `succeeded()` 会返回 `false`。
* 你仍然可以通过 `getStatusCode()` 和 `asString()`/`asBuffer()` 检查状态码和响应体。

需要注意的是，网络层问题（例如超时或连接失败）可能会导致得到的响应中 `getStatusCode()` 为 `0` 或某个哨兵值（视实现而定）。你应当将 `!succeeded()` 视作“此次操作未产生成功的 HTTP 2xx 结果”。

**Example**

```cpp
WebRequest request("https://api.example.com/data");
WebResponse response = request.doGet(5000);

if (response.succeeded()) {
  String body = response.asString();
  // 处理成功响应体
} else {
  int code = response.getStatusCode();
  String reason = response.getStatusReason();
  // 处理非 2xx 状态或网络错误
}
```

*此示例在处理响应体之前先检查请求是否成功。*

---

#### `String getUrl() const`

**Signature**

```cpp
String getUrl() const;
```

**Synopsis**  
返回重定向后的最终 URL。

**Parameters**

* *（无）*

**Return value**

* 包含产生此响应的最终 URL 的 `String`，包括中间可能跟随的重定向。

**In Depth**

在以下场景中，`getUrl()` 尤其有用：

* 你从一个可能发生重定向的 URL 开始（例如从 `http` 重定向到 `https` 或跳转到其他路径）。
* 你希望了解底层 HTTP 客户端在处理重定向后得到的**最终** URL。

例如，如果你请求 `http://example.com`，而服务端重定向到 `https://www.example.com/home`，那么 `getUrl()` 会返回后者。

这在以下场景中特别有帮助：

* 下载那些可能随着时间迁移位置的资源。
* 调试重定向链。

**Example**

```cpp
WebRequest request("http://example.com"); // 可能重定向到 https
WebResponse response = request.doGet();

String finalUrl = response.getUrl();
// 例如 "https://www.example.com/"
```

*此示例展示了在可能重定向后获取最终 URL。*

---

#### `int getStatusCode() const`

**Signature**

```cpp
int getStatusCode() const;
```

**Synopsis**  
返回 HTTP 状态码（如 200、404）。

**Parameters**

* *（无）*

**Return value**

* 由服务器返回的整数型 HTTP 状态码，例如 `200`、`404` 或 `500`。

**In Depth**

**状态码**是服务器如何处理请求的主要指示器：

* **2xx** – 成功（`200 OK`、`201 Created`、`204 No Content` 等）。
* **3xx** – 重定向。
* **4xx** – 客户端错误（`400 Bad Request`、`401 Unauthorized`、`404 Not Found` 等）。
* **5xx** – 服务器错误（`500 Internal Server Error` 等）。

你可以通过 `getStatusCode()` 来：

* 实现更细致的错误处理。
* 在 `succeeded()` 为 `false` 时区分不同类型的失败情况。

对于网络错误，状态码可能为 `0` 或其他由实现定义的值。通常应当将其与 `succeeded()` 以及响应体结合使用，以便进行诊断。

**Example**

```cpp
WebResponse response = WebRequest("https://example.com/missing").doGet();

int code = response.getStatusCode();  // 例如 404
if (code == 404) {
  // 针对 “Not Found” 的特定处理
}
```

*此示例检查特定的 HTTP 状态码。*

---

#### `String getStatusReason() const`

**Signature**

```cpp
String getStatusReason() const;
```

**Synopsis**  
返回 HTTP 状态原因短语（如 "OK"、"Not Found"）。

**Parameters**

* *（无）*

**Return value**

* 包含与状态码对应的原因短语的 `String`，例如 `"OK"`、`"Created"` 或 `"Not Found"`。

**In Depth**

**原因短语**是与状态码一起给出的简短、可读的说明。它主要用于：

* 日志记录与调试。
* 向用户或开发者展示更为信息化的消息。

通常不会使用原因短语作为逻辑判断依据；此类逻辑应以数值状态码为准。但是，在诊断问题时，原因短语能够提供有价值的上下文信息。

**Example**

```cpp
WebResponse response = WebRequest("https://example.com/missing").doGet();

int code = response.getStatusCode();
String reason = response.getStatusReason();
// 例如 code == 404, reason == "Not Found"
```

*此示例同时获取数值状态码和可读的原因短语。*

---

#### `Map getResponseHeaders() const`

**Signature**

```cpp
Map getResponseHeaders() const;
```

**Synopsis**  
返回响应头的映射。

**Parameters**

* *（无）*

**Return value**

* 包含所有响应头的 `Map`。

  * 键：作为 `String` 的响应头名称。
  * 值：作为 `String`（或可转换为 `String` 的类型）的响应头值。

**In Depth**

`getResponseHeaders()` 允许你检查服务器提供的元数据，例如：

* `Content-Type`
* `Content-Length`
* `Date`
* `Server`
* 自定义头（如 `X-Request-ID`、`X-RateLimit-Limit`）

注意事项：

* HTTP 头名称是大小写不敏感的，但存储在 `Map` 中的实际大小写取决于实现。一般应当使用在典型响应中看到的大小写来查找，或者在需要时自行统一大小写。
* 返回的 `Map` 是一个**拷贝**。修改它并不会改变原始响应。

常见用途：

* 在解析 JSON 之前先验证 `Content-Type`。
* 检查缓存或限流相关的响应头。
* 调试服务端配置问题。

**Example**

```cpp
WebResponse response = WebRequest("https://api.example.com/data").doGet();

// 检查响应头
Map headers = response.getResponseHeaders();

String contentType = headers.get("Content-Type", String());
String requestId   = headers.get("X-Request-ID", String());

// 使用这些头部值进行日志或条件逻辑
```

*此示例从响应中读取一些常见的响应头。*

---

#### `Map asJson() const`

**Signature**

```cpp
Map asJson() const;
```

**Synopsis**  
将响应体解析为 JSON 对象。如果不是 JSON 对象则返回空 Map。

**Parameters**

* *（无）*

**Return value**

* 表示从响应体解析得到的 JSON 对象的 `Map`，或者在以下情况时返回空 `Map`：

  * 响应体为空，
  * 响应体不是合法的 JSON，
  * 顶层 JSON 不是一个 JSON **对象**（例如是一个裸数组或基础类型）。

**In Depth**

`asJson()` 是为基于 JSON 的 API 提供的便捷函数：

* 它尝试将响应体解析为 JSON。
* 如果解析成功且顶层值是一个 JSON 对象（`{ ... }`），则返回一个表示该对象的 `Map`。
* 如果解析失败或顶层值不是对象（例如 `[...]`），则返回一个空的 `Map`。

关键点：

* 返回的 `Map` 是一个由你持有的值；你可以按需存储它。
* 使用 `isEmpty()` 检查 JSON 解析是否成功。
* 可以通过 `Map` 的 `get`、`hasKey`、`keys` 等方法访问其中的内容。

如果你预期顶层是一个 JSON **数组**，应当手动解析响应体（例如使用 `List::FromJSONString(response.asString())`），因为 `asJson()` 专门用于 JSON 对象。

**Example**

```cpp
WebResponse response = WebRequest("https://api.example.com/user/42").doGet();

if (!response.succeeded()) {
  // 处理错误...
} else {
  Map json = response.asJson();
  if (!json.isEmpty()) {
    // 使用带默认值的方式提取字段
    String name = json.get("name", String("Unknown"));
    int age     = json.get("age", 0);

    // 使用解析得到的数据...
  } else {
    // 响应体不是 JSON 对象
    String rawBody = response.asString();
    // 处理非预期格式...
  }
}
```

*此示例将响应体解析为 JSON 对象，并使用带默认值的方式安全访问字段。*

---

#### `String asString() const`

**Signature**

```cpp
String asString() const;
```

**Synopsis**  
以字符串形式返回响应体。

**Parameters**

* *（无）*

**Return value**

* 将响应体按 UTF-8 文本解释后得到的 `String`。

**In Depth**

`asString()` 是访问文本响应的主要方式：

* 它以 UTF-8 解码响应体字节并返回一个 `String`。
* 适用于：

  * JSON，
  * HTML，
  * 纯文本，
  * 或任何其它兼容 UTF-8 的内容。

如果响应是二进制内容（例如图像、PDF 或压缩数据），建议优先使用 `asBuffer()`。在二进制数据上调用 `asString()` 可能会得到不可读文本或无效字符。

返回的 `String` 是一个拷贝；即使 `WebResponse` 被销毁，它仍然保持有效。

**Example**

```cpp
WebResponse response = WebRequest("https://example.com/hello.txt").doGet();

if (response.succeeded()) {
  String text = response.asString();
  // text 现在保存了整个响应体的 UTF-8 文本
}
```

*此示例从服务器读取一个纯文本文件并以字符串形式获取。*

---

#### `Buffer asBuffer() const`

**Signature**

```cpp
Buffer asBuffer() const;
```

**Synopsis**  
以缓冲区形式返回响应体。

**Parameters**

* *（无）*

**Return value**

* 包含响应体原始字节的 `Buffer`。

**In Depth**

`asBuffer()` 是 `asString()` 的低层对应接口：

* 它以二进制数据形式返回响应体，不进行任何解码，也不对编码做任何假设。
* 非常适合以下场景：

  * 下载图像。
  * 处理二进制协议。
  * 处理压缩或加密数据。

由于返回的 `Buffer` 是一个拷贝，你可以安全地对其进行修改、切片，或在 `WebResponse` 生命周期结束后继续保存。

你可以将 `asBuffer()` 与其它 `Buffer` 方法结合使用：

* `length()`
* `toBase64()`
* `compress()` / `decompress()`
* `crypt()` 用于加解密

**Example**

```cpp
WebResponse response = WebRequest("https://example.com/image.png").doGet();

if (response.succeeded()) {
  Buffer data = response.asBuffer();

  int size = data.length();
  // 你可以使用 Path/File 将 'data' 写入磁盘，或进一步处理。
}
```

*此示例将一个二进制图像作为原始字节缓冲区获取。*