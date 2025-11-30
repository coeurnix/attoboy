# DateTime

## 概览

`attoboy::DateTime` 类表示一个时间点，具有**毫秒级精度**，始终使用 **UTC**（协调世界时）。它是该库中获取“当前时间”、存储时间戳、测量耗时、以及在机器友好的数字时间戳与人类可读字符串之间相互转换的主要方式。

可以将 `DateTime` 看作是一个紧凑的、基于 RAII 的包装器，其内部包含：

* 一个以**自 Unix 纪元以来的毫秒数**计量的 64 位时间戳，和
* 一小组用于加法、比较以及以 ISO-8601 字符串格式化的操作。

与 `String` 不同，`DateTime` 是**可变的**：`add()` 函数会就地修改对象，并返回其自身的引用以支持链式调用。拷贝和赋值都是安全且廉价的，仅仅复制底层时间戳。

### 基本概念：UTC、Unix 纪元与毫秒

如果你对编程中的时间处理还不熟悉，下面几个核心概念很有帮助：

* **UTC（协调世界时）**  
  UTC 是一种全球通用、与时区无关的时间标准。`DateTime` 始终表示 UTC 时间；它不会跟踪时区或夏令时规则。当你将 `DateTime` 转为字符串时，会得到一个以 `"Z"` 结尾的 ISO-8601 UTC 值（例如 `"2024-01-01T12:00:00.000Z"`）。

* **Unix 纪元（Unix Epoch）**  
  Unix 纪元被定义为 **1970 年 1 月 1 日 00:00:00 UTC**。在内部，`DateTime` 使用一个 64 位整数记录自这一时刻以来经过的**毫秒**数：

  * `0` → 恰好对应 `1970-01-01T00:00:00.000Z`
  * `1` → 之后一毫秒
  * 负值表示纪元之前的时间。

* **毫秒级精度**  
  所有算术运算与差值计算都以毫秒为单位：

  * 一秒 = 1,000 毫秒
  * 一分钟 = 60,000 毫秒
  * 一小时 = 3,600,000 毫秒

这种设计让以下操作变得简单直接：

* 计算两个事件之间的耗时（`diff()`），
* 将一个 `DateTime` 向前或向后移动（`add()`），
* 记录时间戳或将其以数字形式持久化（`timestamp()`），
* 生成和解析 ISO-8601 字符串（`toString()`，以及从 `String` 的构造函数）。

> **注意：** `DateTime` 使用的是系统的挂钟时间（wall-clock time），而不是单调“计时器”时钟。如果系统时间被用户或 NTP 调整，新创建的 `DateTime()` 对象会反映该变更。

---

## 参考

下面的每一条目都涵盖了 `attoboy::DateTime` 的一个公共构造函数、方法或运算符。对每个条目，你可以看到：

* **函数签名（Signature）** – 来自头文件的精确声明。
* **概要（Synopsis）** – 原始的一行 Doxygen 注释。
* **参数（Parameters）**和**返回值（Return value）**说明。
* **深入说明（In Depth）** – 行为细节、注意事项和一个简短示例。

所有示例都假定：

```cpp
using namespace attoboy;
```

---

#### `DateTime()`

**Signature**

```cpp
DateTime();
```

**Synopsis**  
创建一个表示当前时间的 DateTime。

**Parameters**

* *(无)*

**Return value**

* *(构造函数；不适用)*

**In Depth**

默认构造函数从系统时钟捕获**当前 UTC 时间**，精度为毫秒。这是获取“当前时间戳”的最简单方式。

捕获的时间在对象中是固定的；对同一个实例重复调用 `timestamp()` 或 `toString()` 将返回相同的值，除非调用 `add()` 对其进行调整。

**Example**

```cpp
DateTime now;
String iso = now.toString();  // 例如 "2025-11-27T21:00:00.123Z"
```

*此示例创建一个表示当前时刻的 `DateTime`，并将其转换为 ISO-8601 字符串。*

---

#### `DateTime(long long millisSinceEpoch)`

**Signature**

```cpp
DateTime(long long millisSinceEpoch);
```

