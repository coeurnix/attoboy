# Console

## 概述

`attoboy::Console` 类是在 Windows 上构建丰富、交互式命令行应用程序的高级接口。它组合了以下功能：

* 彩色文本输出。
* 自动换行与对齐。
* 光标定位与屏幕控制。
* 带历史和 Tab 补全的交互式行输入。
* 基于事件的输入（键盘和鼠标）。
* 动画进度条。
* 交互式菜单。
* 非阻塞全局热键。

该类使用 **RAII**（Resource Acquisition Is Initialization，资源获取即初始化）来管理控制台状态：

* 当构造一个 `Console` 对象时，它会将终端配置为适合交互使用（UTF-8 输出、鼠标、窗口大小变更处理等）。
* 当对象被销毁时，会恢复原有控制台设置。

同一时间只应存在一个 `Console` 实例。由于 `Console` 不可拷贝，在代码中强制这一点非常简单。

### 控制台基本概念

如果你是控制台编程的新手，下列几个核心概念会很有帮助。

#### 控制台坐标

控制台屏幕是一个**字符网格**。可以把它看作一个二维数组：

* 水平轴是 **X 坐标**（列）。
* 垂直轴是 **Y 坐标**（行）。
* 坐标是 **从 0 开始** 的，所以左上角是 `(0, 0)`。

`Console::width()` 和 `Console::height()` 函数会给出当前网格的大小。

#### 颜色和属性

控制台支持由 `ConsoleColor` 枚举定义的 **16 种颜色**：

* 例如 `CON_RED`、`CON_GREEN`、`CON_WHITE`、`CON_BLACK` 等。

当你使用 `Console::print()` 或 `Console::println()` 打印文本时，可以传入**前景色**和**背景色**：

* 前景色（`fg`）控制文本颜色。
* 背景色（`bg`）控制背景颜色。

默认值为：

* `fg = CON_WHITE`
* `bg = CON_BLACK`

#### 文本对齐与换行

`ConsoleAlign` 枚举定义了文本的对齐方式：

* `CON_ALIGN_LEFT`（默认）
* `CON_ALIGN_CENTER`
* `CON_ALIGN_RIGHT`
* `CON_ALIGN_JUSTIFY`

`Console::printAligned()` 和 `Console::printWrapped()` 会使用这些对齐设置以及可用宽度来布局文本。

还提供了静态辅助函数：

* `Console::Align()` – 返回带填充/对齐的单行文本。
* `Console::Wrap()` – 返回自动换行后的多行字符串。

这些函数本身不会进行绘制；它们只是返回你可以打印或继续处理的 `String`。

#### 交互式输入与事件

`Console::input()` 提供基于行的输入，并通过 `ConsoleInput` 结构控制可选的高级特性：

* Tab 补全。
* 输入历史。
* 密码模式（掩码输入）。
* 多行模式。

`Console::readEvent()` 则是更底层的接口，会以人类可读的字符串形式返回**事件**，例如按键和鼠标动作：`"Enter"`、`"Ctrl+C"` 或 `"MouseLeft@10,5"` 等。

#### 进度条与菜单

控制台可以在屏幕底部显示一个持久化的**进度条**。你可以通过 `Console::progress()` 更新它，并用 `Console::progressHide()` 隐藏它。

`Console::menu()` 方法可以显示一个交互式菜单，支持通过键盘或鼠标导航，并返回所选项的索引。

#### 全局热键

你可以使用 `Console::onHotkey()` 注册**全局热键**：

* 例如 `"Ctrl+Q"` 或 `"Shift+F1"`。
* 当被按下时，会异步调用一个回调函数。
* 你可以通过 `Console::offHotkey()` 取消注册热键。

这可以实现响应式用户界面，即使在等待输入或者更新屏幕的过程中，某些按键组合也能触发动作。

---

## 参考

本节涵盖 `Console` 类的所有公开构造函数、析构函数、成员函数、静态函数和运算符。对于每个符号，你将看到：

* **Signature（签名）** – 头文件中的完整声明。
* **Synopsis（简要说明）** – 原始的单行 Doxygen 注释。
* **Parameters（参数）** – 每个参数的说明。
* **Return value（返回值）** – 返回值说明（如适用）。
* **In Depth（深入说明）** – 详细行为、使用注意和细节。
* 一个带简短说明文字的 **示例** 代码片段。

> 所有示例都假定 `using namespace attoboy;`。

---

#### `Console::Console()`

**Signature**

```cpp
Console();
```

**Synopsis**  
初始化控制台以供交互使用。  
启用鼠标输入、设置窗口大小变更处理，并配置 UTF-8 输出。

**Parameters**

* *(无)*

**Return value**

* *(构造函数；不适用)*

**In Depth**

构造一个 `Console` 对象会：

* 将终端置于适合交互式 UI 的模式：

  * UTF-8 输出。
  * 鼠标事件上报。
  * 处理控制台窗口大小变更，以便 UI 元素可以重新布局。
* 捕获**之前的控制台状态**，以便在析构函数中恢复。

