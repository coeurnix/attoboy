# Math

## 概览

`attoboy::Math` 类是一个精简的 **静态数学工具函数** 和 **常量** 集合，用于覆盖小型工具与实用程序中最常见的数值操作。它提供：

* 常用数学常数，例如 π、e、τ 和 √2。
* 随机数生成（整数、浮点数、布尔值以及列表元素）。
* 整数与浮点帮助函数（绝对值、夹取、最小/最大值等）。
* 取整与截断函数。
* 指数、对数与幂函数。
* 三角函数与双曲函数（使用 **弧度**）。
* 用于插值、角度转换以及浮点值分类的工具函数。
* 简单的 64 位整数算术辅助函数。

所有成员均具备以下特性：

* **静态** – 不需要创建 `Math` 对象，直接调用 `Math::FunctionName(...)`。
* **无状态** – 除了随机数生成器的内部状态外，没有持久状态或配置。
* **无异常** – 与 attoboy 其他部分一致，函数均为 `noexcept`，不会抛出异常。

### 浮点数、无穷大与 NaN

`Math` 中有许多函数是针对 `float` 类型的。`float` 是一种遵循 IEEE-754 标准的 32 位浮点数。这种表示方式有一些重要性质：

* **舍入** – 大多数实数无法被精确表示，因此结果是近似值。
* **无穷大** – 某些操作（如溢出或除以零）会产生正无穷或负无穷。
* **NaN（Not-a-Number）** – 无效操作（例如 `0.0f / 0.0f`）会产生一个特殊值 NaN。

`Math` 类提供了常量与辅助函数来处理这些数值：

* `Math::INF` 和 `Math::NEG_INF` 分别表示正无穷和负无穷。
* `Math::NAN` 表示一个 NaN 值。
* `Math::IsFinite(x)` 判断 `x` 是否为有限值（既不是无穷也不是 NaN）。
* `Math::IsNaN(x)` 判断 `x` 是否为 NaN。

在编写需要稳健处理边界情况的数值代码时，理解这些概念非常重要。

### 弧度与角度

诸如 `Sin`、`Cos` 和 `Tan` 等三角函数期望接收的是 **弧度制角度**，而不是角度制。弧度是度量角度的另一种方式：

* 整个圆是 `2π` 弧度（即 `Math::TAU`）。
* 90 度是 `π/2` 弧度。
* 180 度是 `π` 弧度。

`Math` 类提供：

* `Math::DegToRad(degrees)` – 将角度（度）转换为弧度。
* `Math::RadToDeg(radians)` – 将弧度转换为角度（度）。

当代码使用以度为单位的角度（例如用户输入）而数学函数使用弧度时，请使用这些辅助函数。

---

## 参考

`Math` 的所有成员都是静态的，通过 `Math::Name(...)` 或 `Math::CONSTANT` 访问。下面的章节按功能对相关成员进行分组，以提高可读性。

> **注意：** 所有示例都假设已经声明 `using namespace attoboy;`。

---

#### `Math::PI`

**函数签名**

```cpp
static constexpr float PI = 3.14159265358979323846f;
```

**概要**  
圆周率（π ≈ 3.14159）。

**参数**

* *(无)*

**返回值**

* *(常量；不适用)*

**深入说明**

`Math::PI` 表示数学常数 π，即圆的周长与直径的比值。几乎所有三角或圆相关的计算都会使用它。

由于它被声明为 `constexpr`，因此可以在编译期常量表达式中使用。

**示例**

```cpp
float radius = 2.0f;
float circumference = 2.0f * Math::PI * radius;  // ≈ 12.56637
```

*此示例根据半径使用 π 计算圆的周长。*

---

#### `Math::E`

**函数签名**

```cpp
static constexpr float E = 2.71828182845904523536f;
```

**概要**  
欧拉常数（e ≈ 2.71828）。

**参数**

* *(无)*

**返回值**

* *(常量；不适用)*

**深入说明**

`Math::E` 是欧拉常数 e，即自然对数的底数。它出现在指数增长/衰减方程、概率论以及大量数学领域中。

**示例**

```cpp
float x = 1.0f;
float expApprox = Math::Pow(Math::E, x);  // ≈ e^1 = 2.71828
```

*此示例将 `Math::E` 与 `Math::Pow` 结合，用于近似计算 e¹。*

---

#### `Math::TAU`

**函数签名**

```cpp
static constexpr float TAU = 6.28318530717958647693f;
```

**概要**  
Tau（τ = 2π ≈ 6.28318）。

**参数**

* *(无)*

**返回值**

* *(常量；不适用)*

**深入说明**

`Math::TAU` 被定义为 `2 * π`。某些公式使用 τ 而非 π 会更加简洁，特别是在处理整圈旋转时。

**示例**

```cpp
float rotations = 0.25f;  // 四分之一圈
float angle = rotations * Math::TAU;  // 弧度
```

*此示例将一圈的分数转换为弧度，使用 τ。*

---

#### `Math::SQRT_2`

**函数签名**

```cpp
static constexpr float SQRT_2 = 1.41421356237309504880f;
```

**概要**  
2 的平方根（√2 ≈ 1.41421）。

**参数**

