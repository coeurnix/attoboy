# Embedding

## 概览

`attoboy::Embedding` 类表示由 AI 模型生成的**高维数值向量**，通常用于**语义相似度**相关的任务。嵌入模型不会直接生成文本，而是将一段文本（例如句子、段落或文档）映射为一个由 `float` 值组成的向量。语义相似的文本在这个高维空间中的向量会“彼此接近”。

在 attoboy 库中：

* 嵌入由 `AI` 类通过 `AI::createEmbedding` 创建。
* `Embedding` 对象是这些向量之上的**只读视图**。
* 你可以：

  * 使用 `compare` 计算两个嵌入间的**余弦相似度**。
  * 通过 `getDimensions` 查看维度数量。
  * 当需要手工处理时，通过 `getRawArray` 访问底层原始 `float` 数据。

你永远不会通过 `new` 或公共构造函数直接构造一个 `Embedding`；而是通过一个 `AI` 实例来为你创建它。

### 什么是嵌入向量？

一个嵌入就是一个向量：

* 在此上下文中，一个**向量**只是一个有序的数值列表：[v₀, v₁, …, vₙ₋₁]。
* 每个 `Embedding` 提供：

  * `getDimensions()` – 它包含多少个数。
  * `getRawArray()` – 指向底层 `float` 数组第一个元素的指针。

每一维的确切含义取决于具体模型且对调用者是透明的。你应当将该向量视作一种**特征表示**，用于捕捉原始文本的语义属性。

### 余弦相似度

你对嵌入执行的主要操作是**余弦相似度**，由 `Embedding::compare` 实现：

* 余弦相似度度量高维空间中两个向量之间夹角的余弦值。
* 结果范围为 **[-1, 1]**：

  * `1.0` → 向量方向完全相同（最大相似）。
  * `0.0` → 向量正交（无线性相关性）。
  * `-1.0` → 向量方向完全相反（在某种意义上最大不相似）。

在现代 AI 模型生成的文本嵌入的实际应用中：

* 数值通常介于 `0.0` 与 `1.0` 之间。
* 数值越高表示文本越相似。
* 非常低的数值表示内容无关。

### 典型工作流程

在 attoboy 中，一个常见的嵌入使用流程如下：

1. 配置一个 `AI` 客户端。
2. 调用 `AI::createEmbedding(text)` 获取一个 `Embedding`。
3. 将该嵌入（或其原始浮点数数组）存储起来以备后用。
4. 使用 `Embedding::compare` 比较嵌入以衡量相似度。

例如，你可以：

* 构建一个**语义搜索索引**：为所有文档创建嵌入，并将用户查询的嵌入与已存嵌入进行比较。
* 通过成对相似度比较对相似文本进行聚类。
* 实现推荐系统，根据项目的文本描述查找“相似项目”。

### 不可变性与生命周期

`Embedding` 对象在概念上是**不可变**的：

* 一旦创建，就无法通过公共 API 修改底层向量。
* 拷贝或赋值一个 `Embedding` 会共享底层数据，而不是拷贝一份新的。

生命周期与所有权：

* `Embedding` 对象通过 RAII 管理其底层内存。
* 当最后一个引用某个向量的 `Embedding` 被销毁时，该向量占用的内存会被释放。
* 通过 `getRawArray()` 返回的指针只在对应的 `Embedding` 对象存活期间（以及任何共享副本存活期间）保持有效。

如果 `AI::createEmbedding` 失败，它会返回一个 `Embedding`，其 `getDimensions() == 0`。在使用嵌入之前务必检查其维度。

---

## 参考

下面的每个条目覆盖了 `attoboy::Embedding` 的一个公共构造函数、方法或运算符。对于每个成员，你会看到：

* **Signature** – 头文件中的精确声明。
* **Synopsis** – 原始的一行 Doxygen 注释。
* **Parameters** 与 **Return value** 的说明。
* **In Depth** – 行为细节、注意事项以及一个用例示例。

> 所有示例均假定 `using namespace attoboy;`。

---

#### `Embedding(const Embedding &other)`

**Signature**

```cpp
Embedding(const Embedding &other);
```

**Synopsis**  
Creates a copy (shares the underlying data).

**Parameters**

* `other` – 要拷贝的现有 `Embedding` 实例。

**Return value**

* *(constructor; not applicable)*

**In Depth**

拷贝构造函数会创建一个新的 `Embedding` 对象，它会与 `other` **共享**底层向量数据：

* 不会分配新的向量；现有向量会在内部通过引用计数或其他方式进行共享。
* 两个 `Embedding` 对象查看的是同一份数据。
* 销毁其中一个不会使另一个失效，只要仍至少有一个 `Embedding` 引用该数据。

