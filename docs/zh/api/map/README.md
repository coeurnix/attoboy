# Map

## 概览

`attoboy::Map` 类是一个**键值字典**，支持**混合类型的键和值**。它被设计为标准库映射容器的紧凑替代品，但不依赖 C++ 标准库。相反，它与其他 attoboy 类型紧密集成：

* 基本类型键和值：`bool`、`int`、`float`
* 文本键和值：`String` 和 `const char *`
* 作为值的嵌套容器：`List`、`Map`、`Set`

从高层来看，`Map` 提供：

* **唯一键**——每个键最多出现一次。
* **灵活的键类型**——键可以是数字、字符串或布尔值。
* **灵活的值类型**——值可以是任何受支持类型，包括嵌套容器。
* **高效查找**——通过 `get()` 或 `operator[]` 按键检索值。
* **检查工具**——检查键是否存在、值的类型，以及基于值反向查找键。
* **JSON 集成**——在 JSON 对象字符串和 `Map` 之间转换。

### 键值映射与混合类型

在概念上，映射（或字典）是一个**成对集合**：

* 每一对由一个**键**和一个**值**组成。
* 键是**唯一的**；向已存在的键插入新值会**覆盖**之前的值。
* 键可以被快速用来查找其对应的值。

在 `attoboy::Map` 中：

* 键可以是 `bool`、`int`、`float`、`const char *` 或 `String`。
* 值可以是任何受支持类型：基本类型、`String`、`List`、`Map`、`Set`。
* 映射**不保证顺序**；通过 `keys()` 或 `values()` 迭代时不应依赖插入顺序。

### 类型与 `ValueType`

由于值可以是混合类型，在读取前你可能需要知道给定键存储的是**哪种类型**。`ValueType` 枚举（与 `List` 和 `Set` 共享）包括：

* `TYPE_BOOL`、`TYPE_INT`、`TYPE_FLOAT`
* `TYPE_STRING`、`TYPE_LIST`、`TYPE_MAP`、`TYPE_SET`
* 用于特殊或错误状态的 `TYPE_NULL`、`TYPE_INVALID`、`TYPE_UNKNOWN`

你可以使用 `typeAt()` 在调用 `get()` 或 `operator[]` 时，在决定**要把它读入哪个 C++ 类型**（例如 `int`、`float` 或 `String`）之前，先检查键对应值的类型。

### 值代理（`MapValueView`）

`Map` 使用内部的**值代理类型**（即 `MapValueView`）来简化查找：

* `get()` 和 `operator[]` 等函数返回一个代理对象。
* 该代理可以**隐式转换**为你赋值目标的 C++ 类型。

在实践中，你可以这样写：

```cpp
Map m("name", "Alice", "age", 30);

String name = m.get("name");  // proxy converts to String
int age     = m["age"];       // proxy converts to int
```

你几乎从不需要显式提到 `MapValueView`；你只需：

1. 调用 `get()` 或 `operator[]`。
2. 把结果赋给你想要的类型的变量。

如果键不存在（或存储类型与目标类型不兼容），代理会转换成目标类型合理的**“空值”**（例如整型为 `0`，`bool` 为 `false`，`String` 为空 `String`）。

### 可变性、RAII 与链式调用

`Map` 是**可变的**：

* `put()`、`remove()`、`clear()` 和 `merge()` 等方法会就地修改映射。
* 大多数修改方法返回 `*this`，从而支持流式的**链式调用**：

  ```cpp
  using namespace attoboy;

  Map config;
  config.put("host", "localhost")
        .put("port", 8080)
        .put("debug", true);
  ```

`Map` 也遵循 **RAII** 原则：

* 构造 `Map` 时按需获取内部资源。
* 析构函数会自动释放这些资源。
* 拷贝与赋值会安全管理内部状态；你永远不需要手工释放内存。

### JSON 与配置用例

`Map` 对于配置类数据和 JSON 尤其有用：

* `toJSONString()` 将映射转换为 JSON 对象字符串。
* `FromJSONString()` 解析 JSON 对象字符串并构建一个 `Map`。

这使你可以轻松：

* 表示配置项：`"host" -> "example.com"`、`"timeout" -> 5000`。
* 向消费或产生 JSON 的 API 传递结构化参数。
* 使用嵌套的 `Map` 和 `List` 值来存储层级数据。

