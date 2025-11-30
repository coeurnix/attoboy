# Buffer

## 概览

`attoboy::Buffer` 类是 attoboy 库中核心的**二进制数据容器**。`String` 适用于人类可读文本，而 `Buffer` 则专门用于**原始字节**：文件内容、网络报文、加密载荷、压缩数据以及任何其他非文本信息。

主要特性：

* **可变**：`append`、`prepend`、`insert`、`remove` 和 `reverse` 等操作会就地修改缓冲区并返回 `*this` 以便链式调用。
* **面向字节**：所有索引和长度都以字节为单位，而非字符。
* **功能丰富**：支持 LZ4 压缩、ChaCha20 加密、Base64 编码，以及与 `String` 的互相转换。
* **基于 RAII**：缓冲区的内存自动管理；你不需要自己调用 `free()`。

从高层来看，可以把 `Buffer` 看作是 `std::vector<unsigned char>` 与各类压缩、加密、编码工具库的一个小而专注的替代品——以适合小型二进制的方式实现。

### 字节与二进制数据

与处理**Unicode 字符**的 `String` 不同，`Buffer` 处理的是**字节**：

* 每个元素是 `unsigned char`（0–255）。
* 所有索引都是从起始位置（0 基）算起的字节偏移。
* 除非你显式调用 `toString()` 或从 `String` 构造，否则不会尝试将字节解释为文本。

典型用途包括：

* 通过 `File` 和 `Path` 读写文件。
* 通过 `WebResponse::asBuffer()` 处理来自 `WebRequest` 的二进制响应。
* 管理压缩或加密的数据。

### 可变性与链式调用

`Buffer` 上大多数会修改内容的操作：

* **就地**修改缓冲区。
* 返回 `Buffer &`，这样你可以进行链式调用：

```cpp
using namespace attoboy;

Buffer buf;
buf.append("header")
   .append("payload")
   .reverse()
   .trim();
```

这与不可变的 `String` 不同，后者总是返回新值。

### 压缩（LZ4）与编码（Base64）

`Buffer` 支持对二进制数据的两类常用操作：

* **LZ4 压缩**（`compress()` / `decompress()`）：

  * LZ4 是一种快速、无损的压缩算法。
  * 使用 `compress()` 可以减少对可压缩数据（如文本、JSON、日志）的存储或带宽占用。
  * 使用 `decompress()` 可以对之前由 `compress()` 生成的数据进行逆向操作。

* **Base64 编码**（`toBase64()` / `fromBase64()`）：

  * Base64 将任意字节转换为仅使用 ASCII 字符的 `String`。
  * 在必须通过只支持文本的通道（如 JSON、某些 API、日志）传输二进制数据时非常有用。

### 使用 ChaCha20 的加密

`Buffer::crypt()` 提供基于 **ChaCha20** 的对称加解密，这是一个现代流密码：

* **对称**意味着加密与解密使用的是相同的函数和密钥。
* 你必须提供：

  * 至少 32 字节的**密钥**。
  * 至少 12 字节的 **nonce**（有时称为初始化向量）。

关键性质是：

```cpp
encrypted = plain.crypt(key, nonce);
decrypted = encrypted.crypt(key, nonce); // 相同的 key+nonce
// decrypted 包含原始字节
```

这让在文件、配置或网络消息中保护敏感数据变得简单直接。

### 与 `String` 及其他 attoboy 类型的互操作性

`Buffer` 能与库中的其他部分自然集成：

* 可以从 `String` 的字节构造 `Buffer`，并通过 `toString()` 转回。
* 使用 `File` 将 `Buffer` 对象读写到磁盘或套接字。
* 使用 `WebResponse::asBuffer()` 获取 HTTP 响应体的原始字节。
* 使用 `Buffer::toBase64()` 和 `Buffer::fromBase64()` 在二进制与文本表示之间转换。

---

## 参考

下面的每个条目涵盖 `attoboy::Buffer` 的一个公共构造函数、方法、静态函数或运算符。每个条目包含：

* **Signature** – 头文件中的精确声明。
* **Synopsis** – 原始的一行 Doxygen 注释。
* **Parameters** 与 **Return value** 描述。
* **In Depth** – 详细信息、注意事项以及一个简短用例示例。

> **注意：** 所有示例都假定已使用 `using namespace attoboy;`。

---

#### `Buffer()`

**Signature**

```cpp
Buffer();
```

**Synopsis**  
创建一个空缓冲区。

**Parameters**

* *(无)*

**Return value**

* *(构造函数；不适用)*

**In Depth**

默认构造函数会创建一个缓冲区，具有：

* 长度为 `0`。
* 无已分配的内容（或仅含最小内部表示）。
* `isEmpty()` 返回 `true`。

通常你会先从一个空缓冲区开始，然后通过 `append`、`prepend` 或 `insert` 来构建内容。

**Example**

```cpp
Buffer buf;
bool empty = buf.isEmpty();  // true
```

*此示例构造了一个空缓冲区并检查其中是否包含任何字节。*

