# Arguments

## 概览

`attoboy::Arguments` 类是为 attoboy 应用程序设计的一个小而专注的**命令行参数解析器**。它提供了一种结构化方式来声明：

* **标志（flag）**（布尔开关，如 `-v` 或 `--verbose`），
* **具名参数**（键值选项，如 `-output=log.txt`），
* **位置参数**（通过位置识别的参数，如输入文件名），

然后解析启动你的程序时实际传入的命令行。

与其手动检查 `argv` 并编写自定义解析逻辑，你可以：

1. 定义程序期望的参数。
2. 可选地将某些参数标记为**必需**。
3. 设置人类可读的**描述**和**帮助文本**。
4. 调用 `parseArguments()` 来验证并获取结构化的 `Map` 值集合。

`Arguments` 使用 attoboy 自己的 `String` 和 `Map` 类型，因此能够自然融入该库的其他部分。

### 命令行参数、标志和参数

当程序从终端启动时，通常会接收到一组**命令行参数**，例如：

```text
mytool -verbose -mode=fast input.txt
```

这些参数通常属于以下三类：

* **标志（Flags）** – 仅有存在或不存在两种状态的布尔选项：

  * 示例：`-verbose`、`--dry-run`
  * 通常用于开启或关闭某些功能。

* **具名参数（Named parameters）** – 带显式值的选项：

  * 示例：`-mode=fast` 或 `-output result.json`
  * 用于配置诸如文件路径、运行模式或阈值等内容。

* **位置参数（Positional parameters）** – 根据位置进行解释的值：

  * 示例：`input.txt` 或 `source` 和 `destination`
  * 通常用于诸如必需的输入文件等。

`attoboy::Arguments` 将从原始命令行解析这些参数的繁琐细节进行了抽象：

* 在构造 `Arguments` 对象时，它会**捕获**程序的命令行参数。
* 你**声明**所期望的标志、具名参数和位置参数。
* 你调用 **`parseArguments()`**，它会校验输入，自动处理 `-h/--help`，并返回一个解析后的 `Map`。

### 自动帮助和必需参数

当你使用 `Arguments` 时，解析器会自动处理：

* `-h` / `--help`：打印帮助信息（使用你提供的描述），并返回一个空的 `Map`。
* **必需参数**：如果用户遗漏了必需的标志或参数，`parseArguments()` 会返回一个空的 `Map`，这样你就可以在配置不完整时干净地中止程序，而不是继续运行。

这有助于你：

* 将主程序逻辑与命令行校验分离。
* 为使用 attoboy 构建的工具提供一致的用户体验。

---

## 参考

所有示例都假定：

```cpp
using namespace attoboy;
```

---

#### `Arguments()`

**函数签名**

```cpp
Arguments();
```

**概要**  
创建一个 Arguments 解析器并捕获命令行参数。

**参数**

* *(无)*

**返回值**

* *(构造函数；不适用)*

**详解**

构造一个 `Arguments` 对象会捕获当前进程的命令行参数。通常你会：

1. 在程序开始处构造解析器。
2. 声明你期望的标志和参数。
3. 调用 `parseArguments()` 来解释已捕获的参数。

通常每个程序只需要创建一个 `Arguments` 实例，但如有需要你也可以创建多个（例如，为不同子命令使用不同的解析方式）。

**示例**

```cpp
Arguments args;

// 在解析前声明预期选项
args.addFlag("verbose", "Enable verbose output");
args.addParameter("mode", "Processing mode", "fast");

// 随后，解析捕获的命令行
Map parsed = args.parseArguments();
```

*此示例创建了一个 `Arguments` 解析器，配置了预期选项，并准备解析命令行。*

---

#### `Arguments(const Arguments &other)`

**函数签名**

```cpp
Arguments(const Arguments &other);
```

**概要**  
创建另一个 Arguments 解析器的副本。

**参数**

* `other` – 要拷贝的已有 `Arguments` 实例。

**返回值**

* *(构造函数；不适用)*

**详解**

拷贝构造函数允许你复制一个解析器的配置，包括所有已定义的标志、具名参数、位置参数以及任意已捕获的命令行状态。

典型用例包括：

* 复用一个基础配置，然后在副本上做轻微修改以实现不同解析行为。
* 将解析器状态传递给辅助组件，以便它们可以独立查询参数。