---

## 参考

下面的每个条目涵盖 `attoboy::Map` 的一个公共构造函数、方法、静态函数或运算符。对于每一项，你会看到：

* **函数签名**——头文件中的精确声明。
* **概要**——原始的一行 Doxygen 注释。
* **参数**和**返回值**说明。
* **深入说明**——额外细节、注意事项和简短示例。

> **注意：** 所有示例都假定 `using namespace attoboy;`。

---

**概要（类）**  
具有混合类型键和值的键值映射。  
键必须唯一。不保证顺序。

此概要适用于整个类。下面的章节将详细说明每个成员。

---

#### `Map()`

**函数签名**

```cpp
Map();
```

**概要**  
创建一个空映射。

**参数**

* *(无)*

**返回值**

* *(构造函数；不适用)*

**深入说明**

此构造函数创建的映射具有：

* 没有键。
* 没有关联的值。

在构造完成后：

* `length()` 返回 `0`。
* `isEmpty()` 返回 `true`。

这是使用 `put()` 逐步构建键值集合的标准起点。

**示例**

```cpp
Map settings;
settings.put("debug", true);
settings.put("maxConnections", 100);
```

*此示例从一个空映射开始并添加两个键值对。*

---

#### `Map(int capacity)`

**函数签名**

```cpp
Map(int capacity);
```

**概要**  
创建一个具有预留容量的空映射。

**参数**

* `capacity` – 预计要存储的键值对数量提示。

**返回值**

* *(构造函数；不适用)*

**深入说明**

此构造函数允许你预先为大约 `capacity` 个条目预留内部存储空间。当你知道会存储很多条目时，这可以减少内部重新分配。

映射仍然是空的：在调用 `put()` 之前，`length() == 0`。

**示例**

```cpp
int expectedEntries = 100;
Map cache(expectedEntries);

for (int i = 0; i < expectedEntries; ++i) {
  cache.put(i, String("value"));
}
```

*此示例为一个包含 100 个条目的缓存预先分配空间。*

---

#### `template <typename... Args> Map(const Args &...args)`

**函数签名**

```cpp
template <typename... Args> Map(const Args &...args) : Map() {
  variadic_put_pairs(args...);
}
```

**概要**  
使用键值对创建映射：Map("name", "Alice", "age", 30)

**参数**

* `args` – 将被解释为键值对的一系列值：

  * `args` 按顺序消费：`(key0, value0, key1, value1, ...)`。
  * 键必须是受支持的键类型（`bool`、`int`、`float`、`const char *`、`String`）。
  * 值必须是受支持的值类型。

**返回值**

* *(构造函数；不适用)*

**深入说明**

此构造函数提供一种方便的字面量风格初始化方式：

* 从空映射开始。
* 将参数列表视为 `(key, value)` 对。
* 对每一对调用 `put(key, value)`。

如果参数数量为奇数，根据内部 `variadic_put_pairs()` 的行为，最后一个键会与默认布尔值 `false` 配对，但通常不建议这样用。为清晰与安全起见，应始终以完整的键值对形式提供参数。

如果同一键出现多次，后面的值会覆盖前面的值（类似于重复调用 `put()`）。

**示例**

```cpp
Map user(
  "name", "Alice",
  "age", 30,
  "active", true
);
// user["name"] == "Alice"
// user["age"] == 30
// user["active"] == true
```

*此示例使用类似字面量的语法创建一个小型配置映射。*

---

#### `Map(const Map &other)`

**函数签名**

```cpp
Map(const Map &other);
```

**概要**  
创建另一个映射的副本。

**参数**

* `other` – 要拷贝的现有映射。

**返回值**

* *(构造函数；不适用)*

**深入说明**

拷贝构造函数会生成一个与 `other` 具有相同键值对的新映射：

* `length()` 相同。
* 键包含关系（`hasKey(key)`）相同。
* 通过 `get()` 或 `operator[]` 获取的值与 `other` 中的对应值相等。

虽然内部实现细节可能不同，但可观察行为相同。

**示例**

```cpp
Map original("host", "localhost", "port", 8080);
Map copy(original);
// copy 起初拥有与 original 相同的键值对
```

*此示例复制一个配置映射，以便对其中一个的修改不会影响另一个。*