---

#### `Buffer(int capacity)`

**Signature**

```cpp
Buffer(int capacity);
```

**Synopsis**  
创建一个具有预留容量的空缓冲区。

**Parameters**

* `capacity` – 内部要预留的字节数量。

**Return value**

* *(构造函数；不适用)*

**In Depth**

此构造函数会为至少 `capacity` 个字节分配内部存储，但会保持**长度**为零。当你预先知道大约要追加多少字节并希望尽量减少重新分配时非常有用。

即使预留了容量：

* `length()` 仍为 `0`。
* `isEmpty()` 仍为 `true`。

**Example**

```cpp
Buffer buf(1024);  // 预留约 1 KB 空间
buf.append("header");
```

*此示例先预留容量以便后续高效增长，再追加数据。*

---

#### `Buffer(const String &str)`

**Signature**

```cpp
Buffer(const String &str);
```

**Synopsis**  
通过复制字符串的字节来创建一个缓冲区。

**Parameters**

* `str` – 其 UTF-8 字节将被复制到缓冲区的 `String`。

**Return value**

* *(构造函数；不适用)*

**In Depth**

此构造函数会将 `str` 的原始 UTF-8 字节（即 `str.c_str()` 与 `str.byteLength()` 返回的内容）复制到缓冲区。缓冲区长度变为字符串的字节数，并且不会在存储长度之外额外添加空终止符。

当你需要把文本作为二进制来进行哈希、加密或其它二进制协议处理时，使用该构造函数。

**Example**

```cpp
String text("Hello, world");
Buffer buf(text);  // 包含字符串的 UTF-8 字节
int len = buf.length();  // 等于 text.byteLength()
```

*此示例将一个 `String` 转换为包含相同 UTF-8 字节的二进制缓冲区。*

---

#### `Buffer(const unsigned char *ptr, int size)`

**Signature**

```cpp
Buffer(const unsigned char *ptr, int size);
```

**Synopsis**  
通过从指针复制字节来创建缓冲区。

**Parameters**

* `ptr` – 指向一段字节序列的指针。
* `size` – 要从 `ptr` 复制的字节数。

**Return value**

* *(构造函数；不适用)*

**In Depth**

此构造函数是从原始内存到 `Buffer` 的低层桥梁。它会从 `ptr` 复制恰好 `size` 个字节到缓冲区。这些字节被视为不透明数据；不会进行任何文本解释。

适用场景包括：

* 从外部 API 接收到字节。
* 你有一个定长数组或结构体，希望按原样存储。

**Example**

```cpp
unsigned char raw[4] = {0x01, 0x02, 0x03, 0x04};
Buffer buf(raw, 4);
int len = buf.length();  // 4
```

*此示例从一个小的内存字节数组构造了缓冲区。*

---

#### `Buffer(const Buffer &other)`

**Signature**

```cpp
Buffer(const Buffer &other);
```

**Synopsis**  
创建另一个缓冲区的副本。

**Parameters**

* `other` – 要复制的现有 `Buffer`。

**Return value**

* *(构造函数；不适用)*

**In Depth**

拷贝构造函数会创建一个与 `other` 拥有相同内容的新 `Buffer`。由于 `Buffer` 是可变的，此后对任一缓冲区的修改都不会影响另一个。

**Example**

```cpp
Buffer original;
original.append("data");

Buffer copy(original);
// copy 现在与 original 拥有相同的字节
```

*此示例复制了一个已有缓冲区的内容。*

---

#### `~Buffer()`

**Signature**

```cpp
~Buffer();
```

**Synopsis**  
销毁缓冲区并释放内存。

**Parameters**

* *(无)*

**Return value**

* *(析构函数；不适用)*

**In Depth**

当一个 `Buffer` 离开作用域时，其析构函数会释放所有内部分配的内存。这使用 RAII 语义；你无需手动记住去 free 或 delete 任何东西。

**Example**

```cpp
{
  Buffer temp;
  temp.append("temporary");
} // temp 在此处被销毁；内存被释放
```

*此示例展示了缓冲区在离开作用域时的自动清理。*

---

#### `Buffer &operator=(const Buffer &other)`

**Signature**

```cpp
Buffer &operator=(const Buffer &other);
```

**Synopsis**  
将另一个缓冲区赋值给当前缓冲区。

**Parameters**

* `other` – 其内容应复制到 `*this` 的缓冲区。

**Return value**

* 指向 `*this` 的引用，以支持连等赋值。

**In Depth**

赋值运算符会用 `other` 的副本替换左值（左侧缓冲区）的内容。缓冲区中任何先前存在的字节都会被丢弃。自赋值是安全的。

**Example**

```cpp
Buffer a;
a.append("first");

Buffer b;
b.append("second");

a = b;  // a 现在包含 "second"
```

*此示例将一个缓冲区的内容赋值给另一个缓冲区。*

---

#### `int length() const`

**Signature**

```cpp
int length() const;
```

**Synopsis**  
返回缓冲区中的字节数。

**Parameters**