**示例**

```cpp
Arguments base;
base.addFlag("verbose", "Enable verbose output");

Arguments copy(base);
// copy 现在识别与 base 相同的参数
```

*此示例为在其他地方复用而拷贝了一个已配置的 `Arguments` 解析器。*

---

#### `~Arguments()`

**函数签名**

```cpp
~Arguments();
```

**概要**  
销毁 Arguments 解析器并释放内存。

**参数**

* *(无)*

**返回值**

* *(析构函数；不适用)*

**详解**

析构函数会释放解析器使用的任何内部资源（如已分配的元数据或捕获的参数存储）。由于 `Arguments` 遵循 RAII，只需让对象离开作用域即可完成清理。

**示例**

```cpp
{
  Arguments args;
  // 在此配置并解析
} // args 在此处被销毁；资源被释放
```

*此示例展示了解析器在离开作用域时自动清理资源。*

---

#### `Arguments &operator=(const Arguments &other)`

**函数签名**

```cpp
Arguments &operator=(const Arguments &other);
```

**概要**  
将另一个 Arguments 解析器赋给当前对象。

**参数**

* `other` – 其配置和状态应被复制的解析器。

**返回值**

* 对 `*this` 的引用，允许链式赋值。

**详解**

赋值操作会将当前解析器的配置和捕获状态替换为 `other` 的配置和状态。赋值后：

* 两个对象都识别相同的已声明参数。
* 两个对象看到相同的已解析状态（直到你再次调用 `parseArguments()`）。

自赋值是安全的。

**示例**

```cpp
Arguments args;
args.addFlag("verbose", "Enable verbose output");

Arguments other;
other = args;  // other 现在拥有相同的参数定义
```

*此示例将一个解析器的配置赋值给另一个。*

---

#### `Arguments &addFlag(const String &name, const String &description = String(), bool defaultValue = false, const String &longName = String())`

**函数签名**

```cpp
Arguments &addFlag(const String &name, const String &description = String(),
                   bool defaultValue = false,
                   const String &longName = String());
```

**概要**  
添加一个布尔标志（-name、--longName 或 -name=true/false）。

**参数**

* `name` – 标志的短名称，用于 `-name`（例如，`"verbose"` → `-verbose`）。
* `description` – 可选的人类可读描述，将显示在帮助文本中。
* `defaultValue` – 如果命令行未提供该标志，则使用的默认布尔值。
* `longName` – 可选的长名称，用于 `--longName`（例如，`"verbose"` → `--verbose`）。如果为空，则仅支持 `-name` 形式。

**返回值**

* 对 `*this` 的引用，支持方法链式调用。

**详解**

标志是可以切换开或关的**布尔选项**。通过 `addFlag`，解析器可以识别以下形式：

* `-name` – 通常表示“设为 true”。
* `--longName` – 相同标志的长形式（如果提供）。
* `-name=true` / `-name=false` – 显式设置值。

在解析时：

* 标志的值会以 `String` `"true"` 或 `"false"` 形式存储，并可以通过 `getArgument(name)` 或 `getArgument(longName)` 访问（取决于实现）。
* `defaultValue` 决定当用户未设置该标志时 `getArgument()` 返回什么，以及 `hasArgument()` 是否将其视为存在。

**示例**

```cpp
Arguments args;

args.addFlag("verbose", "Enable verbose logging", false, "verbose");

Map parsed = args.parseArguments();
if (parsed.isEmpty()) {
  // 已显示帮助或缺失必需参数
} else {
  bool verbose = args.getArgument("verbose").toBool();
  if (verbose) {
    LogInfo("Verbose mode enabled");
  }
}
```

*此示例声明了一个默认值为 `false` 的 `-verbose` / `--verbose` 标志，并使用其解析结果控制日志输出。*

---

#### `Arguments &addParameter(const String &name, const String &description = String(), const String &defaultValue = String(), const String &longName = String())`

**函数签名**

```cpp
Arguments &addParameter(const String &name,
                        const String &description = String(),
                        const String &defaultValue = String(),
                        const String &longName = String());
```

**概要**  
添加一个具名参数（-name=value 或 -name value）。

**参数**