---

#### `~Map()`

**函数签名**

```cpp
~Map();
```

**概要**  
销毁映射并释放内存。

**参数**

* *(无)*

**返回值**

* *(析构函数；不适用)*

**深入说明**

当 `Map` 退出作用域或被销毁时，其析构函数会自动：

* 释放内部存储。
* 清理任何相关资源。

由于 attoboy 遵循 RAII，你不需要显式释放映射。

**示例**

```cpp
{
  Map temp("key", "value");
  // 在这里使用 temp
}
// temp 在此处被销毁；其资源被释放
```

*此示例展示了映射离开作用域时的自动清理。*

---

#### `Map &operator=(const Map &other)`

**函数签名**

```cpp
Map &operator=(const Map &other);
```

**概要**  
将另一个映射赋给此映射。

**参数**

* `other` – 其内容应替换当前内容的映射。

**返回值**

* 指向 `*this` 的引用，用于赋值链式调用。

**深入说明**

拷贝赋值运算符会使 `*this` 包含与 `other` 完全相同的键值对。任何之前的内容都会被丢弃。

赋值之后：

* `length()` 与 `other.length()` 相同。
* 对任何键 `k`，`get(k, default)` 在两个映射中的返回值相同。

自赋值（`map = map`）是安全的。

**示例**

```cpp
Map a("x", 1);
Map b("y", 2);

a = b;
// a 现在只包含 b 中的键值对
```

*此示例将一个映射的内容替换为另一个。*

---

#### `int length() const`

**函数签名**

```cpp
int length() const;
```

**概要**  
返回键值对的数量。

**参数**

* *(无)*

**返回值**

* 当前存储的键值对数量。

**深入说明**

`length()` 返回映射中**不同键**的数量。由于每个键都是唯一的：

* `length()` 等于条目数量。
* 向已存在的键插入一个值**不会**增加 `length()`；而是替换之前的值。

**示例**

```cpp
Map m;
m.put("a", 1)
 .put("b", 2)
 .put("a", 3);  // 覆盖键 "a"

int n = m.length();  // 2
```

*此示例展示了覆盖键不会增加映射长度。*

---

#### `bool isEmpty() const`

**函数签名**

```cpp
bool isEmpty() const;
```

**概要**  
如果映射为空则返回 true。

**参数**

* *(无)*

**返回值**

* 当 `length() == 0` 时为 `true`；否则为 `false`。

**深入说明**

`isEmpty()` 是一种便利函数，比检查 `length() == 0` 更能清晰表达意图。

**示例**

```cpp
Map m;
if (m.isEmpty()) {
  m.put("initialized", true);
}
```

*此示例使用 `isEmpty()` 决定何时初始化映射。*

---

#### `MapValueView get(…) const`（无显式默认值的代理查找）

**函数签名**

```cpp
MapValueView get(bool key) const;
MapValueView get(int key) const;
MapValueView get(float key) const;
MapValueView get(const char *key) const;
MapValueView get(const String &key) const;
```

**概要**  
返回键对应值的类型化视图；未找到时返回空类型。

**参数**

* `key` – 要查找的键。提供以下重载：

  * `bool`
  * `int`
  * `float`
  * `const char *`
  * `String`

**返回值**

* 一个 `MapValueView` 代理，表示与 `key` 关联的值；如果未找到该键，则为特殊的**空类型代理**。

**深入说明**

这些重载提供了最常用的、**无需模板**的读值方式：

```cpp
Map config("timeoutMs", 5000, "debug", true);

int timeout = config.get("timeoutMs");  // proxy → int
bool debug  = config.get("debug");      // proxy → bool
```

通常你**不会**直接操作 `MapValueView`。而是将 `get()` 的结果赋给你期望类型的变量：

* 如果键**存在**且存储的值与目标类型兼容，代理会转换为该类型。
* 如果键**不存在**，代理会转换为该类型的**“空值”**：

  * 数值类型为 `0`，
  * `bool` 为 `false`，
  * `String` 为空 `String`，等等。

如果你需要区分“键不存在”和“存在但值像默认值”（例如 `0` 也是有效值），应搭配 `hasKey()` 或 `typeAt()` 使用。

**示例**