* *(无)*

**Return value**

* 缓冲区当前的长度（以字节为单位）。

**In Depth**

`length()` 是度量缓冲区当前持有数据量的主要方式。它不包括内部为扩容预留的额外容量。

在遍历字节、校验大小约束或准备将缓冲区写入文件或套接字时，应使用此方法。

**Example**

```cpp
Buffer buf;
buf.append("ABC");
int n = buf.length();  // 3
```

*此示例检查缓冲区中存储了多少个字节。*

---

#### `bool isEmpty() const`

**Signature**

```cpp
bool isEmpty() const;
```

**Synopsis**  
如果缓冲区为空则返回 true。

**Parameters**

* *(无)*

**Return value**

* 若 `length()` 为 `0` 则为 `true`，否则为 `false`。

**In Depth**

`isEmpty()` 是一个便捷函数，等价于检查 `length() == 0`。它有助于在某些操作前尽早返回，或在需要数据存在的操作前做保护性判断。

**Example**

```cpp
Buffer buf;
if (buf.isEmpty()) {
  buf.append("default");
}
```

*此示例在缓冲区为空时写入默认字节。*

---

#### `const unsigned char *c_ptr(int *len) const`

**Signature**

```cpp
const unsigned char *c_ptr(int *len) const;
```

**Synopsis**  
返回指向数据的指针，并将 len 设置为大小。

**Parameters**

* `len` – 指向 `int` 的指针，会被设置为当前的 `length()`。

**Return value**

* 指向内部只读字节数组的指针；如果缓冲区为空，则可能为 `nullptr`（依赖于具体实现）。

**In Depth**

`c_ptr()` 提供了缓冲区内容的低层视图：

* 它会将 `*len` 设置为有效字节数。
* 它返回的指针在以下条件满足时保持有效：

  * `Buffer` 存在，并且
  * 你没有调用可能导致重新分配的修改操作（如 `append`、`insert` 或 `remove`）。

你不得通过该指针修改内存。

**Example**

```cpp
Buffer buf;
buf.append("ABC");

int len = 0;
const unsigned char *ptr = buf.c_ptr(&len);
// len == 3; ptr 指向字节 'A'、'B'、'C'
```

*此示例获取了一个原始指针和长度，以便与底层 API 互操作。*

---

#### `Buffer &clear()`

**Signature**

```cpp
Buffer &clear();
```

**Synopsis**  
移除所有字节。返回此缓冲区以便链式调用。

**Parameters**

* *(无)*

**Return value**

* `*this` 的引用，支持链式调用。

**In Depth**

`clear()` 会将缓冲区重置为空状态（`length() == 0`）。它可能会保留预留容量，以便在未来的追加操作中复用，但逻辑内容已被清除。

**Example**

```cpp
Buffer buf;
buf.append("data");
buf.clear();  // buf 现在为空
```

*此示例移除了缓冲区中的所有字节，以便重新使用。*

---

#### `Buffer &append(const String &str)`

**Signature**

```cpp
Buffer &append(const String &str);
```

**Synopsis**  
追加字符串的字节。返回此缓冲区以便链式调用。

**Parameters**

* `str` – 其 UTF-8 字节将被追加到缓冲区的 `String`。

**Return value**

* `*this` 的引用。

**In Depth**

此方法会：

* 取得 `str` 的原始 UTF-8 字节。
* 将这些字节追加到当前缓冲区内容的末尾。
* 相应增加 `length()`。

它不会添加额外的终止符或分隔符。

**Example**

```cpp
Buffer buf;
buf.append("header")
   .append("payload");
// 缓冲区现在包含 "headerpayload" 对应的字节
```

*此示例将两个字符串的字节追加起来以形成一段连续的二进制序列。*

---

#### `Buffer &append(const Buffer &other)`

**Signature**

```cpp
Buffer &append(const Buffer &other);
```

**Synopsis**  
追加另一个缓冲区的字节。返回此缓冲区以便链式调用。

**Parameters**

* `other` – 其字节将被追加的 `Buffer`。

**Return value**

* `*this` 的引用。

**In Depth**

此方法会将 `other` 中的所有字节复制到当前缓冲区的末尾。等价于：

```cpp
int len = 0;
const unsigned char *ptr = other.c_ptr(&len);
append(ptr, len);
```

**Example**

```cpp
Buffer a;
a.append("ABC");

Buffer b;
b.append("DEF");

a.append(b);  // a 现在包含 "ABCDEF"
```

*此示例通过将第二个缓冲区追加到第一个，来连接两个缓冲区。*

---

#### `Buffer &append(const unsigned char *ptr, int size)`

**Signature**

```cpp
Buffer &append(const unsigned char *ptr, int size);
```

**Synopsis**  
从指针追加字节。返回此缓冲区以便链式调用。

**Parameters**

* `ptr` – 指向要追加字节的指针。
* `size` – 要追加的字节数。

**Return value**

* `*this` 的引用。

**In Depth**

这是针对原始内存的底层追加操作：