* *(无)*

**返回值**

* *(常量；不适用)*

**深入说明**

`Math::SQRT_2` 是 2 的平方根，即单位正方形对角线的长度。它在几何与归一化计算中十分有用。

**示例**

```cpp
float diagonal = Math::SQRT_2;  // 1x1 正方形的对角线长度
```

*此示例在简单几何计算中使用 √2。*

---

#### `Math::INF`

**函数签名**

```cpp
static const float INF;
```

**概要**  
正无穷大。

**参数**

* *(无)*

**返回值**

* *(常量；不适用)*

**深入说明**

`Math::INF` 表示正无穷大。它可用作标记值，或在搜索最小值时作为“非常大”的初始占位值。

需要注意浮点运算中与无穷大的规则：

* `INF + 1` 仍然是 `INF`。
* 对有限的 `x`，类似 `x < Math::INF` 的比较按预期工作。

**示例**

```cpp
float best = Math::INF;
// ... 之后通过更小的值不断更新 best
```

*此示例将“最佳”得分初始化为正无穷，使得任何实际得分都更小。*

---

#### `Math::NEG_INF`

**函数签名**

```cpp
static const float NEG_INF;
```

**概要**  
负无穷大。

**参数**

* *(无)*

**返回值**

* *(常量；不适用)*

**深入说明**

`Math::NEG_INF` 是 `Math::INF` 的负值对应。它在搜索最大值或表示下溢量等场景中很有用。

**示例**

```cpp
float best = Math::NEG_INF;
// ... 之后通过更大的值不断更新 best
```

*此示例将最大值跟踪器初始化为负无穷大。*

---

#### `Math::NAN`

**函数签名**

```cpp
static const float NAN;
```

**概要**  
非数值（Not-a-Number）。

**参数**

* *(无)*

**返回值**

* *(常量；不适用)*

**深入说明**

`Math::NAN` 表示一个 NaN（Not-a-Number）值。NaN 用于表示未定义或不可表示的结果（例如 `0.0f / 0.0f`）。

重要行为：

* NaN **不等于** 任何值，包括它自身。诸如 `x == Math::NAN` 的比较始终为假。
* 使用 `Math::IsNaN(x)` 来检测 NaN。

**示例**

```cpp
float value = Math::NAN;
// 之后:
if (Math::IsNaN(value)) {
  // 处理无效结果
}
```

*此示例使用 NaN 作为“无效值”的标记，并通过 `IsNaN` 检测它。*

---

#### `static int Random() noexcept`

**函数签名**

```cpp
static int Random() noexcept;
```

**概要**  
返回一个随机整数。

**参数**

* *(无)*

**返回值**

* 一个伪随机整数值。具体范围取决于实现，但通常覆盖 `int` 的完整范围。

**深入说明**

`Random()` 提供一个非确定性的整数，适合在不需要指定范围时做一般用途。如果需要限制范围，请优先使用 `RandomRange`。

该函数为 `noexcept`，并使用内部状态，因此在需要临时随机值时可以安全且方便地调用。

**示例**

```cpp
int raw = Math::Random();
// raw 可以用作种子，或与取模组合用于临时范围限制
```

*此示例获取一个不受范围约束的随机整数。*

---

#### `static float RandomFloat() noexcept`

**函数签名**

```cpp
static float RandomFloat() noexcept;
```

**概要**  
返回区间 [0.0, 1.0) 内的随机浮点数。

**参数**

* *(无)*

**返回值**

* 伪随机 `float`，位于半开区间 `[0.0f, 1.0f)`。

**深入说明**

`RandomFloat()` 非常适合用于概率判定、插值或简单噪声：

* 包含 `0.0f`。
* 不包含 `1.0f`。

你可以通过缩放和平移将结果映射到其它区间：

```cpp
float value = Math::RandomFloat();          // [0, 1)
float scaled = 10.0f * value;               // [0, 10)
float shifted = 5.0f + 10.0f * value;       // [5, 15)
```

**示例**

```cpp
float chance = Math::RandomFloat();
bool success = chance < 0.25f;  // 25% 概率
```

*此示例使用随机浮点数模拟简单的概率判定。*

---

#### `static int RandomRange(int start, int end) noexcept`

**函数签名**

```cpp
static int RandomRange(int start, int end) noexcept;
```

**概要**  
返回区间 [start, end) 内的随机整数。

**参数**

* `start` – 区间下界（包含）。
* `end` – 区间上界（不包含，且必须大于 `start`）。

**返回值**

* 满足 `start <= n < end` 的随机整数 `n`。

**深入说明**

`RandomRange()` 是获取有界随机整数的主要方式。它返回均匀分布在半开区间 `[start, end)` 内的整数。

你需要保证 `end > start`。传入无效边界（如 `end <= start`）可能导致未定义行为或依赖具体实现的行为。

**示例**

```cpp
int roll = Math::RandomRange(1, 7);  // 1..6（含），类似骰子
```

*此示例模拟掷一个六面骰。*

---

#### `static bool RandomBool() noexcept`

**函数签名**

```cpp
static bool RandomBool() noexcept;
```

**概要**  
返回一个随机布尔值。

**参数**

* *(无)*