由于同一时间只应存在一个 `Console` 实例，最好的做法是在程序进入交互式阶段时创建一个实例，并保持其存活，直到不再需要控制台 UI 为止。

**Example**

```cpp
Console console;
console.println("Interactive console initialized.");
```

*此示例构造一个 `Console` 并打印一条确认消息。*

---

#### `Console::~Console()`

**Signature**

```cpp
~Console();
```

**Synopsis**  
恢复原始的控制台状态和设置。

**Parameters**

* *(无)*

**Return value**

* *(析构函数；不适用)*

**In Depth**

当一个 `Console` 对象被销毁时：

* 对控制台配置所做的任何更改（颜色模式、输入处理等）都会被撤销。
* 控制台会回到原始状态，从而避免影响其他工具或 Shell。

这就是 RAII 模式：在构造函数中获取资源，在析构函数中释放资源。

**Example**

```cpp
{
  Console console;
  console.println("This UI uses custom console settings.");
}
// After this block, the console state is restored automatically.
```

*此示例展示了当对象离开作用域时，控制台设置会自动被清理并恢复。*

---

#### `Console::Console(const Console &) = delete`

**Signature**

```cpp
Console(const Console &) = delete;
```

**Synopsis**  
Console 不可拷贝。

**Parameters**

* *(无；此函数已删除，不能被调用)*

**Return value**

* *(不适用；已删除函数)*

**In Depth**

拷贝构造函数被**删除**，这意味着你不能拷贝一个 `Console`：

* 不能写 `Console other = console;`
* 不能按值传入或按值返回。

这可以防止多个 `Console` 对象尝试管理同一个底层控制台状态，从而避免错误。

如果你需要共享一个 `Console`，请通过引用或指针进行传递。

**Example**

```cpp
Console console;

// The following line is NOT allowed and will not compile:
// Console copy = console;  // error: Console copy constructor is deleted
```

*此示例说明不允许拷贝 `Console` 实例。*

---

#### `Console &Console::operator=(const Console &) = delete`

**Signature**

```cpp
Console &operator=(const Console &) = delete;
```

**Synopsis**  
Console 不可赋值。

**Parameters**

* *(无；此运算符已删除，不能被调用)*

**Return value**

* *(不适用；已删除函数)*

**In Depth**

拷贝赋值运算符同样被**删除**，因此你不能将一个 `Console` 赋值给另一个。这与不可拷贝的设计是一致的。

如果你发现自己想要给控制台赋值，通常意味着应该重构代码，使其只持有一个 `Console` 对象，并在需要的地方传递引用。

**Example**

```cpp
Console a;
Console b;

// The following line is NOT allowed and will not compile:
// b = a;  // error: Console assignment operator is deleted
```

*此示例说明你不能将一个 `Console` 实例赋值给另一个。*

---

#### `int Console::width() const`

**Signature**

```cpp
int width() const;
```

**Synopsis**  
返回控制台宽度（以字符数计）。

**Parameters**

* *(无)*

**Return value**

* 当前控制台宽度（字符列数）。

**In Depth**

`width()` 告诉你控制台窗口单行可以容纳多少个字符。由于用户可以调整控制台大小，该值可能会随时间变化。如果你在意布局，应在渲染时调用 `width()`。

常见用途：

* 文本居中或右对齐。
* 设计布局网格。
* 选择换行宽度。

**Example**

```cpp
Console console;
int w = console.width();

String msg("Console width: ");
console.println(msg + String(w));
```

*此示例读取控制台宽度并将其作为消息的一部分打印出来。*

---

#### `int Console::height() const`

**Signature**

```cpp
int height() const;
```

**Synopsis**  
返回控制台高度（以字符数计）。

**Parameters**

* *(无)*

**Return value**

* 当前控制台高度（字符行数）。

**In Depth**

`height()` 告诉你控制台窗口中可以容纳多少行文本。与 `width()` 一样，如果用户调整窗口大小，该值也会改变。

这在以下场景中很有帮助：

* 设计全屏 UI。
* 相对于屏幕底部放置菜单或状态栏。
* 避免输出造成不必要的滚动。

**Example**

```cpp
Console console;
int h = console.height();

console.println("Console height: " + String(h));
```

*此示例打印当前控制台高度（行数）。*

---

#### `Console &Console::print(const String &text, ConsoleColor fg = CON_WHITE, ConsoleColor bg = CON_BLACK)`

**Signature**

```cpp
Console &print(const String &text, ConsoleColor fg = CON_WHITE,
               ConsoleColor bg = CON_BLACK);
```

**Synopsis**  
以可选的前景色和背景色打印文本。

**Parameters**

* `text` – 要在当前光标位置打印的文本。
* `fg` – 前景文本颜色（默认 `CON_WHITE`）。
* `bg` – 背景颜色（默认 `CON_BLACK`）。

**Return value**

* 对 `*this` 的引用，便于链式调用。

**In Depth**

`print()` 向控制台写入文本，但**不会**在末尾添加换行符。光标会根据打印的字符数向前移动。