这种浅层共享使得拷贝 `Embedding` 对象非常高效，在以下场景中很有用：

* 在函数之间按值传递嵌入。
* 将嵌入存储在你自己管理的集合中（例如，通过索引或外部存储间接表示的 attoboy `List`）。

请记住你不会直接构造嵌入；你从 `AI::createEmbedding` 获得它们，然后按需复制。

**Example**

```cpp
using namespace attoboy;

AI ai(String("https://api.openai.com/v1"),
      String("sk-123456"),
      String("text-embedding-3-small"));

Embedding original = ai.createEmbedding("Hello world", -1, 1536);

if (original.getDimensions() > 0) {
  // Create another handle to the same underlying embedding vector.
  Embedding copy(original);

  float similarity = original.compare(copy);  // Should be very close to 1.0
}
```

*此示例通过 `AI::createEmbedding` 创建一个嵌入，然后使用拷贝构造函数复制它；这两个句柄共享同一底层向量。*

---

#### `~Embedding()`

**Signature**

```cpp
~Embedding();
```

**Synopsis**  
Destroys the embedding and frees resources.

**Parameters**

* *(none)*

**Return value**

* *(destructor; not applicable)*

**In Depth**

析构函数会释放 `Embedding` 对其底层数据的句柄：

* 如果这是最后一个引用该向量的 `Embedding`，则该数据会被释放。
* 之前通过 `getRawArray()` 获取的任何指针，在析构后都将变为无效。

由于 `Embedding` 使用 RAII，你无需手动释放内存或执行清理操作。遵循通常的 C++ 生命周期规则：当一个 `Embedding` 离开作用域时，其析构函数会自动运行。

**Example**

```cpp
using namespace attoboy;

Embedding storedEmbedding = /* ... obtained from AI::createEmbedding ... */;

// When `storedEmbedding` goes out of scope, its destructor is called and
// its resources are released when no other references remain.
```

*此示例展示了一个嵌入在离开作用域时，其资源会被自动回收。*

---

#### `Embedding &operator=(const Embedding &other)`

**Signature**

```cpp
Embedding &operator=(const Embedding &other);
```

**Synopsis**  
Assigns another embedding (shares the underlying data).

**Parameters**

* `other` – 赋值来源的现有 `Embedding`。

**Return value**

* 指向 `*this` 的引用，以支持链式赋值。

**In Depth**

赋值运算符会使 `*this` 指向与 `other` **相同的底层向量**：

* `*this` 之前关联的任何数据都会被释放；如果这是该数据的最后一个引用，则数据会被释放。
* 赋值之后，`*this` 与 `other` 共享相同的嵌入值。
* 两者都可以作为同一只读向量的独立句柄来使用。

自赋值是安全的，由实现负责处理。

这种行为与拷贝构造函数保持一致：拷贝和赋值都共享底层数据，而不是深拷贝。

**Example**

```cpp
using namespace attoboy;

Embedding e1 = /* ... */;
Embedding e2 = /* ... */;

// Now e2 will share the same vector as e1.
e2 = e1;

float sim = e1.compare(e2);  // Should be near 1.0
```

*此示例将一个嵌入赋值给另一个，从而得到两个指向同一底层向量的句柄。*

---

#### `float compare(const Embedding &other) const`

**Signature**

```cpp
float compare(const Embedding &other) const;
```

**Synopsis**  
Computes cosine similarity with another embedding. Returns [-1, 1].

**Parameters**

* `other` – 要与 `*this` 比较的另一个 `Embedding`。

**Return value**

* 一个 `float`，表示两个嵌入之间的余弦相似度：

  * 范围：**[-1.0, 1.0]**。
  * 数值越高表示向量越相似。

**In Depth**

`compare` 计算两个嵌入之间的**余弦相似度**：

* 从概念上讲，它度量的是两个向量之间的夹角：

  * `1.0` → 向量方向完全相同。
  * `0.0` → 向量正交（无线性相关性）。
  * `-1.0` → 向量方向完全相反。

实践中的指导：

* 对于来自现代模型的典型文本嵌入：

  * 接近 `1.0` 的数值意味着文本高度相似。
  * 约 `0.5`–`0.8` 的数值通常表示相关但不完全相同的概念。
  * 接近 `0.0` 或为负的数值表示内容无关或差异很大。

你应当仅比较如下嵌入：

* 来自**同一模型**（或明确共享同一嵌入空间的模型）。
* 具有**相同维度数**（`getDimensions()` 返回相同的值）。

比较维度不同或来自无关模型的嵌入不会产生有意义的相似度分数。

