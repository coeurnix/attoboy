# Set

## 概览

`attoboy::Set` 类是一个**无序的唯一值集合**。它被设计成标准库 set 的一个紧凑而灵活的替代方案，但不依赖 C++ 标准库。它与其他 attoboy 类型紧密集成：

* 基本类型：`bool`、`int`、`float`
* 文本：`String`
* 嵌套容器：`List`、`Map`、`Set`

在高层次上，`Set` 提供：

* **唯一性** —— 每个值最多出现一次。
* **无序存储** —— 库不保证任何特定的迭代顺序。
* **混合类型元素** —— 单个集合可以存储不同类型的值。
* **集合运算** —— 并集、交集和差集。
* **JSON 集成** —— 与 JSON 数组字符串之间的转换。

当你更关心**成员关系**（“这个值是否存在？”）而不是顺序或重复值时，此类尤为有用。

### 集合、唯一性与顺序

从概念上讲，集合是一个具有以下特性的数学集合：

* **无重复**
  如果你多次添加相同的值，集合中仍然只包含该值的一份拷贝。
* **无顺序保证**
  集合不承诺值会以插入时的顺序出现。在内部，它可能使用哈希或其他不会保留插入顺序的数据结构。

在 `attoboy::Set` 中：

* `put(value)` 在值不存在时将其加入集合。
* 如果添加已存在的值，不会产生任何效果（重复会被静默忽略）。
* `length()` 统计存储了多少个不同的值。
* `contains(value)` 高效检查成员关系。

### 混合类型与相等性

`Set` 可以存储不同类型的值，例如：

* `Set(1, "two", 3.0f)`
* `Set("apple", String("banana"), List(1, 2))`

相等性与成员检查使用内部的类型感知比较。实际上：

* 存储的 `int` `1` 与字符串 `"1"` 是不同的。
* 不同类型的两个值不会被视为相等，即使它们的文本表现形式看起来相似。

为了在执行集合运算时避免意外情况，尽量在可能的情况下使用类型一致的可比较值（例如全部是整数或全部是字符串）。

### 可变性、RAII 与链式调用

与 `String` 不同，`Set` 是**可变的**：

* `put()`、`remove()`、`clear()` 和 `setUnion()` 等方法会就地修改集合。
* 这些方法返回对 `*this` 的引用，从而支持流式 **链式调用**：

  ```cpp
  using namespace attoboy;

  Set items;
  items.put(1)
       .put(2)
       .put(2)    // ignored duplicate
       .remove(1);
  // items now contains only {2}
  ```

`Set` 也遵循 RAII（资源获取即初始化）模式：

* 构造函数获取所需的内部资源。
* 当集合离开作用域时，析构函数会自动释放内存。
* 拷贝与赋值会正确管理所有权；你无需手动释放任何资源。

### 与 `List` 和 JSON 的集成

`Set` 能与其他 attoboy 容器平滑集成：

* 可以从 `List` 构造集合，并自动**移除重复值**。
* 可以通过 `toList()` 将集合转换为 `List`，在需要索引访问或排序时使用。
* 可以在集合与 JSON 之间转换：

  * `toJSONString()` 生成表示集合内容的 JSON 数组字符串。
  * `FromJSONString()` 解析 JSON 数组字符串并据此构建集合。

这些能力使 `Set` 非常适合表示标签、唯一标识符或任何以成员关系和唯一性优先于顺序的值集合。

---

## 参考

下面的每个条目都介绍了 `attoboy::Set` 的一个公开构造函数、方法、静态函数或运算符。对于每一项，你可以看到：

* **Signature（声明）** —— 头文件中的精确声明。
* **Synopsis（概要）** —— 原始的单行 Doxygen 注释。
* **Parameters（参数）**和 **Return value（返回值）** 描述。
* **In Depth（详解）** —— 额外细节、注意事项和简短示例。

> **注意：** 所有示例均假定已使用 `using namespace attoboy;`。

---

#### `Set()`

**Signature**