**返回值**

* `true` 或 `false`，概率大致各为 50%。

**深入说明**

`RandomBool()` 是围绕随机数生成器的便捷封装，用于抛硬币式的决策。其效果等价于 `Math::RandomFloat() < 0.5f`，但可能更高效。

**示例**

```cpp
bool heads = Math::RandomBool();
```

*此示例模拟一次公平的硬币投掷。*

---

#### `template <typename T> static T RandomChoice(const List &list) noexcept`

**函数签名**

```cpp
template <typename T> static T RandomChoice(const List &list) noexcept;
```

**概要**  
从列表中返回一个随机元素。

**参数**

* `list` – 候选值的 `List`。要得到有意义结果，列表不应为空。

**返回值**

* 列表中类型为 `T` 的随机元素；如果列表为空，则返回 `T()`。

**深入说明**

`RandomChoice<T>()` 会在区间 `[0, list.length())` 中选择一个均匀随机索引，并通过 `list.at<T>(index)` 取得对应元素并转换为 `T`。

若列表为空，函数会返回 `T` 的默认构造值。在很多情况下，这意味着数值类型的 `0` 或空 `String`。

你需要确保列表元素可以转换为 `T`。

**示例**

```cpp
List options;
options.append("red").append("green").append("blue");

String color = Math::RandomChoice<String>(options);
// color 为 "red"、"green" 或 "blue" 之一
```

*此示例从字符串列表中随机选择一个颜色。*

---

#### `static int Abs(int x) noexcept`

**函数签名**

```cpp
static int Abs(int x) noexcept;
```

**概要**  
返回 x 的绝对值。

**参数**

* `x` – 需要求绝对值的整数。

**返回值**

* 等于 `|x|` 的非负整数。

**深入说明**

`Abs(int)` 移除整数的符号，返回非负值。对大多数输入，`Abs(-n)` 与 `-n` 相同。对最小可表示整数（例如 INT_MIN），由于溢出，其行为可能依赖实现，因此避免将该值传给 `Abs()`。

**示例**

```cpp
int delta = -5;
int distance = Math::Abs(delta);  // 5
```

*此示例将带符号差值转换为非负距离。*

---

#### `static float Abs(float x) noexcept`

**函数签名**

```cpp
static float Abs(float x) noexcept;
```

**概要**  
返回 x 的绝对值。

**参数**

* `x` – 浮点值。

**返回值**

* 作为 `float` 的 `|x|`。对 NaN，结果通常仍为 NaN。

**深入说明**

`Abs(float)` 会移除浮点数的符号位，得到非负值。对有限值和无穷大时表现符合预期。

**示例**

```cpp
float error = -0.25f;
float magnitude = Math::Abs(error);  // 0.25f
```

*此示例测量带符号误差的大小。*

---

#### `static int Min(int a, int b) noexcept`

**函数签名**

```cpp
static int Min(int a, int b) noexcept;
```

**概要**  
返回 a 与 b 中较小的值。

**参数**

* `a` – 第一个整数。
* `b` – 第二个整数。

**返回值**

* `a` 与 `b` 中的较小值。如果相等，返回任意一个。

**深入说明**

`Min(int, int)` 是一个用于选取两个整数中较小者的简单辅助函数。

**示例**

```cpp
int width = Math::Min(80, 120);  // 80
```

*此示例选择两个边界中的较小者。*

---

#### `static float Min(float a, float b) noexcept`

**函数签名**

```cpp
static float Min(float a, float b) noexcept;
```

**概要**  
返回 a 与 b 中较小的值。

**参数**

* `a` – 第一个浮点值。
* `b` – 第二个浮点值。

**返回值**

* `a` 与 `b` 中的较小值。包含 NaN 时的行为依赖实现。

**深入说明**

对正常有限值，`Min(float, float)` 的行为与预期一致。如果任一参数为 NaN，结果可能依赖于具体实现；尽量避免使用 NaN 作为输入。

**示例**

```cpp
float alpha = Math::Min(1.0f, 0.5f);  // 0.5f
```

*此示例通过选取较小的浮点数来钳制不透明度值。*

---

#### `static int Max(int a, int b) noexcept`

**函数签名**

```cpp
static int Max(int a, int b) noexcept;
```

**概要**  
返回 a 与 b 中较大的值。

**参数**

* `a` – 第一个整数。
* `b` – 第二个整数。

**返回值**

* `a` 与 `b` 中的较大值。

**深入说明**

`Max(int, int)` 是 `Min(int, int)` 的对应函数，用于获取较大者。

**示例**

```cpp
int height = Math::Max(10, 20);  // 20
```

*此示例选择两个整数中的较大者。*

---

#### `static float Max(float a, float b) noexcept`

**函数签名**

```cpp
static float Max(float a, float b) noexcept;
```

**概要**  
返回 a 与 b 中较大的值。

**参数**

* `a` – 第一个浮点值。
* `b` – 第二个浮点值。

**返回值**

* `a` 与 `b` 中的较大值。

**深入说明**

对有限输入，`Max(float, float)` 的行为符合预期。若涉及 NaN，结果依赖实现；除非显式处理，否则应避免将 NaN 与有效值混用。

