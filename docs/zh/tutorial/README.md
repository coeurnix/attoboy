# 教程

---

## 欢迎你，程序员

编写 *“一跑就对”* 的软件有一种独特的快乐——程序一闪而过就编译完成，生成的可执行文件小到可以随意分享，而且不要求用户安装任何额外东西。这正是 attoboy 邀请你进入的世界。

也许你已经写过 Python 脚本或 JavaScript 应用。你可能听说过 C++ 很强大却也很吓人——充满内存管理陷阱、晦涩语法，以及需要高等学位才能读懂的库。我们要告诉你的是：事情不必那样。

Attoboy 是一个秉持激进理念设计的 C++ 库：*如果 C++ 用起来能像 Python 一样舒适，同时生成的可执行文件还比大多数图片都小，会怎样？* 如果你永远不用考虑内存分配、指针运算，或是几十种不兼容的文本表示方式，会怎样？如果一切都只是……“好用就完事了”呢？

本教程将带你了解 attoboy 的各种能力。读完之后，你就能写出真正有用的程序——命令行工具、文件工具、Web 客户端，甚至是 AI 驱动的应用——全部使用干净、可读的 C++，并编译成通常只有 10 到 100 KB 的可执行文件。

你无需任何 C++ 经验。只要你知道什么是变量、函数和循环，就已经准备好了。开始吧。

---

## 第一个 Attoboy 程序

每个 attoboy 程序都有同样的基本结构：

```cpp
#include <attoboy/attoboy.h>

using namespace attoboy;

void atto_main() {
  Log("Hello, world!");
  Exit();
}
```

来看一下这里发生了什么。第一行包含了 attoboy 库——只需这一个头文件，你就能访问我们将要讨论的所有内容。第二行 `using namespace attoboy;` 让我们可以写 `String` 而不是 `attoboy::String`，让代码更加简洁易读。

函数 `atto_main()` 是程序的起点。和传统从 `main` 函数开始的 C++ 程序不同，attoboy 程序从这里开始。暂时不用纠结为什么——只要记住这是你的入口点，是程序故事开始的地方。

在函数体内，我们使用 `Log()` 打印一条消息。这是 attoboy 向控制台输出文本的方式（如果你做了相关配置，也可以输出到文件）。最后，`Exit()` 告诉程序我们已经完成。每个 attoboy 程序都应该以一次对 `Exit()` 的调用结束。

就是这样。这已经是一个完整、可运行的程序。编译后，它会生成一个只有几 KB 的可执行文件，可以在任何 Windows 机器上运行，而且不需要任何额外文件或安装步骤。

---

## String：你的常驻搭档

在 attoboy 中，如果说有哪个类型会被你反复使用，那就是 `String`。文本在编程中无处不在——文件名、用户输入、消息、数据格式、网络通信——而 attoboy 的 `String` 旨在让处理文本变成一种享受。

### 创建字符串

你可以用很多方式创建字符串：

```cpp
void atto_main() {
  String greeting = "Hello";
  String number = 42;       // "42"
  String pi = 3.14159f;     // "3.14159"
  String truth = true;      // "true"
  
  Log(greeting);
  Log(number);
  Log(pi);
  Log(truth);
  
  Exit();
}
```

注意这里很美妙的一点：attoboy 的 `String` 会自动把数字和布尔值转换为文本。无需格式化函数，也不用操心类型转换。你只管写出你的意图。

### 组合字符串

字符串可以像你预期的那样，通过 `+` 运算符合并在一起：

```cpp
void atto_main() {
  String first = "Good";
  String second = "morning";
  String message = first + " " + second + "!";
  
  Log(message);  // "Good morning!"
  Exit();
}
```

但 attoboy 提供了更加方便的方式。你可以一次性用多个片段创建一个字符串：

```cpp
void atto_main() {
  String name = "Alice";
  int age = 30;
  
  String bio = String("Name: ", name, ", Age: ", age);
  Log(bio);  // "Name: Alice, Age: 30"
  
  Exit();
}
```

这个可变参数构造函数可以接收任意数量的值——字符串、数字、布尔值——并无缝组合它们。

### 探索字符串内容

字符串对自身有很多认知：

```cpp
void atto_main() {
  String text = "Hello, 世界!";
  
  Log("Length: ", text.length());    // 9 个字符
  Log("Bytes: ", text.byteLength());   // 14 字节 (UTF-8!)
  Log("Empty? ", text.isEmpty());    // false
  
  Exit();
}
```

`length()` 和 `byteLength()` 之间的区别很重要。Attoboy 全程使用 UTF-8 编码，这意味着中文汉字或 emoji 等字符可能占用多个字节。`length()` 方法统计的是字符数（程序员称之为“码点”），而 `byteLength()` 统计的是原始字节数。多数情况下，你会更常用 `length()`。