* `name` – 参数的短名称（例如，`"output"` → `-output`）。
* `description` – 在帮助输出中显示的可选人类可读描述。
* `defaultValue` – 如果未提供该参数，则使用的默认字符串值。
* `longName` – 可选的长形式（例如，`"output"` → `--output`）。如果为空，则仅使用短形式。

**返回值**

* 对 `*this` 的引用，支持方法链式调用。

**详解**

具名参数是**键值选项**。解析器通常接受以下形式：

* `-name=value`
* `-name value`
* `--longName=value`
* `--longName value`

值会以 `String` 存储，并可通过 `getArgument(name)` 获取。

如果用户省略该参数：

* `getArgument(name)` 返回 `defaultValue`。
* 如果存在默认值，即使用户未显式提供，`hasArgument(name)` 也会返回 `true`。

这非常适合用于输出路径、运行模式或阈值等设置。

**示例**

```cpp
Arguments args;

args.addParameter("output",
                  "Output file path",
                  "result.txt",
                  "output");

Map parsed = args.parseArguments();
if (parsed.isEmpty()) {
  // 帮助或校验失败
} else {
  String outputPath = args.getArgument("output");
  LogInfo("Writing results to ", outputPath);
}
```

*此示例声明了一个带默认文件名的 `-output` / `--output` 参数，并在解析后读取其值。*

---

#### `Arguments &addPositionalParameter(const String &name, const String &description = String())`

**函数签名**

```cpp
Arguments &addPositionalParameter(const String &name,
                                  const String &description = String());
```

**概要**  
添加一个位置参数（按顺序从剩余参数中填充）。

**参数**

* `name` – 在结果 map 中标识该位置参数所使用的名称。
* `description` – 显示在帮助中的可选人类可读描述。

**返回值**

* 对 `*this` 的引用，支持方法链式调用。

**详解**

位置参数表示**按位置识别**的参数，而不是以 `-` 或 `--` 前缀开头的参数。在所有标志和具名参数解析完之后，解析器会按照声明顺序，将剩余的原始参数依次分配给你声明的位置参数。

例如，如果你定义：

```text
addPositionalParameter("input");
addPositionalParameter("output");
```

而用户运行：

```text
mytool -mode=fast in.txt out.txt
```

则：

* `"input"` 获得 `"in.txt"`，
* `"output"` 获得 `"out.txt"`。

你可以通过 `getArgument("input")` 和 `getArgument("output")` 获取这些值。

**示例**

```cpp
Arguments args;

args.addPositionalParameter("input",  "Input file");
args.addPositionalParameter("output", "Output file");

Map parsed = args.parseArguments();
if (parsed.isEmpty()) {
  // 帮助或缺少必需参数
} else {
  String input  = args.getArgument("input");
  String output = args.getArgument("output");
  LogInfo("Copying from ", input, " to ", output);
}
```

*此示例定义了两个位置参数，并将它们用作源和目标路径。*

---

#### `Arguments &setHelp(const String &helpText)`

**函数签名**

```cpp
Arguments &setHelp(const String &helpText);
```

**概要**  
为 -h/--help 设置自定义帮助文本。

**参数**

* `helpText` – 当请求 `-h` 或 `--help` 时要显示的自定义帮助或用法文本的 `String`。

**返回值**

* 对 `*this` 的引用，支持方法链式调用。

**详解**

默认情况下，解析器可以基于以下信息生成帮助文本：

* 你添加的参数，
* 它们的名称和描述。

`setHelp()` 允许你用自己的帮助内容覆盖或补充这些默认帮助。当用户传入 `-h` 或 `--help` 时，`parseArguments()` 会：

* 显示帮助文本（可能是你的自定义文本），并且
* 返回一个空的 `Map`，以便你中止正常执行流程。

**示例**

```cpp
Arguments args;

args.addFlag("verbose", "Enable verbose mode");
args.addPositionalParameter("input", "Input file");

String help =
  "Usage: mytool [options] <input>\n"
  "\n"
  "Options:\n"
  "  -verbose    Enable verbose output\n";

args.setHelp(help);

Map parsed = args.parseArguments();
if (parsed.isEmpty()) {
  // 已打印帮助或校验失败；退出程序。
}
```

*此示例设置了一个自定义用法消息，当用户请求帮助时会显示该消息。*

---

#### `Arguments &requireArgument(const String &name)`

**函数签名**

```cpp
Arguments &requireArgument(const String &name);
```