**示例**

```cpp
float limit = Math::Max(0.0f, -5.0f);  // 0.0f
```

*此示例通过选择两个浮点数中的较大者，确保值至少为零。*

---

#### `static int Clamp(int x, int minVal, int maxVal) noexcept`

**函数签名**

```cpp
static int Clamp(int x, int minVal, int maxVal) noexcept;
```

**概要**  
将 x 夹取到区间 [minVal, maxVal] 内。

**参数**

* `x` – 输入整数。
* `minVal` – 允许范围的下界。
* `maxVal` – 允许范围的上界（应 ≥ `minVal`）。

**返回值**

* 若 `minVal <= x <= maxVal`，返回 `x`。
* 若 `x < minVal`，返回 `minVal`。
* 若 `x > maxVal`，返回 `maxVal`。

**深入说明**

当需要将数值限制在一个闭区间内时，夹取非常有用。

你应保证 `minVal <= maxVal`；否则行为依赖实现。

**示例**

```cpp
int brightness = 150;
brightness = Math::Clamp(brightness, 0, 100);  // 100
```

*此示例将整数亮度值限制在 0–100 范围内。*

---

#### `static float Clamp(float x, float minVal, float maxVal) noexcept`

**函数签名**

```cpp
static float Clamp(float x, float minVal, float maxVal) noexcept;
```

**概要**  
将 x 夹取到区间 [minVal, maxVal] 内。

**参数**

* `x` – 输入浮点数。
* `minVal` – 下界。
* `maxVal` – 上界。

**返回值**

* 按整数版本同样规则，将 `x` 夹取到 `[minVal, maxVal]` 后返回。

**深入说明**

浮点夹取常用于归一化值（如 0.0 到 1.0），或将输出限制在物理上有意义的范围内。

与整数版本一样，应当保证 `minVal <= maxVal`。

**示例**

```cpp
float t = 1.5f;
t = Math::Clamp(t, 0.0f, 1.0f);  // 1.0f
```

*此示例将参数 `t` 夹取到归一化范围内。*

---

#### `static int Sign(int x) noexcept`

**函数签名**

```cpp
static int Sign(int x) noexcept;
```

**概要**  
根据 x 的符号返回 -1、0 或 1。

**参数**

* `x` – 整数输入。

**返回值**

* 若 `x < 0`，返回 `-1`。
* 若 `x == 0`，返回 `0`。
* 若 `x > 0`，返回 `1`。

**深入说明**

`Sign(int)` 是区分负数、零与正数的一种简洁方式。

**示例**

```cpp
int dx = -3;
int direction = Math::Sign(dx);  // -1
```

*此示例将带符号偏移转换为方向指示符。*

---

#### `static float Sign(float x) noexcept`

**函数签名**

```cpp
static float Sign(float x) noexcept;
```

**概要**  
根据 x 的符号返回 -1.0、0.0 或 1.0。

**参数**

* `x` – 浮点输入。

**返回值**

* 若 `x < 0`，返回 `-1.0f`。
* 若 `x == 0`，返回 `0.0f`。
* 若 `x > 0`，返回 `1.0f`。

**深入说明**

`Sign(float)` 在向量数学和归一化中非常有用。对 NaN 的行为依赖于实现，应尽量避免将 NaN 作为输入。

**示例**

```cpp
float value = 2.5f;
float s = Math::Sign(value);  // 1.0f
```

*此示例检查一个浮点值的符号。*

---

#### `static bool IsEven(int x) noexcept`

**函数签名**

```cpp
static bool IsEven(int x) noexcept;
```

**概要**  
若 x 为偶数则返回 true。

**参数**

* `x` – 整数输入。

**返回值**

* 若 `x` 能被 2 整除则为 `true`；否则为 `false`。

**深入说明**

`IsEven()` 判断 `x` 的最低有效位是否为零。对负整数同样适用。

**示例**

```cpp
int n = 4;
bool even = Math::IsEven(n);  // true
```

*此示例检查一个整数是否为偶数。*

---

#### `static bool IsOdd(int x) noexcept`

**函数签名**

```cpp
static bool IsOdd(int x) noexcept;
```

**概要**  
若 x 为奇数则返回 true。

**参数**

* `x` – 整数输入。

**返回值**

* 若 `x` 不能被 2 整除则为 `true`；否则为 `false`。

**深入说明**

对整数而言，`IsOdd()` 基本上是 `IsEven()` 的取反。

**示例**

```cpp
int n = 5;
bool odd = Math::IsOdd(n);  // true
```

*此示例检查一个整数是否为奇数。*

---

#### `static bool IsPowerOfTwo(int x) noexcept`

**函数签名**

```cpp
static bool IsPowerOfTwo(int x) noexcept;
```

**概要**  
若 x 为 2 的幂则返回 true。

**参数**

* `x` – 整数输入（通常为正数）。

**返回值**

* 若存在非负整数 `n` 使 `x` 为 `2^n`，则为 `true`。
* 对于零、负数或非 2 的幂的值，为 `false`。

**深入说明**

当处理缓冲区大小、内存对齐或其他要求为 2 的幂的底层数量时，此辅助函数非常实用。

**示例**