```cpp
Set();
```

**Synopsis**
Creates an empty set.

**Parameters**

* *(none)*

**Return value**

* *(constructor; not applicable)*

**In Depth**

此构造函数创建一个不含任何值的集合。刚构造完成时：

* `length()` 为 `0`。
* `isEmpty()` 返回 `true`。

通常你会从一个空集合开始，并在之后通过 `put()` 添加值。

**Example**

```cpp
Set flags;
flags.put("debug");
// flags now contains {"debug"}
```

*此示例从空集合开始并添加了一个值。*

---

#### `Set(int capacity)`

**Signature**

```cpp
Set(int capacity);
```

**Synopsis**
Creates an empty set with reserved capacity.

**Parameters**

* `capacity` – Hint for the number of values you expect to store.

**Return value**

* *(constructor; not applicable)*

**In Depth**

此构造函数创建一个空集合，但会预留足够 `capacity` 个元素的内部存储空间。当你事先知道将要插入多少个不同值时，可以减少内部的重新分配次数。

集合本身仍然是空的；在调用 `put()` 之前，`length()` 仍为 `0`。

**Example**

```cpp
int expected = 100;
Set ids(expected);
for (int i = 0; i < expected; ++i) {
  ids.put(i);
}
// ids now contains integers 0..99 (in unspecified order)
```

*此示例为 100 个值预留了容量，然后用这些整数填充集合。*

---

#### `template <typename... Args> Set(const Args &...args)`

**Signature**

```cpp
template <typename... Args> Set(const Args &...args) : Set() {
  variadic_put(args...);
}
```

**Synopsis**
Creates a set with the given values: Set(1, 2, 3)

**Parameters**

* `args` – 要插入的一个或多个值。每个参数都必须是 `Set` 支持的类型（例如 `bool`、`int`、`float`、`const char *`、`String`、`List`、`Map` 或另一个 `Set`）。

**Return value**

* *(constructor; not applicable)*

**In Depth**

这个可变参数构造函数会创建一个新的集合，并使用 `put()` 语义将每个提供的值插入其中：

* 每个参数都会被加入集合。
* 传入参数之间的重复值会被静默忽略。

由于顺序不被保证且重复值会被丢弃，此构造函数非常适合构造小型、类似字面量的集合，并且希望值保持唯一。

**Example**

```cpp
Set numbers(1, 2, 2, 3);
// numbers contains {1, 2, 3}, with the duplicate 2 ignored
```

*此示例通过初始值创建了一个新的集合，展示了重复值不会被存储两次。*

---

#### `Set(const Set &other)`

**Signature**

```cpp
Set(const Set &other);
```

**Synopsis**
Creates a copy of another set.

**Parameters**

* `other` – 要复制的现有集合。

**Return value**

* *(constructor; not applicable)*

**In Depth**

拷贝构造函数会创建一个新集合，包含与 `other` 相同的值。由于集合是无序的，即使内容相同，其内部顺序也可能不同，但：

* `length()` 相同。
* 对任意值调用 `contains(value)` 的结果在两个集合中相同。
* `compare(other)` 和 `operator==` 都会报告二者相等。

**Example**

```cpp
Set original( "a", "b", "c" );
Set copy(original);
// copy initially has the same values as original
```

*此示例复制了一个集合，使两个变量表示等价的集合。*

---

#### `Set(const List &list)`

**Signature**

```cpp
Set(const List &list);
```

**Synopsis**
Creates a set from a list, removing duplicates.

**Parameters**

* `list` – 源 `List`，其中的值将被插入集合。

**Return value**

* *(constructor; not applicable)*

**In Depth**

此构造函数通过以下步骤将 `List` 转换为 `Set`：

1. 遍历 `list` 中的所有元素。
2. 将每个值通过 `put()` 插入集合。

与集合一贯的行为相同，**重复值会被移除**，因此每个不同的值在结果集合中最多出现一次。顺序不会被保留。

**Example**