颜色行为：

* 颜色应用于打印的这段文本。
* 打印后，控制台可能保持新的颜色状态。如果你希望恢复默认颜色，需要显式使用你期望的颜色再次打印。

你可以进行链式调用：

```cpp
console.print("Label: ").print("value", CON_GREEN);
```

**Example**

```cpp
Console console;

console.print("OK ", CON_GREEN);
console.print("ERROR ", CON_RED);
console.print("DEFAULT");
```

*此示例通过链式调用打印三个使用不同颜色的单词。*

---

#### `Console &Console::println(const String &text = "", ConsoleColor fg = CON_WHITE, ConsoleColor bg = CON_BLACK)`

**Signature**

```cpp
Console &println(const String &text = "", ConsoleColor fg = CON_WHITE,
                 ConsoleColor bg = CON_BLACK);
```

**Synopsis**  
打印文本并追加一个换行。

**Parameters**

* `text` – 要打印的文本（可以为空）。
* `fg` – 前景色（默认 `CON_WHITE`）。
* `bg` – 背景色（默认 `CON_BLACK`）。

**Return value**

* 对 `*this` 的引用，便于链式调用。

**In Depth**

`println()` 会打印给定文本，然后将光标移动到下一行开头。如果 `text` 为空（默认值），则仅打印一个空行。

你可以用它实现典型的“日志行”风格输出：

```cpp
console.println("Starting application...");
```

**Example**

```cpp
Console console;

console.println("Normal line");
console.println("Highlighted line", CON_BLACK, CON_YELLOW);
console.println();  // blank line
console.println("Done.");
```

*此示例打印了多行文本，包括一行高亮和一行空行。*

---

#### `Console &Console::clear()`

**Signature**

```cpp
Console &clear();
```

**Synopsis**  
清除整个控制台屏幕。

**Parameters**

* *(无)*

**Return value**

* 对 `*this` 的引用。

**In Depth**

`clear()` 会擦除所有可见内容，并通常将光标移动到左上角 `(0, 0)`。可用于：

* 为全屏样式界面开始一个全新的“帧”。
* 在渲染新布局前清除杂乱输出。

**Example**

```cpp
Console console;

console.println("This will disappear...");
console.clear();
console.println("Screen cleared.");
```

*此示例清空屏幕后打印一条新消息。*

---

#### `Console &Console::printAligned(const String &text, int width, ConsoleAlign align = CON_ALIGN_LEFT, ConsoleColor fg = CON_WHITE, ConsoleColor bg = CON_BLACK)`

**Signature**

```cpp
Console &printAligned(const String &text, int width,
                      ConsoleAlign align = CON_ALIGN_LEFT,
                      ConsoleColor fg = CON_WHITE,
                      ConsoleColor bg = CON_BLACK);
```

**Synopsis**  
在指定宽度内按对齐方式打印文本。  
如果宽度为 -1，则使用当前控制台宽度。

**Parameters**

* `text` – 要打印的文本。
* `width` – 字段宽度（字符数），或 `-1` 表示使用控制台宽度。
* `align` – 对齐模式（`CON_ALIGN_LEFT`、`CON_ALIGN_CENTER`、`CON_ALIGN_RIGHT`、`CON_ALIGN_JUSTIFY`）。
* `fg` – 前景色（默认 `CON_WHITE`）。
* `bg` – 背景色（默认 `CON_BLACK`）。

**Return value**

* 对 `*this` 的引用。

**In Depth**

`printAligned()` 同时处理**对齐**和**填充**：

* 如果 `width` 为正数，文本会在该宽度内对齐。
* 如果 `width == -1`，函数使用当前控制台宽度。
* 会按需添加填充；过长文本可能被截断或换行，具体取决于实现。

典型用途：

* 标题栏或页眉。
* 基于列的布局。
* 居中的横幅。

**Example**

```cpp
Console console;
int w = console.width();

String title("My Application");
console.printAligned(title, w, CON_ALIGN_CENTER, CON_YELLOW);
console.println();
```

*此示例在当前控制台宽度内使用亮色居中显示一个标题。*

---

#### `Console &Console::printWrapped(const String &text, int width = -1, ConsoleColor fg = CON_WHITE, ConsoleColor bg = CON_BLACK)`

**Signature**

```cpp
Console &printWrapped(const String &text, int width = -1,
                      ConsoleColor fg = CON_WHITE,
                      ConsoleColor bg = CON_BLACK);
```

**Synopsis**  
打印带自动换行的文本。  
如果宽度为 -1，则使用当前控制台宽度。

**Parameters**

* `text` – 要打印的文本；可包含已有的换行符。
* `width` – 换行宽度（字符数），或 `-1` 表示使用控制台宽度。
* `fg` – 前景色（默认 `CON_WHITE`）。
* `bg` – 背景色（默认 `CON_BLACK`）。

**Return value**

* 对 `*this` 的引用。

**In Depth**

`printWrapped()` 会：