```cpp
int size = 64;
bool ok = Math::IsPowerOfTwo(size);  // true
```

*此示例检查某个大小是否为 2 的幂，这在某些算法中是常见要求。*

---

#### `static float Floor(float x) noexcept`

**函数签名**

```cpp
static float Floor(float x) noexcept;
```

**概要**  
返回不大于 x 的最大整数。

**参数**

* `x` – 浮点输入。

**返回值**

* 既是整数又不大于 `x` 的最大浮点值。

**深入说明**

`Floor()` 朝 **负无穷方向** 取整：

* `Floor(2.8f)` → `2.0f`
* `Floor(-2.1f)` → `-3.0f`

**示例**

```cpp
float x = 3.7f;
float r = Math::Floor(x);  // 3.0f
```

*此示例将浮点数向下取整到最近的整数。*

---

#### `static float Ceil(float x) noexcept`

**函数签名**

```cpp
static float Ceil(float x) noexcept;
```

**概要**  
返回不小于 x 的最小整数。

**参数**

* `x` – 浮点输入。

**返回值**

* 既是整数又不小于 `x` 的最小浮点值。

**深入说明**

`Ceil()` 朝 **正无穷方向** 取整：

* `Ceil(2.1f)` → `3.0f`
* `Ceil(-2.9f)` → `-2.0f`

**示例**

```cpp
float x = 3.1f;
float r = Math::Ceil(x);  // 4.0f
```

*此示例将浮点数向上取整到最近的整数。*

---

#### `static float Trunc(float x) noexcept`

**函数签名**

```cpp
static float Trunc(float x) noexcept;
```

**概要**  
移除 x 的小数部分（向零截断）。

**参数**

* `x` – 浮点输入。

**返回值**

* 移除小数部分后、向零方向截断的 `x`。

**深入说明**

`Trunc()` 的行为类似先转换为整数，再转换回 `float`：

* `Trunc(2.8f)` → `2.0f`
* `Trunc(-2.8f)` → `-2.0f`

**示例**

```cpp
float x = -3.9f;
float r = Math::Trunc(x);  // -3.0f
```

*此示例通过向零截断，移除负值的小数部分。*

---

#### `static float Round(float x) noexcept`

**函数签名**

```cpp
static float Round(float x) noexcept;
```

**概要**  
将 x 四舍五入到最接近的整数。

**参数**

* `x` – 浮点输入。

**返回值**

* 作为 `float` 的、最接近整数的 `x`。

**深入说明**

`Round()` 将数值四舍五入到最接近的整数。对于 `.5` 结尾的值（如 `2.5f`），其舍入行为依赖实现：可能向零外侧取整，也可能舍入到最近的偶数。

**示例**

```cpp
float x = 2.6f;
float r = Math::Round(x);  // 3.0f
```

*此示例将一个值舍入到最近的整数。*

---

#### `static float Exp2(float x) noexcept`

**函数签名**

```cpp
static float Exp2(float x) noexcept;
```

**概要**  
返回 2^x。

**参数**

* `x` – 指数。

**返回值**

* 作为 `float` 的 `2^x`。

**深入说明**

`Exp2()` 是处理 2 的幂的便捷函数，常用于二进制缩放与信号处理。

**示例**

```cpp
float x = 3.0f;
float value = Math::Exp2(x);  // 8.0f
```

*此示例使用 `Exp2` 计算 2³。*

---

#### `static float Log2(float x) noexcept`

**函数签名**

```cpp
static float Log2(float x) noexcept;
```

**概要**  
返回 log₂(x)。

**参数**

* `x` – 正浮点值。

**返回值**

* `x` 的以 2 为底的对数。对于 `x <= 0`，结果可能为 NaN 或依赖实现。

**深入说明**

`Log2()` 常用于计算表示某个值所需的位数，或在指数与缩放因子之间进行转换。

**示例**

```cpp
float x = 8.0f;
float bits = Math::Log2(x);  // 3.0f
```

*此示例计算 8 的二进制对数。*

---

#### `static float Ln(float x) noexcept`

**函数签名**

```cpp
static float Ln(float x) noexcept;
```

**概要**  
返回自然对数 ln(x)。

**参数**

* `x` – 正浮点值。

**返回值**

* 作为 `float` 的 `ln(x)`。

**深入说明**

`Ln()` 提供自然对数，即以 `Exp()`（以 e 为底的指数函数）的反函数。对 `x <= 0`，结果通常为 NaN 或负无穷，取决于值与实现。

**示例**

```cpp
float x = Math::E;
float value = Math::Ln(x);  // ≈ 1.0f
```

*此示例验证 e 的自然对数约等于 1。*

---

#### `static float Log10(float x) noexcept`

**函数签名**

```cpp
static float Log10(float x) noexcept;
```

**概要**  
返回 log₁₀(x)。

**参数**

* `x` – 正浮点值。

**返回值**

* `x` 的以 10 为底的对数。

**深入说明**

`Log10()` 常用于分贝计算、数量级分析以及科学计数法。

**示例**

```cpp
float x = 1000.0f;
float value = Math::Log10(x);  // 3.0f
```

*此示例计算 1000 的常用对数。*

---

