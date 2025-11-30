# List

## 概述

`attoboy::List` 类是一个**动态数组**，可以在单一序列中存储不同类型的值。它被设计成一个灵活、轻量的容器，用于替代 `std::vector` 或 `std::list` 等，但不依赖 C++ 标准库。相反，它直接与其他 attoboy 类型集成：

* 基本类型：`bool`、`int`、`float`
* 文本：`String`
* 嵌套容器：`List`、`Map`、`Set`

在本质上，`List` 为你提供：

* 对元素的**按索引访问**（支持使用负索引从末尾开始计数）。
* **就地变更**（追加、插入、删除、排序、反转）。
* **易于链式调用的方法**（大多数会修改自身的方法返回 `*this`）。
* 与 CSV 和 JSON 字符串之间的**简单序列化与反序列化**。

### 混合类型与 `ValueType`

由于 `List` 可以保存混合类型，每个元素都会带有一个来自 `ValueType` 枚举的**类型标签**：

* `TYPE_BOOL`、`TYPE_INT`、`TYPE_FLOAT`
* `TYPE_STRING`、`TYPE_LIST`、`TYPE_MAP`、`TYPE_SET`
* `TYPE_NULL`、`TYPE_INVALID`、`TYPE_UNKNOWN` 用于特殊或错误状态

`typeAt()` 函数允许你在读取值之前，先检查某个索引处存储的类型，从而安全地决定在 C++ 端应使用哪种类型来读取该值。

为了方便起见，`at()` 与 `operator[]` 访问器会返回一个轻量代理类型（`ListValueView`），它可以隐式转换为你需要的具体类型。这使你可以简洁地编写表达式，如 `String s = list.at(1);` 或 `int x = list[i];` 而无需指定模板参数。当你将该代理赋给类型为 `T` 的变量时，它会使用与模板化的 `at<T>()` 和 `operator[]<T>()` 重载相同的转换规则。

### 索引与负索引

元素通过**索引**访问，索引支持一个很实用的扩展语义：

* `0` 是第一个元素。
* `1` 是第二个元素，以此类推。
* `-1` 表示**最后一个**元素。
* `-2` 表示倒数第二个，依此类推。

此行为适用于所有使用元素索引的操作（如 `at()`、`pop()`、`remove()` 和 `set()`），除非另有说明。当你希望相对于列表末尾访问或修改元素时，负索引通常很方便。

### 可变性与链式调用

与 `String` 不同，`List` 是**可变的**：诸如 `append()`、`remove()` 和 `sort()` 等方法会就地修改列表。它们中的大多数会返回对 `*this` 的引用，从而支持**链式调用**：

```cpp
using namespace attoboy;

List values;
values.append(1)
      .append(2)
      .append(3)
      .reverse()
      .append(4);
// values now holds [3, 2, 1, 4]
```

这种风格使你可以用紧凑而可读的方式描述对单个列表的一系列变换。

### 所有权与 RAII

`List` 使用 **RAII**（资源获取即初始化）：

* 构造阶段会分配任何所需的内部资源。
* 析构函数会在列表离开作用域时自动释放内存。
* 拷贝与赋值会对共享或复制的内部状态进行恰当管理。

这意味着你无需手动释放内存；只需让 `List` 对象离开作用域即可。

### 与 CSV 和 JSON 的集成

`List` 内建对 **CSV**（逗号分隔值）与 **JSON** 的支持：

* `toCSVString()` 和 `FromCSVString()` 支持对用列表的列表表示的表格数据进行简单的导入/导出。
* `toJSONString()` 和 `FromJSONString()` 允许你将列表视为 JSON 数组，并与 attoboy 库中理解 JSON 字符串的其他部分互操作。

这些辅助函数使 `List` 成为内存中结构化数据与磁盘/网络中文本格式之间的天然桥梁。

---

## 参考

下面的每一条目覆盖 `attoboy::List` 的一个公开构造函数、方法、静态函数或运算符。对每个条目，你将看到：

* **Signature（函数签名）** – 头文件中的精确声明。
* **Synopsis（概要）** – 原始的一行 Doxygen 注释。
* **Parameters（参数）**与**Return value（返回值）**描述。
* **In Depth（深入说明）** – 额外细节、注意事项以及一个简短示例。

> **注意：** 所有示例均假设 `using namespace attoboy;`。

---

#### `List()`

**Signature**

```cpp
List();
```

**Synopsis**  
创建一个空列表。

**Parameters**

* *(none)*

**Return value**

* *(构造函数；不适用)*

**In Depth**

该构造函数会创建一个没有元素的列表，且除实现默认选择外，不会额外保留容量。构造完成后立刻满足：