* 它会从 `ptr` 起复制 `size` 个字节。
* 不会检查或修改这些字节。
* 你有责任保证 `ptr` 至少指向 `size` 个有效字节。

**Example**

```cpp
unsigned char footer[2] = {0x0D, 0x0A};  // CR LF
Buffer buf;
buf.append("line").append(footer, 2);
// buf 现在是 "line\r\n"
```

*此示例将一个原始的双字节序列追加到现有数据之后。*

---

#### `Buffer &prepend(const String &str)`

**Signature**

```cpp
Buffer &prepend(const String &str);
```

**Synopsis**  
在前面插入字符串的字节。返回此缓冲区以便链式调用。

**Parameters**

* `str` – 其字节将被插入到开头的 `String`。

**Return value**

* `*this` 的引用。

**In Depth**

`prepend()` 会将 `str` 的字节插入到缓冲区的最前面，并将现有字节向更高索引移动。

**Example**

```cpp
Buffer buf;
buf.append("body");
buf.prepend("header");
// 缓冲区内容为 "headerbody"
```

*此示例在已有数据前插入了一个头部。*

---

#### `Buffer &prepend(const Buffer &other)`

**Signature**

```cpp
Buffer &prepend(const Buffer &other);
```

**Synopsis**  
在前面插入另一个缓冲区的字节。返回此缓冲区以便链式调用。

**Parameters**

* `other` – 其字节将被插入到开头的缓冲区。

**Return value**

* `*this` 的引用。

**In Depth**

此方法会将 `other` 中的所有字节复制到当前缓冲区的最前面，位于任何已有数据之前。

**Example**

```cpp
Buffer header;
header.append("HDR");

Buffer payload;
payload.append("DATA");

payload.prepend(header);  // payload 现在为 "HDRDATA"
```

*此示例将一个头部缓冲区插入到载荷缓冲区之前。*

---

#### `Buffer &prepend(const unsigned char *ptr, int size)`

**Signature**

```cpp
Buffer &prepend(const unsigned char *ptr, int size);
```

**Synopsis**  
从指针在前面插入字节。返回此缓冲区以便链式调用。

**Parameters**

* `ptr` – 指向要前置的字节的指针。
* `size` – 要前置的字节数。

**Return value**

* `*this` 的引用。

**In Depth**

这是 `prepend` 的原始内存变体。它会在索引 `0` 处插入给定字节。现有内容会被移动以腾出空间。

**Example**

```cpp
unsigned char magic[2] = {0xAB, 0xCD};
Buffer buf;
buf.append("DATA");
buf.prepend(magic, 2);
// 缓冲区以 magic 字节开头，其后为 "DATA"
```

*此示例在缓冲区前面插入了一个小的二进制前缀。*

---

#### `Buffer &insert(int index, const String &str)`

**Signature**

```cpp
Buffer &insert(int index, const String &str);
```

**Synopsis**  
在指定索引插入字符串的字节。返回此缓冲区以便链式调用。

**Parameters**

* `index` – 要插入字节的位置（以字节为单位，0 基）。
* `str` – 其字节将被插入的 `String`。

**Return value**

* `*this` 的引用。

**In Depth**

此方法会：

* 将 `index` 解释为 `0` 到 `length()` 之间的字节位置。
* 在该位置插入 `str` 的 UTF-8 字节。
* 将 `index` 处及之后的现有字节向后移动。

你应确保 `0 <= index <= length()`；超出此范围的行为取决于具体实现。

**Example**

```cpp
Buffer buf;
buf.append("Hello world");
buf.insert(5, ",");  // 在 "Hello" 之后插入逗号
// 缓冲区字节现在表示 "Hello, world"
```

*此示例在已有数据中间插入字符串的字节。*

---

#### `Buffer &insert(int index, const Buffer &other)`

**Signature**

```cpp
Buffer &insert(int index, const Buffer &other);
```

**Synopsis**  
在指定索引插入另一个缓冲区的字节。返回此缓冲区以便链式调用。

**Parameters**

* `index` – 插入开始的字节索引。
* `other` – 其字节将被插入的缓冲区。

**Return value**

* `*this` 的引用。

**In Depth**

此变体会在指定位置插入 `other` 的所有字节。行为类似于将 `other` 的内容“拼接”到当前缓冲区中。

**Example**

```cpp
Buffer buf;
buf.append("Hello world");

Buffer comma;
comma.append(",");

buf.insert(5, comma);
// 结果与使用字符串变体相同
```

*此示例在特定字节索引处将一个缓冲区插入到另一个缓冲区中。*

---

#### `Buffer &insert(int index, const unsigned char *ptr, int size)`

**Signature**

```cpp
Buffer &insert(int index, const unsigned char *ptr, int size);
```

**Synopsis**  
从指针在指定索引插入字节。返回此缓冲区以便链式调用。

**Parameters**

* `index` – 插入开始的字节索引。
* `ptr` – 指向要插入字节的指针。
* `size` – 要插入的字节数。

**Return value**

* `*this` 的引用。

**In Depth**