### 搜索与测试

字符串可以回答自身内容相关的问题：

```cpp
void atto_main() {
  String sentence = "The quick brown fox jumps over the lazy dog.";
  
  if (sentence.contains("fox")) {
    Log("Found a fox!");
  }
  
  if (sentence.startsWith("The")) {
    Log("Starts with 'The'");
  }
  
  if (sentence.endsWith(".")) {
    Log("It's a complete sentence.");
  }
  
  int pos = sentence.getPositionOf("brown");
  Log("'brown' appears at position ", pos);  // 10
  
  int foxCount = sentence.count("o");
  Log("The letter 'o' appears ", foxCount, " times");  // 4
  
  Exit();
}
```

### 提取片段

你可以从字符串中取出某些部分：

```cpp
void atto_main() {
  String text = "Hello, World!";
  
  String first = text.at(0);       // "H"
  String last = text.at(-1);       // "!"  (负索引从末尾开始计数)
  String middle = text.substring(0, 5); // "Hello"
  String end = text.substring(7);     // "World!"
  
  Log(first, " ... ", last);
  Log(middle);
  Log(end);
  
  Exit();
}
```

负索引是一份小礼物——`at(-1)` 返回最后一个字符，`at(-2)` 返回倒数第二个，以此类推。这种模式在 attoboy 的很多地方都存在，如果你用过 Python，会感觉非常自然。

### 变换字符串

在 attoboy 中，字符串是 *不可变的*——一旦创建就不能被修改。像 `upper()` 和 `replace()` 这样的方法会返回新的字符串：

```cpp
void atto_main() {
  String original = "  Hello, World!  ";
  
  String trimmed = original.trim();       // "Hello, World!"
  String shouted = trimmed.upper();       // "HELLO, WORLD!"
  String whispered = trimmed.lower();     // "hello, world!"
  String replaced = trimmed.replace("World", "Universe");  // "Hello, Universe!"
  String reversed = trimmed.reverse();    // "!dlroW ,olleH"
  String repeated = "ha".repeat(3);       // "hahaha"
  
  Log(shouted);
  Log(replaced);
  Log(repeated);
  
  Exit();
}
```

不可变性乍看起来像限制，但其实是一种福音。你永远不用担心某个函数“悄悄改掉了你的字符串”——它做不到。原始值总是安全的。这种模式消除了大量在其他风格程序中常见的 bug。

### 拆分与合并

你经常需要把字符串拆成片段，或者把片段合起来：

```cpp
void atto_main() {
  String csv = "apple,banana,cherry,date";
  List fruits = csv.split(",");
  
  // fruits 现在为 ["apple", "banana", "cherry", "date"]
  
  String reunited = " - ".join(fruits);
  Log(reunited);  // "apple - banana - cherry - date"
  
  // 以空白字符拆分
  String sentence = "The quick brown fox";
  List words = sentence.split();
  Log("Word count: ", words.length());  // 4
  
  // 按行拆分
  String poem = "Roses are red\nViolets are blue";
  List lines = poem.lines();
  Log("Line count: ", lines.length());  // 2
  
  Exit();
}
```

### 模板格式化

对于更复杂的字符串组装，attoboy 提供模板格式化功能：

```cpp
void atto_main() {
  // 位置占位符
  String template1 = "Hello, {0}! You are {1} years old.";
  String result1 = template1.format(List("Alice", 30));
  Log(result1);  // "Hello, Alice! You are 30 years old."
  
  // 命名占位符
  String template2 = "Dear {name}, your order #{order} is ready.";
  String result2 = template2.format(Map("name", "Bob", "order", 12345));
  Log(result2);  // "Dear Bob, your order #12345 is ready."
  
  Exit();
}
```

### 与数字之间的转换

看起来像数字的字符串可以变成真正的数字：

```cpp
void atto_main() {
  String numStr = "42";
  String floatStr = "3.14159";
  String boolStr = "yes";
  
  int num = numStr.toInteger();    // 42
  float pi = floatStr.toFloat();   // 3.14159
  bool flag = boolStr.toBool();    // true
  
  // 转换前先检查
  String maybeNumber = "hello";
  if (maybeNumber.isNumber()) {
    Log("It's a number!");
  } else {
    Log("Not a number");  // 就是这个分支
  }
  
  Exit();
}
```

`toBool()` 方法非常宽容——它可以把 "true"、"yes"、"1" 和 "on"（不区分大小写）识别为 true，其余都认为是 false。这在解析配置文件或用户输入时非常实用。

---

## 收集数据：List、Map 和 Set

单个数据很有用，但程序通常处理的是 *集合*——一组相关的条目。Attoboy 提供三种功能强大的集合类型，每种都有不同用途。

### List：有序集合