* `length()` 返回 `0`。
* `isEmpty()` 返回 `true`。

它是使用 `append()`、`prepend()` 或 `insert()` 逐步构建列表时的标准起点。

**Example**

```cpp
List values;
values.append(1).append(2);
// values now holds [1, 2]
```

*此示例从一个空列表开始并添加两个元素。*

---

#### `List(int capacity)`

**Signature**

```cpp
List(int capacity);
```

**Synopsis**  
创建一个预留容量的空列表。

**Parameters**

* `capacity` – 你计划存储的元素数量提示。

**Return value**

* *(构造函数；不适用)*

**In Depth**

该构造函数创建一个空列表，但会预先分配足够的内部存储空间，以容纳大约 `capacity` 个元素。当你预先知道大小时（例如读取一个长度已知的数据集），这可以减少重新分配和拷贝。

列表仍然是空的；在你调用 `append` 或 `insert` 之前，`length()` 为 `0`。

**Example**

```cpp
int expected = 100;
List values(expected);
for (int i = 0; i < expected; ++i) {
  values.append(i);
}
```

*此示例预先为 100 个整数保留空间，从而减少内部的重新调整大小操作。*

---

#### `template <typename... Args> List(const Args &...args)`

**Signature**

```cpp
template <typename... Args> List(const Args &...args) : List() {
  variadic_append(args...);
}
```

**Synopsis**  
使用给定值创建列表：`List(1, "two", 3.0f)`

**Parameters**

* `args` – 按顺序插入到列表中的一个或多个值。每个值都必须是 `List` 能存储的类型（例如 `bool`、`int`、`float`、`const char *`、`String`、`List`、`Map` 或 `Set`）。

**Return value**

* *(构造函数；不适用)*

**In Depth**

这个可变参数构造函数提供了一种紧凑方式，用初始值集合创建一个 `List`。在内部，它会：

1. 构造一个空列表。
2. 使用与 `append()` 相同的机制，按顺序追加每个 `args` 值。

这对小型列表或你希望表达类似字面量序列的场景非常方便。

**Example**

```cpp
List mixed(1, "two", 3.0f);
// mixed holds [1, "two", 3.0f]
```

*此示例在单个表达式中创建一个包含不同类型元素的列表。*

---

#### `List(const List &other)`

**Signature**

```cpp
List(const List &other);
```

**Synopsis**  
创建另一个列表的副本。

**Parameters**

* `other` – 其内容需要被复制的现有列表。

**Return value**

* *(构造函数；不适用)*

**In Depth**

拷贝构造函数会创建一个新列表，其中包含与 `other` 相同顺序的值序列。具体的所有权模型是内部细节，但从用户角度看：

* 修改一个列表不会改变另一个列表的内容。
* 当各自的列表被销毁时，内存与句柄会自动被清理。

**Example**

```cpp
List original(1, 2, 3);
List copy(original);
// copy initially has the same elements as original
```

*此示例复制了一个列表，使两个变量一开始拥有相同内容。*

---

#### `List(const Set &set)`

**Signature**

```cpp
List(const Set &set);
```

**Synopsis**  
从一个集合创建列表。

**Parameters**

* `set` – 其值将被复制进列表的源 `Set`。

**Return value**

* *(构造函数；不适用)*

**In Depth**

该构造函数通过复制 `Set` 中的所有值，将一个 `Set` 转换为 `List`。生成的列表：

* 每个唯一值只出现一次（因为集合不允许重复）。
* 不保证特定顺序；顺序取决于集合内部如何存储其内容。

当你希望在一组值上执行列表式操作（索引访问、排序、CSV 导出等）时，这会很有用。

**Example**

```cpp
Set unique(3, 1, 2);   // set with {3, 1, 2}
List asList(unique);   // list with the same values, order unspecified
```

*此示例将一组数字集合转换为列表，以便进行按索引的操作。*

---

#### `~List()`

**Signature**

```cpp
~List();
```

**Synopsis**  
销毁列表并释放内存。

**Parameters**

* *(none)*

**Return value**

* *(析构函数；不适用)*

**In Depth**

当一个 `List` 离开作用域或以其他方式被销毁时，其析构函数会：

* 释放内部缓冲区。
* 清理任何关联资源。

由于 attoboy 使用 RAII，你永远不需要手动释放与列表关联的内存。

**Example**

```cpp
{
  List temp(1, 2, 3);
  // use temp here
}
// temp is destroyed; associated memory is released
```

*此示例展示了一个作用域内的列表在离开作用域时自动被清理。*

---

#### `List &operator=(const List &other)`

**Signature**