这是 `insert` 的原始内存形式。当你有数组或外部缓冲区中的字节，想要将其拼接到此 `Buffer` 中时可使用它。

**Example**

```cpp
unsigned char marker[3] = {0x01, 0x02, 0x03};
Buffer buf;
buf.append("DATA");
buf.insert(0, marker, 3);
// buf 以三个 marker 字节开始，然后是 "DATA"
```

*此示例在缓冲区开始位置插入一个标记序列。*

---

#### `Buffer &remove(int start, int end = -1)`

**Signature**

```cpp
Buffer &remove(int start, int end = -1);
```

**Synopsis**  
移除从 start 到 end 的字节。返回此缓冲区以便链式调用。

**Parameters**

* `start` – 要移除的第一个字节的字节索引。
* `end` – 要移除的最后一个字节**之后**的字节索引（不包含）。如果 `end` 为 `-1`，则从 `start` 删除直到缓冲区末尾。

**Return value**

* `*this` 的引用。

**In Depth**

`remove()` 会删除一段连续的字节区间。删除后，后面的字节会向前移动以填补空洞，`length()` 会缩小。

常见模式：

* `remove(0, n)` – 删除前 `n` 个字节。
* `remove(offset, -1)` – 删除从 `offset` 到末尾的所有字节。

索引必须是 `[0, length()]` 范围内的有效字节位置；否则行为未定义。

**Example**

```cpp
Buffer buf;
buf.append("Hello, world");
buf.remove(5, 6);  // 移除逗号
// 缓冲区现在表示 "Hello world"
```

*此示例通过指定起止范围从缓冲区中移除一个字节。*

---

#### `Buffer &reverse()`

**Signature**

```cpp
Buffer &reverse();
```

**Synopsis**  
就地反转字节顺序。返回此缓冲区以便链式调用。

**Parameters**

* *(无)*

**Return value**

* `*this` 的引用。

**In Depth**

`reverse()` 会反转字节顺序：

* 索引为 `0` 的字节与索引为 `length() - 1` 的字节交换，以此类推。

这纯粹是按字节进行的操作。如果缓冲区中包含多字节结构（如 UTF-8 文本或多字节整数），其内部顺序将无法保持，从而可能变得无效。

**Example**

```cpp
Buffer buf;
buf.append("ABC");
buf.reverse();
// 缓冲区现在包含 "CBA"
```

*此示例反转了缓冲区中的字节顺序。*

---

#### `Buffer &trim()`

**Signature**

```cpp
Buffer &trim();
```

**Synopsis**  
将容量收缩至与长度一致。返回此缓冲区以便链式调用。

**Parameters**

* *(无)*

**Return value**

* `*this` 的引用。

**In Depth**

`trim()` 会将缓冲区的内部容量减少到与其当前 `length()` 相匹配。这可以：

* 在确定不再需要增长时释放未使用内存。
* 在序列化或长期存储前让缓冲区更加紧凑。

它不会改变内容或 `length()`。

**Example**

```cpp
Buffer buf(1024);  // 初始预留
buf.append("small");
buf.trim();        // 容量收缩到较小的大小
```

*此示例先预留了额外空间，然后在内容最终确定后将其裁剪掉。*

---

#### `Buffer slice(int start, int end = -1) const`

**Signature**

```cpp
Buffer slice(int start, int end = -1) const;
```

**Synopsis**  
返回一个包含从 start 到 end 字节的新缓冲区。

**Parameters**

* `start` – 要包含的第一个字节的字节索引。
* `end` – 要包含的最后一个字节**之后**的字节索引（不包含）。如果 `end` 为 `-1`，则切片一直延伸到缓冲区末尾。

**Return value**

* 包含指定范围字节的新 `Buffer`。

**In Depth**

`slice()` 是与 `remove()` 相对的非修改操作。它会从选定区间创建一个新缓冲区，而不修改原缓冲区。

可使用它从较大的缓冲区中抽取段落，如头部、载荷或独立记录。

**Example**

```cpp
Buffer buf;
buf.append("HEADERPAYLOAD");

Buffer header = buf.slice(0, 6);  // "HEADER"
Buffer payload = buf.slice(6);    // "PAYLOAD"
```

*此示例使用 `slice()` 将缓冲区拆分为头部和载荷子缓冲区。*

---

#### `Buffer compress() const`

**Signature**

```cpp
Buffer compress() const;
```

**Synopsis**  
返回此缓冲区的 LZ4 压缩版本。

**Parameters**

* *(无)*

**Return value**

* 包含压缩数据的新 `Buffer`。

**In Depth**

`compress()` 会对当前字节应用 LZ4 压缩：

* 对可压缩数据（如文本或 JSON）而言，结果可能比原始数据小。
* 对已经压缩或随机数据来说，结果大小可能近似原始数据，甚至略大。

你只应对此前由 `compress()`（或兼容的 LZ4 编码器）生成的数据调用 `decompress()`。

**Example**

```cpp
Buffer original;
original.append("Lots of repetitive text... Lots of repetitive text...");

Buffer compressed = original.compress();
```