**概要**  
将某个参数标记为必需。

**参数**

* `name` – 要标记为必需的参数名称。这可以是标志、具名参数或位置参数的名称。

**返回值**

* 对 `*this` 的引用，支持方法链式调用。

**详解**

`requireArgument()` 告诉解析器某个参数**必须**由用户提供。如果在调用 `parseArguments()` 时缺少该参数：

* 解析器会将其视为错误。
* 返回一个空的 `Map`。
* 可能会打印错误和帮助信息来告知用户（取决于实现）。

你可以连续调用该函数来将多个参数标记为必需。

**示例**

```cpp
Arguments args;

args.addPositionalParameter("input", "Input file");
args.addParameter("mode", "Processing mode", "fast");
args.requireArgument("input");  // input 为必需参数

Map parsed = args.parseArguments();
if (parsed.isEmpty()) {
  // 要么显示了帮助，要么缺少 "input"；中止。
} else {
  String input = args.getArgument("input");
  String mode  = args.getArgument("mode");
  LogInfo("Processing ", input, " in mode ", mode);
}
```

*此示例强制要求必须提供 `input` 位置参数。*

---

#### `String getArgument(const String &name) const`

**函数签名**

```cpp
String getArgument(const String &name) const;
```

**概要**  
返回参数值（标志返回 "true" 或 "false"）。

**参数**

* `name` – 你想要其值的参数名称。这与 `addFlag`、`addParameter` 或 `addPositionalParameter` 中使用的名称相同。

**返回值**

* 表示该参数值的 `String`：

  * 标志：`"true"` 或 `"false"`。
  * 具名参数：用户提供的值或默认值。
  * 位置参数：若提供，则为该位置上的值。

**详解**

`getArgument()` 是在调用 `parseArguments()` 之后获取解析后参数值的主要方式。

行为：

* 如果命令行中未提供某具名参数，但其有 `defaultValue`，则返回该默认值。
* 如果未提供某标志但有 `defaultValue`，则返回对应的布尔文本（`"true"` 或 `"false"`）。
* 如果参数缺失且没有默认值，其行为依赖具体实现；通常会返回空字符串。

为了避免区分“缺失”与“存在但值为空”的歧义，你可以将 `getArgument()` 与 `hasArgument()` 结合使用。

**示例**

```cpp
Arguments args;

args.addFlag("verbose", "Enable verbose output");
args.addParameter("mode", "Processing mode", "fast");
args.addPositionalParameter("input", "Input file");

Map parsed = args.parseArguments();
if (!parsed.isEmpty()) {
  String input   = args.getArgument("input");
  String mode    = args.getArgument("mode");
  bool verbose   = args.getArgument("verbose").toBool();

  LogInfo("Input: ", input, ", mode: ", mode);
  if (verbose) {
    LogInfo("Verbose mode is enabled");
  }
}
```

*此示例使用 `getArgument()` 获取标志、具名参数和位置参数的值。*

---

#### `bool hasArgument(const String &name) const`

**函数签名**

```cpp
bool hasArgument(const String &name) const;
```

**概要**  
如果参数被设置或具有默认值，则返回 true。

**参数**

* `name` – 要检查的参数名称。

**返回值**

* 如果用户提供了该参数，或其具有默认值，则为 `true`。
* 否则为 `false`。

**详解**

`hasArgument()` 告诉你在解析之后一个参数是否**逻辑上可用**。这包括：

* 在命令行中显式设置的参数。
* 配置了默认值的参数（具名参数和标志）。

这在以下情况中特别有用：

* 你想区分“该参数根本不是配置的一部分”与“该参数存在，但可能为空值”。
* 你需要知道某个值是来自配置（默认值）还是用户显式提供。

注意：缺失的必需参数会导致 `parseArguments()` 返回空 `Map`，因此你通常会在调用 `hasArgument()` 之前先检查结果是否为空。

**示例**

```cpp
Arguments args;

args.addParameter("mode", "Processing mode", "fast");

Map parsed = args.parseArguments();
if (!parsed.isEmpty()) {
  if (args.hasArgument("mode")) {
    String mode = args.getArgument("mode");
    LogInfo("Mode set to ", mode);
  }
}
```

*此示例检查某参数在逻辑上是否可用，然后读取其值。*