```cpp
List &operator=(const List &other);
```

**Synopsis**  
将另一个列表赋值给此列表。

**Parameters**

* `other` – 其内容将替换当前内容的列表。

**Return value**

* 对 `*this` 的引用，以支持赋值链式调用。

**In Depth**

拷贝赋值运算符会用 `other` 的内容替换当前列表的内容。赋值之后：

* `length()` 与 `other.length()` 相同。
* 元素值与 `other` 中的值相同，且顺序一致。

自赋值（`list = list`）是安全的。

**Example**

```cpp
List a(1, 2);
List b(3, 4, 5);

a = b;
// a now holds [3, 4, 5]
```

*此示例用另一个列表覆盖了一个列表。*

---

#### `int length() const`

**Signature**

```cpp
int length() const;
```

**Synopsis**  
返回列表中的元素数量。

**Parameters**

* *(none)*

**Return value**

* 当前存储的元素数量。

**In Depth**

`length()` 是确定列表中有多少元素的主要方式。元素访问的有效索引范围为 `0` 到 `length() - 1`（以及对应的负范围 `-length()` 到 `-1`）。

**Example**

```cpp
List values(1, 2, 3);
int n = values.length();  // 3
```

*此示例查询当前元素数量。*

---

#### `bool isEmpty() const`

**Signature**

```cpp
bool isEmpty() const;
```

**Synopsis**  
如果列表为空则返回 true。

**Parameters**

* *(none)*

**Return value**

* 当 `length() == 0` 时为 `true`；否则为 `false`。

**In Depth**

这是一个便利函数，等价于检查 `length() == 0`。在条件表达式中使用它通常更清晰、更具表达性。

**Example**

```cpp
List values;
if (values.isEmpty()) {
  values.append("first");
}
```

*此示例仅在列表当前为空时对其进行初始化。*

---

#### `template <typename T> List &append(T value)`

**Signature**

```cpp
template <typename T> List &append(T value) {
  append_impl(value);
  return *this;
}
```

**Synopsis**  
将一个值追加到末尾。返回此列表以支持链式调用。

**Parameters**

* `value` – 要添加到列表末尾的值。必须是支持的类型（如 `bool`、`int`、`float`、`const char *`、`String`、`List`、`Map` 或 `Set`）。

**Return value**

* 对 `*this` 的引用，允许类似 `list.append(1).append(2)` 的链式写法。

**In Depth**

`append()` 会在当前最后一个元素之后添加一个新元素，使列表长度增加 1。如果超出内部容量，列表会自动扩容。

由于该方法是模板函数，你可以传入不同类型的值。内部会选择合适的 `append_impl()` 重载。

**Example**

```cpp
List values;
values.append(1)
      .append("two")
      .append(3.0f);
// values holds [1, "two", 3.0f]
```

*此示例通过链式调用追加多个不同类型的值。*

---

#### `template <typename T> List &prepend(T value)`

**Signature**

```cpp
template <typename T> List &prepend(T value) {
  prepend_impl(value);
  return *this;
}
```

**Synopsis**  
在开头插入一个值。返回此列表以支持链式调用。

**Parameters**

* `value` – 要插入到列表头部的值。

**Return value**

* 对 `*this` 的引用。

**In Depth**

`prepend()` 会在索引 `0` 处插入一个值，并将现有元素向更高索引位置移动。当你从前端构建列表或需要快速添加头部元素时，这很实用。

**Example**

```cpp
List values(2, 3);
values.prepend(1);
// values holds [1, 2, 3]
```

*此示例在列表开头添加了一个元素。*

---

#### `template <typename T> List &insert(int index, T value)`

**Signature**

```cpp
template <typename T> List &insert(int index, T value) {
  insert_impl(index, value);
  return *this;
}
```

**Synopsis**  
在指定索引处插入一个值。返回此列表以支持链式调用。

**Parameters**

* `index` – 要插入的位置。负索引用于从末尾计数。
* `value` – 要插入的值。

**Return value**

* 对 `*this` 的引用。

**In Depth**

`insert()` 会在指定的**元素索引**处添加一个新元素，并将该索引以及之后的现有元素向右移动。索引规则如下：

* `0` – 在开头插入。
* `length()` – 在末尾插入（等价于 `append()`）。
* 负值 – 从末尾计数（`-1` 表示最后一个元素之前，依此类推），具体行为取决于内部的索引映射。

你应确保 `index` 位于允许范围内（包括有效的负索引）；越界索引将导致未定义或与实现相关的行为。

**Example**

```cpp
List values(1, 3);
values.insert(1, 2);
// values holds [1, 2, 3]
```