**Synopsis**  
从自 Unix 纪元（1970 年 1 月 1 日）以来的毫秒数创建一个 DateTime。

**Parameters**

* `millisSinceEpoch` – 自 `1970-01-01T00:00:00.000Z`（UTC）以来经过的毫秒数。对于纪元之前的时间可以为负值。

**Return value**

* *(构造函数；不适用)*

**In Depth**

当你已经有一个数字时间戳（例如来自存储、网络协议，或另一个 `DateTime` 的 `timestamp()`）时，可以使用此构造函数。该值会按原样存储，不进行任何时区转换。

典型用法模式：

* 将 `timestamp()` 保存到文件或设置中。
* 之后读取该值并用它构造一个表示同一时间点的 `DateTime`。

**Example**

```cpp
DateTime now;
long long saved = now.timestamp();

// later...
DateTime restored(saved);
String iso = restored.toString();
```

*此示例捕获当前时间，将其毫秒时间戳保存下来，并在稍后从该值重建同一时间点。*

---

#### `DateTime(const String &iso8601)`

**Signature**

```cpp
DateTime(const String &iso8601);
```

**Synopsis**  
从 ISO-8601 字符串（YYYY-MM-DDTHH:MM:SS.fffZ）创建一个 DateTime。

**Parameters**

* `iso8601` – 一个包含 UTC 时间戳的 ISO-8601 格式 `String`，例如 `"2024-05-10T14:30:00.000Z"`。

**Return value**

* *(构造函数；不适用)*

**In Depth**

该构造函数解析一个 ISO-8601 时间戳字符串，并将其转换为 `DateTime`。期望的格式为：

```text
YYYY-MM-DDTHH:MM:SS.fffZ
```

例如：`"2025-11-27T21:15:30.250Z"`。

关键点：

* 末尾的 `'Z'` 表示 UTC；不会解析任何时区偏移。
* 解析失败（格式错误、日期无效）的行为是实现定义的；你应当提供有效字符串，通常是由 `DateTime::toString()` 生成的字符串。

**Example**

```cpp
String text("2024-01-01T00:00:00.000Z");
DateTime start(text);
```

*此示例从一个已知的 ISO-8601 字符串构造一个 `DateTime`。*

---

#### `DateTime(const DateTime &other)`

**Signature**

```cpp
DateTime(const DateTime &other);
```

**Synopsis**  
创建另一个 DateTime 的副本。

**Parameters**

* `other` – 需拷贝的现有 `DateTime` 实例。

**Return value**

* *(构造函数；不适用)*

**In Depth**

拷贝构造函数会复制内部时间戳及其毫秒精度。由于 `DateTime` 是一个简单的值类型，拷贝是廉价且安全的。对某个实例的修改（通过 `add()`）不会影响它的拷贝。

**Example**

```cpp
DateTime original;
DateTime copy(original);
// 两者最初表示同一时间点
```

*此示例演示如何拷贝一个 `DateTime` 值。*

---

#### `~DateTime()`

**Signature**

```cpp
~DateTime();
```

**Synopsis**  
销毁 DateTime 并释放内存。

**Parameters**

* *(无)*

**Return value**

* *(析构函数；不适用)*

**In Depth**

当一个 `DateTime` 对象离开其作用域时，其析构函数会释放任何内部资源。由于 `DateTime` 主要封装了一个时间戳，销毁通常是轻量的，但通过 RAII 可以保证所有实现细节都被安全地处理。

**Example**

```cpp
{
  DateTime temp;
  String iso = temp.toString();
}
// temp 在此处被销毁
```

*此示例展示了一个 `DateTime` 在作用域内自动创建和销毁的过程。*

---

#### `DateTime &operator=(const DateTime &other)`

**Signature**

```cpp
DateTime &operator=(const DateTime &other);
```

**Synopsis**  
将另一个 DateTime 赋值给当前 DateTime。

**Parameters**

* `other` – 其值应被复制到 `*this` 的 `DateTime`。

**Return value**

* 指向 `*this` 的引用，以便进行赋值链式调用。

**In Depth**

赋值运算符会用 `other` 的内部时间戳替换左值对象的时间戳。赋值之后，两个对象表示同一时间点，但之后可分别通过 `add()` 独立修改。