边界情况：

* 如果一个或两个嵌入的 `getDimensions() == 0`，从你的角度看结果是未定义行为；将其视为错误情形，在验证维度之前不要调用 `compare`。
* 如果内部实现检测到嵌入不兼容，它可能返回一个默认值（例如 `0.0`）；应通过构造过程确保嵌入兼容，而不是依赖返回值来判断。

**Example**

```cpp
using namespace attoboy;

AI ai(String("https://api.openai.com/v1"),
      String("sk-123456"),
      String("text-embedding-3-small"));

Embedding eQuery  = ai.createEmbedding("best pizza in New York", -1, 1536);
Embedding eDoc1   = ai.createEmbedding("A guide to New York pizza slices", -1, 1536);
Embedding eDoc2   = ai.createEmbedding("How to care for indoor plants", -1, 1536);

if (eQuery.getDimensions() > 0 &&
    eDoc1.getDimensions()   > 0 &&
    eDoc2.getDimensions()   > 0) {

  float sim1 = eQuery.compare(eDoc1);
  float sim2 = eQuery.compare(eDoc2);

  // sim1 should be significantly higher than sim2, indicating that
  // the pizza article is more relevant to the query than the plants article.
}
```

*此示例将一个查询嵌入与两个文档嵌入进行比较，并利用余弦相似度判断哪个文档更相关。*

---

#### `const float *getRawArray() const`

**Signature**

```cpp
const float *getRawArray() const;
```

**Synopsis**  
Returns a pointer to the raw float array.

**Parameters**

* *(none)*

**Return value**

* 指向内部 `float` 数组第一个元素的指针，该数组表示嵌入向量。
* 如果嵌入无效（例如 `getDimensions() == 0`），可能为 `nullptr`。

**In Depth**

`getRawArray()` 提供对嵌入底层数据的低层访问：

* 该数组的长度正好为 `getDimensions()`。
* 数据是**只读**的；你不得通过该指针进行修改。
* 只要 `Embedding` 对象（以及任何共享副本）仍然存活，该指针就保持有效。

典型用例包括：

* 实现你自己的相似度或距离度量，而不仅限于余弦相似度。
* 将向量导出到其他系统（例如写入文件、通过网络发送或传递给其他库）。
* 将嵌入转换为另一种表示形式（例如量化为整数）以进行存储。

在使用该指针前务必检查 `getDimensions()`。如果嵌入无效（维度为 `0`），不要解引用该指针。

**Example**

```cpp
using namespace attoboy;

AI ai(String("https://api.openai.com/v1"),
      String("sk-123456"),
      String("text-embedding-3-small"));

Embedding emb = ai.createEmbedding("example text", -1, 1536);

int dims = emb.getDimensions();
if (dims > 0) {
  const float *data = emb.getRawArray();

  // For demonstration: compute the sum of all components.
  float sum = 0.0f;
  for (int i = 0; i < dims; ++i) {
    sum = sum + data[i];
  }

  // `sum` now holds the simple aggregate of the embedding components.
}
```

*此示例访问原始浮点数组并对所有维度计算一个简单的总和。*

---

#### `int getDimensions() const`

**Signature**

```cpp
int getDimensions() const;
```

**Synopsis**  
Returns the number of dimensions.

**Parameters**

* *(none)*

**Return value**

* 嵌入向量中 `float` 分量的数量。
* 若嵌入无效或未初始化，则为 `0`。

**In Depth**

`getDimensions()` 告诉你嵌入包含多少个数值。这对于以下方面至关重要：

* 验证嵌入是否创建成功：

  * `> 0` → 有效嵌入。
  * `0` → 错误或未初始化嵌入（例如 `AI::createEmbedding` 失败）。

* 确保两个嵌入可以被有意义地比较或一起处理。
* 在遍历 `getRawArray()` 返回的原始数组时，用于确定循环边界。

正如 `AI` 类文档中所述，典型约定是：

* 如果 `AI::createEmbedding` 失败，它会返回一个 `Embedding`，其 `getDimensions() == 0`。

在执行任何假设向量非空的操作之前，务必检查该值。

**Example**

```cpp
using namespace attoboy;

AI ai(String("https://api.openai.com/v1"),
      String("sk-123456"),
      String("text-embedding-3-small"));

Embedding emb = ai.createEmbedding("hello world", -1, 1536);

int dims = emb.getDimensions();
if (dims <= 0) {
  // Embedding creation failed; handle the error.
} else {
  // Safe to use the embedding: dims components are available.
}
```

*此示例通过检查维度数来判断嵌入是否创建成功，然后再决定是否使用该嵌入。*