* 在**单词边界**处换行，避免在单词中间断行。
* 保留已有的换行（例如已经分好的段落）。
* 使用 `width` 来决定每一行的换行位置。

如果 `width == -1`，函数会在打印时使用 `Console::width()` 返回的当前宽度。这在用户调整控制台大小时尤其有用：每次调用都会适配当前大小。

**Example**

```cpp
Console console;

String paragraph(
  "This is a long paragraph that will be automatically wrapped "
  "to fit the console width without breaking words in the middle."
);

console.printWrapped(paragraph);
console.println();
```

*此示例打印一段文本，并根据控制台宽度自动换行。*

---

#### `Console &Console::setCursor(int x, int y)`

**Signature**

```cpp
Console &setCursor(int x, int y);
```

**Synopsis**  
将光标移动到指定位置（0 基坐标）。

**Parameters**

* `x` – 列位置（0 = 最左侧）。
* `y` – 行位置（0 = 最上方）。

**Return value**

* 对 `*this` 的引用。

**In Depth**

`setCursor()` 会重新定位文本光标。下一次打印的字符将出现在 `(x, y)`。

你应确保：

* `0 <= x < width()`
* `0 <= y < height()`

越界坐标可能会被底层 API 忽略或钳制。

常见用途：

* 只重绘屏幕的一部分。
* 在固定位置实现状态栏或进度指示器。

**Example**

```cpp
Console console;

console.clear();
console.setCursor(0, 0).println("Top left");
console.setCursor(10, 5).println("At (10,5)");
```

*此示例将光标移动到不同位置并在对应位置打印文本。*

---

#### `Console &Console::showCursor()`

**Signature**

```cpp
Console &showCursor();
```

**Synopsis**  
显示文本光标。

**Parameters**

* *(无)*

**Return value**

* 对 `*this` 的引用。

**In Depth**

`showCursor()` 使闪烁的文本光标可见。在以下场景中适用：

* 在当前位置等待用户输入。
* 在表单或输入字段中进行导航。

**Example**

```cpp
Console console;

console.hideCursor();
console.println("Working...");
console.showCursor();
console.println("Ready for input.");
```

*此示例在“工作中”时隐藏光标，准备输入时再显示。*

---

#### `Console &Console::hideCursor()`

**Signature**

```cpp
Console &hideCursor();
```

**Synopsis**  
隐藏文本光标。

**Parameters**

* *(无)*

**Return value**

* 对 `*this` 的引用。

**In Depth**

`hideCursor()` 会移除可见光标，这在以下场景中很有用：

* 显示全屏仪表盘或动画。
* 展示光标位置无意义的菜单。

请记得在程序结束前或回到标准输入提示符前调用 `showCursor()`，以免让用户在没有光标的情况下继续使用终端。

**Example**

```cpp
Console console;

console.hideCursor();
console.println("Rendering a full-screen interface without a blinking cursor...");
```

*此示例在渲染界面时隐藏光标。*

---

### `ConsoleInput` 配置结构体

#### 概述

`ConsoleInput` 结构体用于配置 `Console::input()` 的行为。它是一个简单的纯数据容器，包含四个字段，并具有标准的值语义（构造、拷贝、赋值、析构）。

当你希望超越基本的单行提示，并启用如下特性时，就会用到 `ConsoleInput`：

* 基于候选字符串列表的 **Tab 补全**。
* 带上下箭头导航的持久化**命令历史**。
* 使用掩码字符的**密码输入**。
* 带自定义续行提示符的**多行输入**。

---

#### 参考

##### 字段

每个字段都是 public，可以直接设置。

---

###### `List ConsoleInput::completions`

**Signature**

```cpp
List completions;
```

**Synopsis**  
Tab 补全候选项（空列表表示禁用补全）。

**Meaning**

* 一个用于候选补全字符串的 `List`。
* 如果为空，则禁用 Tab 补全。

**In Depth**

当 `completions` 非空时，`Console::input()` 可以使用列表中的候选值提供 Tab 补全。典型行为：

* 按下 **Tab** 会在候选项中轮换或给出提示。
* 具体 UI（行内替换、提示等）由库的实现决定。

**Example**

```cpp
Console console;
ConsoleInput opts;

// Prepare completion candidates:
opts.completions.append("start").append("status").append("stop");

String cmd = console.input("> ", opts);
console.println("Command: " + cmd);
```

*此示例为 `start`、`status` 和 `stop` 三个命令启用了简单的 Tab 补全。*

---

###### `List ConsoleInput::history`

**Signature**

```cpp
List history;
```

**Synopsis**  
命令历史缓冲区（可读写，空列表表示不使用历史）。  
历史会在每次输入后自动更新。

**Meaning**

* 一个 `List`，用于存储之前输入的行。
* 如果为空，则不记录历史，上/下箭头不会导航到过往命令。

**In Depth**

当 `history` 非空时：

* `Console::input()` 会将该列表同时视为**输入**和**输出**：

  * 读取已有条目，以支持通过 **Up/Down** 箭头进行历史导航。
  * 每次成功输入的行会自动追加到列表中。