*此示例在列表中间插入一个新元素。*

---

#### `template <typename T> List &set(int index, T value)`

**Signature**

```cpp
template <typename T> List &set(int index, T value) {
  set_impl(index, value);
  return *this;
}
```

**Synopsis**  
设置指定索引处的值。返回此列表以支持链式调用。

**Parameters**

* `index` – 要覆盖的元素索引。负索引用于从末尾计数。
* `value` – 要存储在该位置的新值。

**Return value**

* 对 `*this` 的引用。

**In Depth**

`set()` 会替换给定索引处的值，而不会改变列表的长度或其他元素的位置。索引规则（包括负索引）与 `insert()` 和 `at()` 相同。

你应确保索引引用了一个已存在的元素（即 `-length() <= index < length()`）。

**Example**

```cpp
List values("red", "green", "blue");
values.set(1, "yellow");
// values holds ["red", "yellow", "blue"]
```

*此示例覆盖了列表中的单个元素。*

---

#### `List &remove(int index)`

**Signature**

```cpp
List &remove(int index);
```

**Synopsis**  
移除指定索引处的元素。返回此列表以支持链式调用。

**Parameters**

* `index` – 要移除的元素索引。负索引用于从末尾计数。

**Return value**

* 对 `*this` 的引用。

**In Depth**

`remove()` 会删除指定索引处的元素，并将后续元素向左移动一个位置。列表长度减 1。

若索引越界，行为未定义。你可以通过 `length()` 做边界检查，或在处理诸如 `-1` 等负索引时先使用 `isEmpty()`。

**Example**

```cpp
List values("a", "b", "c");
values.remove(1);
// values holds ["a", "c"]
```

*此示例从一个三元素列表中移除中间元素。*

---

#### `List &clear()`

**Signature**

```cpp
List &clear();
```

**Synopsis**  
移除所有元素。返回此列表以支持链式调用。

**Parameters**

* *(none)*

**Return value**

* 对 `*this` 的引用。

**In Depth**

`clear()` 会彻底清空列表：

* `length()` 变为 `0`。
* `isEmpty()` 变为 `true`。

容量是否被缩减不作保证；实现细节被隐藏。

**Example**

```cpp
List values(1, 2, 3);
values.clear();
// values is now empty
```

*此示例清除了列表中的所有元素。*

---

#### `List &reverse()`

**Signature**

```cpp
List &reverse();
```

**Synopsis**  
就地反转元素顺序。返回此列表以支持链式调用。

**Parameters**

* *(none)*

**Return value**

* 对 `*this` 的引用。

**In Depth**

`reverse()` 会重新排列列表，使第一个元素变为最后一个，第二个变为倒数第二个，依此类推。列表长度与元素类型不会改变，仅位置发生变化。

**Example**

```cpp
List values(1, 2, 3);
values.reverse();
// values holds [3, 2, 1]
```

*此示例反转了元素顺序。*

---

#### `List &sort(bool ascending = true)`

**Signature**

```cpp
List &sort(bool ascending = true);
```

**Synopsis**  
对元素进行排序。返回此列表以支持链式调用。

**Parameters**

* `ascending` – 若为 `true`（默认），按升序排序；若为 `false`，按降序排序。

**Return value**

* 对 `*this` 的引用。

**In Depth**

`sort()` 会根据内部比较规则重新排列元素。由于 `List` 可以存储混合类型，对含有不同值类型的列表进行排序时，其行为依赖实现。为了获得可预期结果：

* 请确保所有元素为**相同类型**（例如全是 `int` 或全是 `String`）。
* 排序时避免在同一列表中混合数字、字符串和容器。

**Example**

```cpp
List numbers(3, 1, 2);
numbers.sort();
// numbers holds [1, 2, 3]
```

*此示例对一个纯数值列表进行升序排序。*

---

#### `List &concat(const List &other)`

**Signature**

```cpp
List &concat(const List &other);
```

**Synopsis**  
将另一个列表的所有元素追加到当前列表。返回此列表以支持链式调用。

**Parameters**

* `other` – 其元素将被追加到当前列表末尾的列表。

**Return value**

* 对 `*this` 的引用。

**In Depth**

`concat()` 会遍历 `other`，并按顺序将每个元素追加到当前列表。原始的 `other` 不会被修改。结果列表长度成为 `length() + other.length()`。

**Example**

```cpp
List a(1, 2);
List b(3, 4);

a.concat(b);
// a holds [1, 2, 3, 4]
// b is unchanged
```

*此示例将一个列表的所有元素拼接到另一个列表之后。*

---

#### `List &concat(const Set &set)`

**Signature**