`List` 是一个有序值序列。与很多语言中的数组不同，attoboy 的 `List` 可以保存不同类型的值：

```cpp
void atto_main() {
  // 创建带初始值的列表
  List groceries("milk", "bread", "eggs", "butter");
  
  Log("Shopping list has ", groceries.length(), " items");
  
  // 通过索引访问
  String first = groceries.at(0);   // "milk"
  String last = groceries.at(-1);   // "butter"
  Log("First: ", first, ", Last: ", last);
  
  // 添加元素
  groceries.append("cheese");
  groceries.prepend("coffee");
  
  // 可以混合不同类型！
  List mixed(42, "hello", true, 3.14f);
  
  Exit();
}
```

注意 `groceries.at(0)` 这种写法。如果你愿意，也可以使用 `[]` 运算符：

```cpp
void atto_main() {
  List groceries("milk", "bread", "eggs", "butter");
  Log("First: ", groceries[0], ", Last: ", groceries[-1]);
  Exit();
}
```

List 支持方法链——大多数操作都会返回列表本身，因此你可以写出流畅的代码：

```cpp
void atto_main() {
  List numbers;
  numbers.append(3).append(1).append(4).append(1).append(5).append(9);
  
  // 就地排序
  numbers.sort();  // 现在为: 1, 1, 3, 4, 5, 9
  
  // 就地反转
  numbers.reverse();  // 现在为: 9, 5, 4, 3, 1, 1
  
  // 检查内容
  if (numbers.contains(5)) {
    Log("Found 5!");
  }
  
  int pos = numbers.find(4);
  Log("4 is at position ", pos);
  
  Exit();
}
```

你可以从列表中提取部分内容：

```cpp
void atto_main() {
  List letters("a", "b", "c", "d", "e");
  
  List middle = letters.slice(1, 4);  // ["b", "c", "d"]
  List fromEnd = letters.slice(-2, letters.length());  // ["d", "e"]
  
  Exit();
}
```

### Map：键值对

`Map` 用键关联值。它非常适合结构化数据、配置，或者任何需要按名称查找内容的场景：

```cpp
void atto_main() {
  // 用键值对创建 Map
  Map person("name", "Alice", "age", 30, "city", "Seattle");
  
  // 读取值
  String name = person.get("name", "Unknown");
  int age = person.get("age", 0);
  
  Log(name, " is ", age, " years old");
  
  // 检查键是否存在
  if (person.hasKey("email")) {
    Log("Has email");
  } else {
    Log("No email on record");
  }
  
  // 添加或更新
  person.put("email", "alice@example.com");
  person.put("age", 31);  // 生日快乐！
  
  Exit();
}
```

你可以检查 map 的内容：

```cpp
void atto_main() {
  Map config("debug", true, "timeout", 30, "name", "MyApp");
  
  List allKeys = config.keys();
  List allValues = config.values();
  
  Log("Configuration has ", config.length(), " settings");
  
  Exit();
}
```

### Set：唯一值集合

`Set` 是一个集合，其中每个值最多出现一次。重复元素会被悄悄忽略：

```cpp
void atto_main() {
  Set colors("red", "green", "blue", "red", "green");  // 只有 3 个唯一值
  
  Log("Unique colors: ", colors.length());  // 3
  
  if (colors.contains("purple")) {
    Log("Has purple");
  } else {
    Log("No purple");
  }
  
  colors.put("purple");  // 现在我们有它了
  colors.remove("red");  // 已移除
  
  Exit();
}
```

Set 支持集合运算：

```cpp
void atto_main() {
  Set a(1, 2, 3, 4);
  Set b(3, 4, 5, 6);
  
  Set both = a.duplicate();
  both.intersect(b);  // 3, 4 (在两者中都存在)
  
  Set either = a.duplicate();
  either.setUnion(b);  // 1, 2, 3, 4, 5, 6 (在任一集合中)
  
  Set onlyA = a.duplicate();
  onlyA.subtract(b);  // 1, 2 (在 a 中但不在 b 中)
  
  Exit();
}
```

### JSON 与 CSV

这些集合类型可以自然地与常见数据格式结合使用：

```cpp
void atto_main() {
  // 创建数据
  Map user("name", "Bob", "age", 25);
  
  // 转换为 JSON
  String json = user.toJSONString();
  Log(json);  // {"name":"Bob","age":25}
  
  // 解析 JSON
  Map parsed = Map::FromJSONString(json);
  Log("Name from JSON: ", parsed.get("name", ""));
  
  // 用 CSV 表示表格数据
  List table;
  table.append(List("Name", "Age", "City"));
  table.append(List("Alice", 30, "Seattle"));
  table.append(List("Bob", 25, "Portland"));
  
  String csv = table.toCSVString();
  Log(csv);
  
  Exit();
}
```

---

## 操作文件