```cpp
List values("red", "green", "red", "blue");
Set colors(values);
// colors contains {"red", "green", "blue"}, with duplicates removed
```

*此示例将包含重复值的列表转换为仅包含唯一值的集合。*

---

#### `~Set()`

**Signature**

```cpp
~Set();
```

**Synopsis**
Destroys the set and frees memory.

**Parameters**

* *(none)*

**Return value**

* *(destructor; not applicable)*

**In Depth**

当 `Set` 离开作用域或被销毁时，其析构函数会释放所有内部资源（例如已分配的表或缓冲区）。你无需手动释放任何东西；RAII 模式确保内存会自动清理。

**Example**

```cpp
{
  Set temp(1, 2, 3);
  // use temp here
}
// temp is destroyed here; internal resources are freed
```

*此示例展示了一个作用域内的集合，会在代码块结束时自动清理。*

---

#### `Set &operator=(const Set &other)`

**Signature**

```cpp
Set &operator=(const Set &other);
```

**Synopsis**
Assigns another set to this set.

**Parameters**

* `other` – 其内容将替换当前集合的集合。

**Return value**

* 对 `*this` 的引用，可用于链式赋值。

**In Depth**

拷贝赋值运算符使当前集合的内容与 `other` 完全相同。`*this` 中之前存储的任何值都会被丢弃。

赋值完成后：

* `length()` 与 `other.length()` 相同。
* 对任意值调用 `contains(value)` 的结果在两个集合中相同。

自赋值（`set = set`）是安全的。

**Example**

```cpp
Set a("apple", "banana");
Set b("cherry");

a = b;
// a now contains only the values that were in b
```

*此示例将一个集合的内容替换为另一个集合的内容。*

---

#### `int length() const`

**Signature**

```cpp
int length() const;
```

**Synopsis**
Returns the number of values in the set.

**Parameters**

* *(none)*

**Return value**

* 集合中存储的不同值的数量。

**In Depth**

`length()` 统计当前存在的唯一值数量。由于重复值不会被存储，这个数量就是**不同元素**的个数。

**Example**

```cpp
Set s(1, 2, 2, 3);
int n = s.length();  // 3
```

*此示例表明重复值不会增加集合的长度。*

---

#### `bool isEmpty() const`

**Signature**

```cpp
bool isEmpty() const;
```

**Synopsis**
Returns true if the set is empty.

**Parameters**

* *(none)*

**Return value**

* 若 `length() == 0` 则为 `true`，否则为 `false`。

**In Depth**

`isEmpty()` 是检查集合是否包含任何值的便捷方法。它在语义上等价于 `length() == 0`，但在条件代码中通常更易读。

**Example**

```cpp
Set s;
if (s.isEmpty()) {
  s.put("first");
}
```

*此示例仅在集合当前为空时，为其初始化一个值。*

---

#### `template <typename T> Set &put(T value)`

**Signature**

```cpp
template <typename T> Set &put(T value) {
  put_impl(value);
  return *this;
}
```

**Synopsis**
Adds a value to the set. Duplicates are ignored. Returns this set for chaining.

**Parameters**

* `value` – 要添加的值。必须是 `Set` 支持的类型（`bool`、`int`、`float`、`const char *`、`String`、`List`、`Map`、`Set` 等）。

**Return value**

* 对 `*this` 的引用，用于方法链式调用。

**In Depth**

`put()` 将 `value` 插入集合：

* 如果 `value` 尚不存在，则将其加入，并且 `length()` 增加 1。
* 如果 `value` 已存在，则集合不变；重复值会被静默忽略。

该方法是模板方法，内部的 `put_impl()` 会根据实际类型选择合适的重载。

由于顺序不被保证，`value` 在内部结构中的位置没有任何语义意义。

**Example**

```cpp
Set tags;
tags.put("alpha")
    .put("beta")
    .put("alpha");  // duplicate ignored
// tags contains {"alpha", "beta"}
```

*此示例展示了添加重复值不会改变集合。*

---

#### `template <typename T> Set &remove(T value)`