* 你可以在调用 `input()` 前预先填充历史：

  * 例如，加入默认命令或之前保存的历史记录。

**Example**

```cpp
Console console;
ConsoleInput opts;

console.println("Type commands; Up/Down to navigate history, blank to quit.");

while (true) {
  String line = console.input("> ", opts);
  if (line.trim().isEmpty()) {
    break;
  }
  console.println("You entered: " + line);
}

// opts.history now contains all entered lines
```

*此示例在多次输入调用间启用了持久化历史，并支持上下箭头导航。*

---

###### `bool ConsoleInput::password`

**Signature**

```cpp
bool password;
```

**Synopsis**  
使用星号掩码输入内容以进行密码输入。

**Meaning**

* 如果为 `true`，`Console::input()` 会在用户输入时对字符进行掩码（例如显示为 `*`）。
* 如果为 `false`，输入内容会正常显示。

**In Depth**

密码模式适用于：

* 身份验证提示。
* 密钥或 API Token 等秘密字段。

行为说明：

* 返回的字符串仍包含真实字符。
* 只有在控制台上的**显示**是掩码的。
* 将 `password = true` 与非空的 `history` 组合通常是不好的做法：

  * 密码会存入历史列表。
  * 如果启用 `password`，通常应该令 `history = nullptr`。

**Example**

```cpp
Console console;
ConsoleInput opts;

opts.password = true;   // mask typed characters
opts.history = nullptr; // avoid storing passwords in history

String pwd = console.input("Enter password: ", &opts);
console.println("Password length: " + String(pwd.length()));
```

*此示例读取一个带掩码的密码输入，并避免将其写入历史。*

---

###### `bool ConsoleInput::multiline`

**Signature**

```cpp
bool multiline;
```

**Synopsis**  
允许多行输入（Shift+Enter 或 Ctrl+Enter 插入换行）。

**Meaning**

* 如果为 `true`，`Console::input()` 允许用户插入换行。
* 如果为 `false`，按下 Enter 会立即结束当前输入行。

**In Depth**

在多行模式下：

* 用户可以输入多行文本，例如：

  * 配置块。
  * 脚本片段。
  * 长消息或备注。
* 特殊组合键（如 **Shift+Enter** 或 **Ctrl+Enter**）会插入换行，而不会结束输入。
* `continuation` 字符串会被用作续行的提示符。

`Console::input()` 的最终返回值是一个 `String`，其中可能包含 `'\n'` 字符。

**Example**

```cpp
Console console;
ConsoleInput opts;

opts.multiline = true;
opts.continuation = String("... ");  // prompt for continuation lines

console.println("Enter a multi-line message (finish with plain Enter on an empty line).");
String text = console.input("> ", &opts);

console.println("You entered:");
console.println(text);
```

*此示例启用多行输入，并为续行使用自定义提示符。*

---

###### `String ConsoleInput::continuation`

**Signature**

```cpp
String continuation;
```

**Synopsis**  
多行模式下用于续行的提示符。

**Meaning**

* 当 `multiline` 为 `true` 且用户继续输入下一行时，使用该文本作为提示符。
* 当 `multiline` 为 `false` 时，该字段被忽略。

**In Depth**

在使用多行输入时：

* 第一行使用传给 `Console::input()` 的 `prompt` 参数。
* 后续每一行则使用 `options->continuation`，以便在视觉上将续行与首行区分开来。

一些设计建议：

* 选择简短的续行文本，例如 `"… "` 或 `"  > "`。
* 避免使用过长或多行的提示符，以免影响换行效果。

如果你将 `continuation` 保持为默认的 `String()`，库可能会使用空提示符或内置的默认续行提示（取决于实现）。

**Example**

```cpp
Console console;
ConsoleInput opts;

opts.multiline = true;
opts.continuation = String("... ");

String text = console.input("> ", &opts);
// Additional lines the user enters will start with "... "
```

*此示例设置了自定义的多行续行提示，使后续行在视觉上更易识别。*

---

##### 成员函数

---

###### `ConsoleInput::ConsoleInput()`

**Signature**

```cpp
ConsoleInput();
```

**Synopsis**  
创建默认输入选项（无补全、无历史、单行、明文显示）。

**Parameters**

* *(无)*

**Return value**

* *(构造函数；不适用)*

**In Depth**

默认构造函数将所有字段初始化为合理的默认值：

* `completions = List()` – 无 Tab 补全。
* `history = List()` – 无命令历史。
* `password = false` – 输入为可见明文。
* `multiline = false` – 仅单行输入。
* `continuation = String()` – 空的续行提示。

你可以先构造一个 `ConsoleInput`，然后只修改自己关心的字段。

**Example**

```cpp
ConsoleInput opts;  // all defaults: plain single-line input

opts.password = true;  // enable password masking only
```

*此示例构造默认选项，然后只修改一个字段。*

---

###### `ConsoleInput::ConsoleInput(const ConsoleInput &other)`

**Signature**

```cpp
ConsoleInput(const ConsoleInput &other);
```