```cpp
Map stats("score", 42, "lives", 3);

int score  = stats.get("score");    // 42
int lives  = stats.get("lives");    // 3
int coins  = stats.get("coins");    // 0（缺失键 → 空 int）
bool debug = stats.get("debug");    // false（缺失键 → 空 bool）

if (!stats.hasKey("coins")) {
  // 键确实不存在，即便代理转换成了 0。
}
```

*此示例展示了无模板的代理查找以及缺失键如何转换为默认值。*

---

#### `MapValueView get(…, const DefaultValue &defaultValue) const`（带显式默认值的代理查找）

**函数签名**

```cpp
MapValueView get(bool key, const DefaultValue &defaultValue) const;
MapValueView get(int key, const DefaultValue &defaultValue) const;
MapValueView get(float key, const DefaultValue &defaultValue) const;
MapValueView get(const char *key, const DefaultValue &defaultValue) const;
MapValueView get(const String &key, const DefaultValue &defaultValue) const;
MapValueView get(const char *key, const char *defaultValue) const;
```

**概要**  
返回键对应值的类型化视图；未找到时返回 defaultValue。

**参数**

* `key` – 要查找的键。提供以下重载：

  * `bool`
  * `int`
  * `float`
  * `const char *`
  * `String`
* `defaultValue` – 当 `key` 不存在时使用的回退值。参数类型由你传入的实参推导。

**返回值**

* 一个 `MapValueView` 代理：

  * 若键存在，代理表示存储的值。
  * 若键不存在，代理表示 `defaultValue`。

**深入说明**

这些重载是 `get()` 的**默认值感知**版本：

* 它让你在无需模板的情况下，为每次调用指定**逐次默认值**。
* 返回的代理依然会转换为你赋值目标变量的类型，但当键缺失时，它会像映射中存储了 `defaultValue` 那样进行转换。

例如，如果键 `"retries"` 不存在：

* `get("retries", 3)` 的行为就好像映射在 `"retries"` 下存储了 `3` 一样。

这通常是表达配置默认值最易读的方式。

**示例**

```cpp
Map config("timeoutMs", 5000);

int timeout = config.get("timeoutMs", 1000);   // 5000（存储值）
int retries = config.get("retries", 3);        // 3（回退值）
String host = config.get("host", "localhost"); // "localhost"（回退值）
```

*此示例展示了如何在不使用模板语法的情况下提供显式默认值。*

---

#### `MapValueView operator[](…) const`（代理下标访问）

**函数签名**

```cpp
MapValueView operator[](bool key) const;
MapValueView operator[](int key) const;
MapValueView operator[](float key) const;
MapValueView operator[](const char *key) const;
MapValueView operator[](const String &key) const;
```

**概要**  
返回键对应的值；未找到时返回空类型。

**参数**

* `key` – 要查找的键。提供以下重载：

  * `bool`
  * `int`
  * `float`
  * `const char *`
  * `String`

**返回值**

* 一个 `MapValueView` 代理，表示 `key` 对应的值；如果键不存在则为空类型代理。

**深入说明**

`operator[]` 提供了使用代理进行查找的**简洁数组风格语法**：

```cpp
Map user("name", "Alice", "age", 30);

String name = user["name"]; // proxy → String
int age      = user["age"]; // proxy → int
int score    = user["score"]; // proxy → 0（空 int）
```

其行为与 `get(key)` 相同：

* 如果键存在，代理会转换为存储的值。
* 如果键不存在，代理会转换为你赋值目标类型的**空值**。

由于缺失键会静默转换为默认值，如果你必须区分“缺失”和“存在但值像默认”，应使用 `hasKey()`。

**示例**

```cpp
Map settings("volume", 75, "muted", false);

int  volume = settings["volume"];    // 75
bool muted  = settings["muted"];     // false
bool night  = settings["nightMode"]; // false（缺失 → 空 bool）

if (!settings.hasKey("nightMode")) {
  // 功能完全未配置
}
```

*此示例展示了通过中括号进行查找并由代理执行隐式转换。*

---

#### `template <typename K, typename V> V get(K key, V defaultValue = V()) const`（高级类型化查找）

**函数签名**

```cpp
template <typename K, typename V> V get(K key, V defaultValue = V()) const;
```

**概要**  
返回键对应的值；未找到时返回 defaultValue。

**参数**