自赋值（`dt = dt`）是安全的。

**Example**

```cpp
DateTime a;
DateTime b;

b = a;  // b 现在表示与 a 相同的时间
```

*此示例将一个 `DateTime` 重新赋值为另一个的值。*

---

#### `DateTime &add(long long milliseconds)`

**Signature**

```cpp
DateTime &add(long long milliseconds);
```

**Synopsis**  
增加毫秒数（为负则为减）。返回当前 DateTime 以支持链式调用。

**Parameters**

* `milliseconds` – 要加到当前时间戳上的毫秒数。使用负值表示减去。

**Return value**

* 修改后的 `*this` 引用，可用于链式调用。

**In Depth**

`add()` 按相对时间量调整 `DateTime`。它会就地修改对象，使其向前或向后移动。

常见用法：

* 增加秒数：`dt.add(1000 * seconds);`
* 增加分钟：`dt.add(60 * 1000 * minutes);`
* 增加小时：`dt.add(60 * 60 * 1000 * hours);`

由于它返回对 `*this` 的引用，你可以链式操作：

```cpp
deadline.add(60 * 1000).add(5 * 1000);
```

需要注意的是，添加非常大的正值或负值可能会导致内部时间戳溢出（实现定义行为）。对于典型应用的时间跨度，这通常不是问题。

**Example**

```cpp
DateTime now;
DateTime deadline(now);
deadline.add(5 * 60 * 1000);  // 增加 5 分钟
```

*此示例将当前时间复制到 `deadline`，然后将其向未来平移 5 分钟。*

---

#### `long long diff(const DateTime &other) const`

**Signature**

```cpp
long long diff(const DateTime &other) const;
```

**Synopsis**  
返回以毫秒为单位的差值（若当前对象更晚则为正）。

**Parameters**

* `other` – 要与之比较的 `DateTime`。

**Return value**

* 以毫秒为单位的差值：`timestamp() - other.timestamp()`。  
  结果为：

  * 若 `*this` 晚于 `other`，则为正。
  * 若两者表示同一时间点，则为零。
  * 若 `*this` 早于 `other`，则为负。

**In Depth**

`diff()` 是衡量耗时或时间间隔的主要方式：

* `end.diff(start)` → 从 `start` 到 `end` 过去了多少毫秒。
* `now.diff(past)` → 正的延迟（如果 `now` 更晚）。
* `future.diff(now)` → 距离 `future` 还有多少毫秒（为正）。

内部行为等价于两个时间戳相减。它不会修改任一对象。

**Example**

```cpp
DateTime start;
// ... do some work ...
DateTime end;

long long elapsedMs = end.diff(start);
// elapsedMs 是 start 与 end 之间的毫秒时长
```

*此示例测量两个捕获的时间戳之间的耗时。*

---

#### `int compare(const DateTime &other) const`

**Signature**

```cpp
int compare(const DateTime &other) const;
```

**Synopsis**  
与另一个 DateTime 比较。返回 <0、0 或 >0。

**Parameters**

* `other` – 用于比较的 `DateTime`。

**Return value**

* 若 `*this` 早于 `other`，返回负值。
* 若两者表示同一时间点，返回 `0`。
* 若 `*this` 晚于 `other`，返回正值。

**In Depth**

`compare()` 提供了适用于排序或排序关系的三向比较。它等价于：

```cpp
if (timestamp() < other.timestamp()) return negative;
if (timestamp() > other.timestamp()) return positive;
return 0;
```

负值/正值的具体数值并未指定；只关心它们的符号。

**Example**

```cpp
DateTime a;            // now
DateTime b(a);
b.add(1000);           // 晚一秒

int result = a.compare(b);  // result < 0，因为 a 更早
```

*此示例比较两个 `DateTime` 实例以确定时间先后顺序。*

---

#### `bool equals(const DateTime &other) const`

**Signature**

```cpp
bool equals(const DateTime &other) const;
```

**Synopsis**  
若当前 DateTime 等于另一个则返回 true。

**Parameters**

* `other` – 用于比较的 `DateTime`。