**Synopsis**  
基于另一个 ConsoleInput 创建拷贝。

**Parameters**

* `other` – 要拷贝的现有 `ConsoleInput`。

**Return value**

* *(构造函数；不适用)*

**In Depth**

拷贝构造函数会复制所有配置字段：

* `List` 字段（`completions`、`history`）会被深拷贝：

  * 原对象与副本各自拥有独立的元素副本。
* 值类型字段（`password`、`multiline`、`continuation`）按值复制。

当你希望以现有配置为基础并稍作修改时，可以使用此构造函数。

**Example**

```cpp
ConsoleInput base;
base.multiline = true;

ConsoleInput derived(base);
derived.password = true;  // separate configuration for another prompt
```

*此示例复制一个基础配置，然后仅在副本上调整一个标志。*

---

###### `ConsoleInput::~ConsoleInput()`

**Signature**

```cpp
~ConsoleInput();
```

**Synopsis**  
销毁 ConsoleInput。

**Parameters**

* *(无)*

**Return value**

* *(析构函数；不适用)*

**In Depth**

析构函数会释放 `ConsoleInput` 所拥有的任何内部资源（主要是 `String continuation` 字段）。它**不会**释放 `completions` 或 `history` 所指向的 `List` 实例；这些仍由你自己负责管理。

这使得 `ConsoleInput` 构造与销毁的开销较小，并且适合放在栈上使用。

**Example**

```cpp
{
  ConsoleInput opts;
  opts.password = true;
  // use opts...
}
// opts is destroyed here; continuation is cleaned up automatically
```

*此示例展示了在局部作用域中使用 `ConsoleInput`。*

---

###### `ConsoleInput &ConsoleInput::operator=(const ConsoleInput &other)`

**Signature**

```cpp
ConsoleInput &operator=(const ConsoleInput &other);
```

**Synopsis**  
将另一个 ConsoleInput 赋值给当前对象。

**Parameters**

* `other` – 要拷贝的源配置。

**Return value**

* 对 `*this` 的引用，便于链式赋值。

**In Depth**

赋值运算符会用 `other` 的内容替换当前配置：

* `List` 字段现在指向与 `other` 相同内容的副本。
* 布尔字段和 `continuation` 按值复制。

自赋值是安全的。

**Example**

```cpp
ConsoleInput a;
ConsoleInput b;

a.password = true;
a.multiline = true;

b = a;  // b now has the same configuration as a
```

*此示例通过赋值将一个 `ConsoleInput` 的配置复制到另一个。*

---
---

#### `String Console::input(const String &prompt = "", const ConsoleInput &options = ConsoleInput())`

**Signature**

```cpp
String input(const String &prompt = "",
             const ConsoleInput &options = ConsoleInput());
```

**Synopsis**  
读取一行输入，可选提示符与高级选项。  
支持上下箭头历史、带提示的 Tab 补全等。  
若输入被取消（Ctrl+C），返回空字符串。

**Parameters**

* `prompt` – 在输入区域之前显示的文本（例如 `"> "`）。
* `options` – `ConsoleInput` 配置结构体。不提供时默认为默认构造的 `ConsoleInput`。

**Return value**

* 包含用户输入行的 `String`；若输入被取消（例如通过 `Ctrl+C`），则返回空 `String`。

**In Depth**

`input()` 是一个高级输入函数，支持：

* 提示文本（例如 `"> "`）。
* 可选**历史**（当提供 `options.history` 时）：

  * 上下箭头可以导航之前的命令。
* 可选 **Tab 补全**（当提供 `options.completions` 时）：

  * 基于候选字符串列表进行建议。
* **密码模式**（当 `options.password` 为 true 时）：

  * 输入字符会被掩码（例如显示为 `'*'`）。
* **多行模式**（当 `options.multiline` 为 true 时）：

  * `Shift+Enter` 或 `Ctrl+Enter` 插入换行。
  * 使用 `ConsoleInput::continuation` 作为后续行的提示符。

当 `options` 使用默认构造时，函数采用合理默认值：

* 无历史。
* 无 Tab 补全。
* 单行明文输入。

通过检测返回值是否为空来判断输入是否被取消。

**Example**

```cpp
Console console;
ConsoleInput opts;

// Enable simple history:
List history;
opts.history = &history;

// Prompt for a command:
String line = console.input("> ", opts);

if (!line.isEmpty()) {
  console.println("You typed: " + line);
}
```

*此示例读取一行带历史支持的输入，并将其回显。*

---

#### `String Console::readEvent()`

**Signature**

```cpp
String readEvent();
```

**Synopsis**  
阻塞式读取单个按键或鼠标事件。  
返回描述性名称："A"、"Enter"、"Space"、"Ctrl+C"、"Alt+X"、  
"F1"、"MouseLeft@x,y"、"MouseRight@x,y"、"MouseMove@x,y" 等。

**Parameters**

* *(无)*

**Return value**

* 以可读格式描述事件的 `String`。

**In Depth**