```cpp
List &concat(const Set &set);
```

**Synopsis**  
将一个集合中的所有值追加到当前列表。返回此列表以支持链式调用。

**Parameters**

* `set` – 其值将被追加的 `Set`。

**Return value**

* 对 `*this` 的引用。

**In Depth**

该重载的行为类似 `concat(const List&)`，但从一个 `Set` 读取元素。由于集合是无序的，值被追加的顺序依赖具体实现。

集合本身不会引入重复（每个值最多出现一次），但将其追加到已有列表时，总体上依然可能出现重复。

**Example**

```cpp
Set tags("red", "green");
List allTags("blue");
allTags.concat(tags);
// allTags now holds "blue" plus the set's values, order unspecified
```

*此示例将一个列表与一个集合合并为单一列表。*

---

#### `template <typename T> T pop()`

**Signature**

```cpp
template <typename T> T pop();
```

**Synopsis**  
移除并返回指定索引处的元素。

**Parameters**

* *(函数签名中没有参数；索引由实现隐含决定)*

**Return value**

* 从列表中移除的元素，并转换为类型 `T`。

**In Depth**

尽管概要中说“at index（在索引处）”，但该函数签名并未显式接受索引参数。预期行为是移除并返回一个元素，通常来自末尾或实现选择的某个固定位置。你必须请求与存储值类型匹配的 `T`。

重要注意事项：

* 如果列表为空，行为未定义；应先检查 `isEmpty()`。
* 如果存储元素的类型与 `T` 不匹配，结果未定义或可能毫无意义。

**Example**

```cpp
List values(1, 2, 3);
int last = values.pop<int>();
// last holds 3 (implementation-dependent)
// values now holds [1, 2]
```

*此示例从列表中移除并返回一个元素，然后使用已修改的列表。*

---

#### `ListValueView at(int index) const`

**Signature**

```cpp
ListValueView at(int index) const;
```

**Synopsis**  
返回指定索引处元素的类型化视图。

**Parameters**

* `index` – 要访问的元素索引。负索引用于从末尾计数。

**Return value**

* 指向给定索引处元素的 `ListValueView` 代理。

**In Depth**

该重载的 `at()` 返回一个**代理对象**，它指向索引为 `index` 的元素，但不会立即绑定到某个具体的 C++ 类型。在典型用法中，你无需直接与 `ListValueView` 打交道：

* 当你将结果**赋给**类型为 `T` 的变量时，该代理会转换为 `T`。
* 转换逻辑与 `at<T>(index)` 相同。

这一设计允许你编写简单、惯用的代码：

```cpp
String name = values.at(0);
int count   = values.at(1);
```

而不必显式写出模板参数。

你仍然必须确保：

1. `index` 在边界之内（`-length() <= index < length()`）。
2. 元素的运行时类型与要转换到的 C++ 类型兼容（例如，存储的是数字时才转换为 `int`，存储的是字符串时才转换为 `String`）。

如果违反任一条件，结果未定义或可能毫无意义。

**Example**

```cpp
List values("Alice", "Bob", 42);

// Implicit conversion from ListValueView to String
String first = values.at(0);  // "Alice"

// Implicit conversion from ListValueView to int
int answer = values.at(2);    // 42 (if TYPE_INT-compatible)
```

*此示例展示了非模板版 `at()` 如何返回一个可以方便地转换为所需类型的代理。*

---

#### `template <typename T> T at(int index) const`

**Signature**

```cpp
template <typename T> T at(int index) const;
```

**Synopsis**  
返回指定索引处的元素。

**Parameters**

* `index` – 要访问的元素索引。负索引用于从末尾计数。

**Return value**

* 指定索引处的值，类型为 `T`。

**In Depth**

该模板重载会返回指定索引处元素的一个副本，并显式转换为你请求的类型 `T`。

其概念上等价于先从非模板版 `at(int)` 获得 `ListValueView` 代理，然后将该代理转换为 `T`。主要区别在于：

* 你在调用处**显式**指出类型 `T`（例如 `at<String>(index)`）。
* 在你想强调预期类型或避免依赖隐式转换时，它可以让代码更清晰。

与代理重载一样，你必须：

1. 确保 `index` 在边界内（`-length() <= index < length()`）。
2. 确保元素运行时类型与 `T` 兼容，你可以通过 `typeAt(index)` 进行验证。

**Example**

```cpp
List values("Alice", "Bob");

// Explicit type selection
String name = values.at<String>(0);

// This is equivalent in intent to:
String name2 = values.at(0);  // via ListValueView
```

*此示例演示当你希望类型显式时，如何使用模板版 `at<T>()` 按索引获取字符串元素。*