**Return value**

* 若两者表示自纪元以来的毫秒数完全相同，则为 `true`；否则为 `false`。

**In Depth**

`equals()` 以毫秒精度严格检查时间戳是否相等。它等价于检查双方的 `timestamp()` 是否一致。

此方法在功能上与 `operator==` 相同，但某些代码库更偏好显式方法以增强可读性。

**Example**

```cpp
DateTime a;
DateTime b(a);

bool same = a.equals(b);  // true
```

*此示例使用 `equals()` 验证两个对象是否表示同一时间。*

---

#### `bool operator==(const DateTime &other) const`

**Signature**

```cpp
bool operator==(const DateTime &other) const;
```

**Synopsis**  
若当前 DateTime 等于另一个则返回 true。

**Parameters**

* `other` – 用于比较的 `DateTime`。

**Return value**

* 若时间戳相等，则为 `true`；否则为 `false`。

**In Depth**

该运算符提供 C++ 习惯用法中的相等比较：

```cpp
if (a == b) { ... }
```

内部实现等价于调用 `equals(other)`。

**Example**

```cpp
DateTime a;
DateTime b(a);

if (a == b) {
  String msg("Times are equal");
}
```

*此示例使用 `==` 运算符来检查是否相等。*

---

#### `bool operator!=(const DateTime &other) const`

**Signature**

```cpp
bool operator!=(const DateTime &other) const;
```

**Synopsis**  
若当前 DateTime 不等于另一个则返回 true。

**Parameters**

* `other` – 用于比较的 `DateTime`。

**Return value**

* 若时间戳不同，则为 `true`；若相同，则为 `false`。

**In Depth**

该运算符只是 `operator==` 的逻辑非。当你希望在“不相等”条件下分支时使用它：

```cpp
if (a != b) { ... }
```

**Example**

```cpp
DateTime scheduled;
DateTime actual;

if (actual != scheduled) {
  String msg("Schedule changed");
}
```

*此示例用于检测实际时间是否与计划时间不同。*

---

#### `long long timestamp() const`

**Signature**

```cpp
long long timestamp() const;
```

**Synopsis**  
返回自 Unix 纪元（1970 年 1 月 1 日）以来的毫秒数。

**Parameters**

* *(无)*

**Return value**

* 64 位整数，表示自 `1970-01-01T00:00:00.000Z`（UTC）以来经过的毫秒数。

**In Depth**

`timestamp()` 暴露了 `DateTime` 的底层数字表示形式。在以下场景中很有用：

* 以紧凑、机器友好的形式持久化时间。
* 在 `DateTime` API 之外执行算术运算。
* 与使用毫秒时间戳的其他系统进行互操作。

需要注意的是，这基于挂钟时间；如果系统时钟向后调整，以后创建的 `DateTime()` 实例的时间戳可能比之前的更小。

**Example**

```cpp
DateTime now;
long long ms = now.timestamp();
// ms 可以保存到文件或通过网络发送
```

*此示例从 `DateTime` 中获取原始毫秒时间戳。*

---

#### `String toString() const`

**Signature**

```cpp
String toString() const;
```

**Synopsis**  
返回 ISO-8601 格式的字符串。

**Parameters**

* *(无)*

**Return value**

* 包含 UTC 时间戳的 ISO-8601 格式 `String`，例如 `"YYYY-MM-DDTHH:MM:SS.fffZ"`。

**In Depth**

`toString()` 将内部时间戳转换为人类可读的 UTC ISO-8601 字符串，包括毫秒以及末尾表示 UTC 的 `'Z'`。

典型用途：

* 记录日志时间戳。
* 将时间值嵌入到 JSON 或配置文件中。
* 与 `DateTime(const String &iso8601)` 构造函数配合进行往返转换。

`timestamp()` 和 `toString()` 的组合同时为你提供同一时间点的机器友好和人类友好表示。

**Example**

```cpp
DateTime now;
String iso = now.toString();  // 例如 "2025-11-27T21:05:30.123Z"
```

*此示例将 `DateTime` 转换为适合记录或存储的标准 ISO-8601 UTC 字符串。*