`readEvent()` 提供低层级、基于事件的输入：

* 它会阻塞，直到有按键或鼠标事件发生。
* 返回的字符串描述该事件：

  * `"A"`、`"b"`、`"Enter"`、`"Space"`。
  * `"Ctrl+C"`、`"Alt+X"`、`"Shift+F1"`。
  * 鼠标事件如 `"MouseLeft@10,5"` 或 `"MouseMove@20,3"`。

适用于：

* 自定义输入处理循环。
* 交互式菜单或游戏。
* 处理点击特定屏幕区域的鼠标事件。

你可以解析返回的字符串以理解事件类型及其坐标。

**Example**

```cpp
Console console;

console.println("Press any key or click the mouse. Press Esc to quit.");

while (true) {
  String event = console.readEvent();
  console.println("Event: " + event);

  if (event == "Escape") {
    break;
  }
}
```

*此示例打印每个发生的事件（按键或鼠标），直到用户按下 Esc。*

---

#### `Console &Console::progress(float percent, const String &label = "")`

**Signature**

```cpp
Console &progress(float percent, const String &label = "");
```

**Synopsis**  
在控制台底部显示或更新动画进度条。  
`percent`: 0.0 到 1.0 表示确定进度，-1.0 表示不定式旋转指示器。  
进度条在数值变化时平滑动画，并随控制台大小变化自动调整。

**Parameters**

* `percent` – 进度值：

  * 在 `0.0` 和 `1.0` 之间表示确定性进度。
  * `-1.0` 表示不定式旋转指示器（进度未知）。
* `label` – 可选文本标签，显示在进度条旁边。

**Return value**

* 对 `*this` 的引用。

**In Depth**

`progress()` 管理位于控制台底部的一个持久化进度条：

* 它会预留一块区域，仅更新进度条区域，而不会影响主 UI。
* 当多次调用的 `percent` 增大时，进度条会平滑动画变化。
* 如果控制台被调整大小，进度条布局会适配新宽度。
* 当你不确定任务持续时间，只想显示活动指示时，可使用 `percent == -1.0f`。

你可以在任务进行过程中多次调用 `progress()` 更新进度。

**Example**

```cpp
Console console;

for (int i = 0; i <= 100; ++i) {
  float p = i / 100.0f;
  console.progress(p, "Processing...");
}
console.progressHide();
console.println("Done.");
```

*此示例模拟一个正在推进的任务，并相应更新进度条。*

---

#### `Console &Console::progressHide()`

**Signature**

```cpp
Console &progressHide();
```

**Synopsis**  
隐藏进度条并恢复该行。

**Parameters**

* *(无)*

**Return value**

* 对 `*this` 的引用。

**In Depth**

在调用过一次或多次 `progress()` 之后，可以使用 `progressHide()`：

* 将进度条从屏幕上移除。
* 恢复下方内容或释放该行供后续输出使用。

在操作完成或被取消时调用它是一个良好的实践。

**Example**

```cpp
Console console;

console.progress(-1.0f, "Working...");
// ... do some work ...
console.progressHide();
console.println("Operation finished.");
```

*此示例展示了一个不定式进度指示器，随后将其隐藏。*

---

#### `int Console::menu(const List &options, const String &title = "")`

**Signature**

```cpp
int menu(const List &options, const String &title = "");
```

**Synopsis**  
显示交互式菜单并返回所选索引（从 0 开始）。  
若被取消（Esc 键）则返回 -1。  
支持方向键、数字键（1-9）、鼠标悬停/点击以及首字母跳转。

**Parameters**

* `options` – 菜单条目的 `List`；每个元素会被转换为 `String`。
* `title` – 显示在菜单上方的可选标题。

**Return value**

* 选中项的索引（从 0 开始）；若用户取消（例如按 Esc），则返回 `-1`。

**In Depth**

`menu()` 会管理整个交互过程：

* 渲染带高亮的菜单。
* 支持多种输入方式：

  * 方向键上下移动。
  * 数字键（`1`–`9`）直接跳转到选项。
  * 鼠标悬停和点击进行选择。
  * 首字母跳转：按下字母跳到下一个以该字母开头的选项。
* 当用户选择某项或取消时返回。

你可以多次调用它，以顺序显示不同的菜单。

**Example**

```cpp
Console console;

List options;
options.append("Start").append("Settings").append("Quit");

int choice = console.menu(options, "Main Menu");

if (choice == 0) {
  console.println("Starting...");
} else if (choice == 1) {
  console.println("Opening settings...");
} else if (choice == 2) {
  console.println("Quitting...");
} else {
  console.println("Menu cancelled.");
}
```

*此示例显示一个简单的主菜单，并根据用户选择执行不同逻辑。*

---

#### `Console &Console::onHotkey(const String &key, void (*callback)(void *), void *arg = nullptr)`

**Signature**

```cpp
Console &onHotkey(const String &key, void (*callback)(void *),
                  void *arg = nullptr);
```