不能与文件系统交互的程序，其能力是非常有限的。Attoboy 通过两个协同工作的类型让文件操作变得简单：用于文件元数据和导航的 `Path`，以及用于读写内容的 `File`。

### Path：在文件系统中行走

`Path` 表示文件系统中的一个位置。它可以告诉你那里有什么，并帮助你操作文件：

```cpp
void atto_main() {
  Path docs = Path::GetDocumentsDirectory();
  Log("Your documents are at: ", docs.toString());
  
  Path home = Path::GetHomeDirectory();
  Path desktop = Path(home.toString() + "\\Desktop");
  
  if (desktop.exists()) {
    Log("Desktop exists");
    if (desktop.isDirectory()) {
      Log("It's a directory (as expected)");
    }
  }
  
  Exit();
}
```

Path 可以揭示关于文件的详细信息：

```cpp
void atto_main() {
  Path file("C:\\Windows\\notepad.exe");
  
  if (file.exists() && file.isRegularFile()) {
    Log("File: ", file.getName());
    Log("Extension: ", file.getExtension());
    Log("Size: ", file.getSize(), " bytes");
    Log("Modified: ", file.getModifiedOn().toString());
    Log("Parent: ", file.getParentDirectory().toString());
  }
  
  Exit();
}
```

### 读写文件

在简单场景下，`Path` 自身就提供了便捷方法：

```cpp
void atto_main() {
  Path configPath("settings.txt");
  
  // 将字符串写入文件
  configPath.writeFromString("theme=dark\nfontSize=14\n");
  
  // 读回内容
  String contents = configPath.readToString();
  Log("Config contents:\n", contents);
  
  // 追加更多内容
  configPath.appendFromString("language=en\n");
  
  Exit();
}
```

### 列出目录内容

你可以查看某个目录中有什么：

```cpp
void atto_main() {
  Path current = Path::GetWorkingDirectory();
  
  List children = current.listChildren();
  Log("Files in current directory:");
  
  for (int i = 0; i < children.length(); i++) {
    String name = children.at(i);
    Path child(current.toString() + "\\" + name);
    
    if (child.isDirectory()) {
      Log("  [DIR]  ", name);
    } else {
      Log("  [FILE] ", name, " (", child.getSize(), " bytes)");
    }
  }
  
  Exit();
}
```

### 创建目录与移动文件

```cpp
void atto_main() {
  Path newDir("my_project");
  
  // 创建目录（必要时也会创建父目录）
  if (newDir.makeDirectory()) {
    Log("Created ", newDir.toString());
  }
  
  // 在其中创建一个文件
  Path readme(newDir.toString() + "\\README.txt");
  readme.writeFromString("# My Project\n\nWelcome!");
  
  // 复制文件
  Path backup(newDir.toString() + "\\README.backup.txt");
  readme.copyTo(backup);
  
  // 移动/重命名
  Path newName(newDir.toString() + "\\README.md");
  readme.moveTo(newName);
  
  Exit();
}
```

### 临时文件

当你需要临时存储空间时：

```cpp
void atto_main() {
  Path tempFile = Path::CreateTemporaryFile("myapp_", ".tmp");
  Log("Temporary file: ", tempFile.toString());
  
  tempFile.writeFromString("Temporary data here");
  
  // 完成后别忘了清理
  tempFile.deleteFile();
  
  Exit();
}
```

### File 类：流式 I/O

对于需要更多控制的读写操作——特别是大文件或网络连接——请使用 `File` 类：

```cpp
void atto_main() {
  Path logPath("application.log");
  
  File logFile(logPath);
  if (logFile.isValid()) {
    logFile.write("Application started\n");
    logFile.write("Performing initialization...\n");
    logFile.flush();  // 确保数据写入磁盘
  }
  
  // 读回内容
  logFile.setPosition(0);  // 回到文件开头
  String contents = logFile.readAllToString();
  Log("Log contents: ", contents);
  
  logFile.close();
  
  Exit();
}
```

---

## 命令行参数

大多数有用的程序都会从命令行接收输入。Attoboy 的 `Arguments` 类让参数解析变得轻而易举：

```cpp
void atto_main() {
  Arguments args;
  
  // 定义可以接受的参数
  args.addFlag("v", "Enable verbose output", false, "verbose");
  args.addFlag("h", "Show help", false, "help");
  args.addParameter("o", "Output file path", "output.txt", "output");
  args.addPositionalParameter("input", "Input file to process");
  
  // 解析
  Map parsed = args.parseArguments();
  
  if (parsed.isEmpty()) {
    Exit(1);  // 用户请求帮助或缺少必需参数
  }
  
  bool verbose = parsed.get("v", "false").toBool();
  String outputPath = parsed.get("o", "");
  String inputPath = parsed.get("input", "");
  
  if (verbose) {
    Log("Processing ", inputPath, " -> ", outputPath);
  }
  
  // ... 在这里做真正的工作 ...
  
  Exit();
}
```