#### `static float Exp(float x) noexcept`

**函数签名**

```cpp
static float Exp(float x) noexcept;
```

**概要**  
返回 e^x。

**参数**

* `x` – 指数。

**返回值**

* 作为 `float` 的 `e^x`。

**深入说明**

`Exp()` 是以 `Math::E` 为底的指数函数，是 `Ln()` 的反函数。

**示例**

```cpp
float x = 1.0f;
float value = Math::Exp(x);  // ≈ 2.71828
```

*此示例计算 e¹。*

---

#### `static float Pow(float x, float y) noexcept`

**函数签名**

```cpp
static float Pow(float x, float y) noexcept;
```

**概要**  
返回 x^y。

**参数**

* `x` – 底数。
* `y` – 指数。

**返回值**

* `x` 的 `y` 次幂。

**深入说明**

`Pow()` 是通用幂函数。对于特例（如 2 的幂或平方），使用更专门的函数可能更高效，但 `Pow()` 提供了最大灵活性。

某些 `x` 与 `y` 的组合可能产生 NaN（例如底数为负且指数为非整数时）。

**示例**

```cpp
float base = 2.0f;
float exp = 4.0f;
float value = Math::Pow(base, exp);  // 16.0f
```

*此示例计算 2 的 4 次幂。*

---

#### `static int Pow2(int x) noexcept`

**函数签名**

```cpp
static int Pow2(int x) noexcept;
```

**概要**  
返回 2^x（整数版本）。

**参数**

* `x` – 指数（整数）。

**返回值**

* 若结果可在 `int` 范围内表示，则为等于 `2^x` 的整数。

**深入说明**

`Pow2(int)` 通常实现为位移运算，因此在计算 2 的整数幂时非常高效。如果 `x` 过大，结果可能溢出；你需要自行保证 `x` 处于安全范围内。

**示例**

```cpp
int bits = 5;
int value = Math::Pow2(bits);  // 32
```

*此示例以整数形式计算 2⁵。*

---

#### `static float Sqrt(float x) noexcept`

**函数签名**

```cpp
static float Sqrt(float x) noexcept;
```

**概要**  
返回 x 的平方根。

**参数**

* `x` – 非负浮点值。

**返回值**

* `x` 的平方根。对于负数 `x`，结果通常为 NaN。

**深入说明**

`Sqrt()` 是标准平方根函数。对负输入，实数范围内没有定义，因此通常得到 NaN。

**示例**

```cpp
float area = 9.0f;
float side = Math::Sqrt(area);  // 3.0f
```

*此示例根据正方形面积求边长。*

---

#### `static float Sin(float x) noexcept`

**函数签名**

```cpp
static float Sin(float x) noexcept;
```

**概要**  
返回 x 的正弦值（弧度）。

**参数**

* `x` – 弧度制角度。

**返回值**

* `x` 的正弦值。

**深入说明**

`Sin()` 使用的是弧度而非角度。如果需要，可使用 `DegToRad()` 将角度转换为弧度。

**示例**

```cpp
float deg = 90.0f;
float rad = Math::DegToRad(deg);
float s = Math::Sin(rad);  // ≈ 1.0f
```

*此示例通过角度转弧度来计算 90 度的正弦值。*

---

#### `static float Cos(float x) noexcept`

**函数签名**

```cpp
static float Cos(float x) noexcept;
```

**概要**  
返回 x 的余弦值（弧度）。

**参数**

* `x` – 弧度制角度。

**返回值**

* `x` 的余弦值。

**深入说明**

`Cos()` 是余弦函数，同样使用弧度。与 `Sin()` 结合可用于基本向量与旋转运算。

**示例**

```cpp
float deg = 0.0f;
float rad = Math::DegToRad(deg);
float c = Math::Cos(rad);  // 1.0f
```

*此示例计算 0 度的余弦值。*

---

#### `static float Tan(float x) noexcept`

**函数签名**

```cpp
static float Tan(float x) noexcept;
```

**概要**  
返回 x 的正切值（弧度）。

**参数**

* `x` – 弧度制角度。

**返回值**

* `x` 的正切值。在 π/2 的奇数倍附近可能趋于无穷大。

**深入说明**

在正切函数未定义的点（如 `π/2`、`3π/2` 等）附近使用 `Tan()` 需格外小心，因为结果可能非常大，甚至溢出或失去精度。

**示例**

```cpp
float deg = 45.0f;
float rad = Math::DegToRad(deg);
float t = Math::Tan(rad);  // ≈ 1.0f
```

*此示例计算 45 度的正切值。*

---

#### `static float Atan2(float y, float x) noexcept`

**函数签名**

```cpp
static float Atan2(float y, float x) noexcept;
```

**概要**  
返回 y/x 的反正切值（弧度），并使用符号确定象限。

**参数**

* `y` – Y 分量。
* `x` – X 分量。

**返回值**

* 从正 X 轴到点 `(x, y)` 的弧度角，通常位于区间 `(-π, π]`。

**深入说明**

当同时拥有 X 与 Y 分量时，应优先使用 `Atan2()` 而非 `Atan()`，因为 `Atan2()` 能正确确定结果所在象限。它被广泛用于将笛卡尔坐标转换为极坐标。