**Synopsis**  
注册非阻塞的全局热键回调。  
按键格式示例："Ctrl+Q"、"Alt+X"、"Shift+F1"、"F12"、"Ctrl+Shift+S"。  
当按键被触发时，回调会被异步调用。

**Parameters**

* `key` – 热键描述字符串，如 `"Ctrl+Q"` 或 `"F12"`。
* `callback` – 回调函数指针，接收一个 `void *` 参数。
* `arg` – 传递给回调的可选用户数据指针。

**Return value**

* 对 `*this` 的引用。

**In Depth**

`onHotkey()` 可注册**全局热键**，其特性包括：

* 在后台被监控。
* 当按下对应组合键时触发回调。
* 不会阻塞主线程或 UI 循环。

适用场景：

* 紧急退出键（`"Ctrl+Q"`）。
* 视图切换。
* 暂停/恢复操作。

重要注意事项：

* 回调是异步运行的；避免在其中进行耗时或阻塞操作。
* 如果在回调中修改共享数据，应保证适当的同步（例如使用库中的 `Mutex`）。

**Example**

```cpp
void QuitCallback(void *arg) {
  Console *console = (Console *)arg;
  console->println("Quit hotkey pressed.");
  // You might set a global flag here to stop the main loop.
}

Console console;

console.onHotkey("Ctrl+Q", QuitCallback, &console);
console.println("Press Ctrl+Q to trigger the hotkey (and then exit).");

// In a real program, you would now run some loop or UI logic.
```

*此示例注册了一个 Ctrl+Q 热键，当按下时会打印一条消息。*

---

#### `Console &Console::offHotkey(const String &key)`

**Signature**

```cpp
Console &offHotkey(const String &key);
```

**Synopsis**  
取消注册之前注册的热键。

**Parameters**

* `key` – 热键描述字符串，必须与 `onHotkey()` 中使用的一致。

**Return value**

* 对 `*this` 的引用。

**In Depth**

`offHotkey()` 会禁用某个热键，使其不再触发回调。你应在以下场景中使用它：

* 在销毁 `Console` 之前，如果你希望停止监听额外事件。
* 当某个使用热键的功能或模式被关闭时。

如果该按键尚未注册，调用通常是空操作。

**Example**

```cpp
Console console;

console.onHotkey("Ctrl+Q", nullptr);  // Example registration (callback omitted)
console.offHotkey("Ctrl+Q");          // Disable the hotkey again
```

*此示例展示了如何通过按键名称取消注册热键。*

---

#### `static String Console::Align(const String &text, int width, ConsoleAlign align = CON_ALIGN_LEFT)`

**Signature**

```cpp
static String Align(const String &text, int width,
                    ConsoleAlign align = CON_ALIGN_LEFT);
```

**Synopsis**  
返回按指定宽度填充并对齐的文本。  
适用于构建格式化输出或表格。

**Parameters**

* `text` – 要对齐的文本。
* `width` – 目标宽度（字符数）。
* `align` – 对齐模式（`CON_ALIGN_LEFT`、`CON_ALIGN_CENTER`、`CON_ALIGN_RIGHT`、`CON_ALIGN_JUSTIFY`）。

**Return value**

* 包含对齐和填充后文本的 `String`。

**In Depth**

`Align()` 是一个**纯格式化辅助函数**：

* 它**不会**向控制台打印任何内容。
* 会返回一行带有必要填充的文本。
* 如果文本长度超过 `width`，可能被截断（取决于实现）或保持原样；在设计宽度时应考虑这一点。

它适用于在打印或记录日志之前生成对齐的列或格式化文本。

**Example**

```cpp
Console console;

String label = Console::Align("Name", 10, CON_ALIGN_LEFT);
String value = Console::Align("Alice", 10, CON_ALIGN_RIGHT);

console.println(label + value);
// e.g., "Name          Alice"
```

*此示例构建了一行格式化文本，包含一个左对齐的标签和一个右对齐的值。*

---

#### `static String Console::Wrap(const String &text, int width)`

**Signature**

```cpp
static String Wrap(const String &text, int width);
```

**Synopsis**  
返回按指定宽度自动换行的文本。  
保留已有换行符，并在单词边界处进行换行。

**Parameters**

* `text` – 要换行的原始段落文本。
* `width` – 最大行宽（字符数）。

**Return value**

* 一个 `String`，其中插入了换行符以保证每行都不超过 `width`。

**In Depth**

`Wrap()` 接收一段较长的字符串并返回其版本：

* 在单词之间插入换行，以使行宽保持在 `width` 以内。
* 保留已有的换行，因此可以对多段文本进行换行。

由于它返回一个 `String`，你可以：

* 直接用 `println()` 或 `printWrapped()` 打印。
* 再通过 `lines()` 或 `split()` 进一步拆分处理。

**Example**

```cpp
Console console;

String text(
  "This is some long text that we want to wrap to a fixed width "
  "before printing it."
);

String wrapped = Console::Wrap(text, 40);
console.println(wrapped);
```

*此示例使用 `Console::Wrap()` 先将一段文本按固定宽度换行，然后再打印出来。*