**Signature**

```cpp
template <typename T> Set &remove(T value) {
  remove_impl(value);
  return *this;
}
```

**Synopsis**
Removes a value from the set. Returns this set for chaining.

**Parameters**

* `value` – 要移除的值。

**Return value**

* 对 `*this` 的引用。

**In Depth**

`remove()` 会在集合中存在该值时将其移除。成功移除后：

* `contains(value)` 返回 `false`。
* `length()` 减少 1。

如果该值不存在，则此操作等同于无操作。

类型匹配非常重要：只有根据集合比较规则被视为相等的值才会被移除。

**Example**

```cpp
Set ids(1, 2, 3);
ids.remove(2);
// ids contains {1, 3}
```

*此示例从集合中移除了一个值，其余值保持不变。*

---

#### `Set &clear()`

**Signature**

```cpp
Set &clear();
```

**Synopsis**
Removes all values. Returns this set for chaining.

**Parameters**

* *(none)*

**Return value**

* 对 `*this` 的引用。

**In Depth**

`clear()` 会彻底清空集合：

* `length()` 变为 `0`。
* `isEmpty()` 返回 `true`。
* 所有之前的值都会被移除。

为了效率考虑，内部容量可能会保留，但集合的行为等同于新构造的集合。

**Example**

```cpp
Set s(1, 2, 3);
s.clear();
// s is now empty
```

*此示例从集合中移除所有值。*

---

#### `Set &setUnion(const Set &other)`

**Signature**

```cpp
Set &setUnion(const Set &other);
```

**Synopsis**
Adds all values from another set (union). Returns this set for chaining.

**Parameters**

* `other` – 其值将被合并进当前集合的另一个集合。

**Return value**

* 对 `*this` 的引用。

**In Depth**

`setUnion()` 将当前集合修改为 `*this` 与 `other` 的**并集**：

* 任一集合中存在的所有值，最终都会出现在 `*this` 中。
* `other` 中已存在于 `*this` 的值会被视为重复并被忽略。

`other` 集合本身不会被修改。

**Example**

```cpp
Set a(1, 2);
Set b(2, 3);

a.setUnion(b);
// a contains {1, 2, 3}
// b remains {2, 3}
```

*此示例将两个集合的所有值合并到一个集合中，而不会引入重复值。*

---

#### `Set &intersect(const Set &other)`

**Signature**

```cpp
Set &intersect(const Set &other);
```

**Synopsis**
Keeps only values in both sets (intersection). Returns this set for chaining.

**Parameters**

* `other` – 要与之求交集的另一个集合。

**Return value**

* 对 `*this` 的引用。

**In Depth**

`intersect()` 会修改当前集合，仅保留同时存在于 `*this` 和 `other` 中的值：

* 不存在于 `other` 中的值会从 `*this` 中移除。
* 存在于 `other` 但不存在于 `*this` 的值不会被添加进来。

用数学形式表示：

```text
*this := *this ∩ other
```

**Example**

```cpp
Set a(1, 2, 3);
Set b(2, 3, 4);

a.intersect(b);
// a contains {2, 3}
```

*此示例计算两个集合的交集，仅保留共有值。*

---

#### `Set &subtract(const Set &other)`

**Signature**

```cpp
Set &subtract(const Set &other);
```

**Synopsis**
Removes all values in other (difference). Returns this set for chaining.

**Parameters**

* `other` – 其中的值将从当前集合中移除的集合。

**Return value**

* 对 `*this` 的引用。

**In Depth**

`subtract()` 执行**集合差**运算：

* 所有同时存在于 `*this` 和 `other` 中的值都会从 `*this` 中移除。
* 仅在 `*this` 中存在的值会被保留下来。
* `other` 不会被修改。

用数学形式表示：

```text
*this := *this − other
```

**Example**

```cpp
Set a(1, 2, 3, 4);
Set b(2, 4);

a.subtract(b);
// a contains {1, 3}
```