---

#### `Map parseArguments(bool suppressHelp = false)`

**函数签名**

```cpp
Map parseArguments(bool suppressHelp = false);
```

**概要**  
解析参数并返回一个 Map。如果显示了帮助或缺失必需参数，则返回空 Map。

**参数**

* `suppressHelp` – 如果为 `false`（默认），解析器在适当情况下会自动显示帮助和错误消息（例如，对 `-h` / `--help` 或缺失必需参数）。如果为 `true`，则只进行解析并返回结果，而不会自动显示帮助；你可以自行处理帮助和错误。

**返回值**

* 包含解析后参数的 `Map`，或者在以下情况返回**空 Map**：

  * 用户请求了帮助（`-h` 或 `--help`），或
  * 一项或多项必需参数缺失，或
  * 发生其他解析错误（取决于实现）。

**详解**

`parseArguments()` 是核心操作：

1. 使用通过 `addFlag`、`addParameter`、`addPositionalParameter` 和 `requireArgument` 构建的配置来解释已捕获的命令行参数。
2. 校验输入（必需参数、值格式等）。
3. 然后：

   * 返回一个非空的、包含解析值的 `Map`，或
   * 返回一个空 `Map`，并在 `suppressHelp` 为 `false` 时显示帮助和/或错误消息。

返回的 `Map` 通常用于迭代或与外部 API 协作，但在大多数程序中，你主要会使用 `getArgument()` 和 `hasArgument()` 以获得更便捷的访问方式。

典型使用模式：

```cpp
Map parsed = args.parseArguments();
if (parsed.isEmpty()) {
  // 不要继续执行；用户需要帮助或必须修正输入。
  return;
}
// 在这里安全地使用参数。
```

如果你传入 `suppressHelp = true`，你可以自行检测帮助请求，通过检查参数并按需打印自定义消息。

**示例**

```cpp
Arguments args;

args.addFlag("verbose", "Enable verbose output");
args.addParameter("mode", "Processing mode", "fast");
args.addPositionalParameter("input", "Input file");
args.requireArgument("input");

Map parsed = args.parseArguments();
// 如果请求了帮助或缺少 "input"，parsed 将为空。
if (parsed.isEmpty()) {
  // 中止程序逻辑；用户要么已看到帮助，要么需要修正参数。
} else {
  String input  = args.getArgument("input");
  String mode   = args.getArgument("mode");
  bool verbose  = args.getArgument("verbose").toBool();

  LogInfo("Input: ", input, ", mode: ", mode);
  if (verbose) {
    LogInfo("Verbose output enabled");
  }
}
```

*此示例演示了完整的典型流程：配置参数、强制要求必需参数、解析命令行，并使用解析结果。*

---

#### `List getAllArguments() const`

**函数签名**

```cpp
List getAllArguments() const;
```

**概要**  
返回所有原始命令行参数的列表（包括 `argv[0]`）。

**参数**

* *(无)*

**返回值**

* 一个包含 `Arguments` 对象捕获的每个命令行参数的 `String` 的 `List`：

  * 索引 `0` 对应 `argv[0]`（通常为程序名）。
  * 后续元素按操作系统传递给进程的顺序依次对应每个参数标记。

**详解**

虽然 `parseArguments()` 为你提供了命令行的已验证、结构化视图，但 `getAllArguments()` 会暴露**原始**参数，即在构造 `Arguments` 实例时捕获到的原样内容。

这在以下情况下很有用：

* 你想要记录或审计完整命令行，用于调试或诊断。
* 你需要为某些未被 `addFlag`、`addParameter` 或 `addPositionalParameter` 覆盖的特殊情况实现自定义解析逻辑。
* 你希望比较高层的解析配置与进程实际收到的原始 `argv`。

返回的 `List` 独立于已解析的 `Map`；调用 `parseArguments()` 不会改变原始参数列表。通常，列表中的每一项都对应于 C 或 C++ 中传统 `argv[]` 数组的一个元素。

**示例**

```cpp
Arguments args;

// 检查或记录原始命令行
List all = args.getAllArguments();

for (int i = 0; i < all.size(); ++i) {
  LogInfo("argv[", i, "] = ", all[i]);
}
```

*此示例检索每个原始命令行参数（包括程序名），并按顺序将其记录下来，用于调试或审计。*