*此示例压缩了一个包含可压缩文本的缓冲区。*

---

#### `Buffer decompress() const`

**Signature**

```cpp
Buffer decompress() const;
```

**Synopsis**  
返回此缓冲区的解压缩版本。

**Parameters**

* *(无)*

**Return value**

* 包含解压缩数据的新 `Buffer`。

**In Depth**

`decompress()` 会对先前由 `compress()` 生成的数据执行 LZ4 解压缩。如果缓冲区内容不是有效的压缩数据，其行为取决于具体实现（通常是报错或返回空结果）。

你应当：

* 确保在调用该函数前确知缓冲区是否已被压缩。
* 在应用中使用简单约定（如文件扩展名或头部标记）来跟踪压缩状态。

**Example**

```cpp
Buffer original;
original.append("Text to compress");

Buffer compressed = original.compress();
Buffer restored   = compressed.decompress();
// restored 应包含与 original 相同的字节
```

*此示例展示了先压缩再解压缩缓冲区以恢复原始字节。*

---

#### `Buffer crypt(const String &key, const String &nonce) const`

**Signature**

```cpp
Buffer crypt(const String &key, const String &nonce) const;
```

**Synopsis**  
使用 ChaCha20（对称）加解密。Key ≥32 字节，nonce ≥12 字节。

**Parameters**

* `key` – 其字节用作加密密钥的 `String`（至少 32 字节）。
* `nonce` – 其字节用作 nonce 的 `String`（至少 12 字节）。

**Return value**

* 包含加密或解密结果字节的新 `Buffer`。

**In Depth**

此重载使用以 `String` 形式提供的密钥和 nonce。ChaCha20 是对称密码：使用**相同**的密钥和 nonce 连续调用两次 `crypt()` 会恢复原始字节。

注意事项：

* 密钥和 nonce 的长度必须达到或超过前述阈值。
* 若不满足长度要求，或出现其他底层错误，行为取决于实现；在应用层应将其视为失败情况。

**Example**

```cpp
String key("0123456789abcdef0123456789abcdef");  // 32-byte key
String nonce("unique-nonce-123");               // ≥12 bytes

Buffer plain;
plain.append("secret data");

Buffer encrypted = plain.crypt(key, nonce);
Buffer decrypted = encrypted.crypt(key, nonce);
// decrypted 应与 plain 一致
```

*此示例使用基于 `String` 的密钥与 nonce 对数据进行加密和解密。*

---

#### `Buffer crypt(const String &key, const Buffer &nonce) const`

**Signature**

```cpp
Buffer crypt(const String &key, const Buffer &nonce) const;
```

**Synopsis**  
使用 ChaCha20（对称）加解密。Key ≥32 字节，nonce ≥12 字节。

**Parameters**

* `key` – 以 `String` 形式提供的加密密钥（其字节用作密钥材料）。
* `nonce` – 以 `Buffer` 形式提供的加密 nonce（其字节用作 nonce）。

**Return value**

* 包含加密或解密数据的新 `Buffer`。

**In Depth**

当你的 nonce 自然是二进制形式（例如由随机数生成）但仍希望以 `String` 管理密钥时，可以使用此变体。其语义与另一重载相同：用于加密与解密的 key+nonce 对必须完全一致。

**Example**

```cpp
String key("0123456789abcdef0123456789abcdef");

Buffer nonce;
nonce.append("nonce-binary");  // 二进制 nonce

Buffer data;
data.append("payload");

Buffer encrypted = data.crypt(key, nonce);
Buffer decrypted = encrypted.crypt(key, nonce);
```

*此示例使用字符串密钥与存储在缓冲区中的二进制 nonce。*

---

#### `Buffer crypt(const Buffer &key, const String &nonce) const`

**Signature**

```cpp
Buffer crypt(const Buffer &key, const String &nonce) const;
```

**Synopsis**  
使用 ChaCha20（对称）加解密。Key ≥32 字节，nonce ≥12 字节。

**Parameters**

* `key` – 以 `Buffer` 存储的加密密钥字节。
* `nonce` – 以 `String` 存储的 nonce。

**Return value**

* 包含加密或解密数据的新 `Buffer`。

**In Depth**

此重载与上一个重载互为镜像：密钥为 `Buffer`，nonce 为 `String`。当你的密钥来自二进制来源（例如从文件加载或随机生成），但 nonce 以文本形式存在时，这会很有用。

**Example**

```cpp
Buffer key;
key.append("0123456789abcdef0123456789abcdef");

String nonce("nonce-text-123");

Buffer payload;
payload.append("binary payload");

Buffer encrypted = payload.crypt(key, nonce);
Buffer decrypted = encrypted.crypt(key, nonce);
```

*此示例使用二进制密钥缓冲区与文本 nonce。*

---

#### `Buffer crypt(const Buffer &key, const Buffer &nonce) const`

**Signature**

```cpp
Buffer crypt(const Buffer &key, const Buffer &nonce) const;
```