有了这套配置，你的程序会自动处理：

* `myprogram.exe -h` 或 `myprogram.exe --help`（显示帮助）
* `myprogram.exe -v --output=result.txt input.txt`
* `myprogram.exe input.txt -o output.txt --verbose`

参数可以以任意顺序出现，开关和带值参数既可以使用短形式（`-v`），也可以使用长形式（`--verbose`）。

---

## 与时间打交道

`DateTime` 类以毫秒精度处理时间戳：

```cpp
void atto_main() {
  // 当前时间
  DateTime now;
  Log("Current time: ", now.toString());
  
  // 由 ISO 字符串创建
  DateTime birthday("2000-01-15T08:30:00.000Z");
  
  // 差值（毫秒）
  long long ageMs = now.diff(birthday);
  long long ageYears = ageMs / (1000LL * 60 * 60 * 24 * 365);
  Log("Approximate age: ", ageYears, " years");
  
  // 时间相加
  DateTime tomorrow = DateTime(now);
  tomorrow.add(24 * 60 * 60 * 1000);  // 增加 24 小时（毫秒）
  Log("Tomorrow: ", tomorrow.toString());
  
  Exit();
}
```

---

## 二进制数据与 Buffer

`String` 用于文本，而 `Buffer` 用于处理原始二进制数据——文件内容、加密数据、网络报文等等：

```cpp
void atto_main() {
  // 从字符串创建
  Buffer data("Hello, binary world!");
  Log("Buffer size: ", data.length(), " bytes");
  
  // 转换回字符串
  String text = data.toString();
  Log("As string: ", text);
  
  Exit();
}
```

### 压缩

Buffer 可以使用极其快速的 LZ4 算法进行压缩：

```cpp
void atto_main() {
  // 创建一些可压缩性很好的重复数据
  String original = String("This is some text that compresses well. ").repeat(100);
  Buffer uncompressed(original);
  
  Log("Original size: ", uncompressed.length(), " bytes");
  
  Buffer compressed = uncompressed.compress();
  Log("Compressed size: ", compressed.length(), " bytes");
  
  Buffer restored = compressed.decompress();
  Log("Restored size: ", restored.length(), " bytes");
  Log("Match: ", (restored == uncompressed));
  
  Exit();
}
```

### 加密

Buffer 支持 ChaCha20 加密算法，这是一种现代、安全的密码算法：

```cpp
void atto_main() {
  String secret = "This is my secret message!";
  Buffer plaintext(secret);
  
  // key 至少需要 32 字节，nonce 至少需要 12 字节
  String key = "my-32-byte-encryption-key-here!!";
  String nonce = "unique-nonce";
  
  // 加密
  Buffer ciphertext = plaintext.crypt(key, nonce);
  Log("Encrypted (base64): ", ciphertext.toBase64());
  
  // 解密（相同的操作与同一 key/nonce）
  Buffer decrypted = ciphertext.crypt(key, nonce);
  Log("Decrypted: ", decrypted.toString());
  
  Exit();
}
```

### Base64 编码

用于以文本形式传输二进制数据：

```cpp
void atto_main() {
  Buffer binary(String("Hello!"));
  String encoded = binary.toBase64();
  Log("Base64: ", encoded);  // "SGVsbG8h"
  
  Buffer decoded = Buffer::fromBase64(encoded);
  Log("Decoded: ", decoded.toString());  // "Hello!"
  
  Exit();
}
```

---

## 触达网络：Web 请求

Attoboy 让 HTTP 请求变得简单：

```cpp
void atto_main() {
  // 简单的 GET 请求
  WebRequest request("https://api.github.com/users/octocat");
  WebResponse response = request.doGet();
  
  if (response.succeeded()) {
    Map userData = response.asJson();
    String name = userData.get("name", "Unknown");
    String bio = userData.get("bio", "No bio");
    
    Log("Name: ", name);
    Log("Bio: ", bio);
  } else {
    Log("Request failed: ", response.getStatusCode(), " ", response.getStatusReason());
  }
  
  Exit();
}
```

### POST 请求

向服务器发送数据：

```cpp
void atto_main() {
  Map payload("title", "My Post", "body", "This is the content", "userId", 1);
  
  WebRequest request("https://jsonplaceholder.typicode.com/posts");
  WebResponse response = request.doPost(payload);
  
  if (response.succeeded()) {
    Log("Created! Response: ", response.asString());
  }
  
  Exit();
}
```

### 下载文件

```cpp
void atto_main() {
  bool success = WebRequest::Download(
    "https://example.com/file.zip",
    "downloaded.zip"
  );
  
  if (success) {
    Log("Download complete!");
  } else {
    Log("Download failed");
  }
  
  Exit();
}
```