* `key` – 要查找的键。通常为 `bool`、`int`、`float`、`const char *` 或 `String`。
* `defaultValue` – 当 `key` 不存在时返回的值。默认为默认构造的 `V`。

**返回值**

* 若键存在，则返回与其关联的值并转换为类型 `V`；否则返回 `defaultValue`。

**深入说明**

此模板版本的 `get()` 提供**直接的类型化访问**，无需显式的代理变量。它在功能上类似于：

```cpp
V value = map.get(key, defaultValue);
```

使用代理重载，但：

* 类型 `V` 绑定在模板参数上，而不是中间变量上。
* 可以更直接地绕过或特化映射的内部代理实现。

对大部分代码来说，非模板的代理重载更简单、更易读：

```cpp
int timeout = config.get("timeoutMs", 1000);      // 推荐
int timeout2 = config.get<String,int>("timeoutMs", 1000); // 高级用法
```

当你需要对模板参数进行显式控制，或者在依赖模板实参推导的代码模式中工作时，可以使用此模板形式。

**示例**

```cpp
Map config("timeoutMs", 5000);

int timeout = config.get<String,int>("timeoutMs", 1000); // 5000
int retries = config.get<String,int>("retries", 3);      // 3（默认值）
```

*此示例用显式模板参数复现基于代理的 `get()` 行为。*

---

#### `template <typename K, typename V> V operator[](K key) const`（高级类型化下标访问）

**函数签名**

```cpp
template <typename K, typename V> V operator[](K key) const;
```

**概要**  
返回键对应的值；未找到时返回空类型值。

**参数**

* `key` – 要查找的键。

**返回值**

* 若找到，则返回与 `key` 关联的值并转换为类型 `V`；否则返回默认构造的 `V`（`V` 的“空类型”）。

**深入说明**

此模板运算符提供了 `operator[]` 的**显式类型变体**，结果类型由模板参数 `V` 决定：

```cpp
int  score = map.operator[]<String,int>("score");
bool ok    = map.operator[]<int,bool>(42);
```

在多数情况下，基于代理的重载更自然：

```cpp
int  score = map["score"]; // 基于代理
bool ok    = map[42];      // 基于代理
```

但在以下场景中你仍可以使用此模板形式：

* 你想显式指定 `V`，或者
* 你在模板密集的代码中工作，且显式模板实参有助于推导。

缺失键的行为与基于代理的 `operator[]` 类似：

* 如果键不存在，运算符会返回默认构造的 `V`。

**示例**

```cpp
Map status("ready", true);

bool ready = status.operator[]<String,bool>("ready");  // true
bool error = status.operator[]<String,bool>("error");  // false（默认 bool）
```

*此示例使用模板形式显式控制结果类型，与基于代理的行为保持一致。*

---

#### `template <typename K> bool hasKey(K key) const`

**函数签名**

```cpp
template <typename K> bool hasKey(K key) const;
```

**概要**  
如果映射包含该键则返回 true。

**参数**

* `key` – 要测试是否存在的键。

**返回值**

* 若键存在则为 `true`；否则为 `false`。

**深入说明**

`hasKey()` 是主要的**存在性检查**函数：

* 当你需要知道某个键是否已被设置时使用它。
* 它避免了 `operator[]` 返回默认值带来的歧义。

由于键是唯一的，`hasKey()` 告诉你是否正好存在一个与该键关联的值。

**示例**

```cpp
Map config("debug", true);

if (config.hasKey("debug")) {
  bool debugEnabled = config.get<String,bool>("debug", false);
}
```

*此示例在检索值之前检查键是否存在。*

---

#### `template <typename K> ValueType typeAt(K key) const`

**函数签名**

```cpp
template <typename K> ValueType typeAt(K key) const;
```

**概要**  
返回键对应值的类型；未找到时返回 TYPE_INVALID。

**参数**

* `key` – 想要检查其值类型的键。

**返回值**

* 描述存储类型的 `ValueType` 枚举值；如果键不存在则为 `TYPE_INVALID`。

**深入说明**

`typeAt()` 允许你在读取值之前**检查**与给定键关联的值的类型。这在可存储混合类型的映射中尤为重要：

* 使用 `typeAt()` 来保护后续以特定 `V` 调用 `get()` 或 `operator[]`。
* 如果返回 `TYPE_INVALID`，则表示键不存在。