**示例**

```cpp
float x = 0.0f;
float y = 1.0f;
float angle = Math::Atan2(y, x);  // ≈ π/2
```

*此示例计算一个指向正上方向量的角度。*

---

#### `static float Atan(float x) noexcept`

**函数签名**

```cpp
static float Atan(float x) noexcept;
```

**概要**  
返回 x 的反正切值（弧度）。

**参数**

* `x` – 输入值。

**返回值**

* `atan(x)` 的弧度值，通常位于区间 `(-π/2, π/2)`。

**深入说明**

`Atan()` 返回正切为 `x` 的角度。当你有单独的 X 和 Y 分量时，应优先使用 `Atan2()`，以正确处理象限问题。

**示例**

```cpp
float slope = 1.0f;
float angle = Math::Atan(slope);  // ≈ π/4
```

*此示例将斜率 1 转换为弧度角。*

---

#### `static float Asin(float x) noexcept`

**函数签名**

```cpp
static float Asin(float x) noexcept;
```

**概要**  
返回 x 的反正弦值（弧度）。

**参数**

* `x` – 输入值，范围为 [-1, 1]。

**返回值**

* 正弦值为 `x` 的弧度角。若 `x` 超出 [-1, 1]，结果通常为 NaN。

**深入说明**

`Asin()` 在从正弦值恢复角度时十分有用，例如在三角形计算中。

**示例**

```cpp
float v = 0.5f;
float angle = Math::Asin(v);  // ≈ 0.5236（30 度）
```

*此示例计算正弦为 0.5 的角度。*

---

#### `static float Cosh(float x) noexcept`

**函数签名**

```cpp
static float Cosh(float x) noexcept;
```

**概要**  
返回 x 的双曲余弦值。

**参数**

* `x` – 输入值。

**返回值**

* `x` 的双曲余弦值。

**深入说明**

`Cosh()` 是余弦函数的双曲对应，以指数函数定义。它比 `Sin`、`Cos`、`Tan` 使用频率低，但在一些高级数学模型中很有用。

**示例**

```cpp
float x = 1.0f;
float value = Math::Cosh(x);
```

*此示例计算 1 的双曲余弦值。*

---

#### `static float Mod(float x, float y) noexcept`

**函数签名**

```cpp
static float Mod(float x, float y) noexcept;
```

**概要**  
返回 x/y 的浮点余数。

**参数**

* `x` – 被除数。
* `y` – 除数（不应为零）。

**返回值**

* 作为 `float` 的 `x / y` 的余数。

**深入说明**

`Mod()` 的行为类似于 `x - y * floor(x / y)` 或相关的浮点余数函数。如果 `y` 为零，结果未定义；必须避免除以零。

**示例**

```cpp
float angle = Math::Mod(370.0f, 360.0f);  // 10.0f
```

*此示例使用浮点余数将角度折算到 0–360 度范围。*

---

#### `static float Lerp(float a, float b, float t) noexcept`

**函数签名**

```cpp
static float Lerp(float a, float b, float t) noexcept;
```

**概要**  
按 t 在 a 与 b 间进行线性插值。

**参数**

* `a` – 起始值。
* `b` – 结束值。
* `t` – 插值参数，通常位于 `[0.0f, 1.0f]`。

**返回值**

* 作为 `float` 的 `(1 - t) * a + t * b`。

**深入说明**

`Lerp()` 广泛用于动画与平滑过渡：

* `t = 0` → 返回 `a`。
* `t = 1` → 返回 `b`。
* `t` 在 0 与 1 之间 → 在 `a` 与 `b` 之间插值。

`[0, 1]` 之外的 `t` 会在端点之外进行外推。

**示例**

```cpp
float start = 0.0f;
float end = 10.0f;
float halfway = Math::Lerp(start, end, 0.5f);  // 5.0f
```

*此示例计算两个值的中点。*

---

#### `static float Step(float edge, float x) noexcept`

**函数签名**

```cpp
static float Step(float edge, float x) noexcept;
```

**概要**  
若 x < edge 则返回 0，否则返回 1。

**参数**

* `edge` – 阈值。
* `x` – 输入值。

**返回值**

* 若 `x < edge`，返回 `0.0f`。
* 若 `x >= edge`，返回 `1.0f`。

**深入说明**

`Step()` 实现了一个简单的阶跃函数，常用于阈值处理：

* 小于 `edge` 的值映射为 0。
* 大于或等于 `edge` 的值映射为 1。

它在数值计算中用于快速门控逻辑。

**示例**

```cpp
float edge = 0.5f;
float x = 0.7f;
float result = Math::Step(edge, x);  // 1.0f
```

*此示例根据阈值将连续值转换为二值输出。*

---

#### `static float DegToRad(float degrees) noexcept`

**函数签名**

```cpp
static float DegToRad(float degrees) noexcept;
```

**概要**  
将角度（度）转换为弧度。

**参数**

* `degrees` – 以度为单位的角度。

**返回值**

* 等价的弧度角。

**深入说明**

当你得到的角度是以度为单位（例如来自用户输入或文件数据），而需要调用使用弧度的三角函数时，使用 `DegToRad()`。