**Synopsis**  
使用 ChaCha20（对称）加解密。Key ≥32 字节，nonce ≥12 字节。

**Parameters**

* `key` – 以 `Buffer` 形式提供的加密密钥字节。
* `nonce` – 以 `Buffer` 形式提供的 nonce 字节。

**Return value**

* 包含加密或解密数据的新 `Buffer`。

**In Depth**

这是最通用的重载：密钥和 nonce 都是二进制形式。当你通过安全随机 API 生成或以二进制文件存储二者时，可使用此重载。

同样地：

* 解密必须使用与加密时相同的 key+nonce。
* 必须满足长度要求。

**Example**

```cpp
Buffer key;
key.append("0123456789abcdef0123456789abcdef");  // ≥32 bytes

Buffer nonce;
nonce.append("0123456789ab");                    // ≥12 bytes

Buffer secret;
secret.append("top secret");

Buffer encrypted = secret.crypt(key, nonce);
Buffer decrypted = encrypted.crypt(key, nonce);
```

*此示例使用二进制密钥与 nonce 缓冲区进行加解密。*

---

#### `String toBase64() const`

**Signature**

```cpp
String toBase64() const;
```

**Synopsis**  
将缓冲区转换为 Base64 编码的字符串。

**Parameters**

* *(无)*

**Return value**

* 包含缓冲区字节 Base64 表示形式的 `String`。

**In Depth**

Base64 编码会将任意二进制数据映射为一组有限的 ASCII 字符。适用场景包括：

* 需要将二进制数据嵌入 JSON 或其他基于文本的格式时。
* 希望为日志或调试提供缓冲区的可打印表示时。

生成的 `String` 可通过 `fromBase64()` 再转换回 `Buffer`。

**Example**

```cpp
Buffer data;
data.append("binary data");

String encoded = data.toBase64();
// encoded 可以安全地放入 JSON 或日志中
```

*此示例将缓冲区内容编码为 Base64 文本。*

---

#### `static Buffer fromBase64(const String &base64)`

**Signature**

```cpp
static Buffer fromBase64(const String &base64);
```

**Synopsis**  
从 Base64 编码字符串创建缓冲区。

**Parameters**

* `base64` – 包含 Base64 编码文本的 `String`。

**Return value**

* 包含解码后字节的新 `Buffer`。

**In Depth**

`fromBase64()` 会将 Base64 文本解码回原始字节：

* 它期望 `base64` 是合法的 Base64。
* 对于无效输入，其行为由实现决定（很可能返回空缓冲区或部分结果）。你应在应用层将格式错误的 Base64 视为错误。

**Example**

```cpp
Buffer original;
original.append("binary data");

String encoded = original.toBase64();
Buffer decoded = Buffer::fromBase64(encoded);
// decoded 应包含与 original 相同的字节
```

*此示例演示了通过 Base64 编码与解码进行数据往返。*

---

#### `String toString(const String &encoding = "utf-8") const`

**Signature**

```cpp
String toString(const String &sourceEncoding = "utf-8") const;
```

**Synopsis**  
根据指定的 sourceEncoding 将缓冲区字节转换为 String。encoding 默认为 "utf-8"。

**Parameters**

* `sourceEncoding` – 缓冲区字节当前所使用的字符编码名称。默认为 `"utf-8"`。这些字节会被按此编码解释为文本，并转换为 `String` 的内部 UTF-8 表示。

**Return value**

* 从缓冲区字节构建的 `String`，内容由指定编码转换为 UTF-8。

**In Depth**

`toString()` 会将缓冲区内容视为以 `sourceEncoding` 编码的**文本**，并将其转换为 UTF-8 的 `String`。抽象来看：

* 缓冲区持有从文件、套接字或其他二进制源读取到的原始字节。
* 假定这些字节按指定编码（例如 UTF-8、ANSI/Windows-1252 或其他代码页）表示文本。
* `toString()` 会解码这些字节并生成内部为 UTF-8 表示的 `String`。

采用默认的 `"utf-8"` 时，假定这些字节已经是 UTF-8，将其封装（或在封装前进行校验）为 `String`。当你传入其他编码名称时，该函数会尝试**从该编码转换到 UTF-8**。

如果字节对给定编码无效，结果取决于底层转换实现。常见行为包括：

* 将非法序列替换为占位符字符。
* 生成部分结果或空结果。
* 返回空的 `String`。

只有当你确信缓冲区中保存的是以指定编码编码的文本数据时（例如以 ANSI 保存的日志文件，或来自旧系统的响应），才应调用 `toString()`。对于压缩、加密或任意二进制数据，应继续以 `Buffer` 形式处理原始字节，避免转换为 `String`。

**Example**

```cpp
Buffer buf;
// ... 将以 ANSI (Windows-1252) 编码的文件或套接字原始字节读入 buf ...

// 将存储的字节按 Windows-1252 ("ANSI") 解释并转换为 UTF-8 String
String text = buf.toString("ansi");

// 对已为 UTF-8 的数据，可以依赖默认值：
Buffer utf8Buf;
// ... 将 UTF-8 文本字节填充到 utf8Buf ...
String utf8Text = utf8Buf.toString();  // 等同于 utf8Buf.toString("utf-8")
```