**示例**

```cpp
Map data("name", "Alice", "age", 30);

ValueType nameType = data.typeAt("name"); // 可能为 TYPE_STRING
ValueType ageType  = data.typeAt("age");  // 可能为 TYPE_INT
ValueType missing  = data.typeAt("score"); // TYPE_INVALID
```

*此示例检查多个键的类型，以决定如何读取其值。*

---

#### `template <typename K, typename V> K findValue(V value) const`

**函数签名**

```cpp
template <typename K, typename V> K findValue(V value) const;
```

**概要**  
返回第一个具有指定值的键；未找到时返回空类型键。

**参数**

* `value` – 要查找的值。
* 模板参数 `K` – 你期望返回的键类型。

**返回值**

* 第一个类型为 `K` 且其关联值等于 `value` 的键；如果未找到，则返回默认构造的 `K`（“空类型”键）。

**深入说明**

`findValue()` 执行反向查找：

* 它扫描映射并查找值等于 `value` 的条目。
* 如果找到，会返回对应的键并转换为类型 `K`。
* 如果未找到，则返回 `K()`（例如数值键为 `0`，`bool` 键为 `false`，`String` 键为空 `String`）。

重要说明：

* 由于映射中可能有多个键存储相同的值，`findValue()` 会返回根据内部迭代顺序（不保证）找到的**第一个**匹配键。
* 你应选择与实际键类型匹配的 `K`；否则结果可能没有意义。

**示例**

```cpp
Map status(
  "ready", true,
  "connected", true,
  "error", false
);

String key = status.findValue<String,bool>(false);
// key 可能是 "error"（如果这是第一个值为 false 的条目）
```

*此示例查找第一个值为 `false` 的键。*

---

#### `List keys() const`

**函数签名**

```cpp
List keys() const;
```

**概要**  
返回包含所有键的列表。

**参数**

* *(无)*

**返回值**

* 一个包含映射中所有键的 `List`。顺序不保证。

**深入说明**

`keys()` 会将所有键收集到一个 `List` 中：

* 返回列表中的每个元素都是一个键，通常类型为 `String`、`int` 或 `bool`。
* 列表中的键顺序未定义，并且在不同运行或版本之间可能变化。

这对于迭代、调试或导出映射非常有用。

**示例**

```cpp
Map user("name", "Alice", "age", 30);
List keyList = user.keys();
// keyList 包含所有键（例如 ["name", "age"]，顺序任意）
```

*此示例获取键列表以便迭代或检查。*

---

#### `List values() const`

**函数签名**

```cpp
List values() const;
```

**概要**  
返回包含所有值的列表。

**参数**

* *(无)*

**返回值**

* 一个包含映射中所有值的 `List`。顺序与内部键顺序对应，但不保证。

**深入说明**

`values()` 会将所有值收集到一个 `List` 中：

* 返回列表中的每个元素是某个键对应的值。
* 值的顺序对应于键的内部顺序，该顺序未定义。

返回的列表可能包含混合类型（`int`、`String`、`bool`、容器等），你可以在 `List` 上使用 `typeAt()` 检查这些元素。

**示例**

```cpp
Map user("name", "Alice", "age", 30);
List vals = user.values();
// vals 包含 ["Alice", 30]（顺序未定义，但与 keys() 一致）
```

*此示例获取值列表以便后续处理。*

---

#### `template <typename K, typename V> Map &put(K key, V value)`

**函数签名**

```cpp
template <typename K, typename V> Map &put(K key, V value);
```

**概要**  
将 key 设置为 value。返回此映射以支持链式调用。

**参数**

* `key` – 要插入或更新的键。
* `value` – 要与键关联的值。

**返回值**

* 指向 `*this` 的引用，以支持链式调用。

**深入说明**

`put()` 插入或更新一个键值对：

* 如果 `key` **不存在**，`put()` 会添加新条目，并使 `length()` 增加 1。
* 如果 `key` **已存在**，`put()` 会覆盖该键对应的现有值；`length()` 不变。

类型注意事项：

* `key` 必须是受支持的键类型。
* `value` 必须是受支持的值类型。

这是构建和更新映射的主要方法。

**示例**