---

## 与 AI 对话

Attoboy 最强大的特性之一，是通过任意兼容 OpenAI 的 API 内置支持 AI 语言模型：

```cpp
void atto_main() {
  // 连接到一个 AI 服务
  AI ai(
    "https://api.openai.com/v1",       // 基础 URL
    GetEnv("OPENAI_API_KEY"),        // 来自环境变量的 API key
    "gpt-5-mini"                 // 模型名称
  );
  
  // 设置 system prompt（可选）
  ai.setSystemPrompt("You are a helpful assistant who gives concise answers.");
  
  // 提问
  String answer = ai.ask("What is the capital of France?");
  
  if (!answer.isEmpty()) {
    Log("AI says: ", answer);
  } else {
    Log("Error getting response");
  }
  
  Exit();
}
```

### 多轮对话

用于来回交流的对话场景：

```cpp
void atto_main() {
  AI ai("https://api.openai.com/v1", GetEnv("OPENAI_API_KEY"), "gpt-5-mini");
  Conversation chat = ai.createConversation();
  
  Log(chat.ask("Hi! I'm learning about astronomy."));
  Log(chat.ask("What's the closest star to Earth?"));
  Log(chat.ask("How long would it take to travel there?"));
  
  // AI 会记住这段对话的上下文
  
  Exit();
}
```

### 用于语义搜索的 Embedding

Embedding 会把文本转换为数值向量，捕捉其语义，从而实现语义相似度比较：

```cpp
void atto_main() {
  AI ai("https://api.openai.com/v1", GetEnv("OPENAI_API_KEY"), "text-embedding-3-small");
  
  Embedding dogEmbed = ai.createEmbedding("dog");
  Embedding puppyEmbed = ai.createEmbedding("puppy");
  Embedding carEmbed = ai.createEmbedding("car");
  
  float dogPuppy = dogEmbed.compare(puppyEmbed);  // 高相似度
  float dogCar = dogEmbed.compare(carEmbed);    // 低相似度
  
  Log("dog vs puppy similarity: ", dogPuppy);
  Log("dog vs car similarity: ", dogCar);
  
  Exit();
}
```

---

## 构建控制台界面

对于交互式命令行应用，attoboy 提供功能丰富的 `Console` 类：

```cpp
void atto_main() {
  Console con;
  
  // 彩色输出
  con.println("Welcome!", CON_CYAN);
  con.print("Status: ", CON_WHITE);
  con.println("OK", CON_GREEN);
  
  // 格式化输出
  con.printWrapped("This is a long paragraph that will automatically "
    "wrap to fit the console width, making it easy to display "
    "large amounts of text neatly.", -1, CON_GRAY);
  
  Exit();
}
```

### 交互式输入

`Console` 提供了强大的输入功能：

```cpp
void atto_main() {
  Console con;
  
  // 简单输入
  String name = con.input("Enter your name: ");
  con.println(String("Hello, ", name, "!"), CON_YELLOW);
  
  // 密码输入（用星号遮挡）
  ConsoleInput passOptions;
  passOptions.password = true;
  String password = con.input("Password: ", passOptions);
  
  // 带历史记录和 Tab 补全的输入
  ConsoleInput shellOptions;
  shellOptions.completions = List("help", "exit", "status", "run", "config");
  
  String command = con.input("> ", shellOptions);
  
  Exit();
}
```

### 进度条

用于长时间运行的操作：

```cpp
void atto_main() {
  Console con;
  
  con.println("Processing files...");
  
  for (int i = 0; i <= 100; i++) {
    con.progress(i / 100.0f, String("Progress: ", i, "%"));
    Sleep(50);  // 模拟工作
  }
  
  con.progressHide();
  con.println("Done!", CON_GREEN);
  
  Exit();
}
```

### 交互式菜单

```cpp
void atto_main() {
  Console con;
  
  List options("Start new game", "Load saved game", "Options", "Exit");
  int choice = con.menu(options, "Main Menu");
  
  if (choice == 0) {
    con.println("Starting new game...");
  } else if (choice == 3 || choice == -1) {  // 若用户按下 Escape，则为 -1
    con.println("Goodbye!");
  }
  
  Exit();
}
```

---

## 运行其他程序

有时你需要启动外部程序。`Subprocess` 类可以帮助你做到这一点：

```cpp
void atto_main() {
  // 运行并等待结束
  int exitCode = Subprocess::Run(Path("C:\\Windows\\notepad.exe"));
  Log("Notepad exited with code ", exitCode);
  
  // 捕获输出
  String output = Subprocess::Capture(Path("C:\\Windows\\System32\\ipconfig.exe"), "/all");
  Log("IP Configuration:\n", output);
  
  // 启动后不等待（fire and forget）
  Subprocess::Start(Path("C:\\Windows\\explorer.exe"), "C:\\");
  
  Exit();
}
```