*此示例将以 ANSI 代码页存储的字节转换为 UTF-8 `String`，并展示了对已为 UTF-8 数据的默认路径。*

---

#### `bool compare(const Buffer &other) const`

**Signature**

```cpp
bool compare(const Buffer &other) const;
```

**Synopsis**  
当此缓冲区等于另一个缓冲区时返回 true。

**Parameters**

* `other` – 要比较的缓冲区。

**Return value**

* 若两个缓冲区具有完全相同的字节则为 `true`，否则为 `false`。

**In Depth**

`compare()` 会检查逐字节的完全相等，包括长度。其行为等价于 `operator==`，只是以命名方法的形式呈现。

**Example**

```cpp
Buffer a;
a.append("DATA");

Buffer b;
b.append("DATA");

bool same = a.compare(b);  // true
```

*此示例比较两个缓冲区是否完全相同。*

---

#### `int hash() const`

**Signature**

```cpp
int hash() const;
```

**Synopsis**  
返回此缓冲区的哈希码。

**Parameters**

* *(无)*

**Return value**

* 根据缓冲区字节计算出的整数哈希码。

**In Depth**

该哈希码适用于在自定义的基于哈希的数据结构中组织缓冲区。算法未作规定，也可能跨版本发生变化，因此不应持久化哈希值，也不应依赖其在不同运行之间保持稳定。

**Example**

```cpp
Buffer key;
key.append("binary-key");

int h = key.hash();
// 将 h 用于自定义哈希表或映射
```

*此示例根据缓冲区内容计算哈希码。*

---

#### `bool operator==(const Buffer &other) const`

**Signature**

```cpp
bool operator==(const Buffer &other) const;
```

**Synopsis**  
当此缓冲区等于另一个缓冲区时返回 true。

**Parameters**

* `other` – 要比较的缓冲区。

**Return value**

* 若缓冲区相等则为 `true`，否则为 `false`。

**In Depth**

此运算符会对两个缓冲区执行逐字节的相等性比较。与 `compare()` 相比，这是更惯用的写法。

**Example**

```cpp
Buffer a;
a.append("X");

Buffer b;
b.append("X");

bool equal = (a == b);  // true
```

*此示例使用相等运算符比较两个缓冲区。*

---

#### `bool operator!=(const Buffer &other) const`

**Signature**

```cpp
bool operator!=(const Buffer &other) const;
```

**Synopsis**  
当此缓冲区不等于另一个缓冲区时返回 true。

**Parameters**

* `other` – 要比较的缓冲区。

**Return value**

* 若缓冲区不同则为 `true`，若相等则为 `false`。

**In Depth**

此运算符只是 `operator==` 的逻辑非。它可让你用自然的方式表达“不相等”的判断。

**Example**

```cpp
Buffer a;
a.append("A");

Buffer b;
b.append("B");

if (a != b) {
  // 缓冲区内容不同
}
```

*此示例在两个缓冲区不包含相同字节时进行分支。*

---

#### `Buffer operator+(const String &str) const`

**Signature**

```cpp
Buffer operator+(const String &str) const;
```

**Synopsis**  
返回一个在末尾追加了字符串字节的新缓冲区。

**Parameters**

* `str` – 其字节将被追加到此缓冲区的新 `String`。

**Return value**

* 新的 `Buffer`，其内容为原始字节后接字符串字节。

**In Depth**

此运算符提供一种便捷方式：在不修改任一操作数的前提下，通过在现有缓冲区后追加一个 `String` 来构造新缓冲区。其行为类似于：

```cpp
Buffer result(*this);
result.append(str);
return result;
```

**Example**

```cpp
Buffer prefix;
prefix.append("HDR");

Buffer combined = prefix + String("BODY");
// combined 包含 "HDRBODY" 的字节
// prefix 未被修改
```

*此示例使用 `operator+` 从一个已有缓冲区和一个字符串构造了新缓冲区。*

---

#### `Buffer operator+(const Buffer &other) const`

**Signature**

```cpp
Buffer operator+(const Buffer &other) const;
```

**Synopsis**  
返回一个在末尾追加了另一个缓冲区字节的新缓冲区。

**Parameters**

* `other` – 其字节应被追加到此缓冲区的缓冲区。

**Return value**

* 新的 `Buffer`，其内容为当前缓冲区字节后接 `other` 的字节。

**In Depth**

此运算符是 `append(other)` 的非修改对应版本。它允许你在不改变原始缓冲区的情况下将两个缓冲区合并为新的一个。

**Example**

```cpp
Buffer a;
a.append("AAA");

Buffer b;
b.append("BBB");

Buffer combined = a + b;
// combined: "AAABBB"
// a 与 b 保持不变
```

*此示例使用 `operator+` 将两个缓冲区连接为一个新的缓冲区。*