```cpp
Map config;
config.put("host", "localhost")
      .put("port", 8080)
      .put("port", 9090); // 覆盖之前的端口
```

*此示例设置并随后更新特定键的配置值。*

---

#### `template <typename K> Map &remove(K key)`

**函数签名**

```cpp
template <typename K> Map &remove(K key);
```

**概要**  
移除键值对。返回此映射以支持链式调用。

**参数**

* `key` – 要删除其条目的键。

**返回值**

* 指向 `*this` 的引用。

**深入说明**

`remove()` 会删除 `key` 的条目（如果存在）：

* 成功删除后，`hasKey(key)` 返回 `false`。
* `length()` 减少 1。
* 如果 `key` 不存在，映射保持不变。

这是从映射中删除条目的主要方式。

**示例**

```cpp
Map config("host", "localhost", "port", 8080);
config.remove("port");
// config 中不再存在 "port"
```

*此示例从配置映射中删除一个键值对。*

---

#### `Map &clear()`

**函数签名**

```cpp
Map &clear();
```

**概要**  
移除所有键值对。返回此映射以支持链式调用。

**参数**

* *(无)*

**返回值**

* 指向 `*this` 的引用。

**深入说明**

`clear()` 会清空映射：

* `length()` 变为 `0`。
* `isEmpty()` 返回 `true`。
* 所有现有键值对都被移除。

内部容量可能会被保留以提升性能，但在逻辑上该映射行为与新构造的一样。

**示例**

```cpp
Map cache("a", 1, "b", 2);
cache.clear();
// cache 现在为空
```

*此示例清空缓存映射中的所有条目。*

---

#### `Map &merge(const Map &other)`

**函数签名**

```cpp
Map &merge(const Map &other);
```

**概要**  
将另一个映射合并到此映射中。返回此映射以支持链式调用。

**参数**

* `other` – 其条目将被合并到 `*this` 的源映射。

**返回值**

* 指向 `*this` 的引用。

**深入说明**

`merge()` 合并两个映射：

* 对于 `other` 中的每个键：

  * 如果该键在 `*this` 中**不存在**，则会被添加。
  * 如果该键**已存在**，`other` 中的值会**覆盖** `*this` 中现有值。

`other` 本身不会被修改。

这对于叠加配置或应用覆盖非常有用。

**示例**

```cpp
Map defaults("host", "localhost", "port", 8080);
Map overrides("port", 9090, "debug", true);

defaults.merge(overrides);
// defaults 现在包含：
//   "host"  -> "localhost"
//   "port"  -> 9090     （被覆盖）
//   "debug" -> true     （被添加）
```

*此示例将用户提供的覆盖配置合并到默认设置中。*

---

#### `Map duplicate() const`

**函数签名**

```cpp
Map duplicate() const;
```

**概要**  
返回此映射的副本。

**参数**

* *(无)*

**返回值**

* 一个包含与原始映射相同键值对的新 `Map`。

**深入说明**

`duplicate()` 提供一种显式的、方法风格的方式来拷贝映射。当你希望：

* 保留原始映射，
* 在工作副本上进行修改，

时尤为有用。

其行为与使用拷贝构造函数相同，但在链式调用中可读性更好：

```cpp
Map modified = original.duplicate().merge(overrides);
```

**示例**

```cpp
Map base("mode", "production");
Map work = base.duplicate();

work.put("mode", "development");
// base 仍然是 "mode" = "production"
// work 则是 "mode" = "development"
```

*此示例展示了如何在副本上修改而不影响原始映射。*

---

#### `String toJSONString() const`

**函数签名**

```cpp
String toJSONString() const;
```

**概要**  
将此映射转换为 JSON 对象字符串。

**参数**

* *(无)*

**返回值**

* 包含映射 JSON 对象表示形式的 `String`。

**深入说明**

`toJSONString()` 会将映射序列化为 JSON 对象，例如：

```json
{"name":"Alice","age":30,"active":true}
```

键和值的处理：

* 键会转换为 JSON 对象键（字符串）。非字符串键会转为字符串形式（例如数值键转为其十进制字符串表示）。
* 值会根据其类型转换为 JSON：

  * 数值 → JSON 数字。
  * 布尔值 → `true`/`false`。
  * `String` → 带适当转义的 JSON 字符串。
  * `List`、`Map`、`Set` → 嵌套的 JSON 数组或对象。