用于与子进程进行交互式通信：

```cpp
void atto_main() {
  Subprocess proc(Path("C:\\Windows\\System32\\cmd.exe"));
  
  // 发送命令
  proc.write("echo Hello from attoboy!\r\n");
  proc.write("exit\r\n");
  
  // 等待并读取输出
  proc.wait();
  String output = proc.readToString();
  Log("Output:\n", output);
  
  Exit();
}
```

---

## 线程与并行工作

当你需要同时做多件事时：

```cpp
void *backgroundTask(void *arg) {
  String name = *((String *)arg);
  Log("Background task started: ", name);
  Sleep(2000);  // 模拟工作
  Log("Background task finished: ", name);
  return nullptr;
}

void atto_main() {
  String taskName = "DataProcessor";
  Thread worker(backgroundTask, &taskName);
  
  Log("Main thread continues...");
  Sleep(1000);
  Log("Main thread waiting for worker...");
  
  worker.await();  // 等待完成
  Log("All done!");
  
  Exit();
}
```

用于保护共享数据：

```cpp
int sharedCounter = 0;
Mutex counterLock;

void *incrementer(void *arg) {
  for (int i = 0; i < 1000; i++) {
    counterLock.lock();
    sharedCounter++;
    counterLock.unlock();
  }
  return nullptr;
}

void atto_main() {
  Thread t1(incrementer, nullptr);
  Thread t2(incrementer, nullptr);
  
  t1.await();
  t2.await();
  
  Log("Final counter value: ", sharedCounter);  // 安全地得到 2000
  
  Exit();
}
```

---

## 数学运算

`Math` 类提供所有常见的数学函数：

```cpp
void atto_main() {
  // 常量
  Log("Pi: ", Math::PI);
  Log("E: ", Math::E);
  
  // 随机数
  Log("Random int: ", Math::Random());
  Log("Random float [0,1): ", Math::RandomFloat());
  Log("Random in range [1,10): ", Math::RandomRange(1, 10));
  Log("Random bool: ", Math::RandomBool());
  
  // 常见运算
  Log("Abs(-5): ", Math::Abs(-5));
  Log("Min(3,7): ", Math::Min(3, 7));
  Log("Max(3,7): ", Math::Max(3, 7));
  Log("Clamp(15, 0, 10): ", Math::Clamp(15, 0, 10));  // 10
  
  // 三角函数（弧度）
  float angle = Math::PI / 4;  // 45 度
  Log("Sin(45°): ", Math::Sin(angle));
  Log("Cos(45°): ", Math::Cos(angle));
  
  // 幂与平方根
  Log("Sqrt(16): ", Math::Sqrt(16.0f));
  Log("Pow(2, 10): ", Math::Pow(2.0f, 10.0f));
  
  // 取整
  Log("Floor(3.7): ", Math::Floor(3.7f));  // 3
  Log("Ceil(3.2): ", Math::Ceil(3.2f));    // 4
  Log("Round(3.5): ", Math::Round(3.5f));  // 4
  
  Exit();
}
```

---

## 日志

在整个教程中我们都在使用 `Log()`，但 attoboy 提供了更完善的日志系统：

```cpp
void atto_main() {
  // 不同日志级别
  LogDebug("This is a debug message");
  LogInfo("This is an info message");
  LogWarning("This is a warning");
  LogError("This is an error");
  
  // 将日志输出到文件而不是控制台
  EnableLoggingToFile("app.log");
  
  LogInfo("This goes to the file");
  LogInfo("So does this");
  
  // 再切回控制台
  EnableLoggingToConsole();
  LogInfo("Back to console");
  
  Exit();
}
```

日志级别在编译期被控制以保证高效。默认情况下，`LogInfo`、`LogWarning` 和 `LogError` 是启用的，而 `LogDebug` 需要在包含头文件之前定义 `ATTOBOY_LOG_DEBUG_ENABLE`。

---

## 环境与系统信息

```cpp
void atto_main() {
  // 环境变量
  String path = GetEnv("PATH");
  Log("PATH has ", path.length(), " characters");
  
  SetEnv("MY_APP_CONFIG", "production");
  
  // 用户信息
  Log("User: ", GetUserName());
  Log("Display name: ", GetUserDisplayName());
  
  // 进程 ID
  Log("Process ID: ", GetProcessId());
  
  // 暂停
  Log("Waiting 2 seconds...");
  Sleep(2000);
  Log("Done waiting!");
  
  Exit();
}
```

---

## Windows 注册表

用于 Windows 特有的配置存储：