转换公式为：`degrees * (π / 180)`。

**示例**

```cpp
float degrees = 180.0f;
float radians = Math::DegToRad(degrees);  // ≈ π
```

*此示例将 180 度转换为弧度。*

---

#### `static float RadToDeg(float radians) noexcept`

**函数签名**

```cpp
static float RadToDeg(float radians) noexcept;
```

**概要**  
将弧度转换为角度（度）。

**参数**

* `radians` – 以弧度为单位的角度。

**返回值**

* 等价的度数角。

**深入说明**

`RadToDeg()` 是 `DegToRad()` 的反函数，用于将弧度转换回度数，常用于展示，因为度更直观易懂。

**示例**

```cpp
float angle = Math::PI;
float degrees = Math::RadToDeg(angle);  // ≈ 180.0f
```

*此示例将 π 弧度转换为度数。*

---

#### `static bool IsFinite(float x) noexcept`

**函数签名**

```cpp
static bool IsFinite(float x) noexcept;
```

**概要**  
若 x 为有限值（既不是无穷也不是 NaN）则返回 true。

**参数**

* `x` – 浮点值。

**返回值**

* 若 `x` 既不是无穷也不是 NaN，则为 `true`；否则为 `false`。

**深入说明**

当你需要验证计算结果是否溢出或产生 NaN 时，`IsFinite()` 非常关键。它能将普通数值与特殊情况区分开来。

**示例**

```cpp
float value = Math::INF;
bool ok = Math::IsFinite(value);  // false
```

*此示例展示正无穷并不被视作有限值。*

---

#### `static bool IsNaN(float x) noexcept`

**函数签名**

```cpp
static bool IsNaN(float x) noexcept;
```

**概要**  
若 x 为 NaN 则返回 true。

**参数**

* `x` – 浮点值。

**返回值**

* 若 `x` 为 NaN，则为 `true`；否则为 `false`。

**深入说明**

必须使用 `IsNaN()` 来检测 NaN，因为诸如 `x == Math::NAN` 的比较始终为假。

在你怀疑某个计算可能产生无效结果时，应使用它进行检查。

**示例**

```cpp
float value = Math::NAN;
bool bad = Math::IsNaN(value);  // true
```

*此示例使用 `IsNaN` 检测 NaN 值。*

---

#### `static long long Add64(long long a, long long b) noexcept`

**函数签名**

```cpp
static long long Add64(long long a, long long b) noexcept;
```

**概要**  
返回 a + b 的和（64 位整数）。

**参数**

* `a` – 第一个操作数。
* `b` – 第二个操作数。

**返回值**

* 作为 64 位整数的 `a + b`。

**深入说明**

`Add64()` 对两个 64 位整数执行加法。如果结果溢出了 `long long` 的范围，其行为与 C++ 中原生整数溢出一样，依赖实现。

**示例**

```cpp
long long a = 1000000000LL;
long long b = 2000000000LL;
long long sum = Math::Add64(a, b);
```

*此示例对两个较大的 64 位整数求和。*

---

#### `static long long Sub64(long long a, long long b) noexcept`

**函数签名**

```cpp
static long long Sub64(long long a, long long b) noexcept;
```

**概要**  
返回 a - b 的差（64 位整数）。

**参数**

* `a` – 被减数。
* `b` – 减数。

**返回值**

* 作为 64 位整数的 `a - b`。

**深入说明**

`Sub64()` 用于两个 64 位整数的减法。与加法类似，溢出行为依赖实现。

**示例**

```cpp
long long total = 5000LL;
long long used = 1200LL;
long long remaining = Math::Sub64(total, used);  // 3800
```

*此示例通过减法计算剩余的 64 位值。*

---

#### `static long long Div64(long long a, long long b) noexcept`

**函数签名**

```cpp
static long long Div64(long long a, long long b) noexcept;
```

**概要**  
返回 a / b 的商（64 位整数）。

**参数**

* `a` – 被除数。
* `b` – 除数（不可为零）。

**返回值**

* 作为 64 位整数的商 `a / b`，使用整数除法（舍弃小数部分）。

**深入说明**

`Div64()` 对两个 64 位整数进行相除。除数为零是未定义行为，必须避免。结果向零截断。

**示例**

```cpp
long long bytes = 4096LL;
long long blockSize = 512LL;
long long blocks = Math::Div64(bytes, blockSize);  // 8
```

*此示例计算指定字节数内可容纳多少完整数据块。*

---

#### `static long long Mod64(long long a, long long b) noexcept`

**函数签名**

```cpp
static long long Mod64(long long a, long long b) noexcept;
```

**概要**  
返回 a / b 的余数（64 位整数）。

**参数**

* `a` – 被除数。
* `b` – 除数（不可为零）。

**返回值**

* 整数除法 `a / b` 的余数。

**深入说明**

`Mod64()` 与 `Div64()` 互补，用于返回余数。与任何整数取模运算一样，除以零是未定义行为。

**示例**

```cpp
long long bytes = 4100LL;
long long blockSize = 512LL;
long long remainder = Math::Mod64(bytes, blockSize);  // 4
```

*此示例在按固定块大小划分时，计算剩余的字节数。*