*此示例移除了出现在另一个集合中的所有值。*

---

#### `template <typename T> bool contains(T value) const`

**Signature**

```cpp
template <typename T> bool contains(T value) const;
```

**Synopsis**
Returns true if the set contains the value.

**Parameters**

* `value` – 要测试成员关系的值。

**Return value**

* 如果该值存在于集合中则为 `true`，否则为 `false`。

**In Depth**

`contains()` 是主要的成员测试函数。它检查集合中是否存储了与 `value` 相等的值。为了获得可预期的结果，你应当传入与插入时相同（或兼容）的类型。

**Example**

```cpp
Set roles("admin", "user");
bool isAdmin = roles.contains("admin");  // true
bool isGuest = roles.contains("guest");  // false
```

*此示例检查集合中是否存在某些角色。*

---

#### `Set duplicate() const`

**Signature**

```cpp
Set duplicate() const;
```

**Synopsis**
Returns a copy of this set.

**Parameters**

* *(none)*

**Return value**

* 包含与原集合相同值的新 `Set`。

**In Depth**

`duplicate()` 显式创建集合的一个副本。当你希望在副本上执行修改操作（如 `setUnion()` 或 `subtract()`），同时保持原集合不变时，这非常有用。

**Example**

```cpp
Set original(1, 2, 3);
Set copy = original.duplicate();

copy.remove(2);
// original still contains {1, 2, 3}
// copy now contains {1, 3}
```

*此示例演示了如何在修改集合副本的同时保留原集合。*

---

#### `List toList() const`

**Signature**

```cpp
List toList() const;
```

**Synopsis**
Returns a list of all values in the set.

**Parameters**

* *(none)*

**Return value**

* 包含集合中所有值的 `List`，顺序未指定。

**In Depth**

`toList()` 将集合转换为 `List`：

* 集合中的每个值在结果列表中恰好出现一次。
* 元素顺序不被保证，并且在不同的运行或版本之间可能会变化。

当你需要索引访问、排序、导出 CSV，或其他基于列表的操作，而原始数据又天然是唯一值集合时，这个转换非常有用。

**Example**

```cpp
Set numbers(3, 1, 2);
List asList = numbers.toList();
// asList contains the same values {1, 2, 3} in some order
```

*此示例将集合转换为列表以便进一步处理。*

---

#### `String toJSONString() const`

**Signature**

```cpp
String toJSONString() const;
```

**Synopsis**
Converts this set to a JSON array string.

**Parameters**

* *(none)*

**Return value**

* 包含该集合 JSON 数组表示形式的 `String`。

**In Depth**

`toJSONString()` 会将集合序列化为 JSON 数组。例如，包含 `1`、`"two"` 和 `true` 的集合可能会被序列化为：

```json
[1,"two",true]
```

重要说明：

* JSON 数组中元素的顺序不被保证。
* 值会根据其类型转换为 JSON：

  * 数值 → JSON 数字
  * 布尔值 → `true`/`false`
  * 字符串 → JSON 字符串
  * 嵌套容器（`List`、`Map`、`Set`）→ JSON 数组/对象

**Example**

```cpp
Set flags("alpha", "beta");
String json = flags.toJSONString();
// e.g., ["alpha","beta"] (order not guaranteed)
```

*此示例将字符串集合序列化为 JSON 数组字符串。*

---

#### `static Set FromJSONString(const String &json)`

**Signature**

```cpp
static Set FromJSONString(const String &json);
```

**Synopsis**
Creates a set from a JSON array string.

**Parameters**

* `json` – 要解析的 JSON 数组文本。

**Return value**

* 从 JSON 数组解析得到的 `Set`。

**In Depth**

`FromJSONString()` 会解析一个 JSON 数组字符串并构建集合：

* JSON 数组中的每个元素都会成为集合中的一个值。
* 如果 JSON 中包含重复元素，结果集合中仍然只会保留每个值的一份拷贝。
* 嵌套数组和对象分别会转换为 `List` 和 `Map` 值，与库中其他 JSON 相关函数保持一致。