```cpp
void atto_main() {
  Registry myAppSettings("HKEY_CURRENT_USER\\Software\\MyApp");
  
  // 如有必要则创建
  if (!myAppSettings.exists()) {
    myAppSettings.create();
  }
  
  // 存储值
  myAppSettings.setStringValue("lastUser", "alice");
  myAppSettings.setIntegerValue("windowWidth", 1024);
  myAppSettings.setIntegerValue("windowHeight", 768);
  
  // 再读回来
  String user = myAppSettings.getStringValue("lastUser");
  int width = myAppSettings.getIntegerValue("windowWidth");
  
  Log("Last user was ", user, " with window ", width, " pixels wide");
  
  Exit();
}
```

---

## 完整示例：迷你笔记应用

让我们把若干概念组合起来，写一个小而实用的应用程序：

```cpp
#include <attoboy/attoboy.h>

using namespace attoboy;

Path getNotesFile() {
  Path appData = Path::GetLocalAppDirectory();
  Path notesDir(appData.toString() + "\\MiniNotes");
  notesDir.makeDirectory();
  return Path(notesDir.toString() + "\\notes.json");
}

List loadNotes() {
  Path notesFile = getNotesFile();
  if (notesFile.exists()) {
    String json = notesFile.readToString();
    return List::FromJSONString(json);
  }
  return List();
}

void saveNotes(List notes) {
  Path notesFile = getNotesFile();
  String json = notes.toJSONString();
  notesFile.writeFromString(json);
}

void atto_main() {
  Console con;
  List notes = loadNotes();

  con.println("Mini Notes", CON_CYAN);
  con.println(String("-").repeat(40), CON_DARK_GRAY);

  ConsoleInput inputOptions;
  inputOptions.completions = List("add", "list", "delete", "quit");
  inputOptions.history = List();

  bool running = true;
  while (running) {
    String cmd = con.input("> ", inputOptions);

    if (cmd == "quit" || cmd.isEmpty()) {
      running = false;
    } else if (cmd == "add") {
      String note = con.input("Note: ");
      if (!note.isEmpty()) {
        Map entry("text", note, "created", DateTime().toString());
        notes.append(entry);
        saveNotes(notes);
        con.println("Note added!", CON_GREEN);
      }
    } else if (cmd == "list") {
      if (notes.isEmpty()) {
        con.println("No notes yet.", CON_DARK_GRAY);
      } else {
        for (int i = 0; i < notes.length(); i++) {
          Map note = notes.at(i);
          String text = note.get("text", "");
          con.println(String(i + 1, ". ", text));
        }
      }
    } else if (cmd.startsWith("delete ")) {
      String numStr = cmd.substring(7);
      int idx = numStr.toInteger() - 1;
      if (idx >= 0 && idx < notes.length()) {
        notes.remove(idx);
        saveNotes(notes);
        con.println("Note deleted.", CON_YELLOW);
      } else {
        con.println("Invalid note number.", CON_RED);
      }
    } else {
      con.println("Commands: add, list, delete <num>, quit", CON_DARK_GRAY);
    }
  }

  con.println("Goodbye!", CON_CYAN);
  Exit();
}
```

这个小程序展示了文件 I/O、JSON 持久化、带 Tab 补全的控制台交互、彩色输出以及一个简单的命令循环。编译后，它会生成大约 35 KB 的可执行文件，能持久化存储笔记，并在任何 Windows 系统上运行。

---

## 结束语

现在你已经见识了 attoboy 的广度：从基础字符串处理到文件 I/O，从 Web 请求到 AI 集成，从控制台界面到多线程。这个库遵循一个简单的理念：编写 C++ 应该既自然又安全，而生成的程序应该足够小巧，能够轻松分享。

在继续探索时，记住几条对你大有裨益的原则：

**拥抱不可变性。** 当一个 `String` 方法返回新字符串而不是修改原值，这是优点。你的数据保持可预测。

**信任 RAII 模式。** 当你创建 `File`、`Console` 或 `Thread` 对象时，它们在离开作用域时会自动清理自身。你不需要记住关闭资源或释放内存——attoboy 会处理好这一切。

**善用方法链。** 许多 attoboy 方法会返回对象自身，让你可以写出类似 `list.append(1).append(2).sort().reverse()` 这样的表达式式代码。

**检查结果。** 像 `isEmpty()`、`isValid()` 和 `succeeded()` 这样的函数，可以帮助你优雅地处理错误情况。

**从简单开始。** 你的第一个程序不必用上线程、AI 或网络请求。先从字符串和文件着手，然后随着需求增长再扩展。

你用 attoboy 创建的这些微小可执行文件，都是完整、自包含的程序。无需安装器、无需运行时依赖、没有兼容性烦恼。只是小巧、快速、能力不俗的软件，你可以把它们分享给任何人。

欢迎来到 attoboy 的世界。我们相信你会在这里玩得很开心。