---

#### `ListValueView operator[](int index) const`

**Signature**

```cpp
ListValueView operator[](int index) const;
```

**Synopsis**  
返回指定索引处元素的类型化视图。

**Parameters**

* `index` – 要访问的元素索引。负索引用于从末尾计数。

**Return value**

* 指向给定索引处元素的 `ListValueView` 代理。

**In Depth**

该重载启用了惯用的**下标语法**来访问元素，同时仍然支持混合类型。它返回与 `at(int)` 相同的代理类型，因此你可以编写：

```cpp
String name = values[0];
int    age  = values[1];
```

该代理会根据左值的 C++ 类型（或初始化目标类型）进行转换，其规则与 `at<T>()` 相同。

关键点：

* 索引规则（包括负索引）与 `at()` 完全一致。
* 不会自动进行边界和类型兼容性检查；如有需要，你可以使用 `length()` 与 `typeAt()` 做保护。
* 如果元素的运行时类型与目标 C++ 类型不兼容，结果未定义或可能毫无意义。

如果你偏好更显式、更不易出错的代码，可以将下标访问与 `typeAt()` 结合，以决定如何解释该值。

**Example**

```cpp
List values("Alice", "Bob", 42);

String second = values[1];  // "Bob"
int answer    = values[2];  // 42 (if TYPE_INT-compatible)
```

*此示例展示了使用返回代理的重载进行惯用的下标访问。*

---

#### `template <typename T> T operator[](int index) const`

**Signature**

```cpp
template <typename T> T operator[](int index) const;
```

**Synopsis**  
返回指定索引处的元素。

**Parameters**

* `index` – 要访问的元素索引。负索引用于从末尾计数。

**Return value**

* 指定索引处的值，类型为 `T`。

**In Depth**

这个模板重载是基于代理的 `operator[](int)` 的更显式版本：

* 它会立即返回一个类型为 `T` 的值。
* 行为等价于调用 `list.at(index)`（代理）再将其转换为 `T`。

在大部分日常代码中，你会使用非模板版 `operator[]`，让编译器根据赋值或初始化自动推断转换类型。模板版本有用的场景包括：

* 你希望显式指出预期类型，或者
* 在调用 `operator[]` 的上下文中，目标类型从周围代码中并不明显。

索引与类型匹配的规则与 `at<T>()` 完全相同。

**Example**

```cpp
List values("Alice", "Bob");

// Explicit template syntax
String second = values.operator[]<String>(1);

// Semantically similar, but more concise:
String second2 = values[1];
```

*此示例对比了使用显式模板的访问方式与更简洁的代理式下标语法。*

---

#### `ValueType typeAt(int index) const`

**Signature**

```cpp
ValueType typeAt(int index) const;
```

**Synopsis**  
返回指定索引处元素的类型。

**Parameters**

* `index` – 要检查的元素索引。负索引用于从末尾计数。

**Return value**

* 描述存储类型的 `ValueType` 枚举值（例如 `TYPE_INT`、`TYPE_STRING` 等）。

**In Depth**

`typeAt()` 允许你在通过 `at<T>()`、`pop<T>()` 或 `operator[]<T>()` 取值之前，安全地判定给定索引处存储的是哪种类型。如果索引越界，结果未定义；你应先进行范围检查。

**Example**

```cpp
List values(1, "two", 3.0f);
ValueType t0 = values.typeAt(0);  // TYPE_INT (typically)
ValueType t1 = values.typeAt(1);  // TYPE_STRING
ValueType t2 = values.typeAt(2);  // TYPE_FLOAT
```

*此示例检查混合列表中若干元素的类型。*

---

#### `List slice(int start, int end) const`

**Signature**

```cpp
List slice(int start, int end) const;
```

**Synopsis**  
返回一个包含从 start 到 end 的元素的新列表。

**Parameters**

* `start` – 要包含的第一个元素的索引。负索引用于从末尾计数。
* `end` – **不包含**的最后一个元素的索引（上界开区间）。负索引用于从末尾计数。

**Return value**

* 包含 `[start, end)` 范围内元素的新 `List`。

**In Depth**

`slice()` 会对列表的一个子区间创建浅拷贝。例如：

* `slice(0, length())` 返回整个列表的副本。
* `slice(1, 3)` 返回索引为 `1` 和 `2` 的元素。

索引语义与 `at()` 和 `set()` 相同。你必须确保 `start` 与 `end` 描述的是一个有效子区间。

**Example**

```cpp
List values(10, 20, 30, 40);
List middle = values.slice(1, 3);
// middle holds [20, 30]
```

*此示例从一个更大的列表中提取中间子列表。*