如果输入不是有效的 JSON 数组，结果可能是空集合或反映错误状态；你应当适当验证或处理异常输入。

**Example**

```cpp
String json("[\"alpha\",\"beta\",\"alpha\"]");
Set flags = Set::FromJSONString(json);
// flags contains {"alpha", "beta"}
```

*此示例将 JSON 数组解析为集合，并自动移除重复值。*

---

#### `bool compare(const Set &other) const`

**Signature**

```cpp
bool compare(const Set &other) const;
```

**Synopsis**
Returns true if this set equals the other set.

**Parameters**

* `other` – 要进行比较的集合。

**Return value**

* 若两个集合包含完全相同的值则为 `true`，否则为 `false`。

**In Depth**

`compare()` 按集合语义检查相等性：

* 如果一个集合中的每个值都出现在另一个集合中，反之亦然，则两个集合相等。
* 内部顺序无关紧要；只比较成员是否一致。
* 由于集合不存储重复值，重复次数的差异不存在。

此函数在功能上等价于 `operator==`，主要作为一个具名替代。

**Example**

```cpp
Set a(1, 2, 3);
Set b(3, 2, 1);
Set c(1, 2);

bool ab = a.compare(b);  // true (same values)
bool ac = a.compare(c);  // false (c is missing 3)
```

*此示例展示了忽略顺序但要求值集合完全相同的相等性比较。*

---

#### `bool operator==(const Set &other) const`

**Signature**

```cpp
bool operator==(const Set &other) const;
```

**Synopsis**
Returns true if this set equals the other set.

**Parameters**

* `other` – 要比较的集合。

**Return value**

* 如果两个集合相等则为 `true`，否则为 `false`。

**In Depth**

此运算符提供集合相等性的惯用语法：

```cpp
if (a == b) { ... }
```

其内部使用与 `compare()` 相同的逻辑：当且仅当两个集合包含相同的值（忽略内部顺序）时，它们才被视为相等。

**Example**

```cpp
Set a("x", "y");
Set b("y", "x");
Set c("x");

bool same = (a == b); // true
bool diff = (a == c); // false
```

*此示例演示了使用 `==` 运算符对集合进行相等性比较。*

---

#### `bool operator!=(const Set &other) const`

**Signature**

```cpp
bool operator!=(const Set &other) const;
```

**Synopsis**
Returns true if this set does not equal the other set.

**Parameters**

* `other` – 要比较的集合。

**Return value**

* 若两个集合不相等则为 `true`，若它们相等则为 `false`。

**In Depth**

此运算符是 `operator==` 的逻辑取反版本。当你希望基于“不相等”进行分支时非常方便：

```cpp
if (a != b) { ... }
```

**Example**

```cpp
Set a(1, 2);
Set b(2, 3);

if (a != b) {
  // sets contain different values
}
```

*此示例使用不等比较来检测两个集合是否不同。*

---

#### `template <typename T> Set operator+(T value) const`

**Signature**

```cpp
template <typename T> Set operator+(T value) const {
  Set result = duplicate();
  result.put(value);
  return result;
}
```

**Synopsis**
Returns a new set with the value added.

**Parameters**

* `value` – 要添加到当前集合副本中的值。

**Return value**

* 一个新的 `Set`，其中包含原集合的所有值，以及 `value`（如果其之前尚未存在）。

**In Depth**

此运算符提供一种**函数式风格**的方式来扩展集合，而不修改原集合本身：

1. 复制当前集合。
2. 向副本中添加 `value`（使用 `put()` 语义）。
3. 返回新的集合，原集合保持不变。

由于集合会忽略重复值，当添加已存在的值时，得到的集合内容与原集合相同。

**Example**

```cpp
Set base("a", "b");
Set extended = base + "c";
// base still contains {"a", "b"}
// extended contains {"a", "b", "c"}
```

*此示例使用 `+` 运算符创建了一个带有额外值的新集合，同时保留了原集合。*