生成 JSON 中键的顺序不保证。

**示例**

```cpp
Map user("name", "Alice", "age", 30, "active", true);
String json = user.toJSONString();
// 例如 {"name":"Alice","age":30,"active":true}
```

*此示例将用户映射转换为适用于日志记录或网络传输的 JSON 对象字符串。*

---

#### `static Map FromJSONString(const String &json)`

**函数签名**

```cpp
static Map FromJSONString(const String &json);
```

**概要**  
从 JSON 对象字符串创建映射。

**参数**

* `json` – 要解析的 JSON 对象文本。

**返回值**

* 表示已解析 JSON 对象的 `Map`。

**深入说明**

`FromJSONString()` 解析一个 JSON 对象字符串，并构造一个 `Map`，其中：

* 每个 JSON 键成为映射中的键（通常是 `String`）。
* 每个 JSON 值成为对应的 attoboy 值：

  * JSON 数字 → 数值类型。
  * 布尔值 → `bool`。
  * 字符串 → `String`。
  * 数组 → `List`。
  * 对象 → 嵌套 `Map`。

如果输入字符串不是有效的 JSON 对象，结果映射可能为空或反映错误状态；你应根据需要进行验证或错误处理。

**示例**

```cpp
String json("{\"name\":\"Alice\",\"age\":30,\"active\":true}");
Map user = Map::FromJSONString(json);

// user.get<String,String>("name", String())  -> "Alice"
// user.get<String,int>("age", 0)             -> 30
// user.get<String,bool>("active", false)     -> true
```

*此示例将 JSON 对象解析为映射以便进一步处理。*

---

#### `bool compare(const Map &other) const`

**函数签名**

```cpp
bool compare(const Map &other) const;
```

**概要**  
若此映射与另一个映射相等则返回 true。

**参数**

* `other` – 要比较的映射。

**返回值**

* 若两个映射包含完全相同的键及其关联值则为 `true`；否则为 `false`。

**深入说明**

`compare()` 使用字典语义检查映射相等性：

* 两个映射必须具有相同数量的条目。
* 对于一个映射中的每个键，另一个映射必须：

  * 包含该键。
  * 在该键下存储相等的值。

顺序无关紧要；只要键及其关联的值匹配即可。

此方法在功能上等同于 `operator==`。

**示例**

```cpp
Map a("x", 1, "y", 2);
Map b("y", 2, "x", 1);
Map c("x", 1);

bool ab = a.compare(b);  // true（相同的键值对）
bool ac = a.compare(c);  // false（c 缺少 "y"）
```

*此示例展示了忽略顺序但要求键值内容完全相同的相等性检查。*

---

#### `bool operator==(const Map &other) const`

**函数签名**

```cpp
bool operator==(const Map &other) const;
```

**概要**  
若此映射与另一个映射相等则返回 true。

**参数**

* `other` – 要比较的映射。

**返回值**

* 若映射相等则为 `true`；否则为 `false`。

**深入说明**

此运算符提供惯用的相等比较语法：

```cpp
if (a == b) { ... }
```

它采用与 `compare()` 相同的判定标准：

* 键集合相同。
* 每个键的值相同。

顺序无关。

**示例**

```cpp
Map a("a", 1, "b", 2);
Map b("b", 2, "a", 1);
Map c("a", 1, "b", 3);

bool same = (a == b); // true
bool diff = (a == c); // false（"b" 对应值不同）
```

*此示例演示了使用 `==` 运算符比较映射。*

---

#### `bool operator!=(const Map &other) const`

**函数签名**

```cpp
bool operator!=(const Map &other) const;
```

**概要**  
若此映射与另一个映射不相等则返回 true。

**参数**

* `other` – 要比较的映射。

**返回值**

* 若映射不相等则为 `true`；若相等则为 `false`。

**深入说明**

此运算符只是 `operator==` 的逻辑取反：

* 在 `operator==` 会返回 `false` 时，它返回 `true`。

当你想基于不等条件分支时非常方便。

**示例**

```cpp
Map a("key", 1);
Map b("key", 2);

if (a != b) {
  // 两个映射中 "key" 对应的值不同
}
```

*此示例使用不等比较来检测映射内容的差异。*