---

#### `List duplicate() const`

**Signature**

```cpp
List duplicate() const;
```

**Synopsis**  
返回此列表的副本。

**Parameters**

* *(none)*

**Return value**

* 包含与原列表相同元素的新 `List`。

**In Depth**

`duplicate()` 提供了一种显式获取列表副本的方式。它在语义上等价于拷贝构造函数，但在链式调用中通常更清晰：

```cpp
List copy = original.duplicate().reverse();
```

**Example**

```cpp
List original(1, 2, 3);
List copy = original.duplicate();
// copy initially holds [1, 2, 3]
```

*此示例复制一个列表以便在单独的副本上进行操作。*

---

#### `template <typename T> int find(T value) const`

**Signature**

```cpp
template <typename T> int find(T value) const;
```

**Synopsis**  
返回值第一次出现的索引，若未找到则返回 -1。

**Parameters**

* `value` – 要搜索的值。其类型必须可与存储的元素进行比较。

**Return value**

* 第一个匹配元素的索引，若未找到则为 `-1`。

**In Depth**

`find()` 会从列表开头开始扫描，查找等于 `value` 的元素。相等性由列表内部的类型特定比较决定。为了获得一致结果：

* 传入的 `value` 类型应与期望匹配的元素类型相同。
* 避免在不清楚比较语义时搜索复杂的嵌套容器。

**Example**

```cpp
List values("apple", "banana", "apple");
int firstApple = values.find("apple");  // 0
int missing = values.find("pear");      // -1
```

*此示例查找列表中字符串第一次出现的位置。*

---

#### `template <typename T> bool contains(T value) const`

**Signature**

```cpp
template <typename T> bool contains(T value) const;
```

**Synopsis**  
如果列表包含该值，则返回 true。

**Parameters**

* `value` – 要搜索的值。

**Return value**

* 若 `find(value)` 返回非负索引则为 `true`；否则为 `false`。

**In Depth**

`contains()` 是对 `find()` 的便利封装，以布尔形式返回结果。它非常适合用于快速成员测试。

**Example**

```cpp
List values(1, 2, 3);
bool hasTwo = values.contains(2);  // true
bool hasFive = values.contains(5); // false
```

*此示例检查列表中是否存在某些值。*

---

#### `String toCSVString() const`

**Signature**

```cpp
String toCSVString() const;
```

**Synopsis**  
将列表的列表转换为 CSV 字符串。

**Parameters**

* *(none)*

**Return value**

* 一个包含 CSV 表示的 `String`。

**In Depth**

`toCSVString()` 期望当前列表表示表格的**行**，其中每个元素本身是一个代表该行各列的 `List`：

* 每个内部列表对应一行 CSV。
* 每个内部列表中的元素成为该行中的单元格值。

该函数会处理常见 CSV 需求，如用逗号分隔值并以换行结束行。确切的引号和转义规则依赖具体实现。

**Example**

```cpp
List row1("Alice", 30);
List row2("Bob", 25);

List table;
table.append(row1).append(row2);

String csv = table.toCSVString();
// csv might be:
// Alice,30
// Bob,25
```

*此示例将一个列表的列表形式的表格转换为简单的 CSV 字符串。*

---

#### `static List FromCSVString(const String &csv)`

**Signature**

```cpp
static List FromCSVString(const String &csv);
```

**Synopsis**  
从 CSV 字符串创建列表的列表。

**Parameters**

* `csv` – 要解析的 CSV 格式文本。

**Return value**

* 一个 `List`，其中每个元素是表示一行的内部 `List`，每个内部元素是单元格值（通常为 `String`）。

**In Depth**

`FromCSVString()` 是 `toCSVString()` 的逆操作：

* 输入：CSV 文本，可能含多行。
* 输出：行的列表，每一行是单元格值的列表。

它允许你从 CSV 文本内容重构结构化的表格数据。

**Example**

```cpp
String csv("Alice,30\nBob,25\n");
List table = List::FromCSVString(csv);

// table[0] is ["Alice", "30"]
// table[1] is ["Bob", "25"]
```

*此示例将 CSV 文本解析为列表的列表表示形式。*

---

#### `String toJSONString() const`

**Signature**

```cpp
String toJSONString() const;
```

**Synopsis**  
将此列表转换为 JSON 数组字符串。

**Parameters**

* *(none)*

**Return value**

* 一个包含该列表 JSON 数组表示的 `String`。

**In Depth**

`toJSONString()` 会将列表序列化为 JSON 数组字符串。每个元素会根据其类型转换为 JSON：

* 数字变为 JSON 数字。
* 布尔值变为 `true`/`false`。
* 字符串变为带引号的 JSON 字符串，并进行适当转义。
* 嵌套的 `List`、`Map` 与 `Set` 元素变为嵌套数组或对象。

这对于日志记录、配置以及与基于 JSON 的 API 进行通信非常有用。

**Example**

```cpp
List values;
values.append("Alice").append(30).append(true);

String json = values.toJSONString();
// e.g., ["Alice",30,true]
```

*此示例将一个包含不同类型元素的列表转换为 JSON 数组字符串。*

---

#### `static List FromJSONString(const String &json)`

**Signature**

```cpp
static List FromJSONString(const String &json);
```

**Synopsis**  
从 JSON 数组字符串创建列表。

**Parameters**

* `json` – 要解析的 JSON 数组文本。

**Return value**

* 一个表示已解析 JSON 数组的 `List`。

**In Depth**

`FromJSONString()` 会解析 JSON 数组字符串并构造一个 `List`，其中：

* 每个 JSON 元素成为列表中的一个对应值。
* JSON 嵌套数组与对象分别成为 `List` 与 `Map` 值。

如果输入 JSON 不是合法数组，得到的列表可能为空或处于错误状态；如有需要，你应对输入进行校验。

**Example**

```cpp
String json("[\"Alice\",30,true]");
List values = List::FromJSONString(json);

// values[0] == "Alice"
// values[1] == "30" or integer 30 (implementation-dependent representation)
// values[2] == true
```

*此示例将 JSON 数组文本解析为适合进一步处理的 `List`。*

---

#### `bool compare(const List &other) const`

**Signature**

```cpp
bool compare(const List &other) const;
```

**Synopsis**  
如果此列表等于另一个列表则返回 true。

**Parameters**

* `other` – 要进行比较的列表。

**Return value**

* 若两个列表长度相同且对应元素相等则为 `true`；否则为 `false`。

**In Depth**

`compare()` 会执行元素逐个比较：

* 首先比较长度。
* 然后逐个比较元素（包括类型和值）。

它在功能上等价于 `operator==`，主要面向偏好显式方法调用风格的代码。

**Example**

```cpp
List a(1, 2, 3);
List b(1, 2, 3);
List c(1, 2);

bool ab = a.compare(b);  // true
bool ac = a.compare(c);  // false
```

*此示例使用 `compare()` 比较两个列表是否相等。*

---

#### `bool operator==(const List &other) const`

**Signature**

```cpp
bool operator==(const List &other) const;
```

**Synopsis**  
如果此列表等于另一个列表则返回 true。

**Parameters**

* `other` – 要比较的列表。

**Return value**

* 若列表相等则为 `true`；否则为 `false`。

**In Depth**

该运算符是使用 `==` 测试列表相等性的惯用方式。它委托给与 `compare()` 相同的逻辑，比较长度与元素。

**Example**

```cpp
List a(1, 2, 3);
List b(1, 2, 3);
List c(3, 2, 1);

bool sameOrder = (a == b); // true
bool diffOrder = (a == c); // false
```

*此示例展示了基于值与顺序的相等性测试。*

---

#### `bool operator!=(const List &other) const`

**Signature**

```cpp
bool operator!=(const List &other) const;
```

**Synopsis**  
如果此列表不等于另一个列表则返回 true。

**Parameters**

* `other` – 要比较的列表。

**Return value**

* 若列表不相等为 `true`；若相等则为 `false`。

**In Depth**

该运算符仅是 `operator==` 的逻辑取反。

**Example**

```cpp
List a(1, 2, 3);
List b(1, 2);

if (a != b) {
  // lists differ in length or content
}
```

*此示例使用不等运算符来检测差异。*

---

#### `template <typename T> List operator+(T value) const`

**Signature**

```cpp
template <typename T> List operator+(T value) const {
  List result = duplicate();
  result.append(value);
  return result;
}
```

**Synopsis**  
返回一个在当前列表基础上追加该值的新列表。

**Parameters**

* `value` – 要追加到当前列表副本末尾的值。

**Return value**

* 新的 `List`，包含原列表的所有元素，随后是 `value`。

**In Depth**

该运算符提供了一种函数式风格的方式，用于在不修改原列表的情况下构建新列表：

* 它复制当前列表。
* 在副本上追加 `value`。
* 返回这个新列表。

当你希望保留原列表，并在扩展版本上进行操作时，这非常有用。

**Example**

```cpp
List base(1, 2);
List extended = base + 3;
// base still holds [1, 2]
// extended holds [1, 2, 3]
```

*此示例使用 `+` 运算符创建了一个多出一个元素的新列表，而原列表保持不变。*