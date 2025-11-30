# Path

## 概览

`attoboy::Path` 类表示一个**不可变的文件系统路径**，并提供丰富的辅助功能用于：

* 检查路径所指向的对象（文件、目录、符号链接等）。
* 查询大小、时间戳和属性等元数据。
* 执行文件系统操作（创建目录、移动/复制/删除文件）。
* 通过便捷函数读写文件。
* 发现系统位置（用户主目录、文档目录、临时文件目录等）。

它为小型 Windows 工具而设计，同时屏蔽了大部分底层细节。`Path` 本质上是一个**值对象**，内部持有一个路径字符串，并围绕它提供高层操作。路径字符串一旦在构造 `Path` 时确定，就不会再改变；像 `moveTo()` 这样的操作改变的是文件系统，而不是该 `Path` 实例本身。

### 文件系统概念

如果你对文件系统术语不太熟悉，下面是 `Path` 所涉及的一些核心概念：

* **常规文件（regular file）**：普通数据文件（例如 `config.txt`）。
* **目录（directory）**：可以包含文件和其他目录的文件夹。
* **命名管道（named pipe）**：一种用于进程间通信的特殊类文件对象。
* **符号链接（symbolic link / symlink）**：指向另一路径的引用。访问该符号链接通常会作用于其目标。
* **文件属性（file attributes）**：例如“只读”或“隐藏”等标志位。
* **当前工作目录（current working directory）**：用于解释相对路径的基准目录。
* **卷（volume）**：Windows 上的驱动器，例如 `C:` 或 `D:`。

`Path` 类允许你在保持代码简洁可读的同时，对这些实体进行导航、检查和操作。

### 不可变性和值语义

在其文本表示层面，`Path` 是**不可变的**：

* 一旦创建 `Path`，其内部的字符串就不会改变。
* 像 `moveTo()` 或 `deleteFile()` 这样的操作作用于该路径所指向的文件系统条目，但不会改变 `Path` 对象本身。
* 拷贝和赋值 `Path` 实例是轻量且安全的；它们都表示同一个底层路径值。

这使得在整个应用程序中传递路径变得简单，不必担心存储的路径字符串被意外修改。

---

## 参考

下面的每一条目都介绍了 `attoboy::Path` 的一个公共构造函数、成员函数、静态函数或运算符。

对于每个成员，你将看到：

* **Signature（函数签名）** – 头文件中的精确声明。
* **Synopsis（简介）** – 原始的一行 Doxygen 注释。
* **Parameters（参数）** 和 **Return value（返回值）**。
* **In Depth（详解）** – 目的、行为、注意事项以及一个简短示例。

所有示例都假定：

```cpp
using namespace attoboy;
```

---

#### `Path(const String &pathStr)`

**Signature**

```cpp
Path(const String &pathStr);
```

**Synopsis**  
Creates a path from a string.

**Parameters**

* `pathStr` – 路径的文本表示（绝对或相对），通常为 Windows 风格路径，如 `"C:\\temp\\file.txt"`。

**Return value**

* *（构造函数；不适用）*

**In Depth**

这是构造 `Path` 的主要方式。字符串会被原样存储；`Path` 不会自动对其进行规范化或标准化。相同的文本路径在操作系统解释时会始终指向同一文件系统条目。

你可以传递一个显式的 `String` 或字符串字面量，得益于 `String` 的构造函数：

```cpp
Path p("C:\\temp\\log.txt");  // 隐式构造 String
```

路径可以是：

* **绝对路径**，包含驱动器号（如 `"C:\\users\\alice\\file.txt"`）。
* **相对路径**，相对于当前工作目录进行解释（如 `"logs\\today.txt"`）。

**Example**

```cpp
Path configPath("C:\\myapp\\config.json");
bool exists = configPath.exists();
```

*此示例从字面量创建一个路径，并检查该文件是否存在。*

---

#### `Path(const Path &other)`

**Signature**

```cpp
Path(const Path &other);
```

**Synopsis**  
Creates a copy of another path.

**Parameters**

* `other` – 要拷贝的已有 `Path`。

**Return value**

* *（构造函数；不适用）*

**In Depth**

拷贝构造函数创建一个新的 `Path`，其内容和行为与 `other` 完全相同。由于在字符串表示上 `Path` 是不可变的，因此在代码的不同部分共享和使用拷贝都是安全的。

**Example**

```cpp
Path original("C:\\data\\input.txt");
Path copy(original);

bool same = copy.equals(original);  // true
```

*此示例拷贝一个路径，并确认两个实例表示同一位置。*

---

#### `~Path()`

**Signature**

```cpp
~Path();
```

**Synopsis**  
Destroys the path and frees memory.

**Parameters**

* *（无）*

**Return value**

* *（析构函数；不适用）*

**In Depth**

当一个 `Path` 变量离开其作用域时，其析构函数将释放任何内部资源（例如为底层字符串分配的堆内存）。不会自动发生任何文件系统更改；仅仅是你的代码不再引用该路径。

**Example**

```cpp
{
  Path temp("C:\\temp\\file.tmp");
  // 在此处使用 temp...
}
// temp 在这里被销毁；不会自动删除文件。
```

*此示例展示了 `Path` 的生命周期结束，并不会影响实际文件。*

---

#### `Path &operator=(const Path &other)`

**Signature**

```cpp
Path &operator=(const Path &other);
```

**Synopsis**  
Assigns another path to this path.

**Parameters**

* `other` – 其值将赋给当前实例的 `Path`。

**Return value**

* 对 `*this` 的引用，以便链式赋值。

**In Depth**

赋值运算符将当前路径值替换为 `other` 的路径值。赋值后，这两个 `Path` 对象表示相同的文本路径，后续操作的行为也完全一致。

自赋值是安全的。

**Example**

```cpp
Path p("C:\\old\\file.txt");
Path q("C:\\new\\file.txt");

p = q;  // p 现在引用 "C:\\new\\file.txt"
```

*此示例将一个 `Path` 变量重新赋值为指向新位置。*

---

#### `bool exists() const`

**Signature**

```cpp
bool exists() const;
```

**Synopsis**  
Returns true if the path exists on the filesystem.

**Parameters**

* *（无）*

**Return value**

* 如果路径当前指向某个存在的文件系统条目，则为 `true`；否则为 `false`。

**In Depth**

`exists()` 会检查该路径是否指向任何被文件系统识别的对象：常规文件、目录、符号链接、命名管道或其他特殊文件。

在调用 `getSize()`、`readToString()` 或 `listChildren()` 等操作之前，它是一个很好的起点。

**Example**

```cpp
Path logPath("C:\\logs\\today.log");
if (!logPath.exists()) {
  logPath.writeFromString("Log created.\n");
}
```

*此示例仅在日志文件尚不存在时创建一个新的日志文件。*

---

#### `bool isRegularFile() const`

**Signature**

```cpp
bool isRegularFile() const;
```

**Synopsis**  
Returns true if the path is a regular file.

**Parameters**

* *（无）*

**Return value**

* 如果路径存在并指向一个常规文件，则为 `true`；否则为 `false`。

**In Depth**

此函数将普通数据文件（例如文本文件、二进制文件）与目录或特殊类型区分开来。如果路径不存在，则返回 `false`。

在读取或写入文件内容前，可以使用它确保你处理的是一个文件。

**Example**

```cpp
Path candidate("C:\\data\\input.txt");
if (candidate.isRegularFile()) {
  String contents = candidate.readToString();
}
```

*此示例仅在路径是常规文件时才读取文件。*

---

#### `bool isDirectory() const`

**Signature**

```cpp
bool isDirectory() const;
```

**Synopsis**  
Returns true if the path is a directory.

**Parameters**

* *（无）*

**Return value**

* 如果路径存在且为目录，则为 `true`；否则为 `false`。

**In Depth**

`isDirectory()` 检查路径是否指向一个目录（文件夹）。可以使用 `listChildren()` 列出目录内容，也可以通过 `makeDirectory()` 创建目录。

**Example**

```cpp
Path folder("C:\\data");
if (!folder.isDirectory()) {
  folder.makeDirectory(true);
}
```

*此示例在不存在为目录时递归创建该目录。*

---

#### `bool isNamedPipe() const`

**Signature**

```cpp
bool isNamedPipe() const;
```

**Synopsis**  
Returns true if the path is a named pipe.

**Parameters**

* *（无）*

**Return value**

* 如果路径存在且被识别为命名管道，则为 `true`；否则为 `false`。

**In Depth**

命名管道用于进程间通信。在某些路径既可能是文件也可能是管道的场景下，此检查非常有用。

**Example**

```cpp
Path p("\\\\.\\pipe\\myapp");
bool pipeExists = p.isNamedPipe();
```

*此示例检查某个特定命名管道是否存在。*

---

#### `bool isSymbolicLink() const`

**Signature**

```cpp
bool isSymbolicLink() const;
```

**Synopsis**  
Returns true if the path is a symbolic link.

**Parameters**

* *（无）*

**Return value**

* 如果路径存在且为符号链接，则为 `true`；否则为 `false`。

**In Depth**

符号链接（symlink）是指向其他路径的条目，它们可以指向文件或目录。使用 `getSymbolicLinkTarget()` 可以获取其目标路径。

**Example**

```cpp
Path p("C:\\shortcut.lnk");
if (p.isSymbolicLink()) {
  Path target = p.getSymbolicLinkTarget();
}
```

*此示例检测一个符号链接，然后获取其目标路径。*

---

#### `bool isOther() const`

**Signature**

```cpp
bool isOther() const;
```

**Synopsis**  
Returns true if the path is a device or other special file.

**Parameters**

* *（无）*

**Return value**

* 如果路径指向的文件系统对象既不是常规文件、目录、命名管道也不是符号链接，则为 `true`；否则为 `false`。

**In Depth**

此函数用于处理剩余类型的文件系统对象，如设备或其他与平台相关的特殊条目。对于大多数应用级代码，你主要会使用 `isRegularFile()` 和 `isDirectory()`。

**Example**

```cpp
Path p("C:\\Device\\SomeSpecialObject");
bool special = p.isOther();
```

*此示例检查一个路径是否指向特殊文件系统对象。*

---

#### `long long getSize() const`

**Signature**

```cpp
long long getSize() const;
```

**Synopsis**  
Returns the file size in bytes (0 if unavailable).

**Parameters**

* *（无）*

**Return value**

* 如果可用，则返回字节大小；若无法确定大小（例如路径不存在或不是常规文件），则返回 `0`。

**In Depth**

`getSize()` 主要对常规文件有意义。对于目录或特殊文件，其大小可能没有意义，或被报告为 `0`。

为避免歧义，在需要文件大小时请先调用 `isRegularFile()`。

**Example**

```cpp
Path filePath("C:\\data\\report.pdf");
if (filePath.isRegularFile()) {
  long long bytes = filePath.getSize();
}
```

*此示例在存在时获取一个常规文件的大小。*

---

#### `DateTime getCreatedOn() const`

**Signature**

```cpp
DateTime getCreatedOn() const;
```

**Synopsis**  
Returns the creation time.

**Parameters**

* *（无）*

**Return value**

* 表示文件系统对象创建时间的 `DateTime`。

**In Depth**

返回的 `DateTime` 反映文件系统对于创建时间的记录。如果路径不存在或无法获取元数据，结果将体现实现定义的回退值；最佳做法是在此之前调用 `exists()`。

**Example**

```cpp
Path p("C:\\data\\file.txt");
if (p.exists()) {
  DateTime created = p.getCreatedOn();
  String iso = created.toString();
}
```

*此示例获取并格式化文件的创建时间。*

---

#### `DateTime getModifiedOn() const`

**Signature**

```cpp
DateTime getModifiedOn() const;
```

**Synopsis**  
Returns the last modification time.

**Parameters**

* *（无）*

**Return value**

* 表示文件或目录内容上次被修改时间的 `DateTime`。

**In Depth**

该时间戳通常在文件数据或目录条目发生变化时更新。与其他元数据一样，建议先调用 `exists()`，以避免依赖未定义结果。

**Example**

```cpp
Path p("C:\\data\\log.txt");
if (p.exists()) {
  DateTime lastModified = p.getModifiedOn();
}
```

*此示例获取文件的最后修改时间。*

---

#### `DateTime getAccessedOn() const`

**Signature**

```cpp
DateTime getAccessedOn() const;
```

**Synopsis**  
Returns the last access time.

**Parameters**

* *（无）*

**Return value**

* 表示文件或目录上次被访问时间的 `DateTime`。

**In Depth**

此时间戳可以反映文件上次被打开或读取的时间，尽管某些系统或设置可能为了性能而禁用或延迟访问时间的更新。

**Example**

```cpp
Path p("C:\\data\\file.txt");
if (p.exists()) {
  DateTime lastAccessed = p.getAccessedOn();
}
```

*此示例在可用时读取文件的最后访问时间。*

---

#### `bool isReadOnly() const`

**Signature**

```cpp
bool isReadOnly() const;
```

**Synopsis**  
Returns true if the file is read-only.

**Parameters**

* *（无）*

**Return value**

* 如果文件或目录设置了只读属性，则为 `true`；否则为 `false`。

**In Depth**

只读属性是 Windows 上众多文件属性之一。当 `isReadOnly()` 返回 `true` 时，尝试修改或删除该文件可能失败，除非先清除此属性或使用足够的权限。

对于不存在的路径，此函数通常返回 `false`。

**Example**

```cpp
Path p("C:\\data\\config.ini");
if (p.isReadOnly()) {
  // 在写入前决定如何处理这个情况。
}
```

*此示例检查配置文件是否被标记为只读。*

---

#### `bool setReadOnly(bool isReadOnly = true) const`

**Signature**

```cpp
bool setReadOnly(bool isReadOnly = true) const;
```

**Synopsis**  
Sets the read-only attribute. Returns true on success.

**Parameters**

* `isReadOnly` – 若为 `true`，则启用只读属性；若为 `false`，则清除此属性。

**Return value**

* 若属性成功更改则返回 `true`；若操作失败（例如文件不存在或权限不足），则返回 `false`。

**In Depth**

此函数会修改该路径所引用的文件系统条目的只读属性。`Path` 对象本身保持不变。

在需要保护文件避免被意外修改，或在写入前需临时清除只读属性时，可以使用此函数。

**Example**

```cpp
Path p("C:\\data\\config.ini");

// 使配置文件可写。
if (p.isReadOnly()) {
  p.setReadOnly(false);
}
```

*此示例在需要时通过清除只读属性来确保文件可写。*

---

#### `bool isHidden() const`

**Signature**

```cpp
bool isHidden() const;
```

**Synopsis**  
Returns true if the file is hidden.

**Parameters**

* *（无）*

**Return value**

* 如果文件系统条目具有隐藏属性，则为 `true`；否则为 `false`。

**In Depth**

隐藏文件和目录通常不会在文件浏览器中默认显示。此函数可用于检测这类条目。

对于不存在的路径，结果通常为 `false`。

**Example**

```cpp
Path p("C:\\data\\secret.txt");
bool hidden = p.isHidden();
```

*此示例检查一个文件是否被标记为隐藏。*

---

#### `bool setHidden(bool isHidden = true) const`

**Signature**

```cpp
bool setHidden(bool isHidden = true) const;
```

**Synopsis**  
Sets the hidden attribute. Returns true on success.

**Parameters**

* `isHidden` – 若为 `true`，则设置隐藏属性；若为 `false`，则清除此属性。

**Return value**

* 如果属性成功更改，则返回 `true`；否则返回 `false`。

**In Depth**

此函数会更改实际文件系统条目的隐藏属性。在希望将辅助文件、缓存目录或内部数据从普通目录列表中隐藏时十分有用。

**Example**

```cpp
Path cacheDir("C:\\myapp\\cache");
cacheDir.makeDirectory(true);
cacheDir.setHidden(true);
```

*此示例创建一个缓存目录并将其标记为隐藏。*

---

#### `bool moveTo(const Path &dest) const`

**Signature**

```cpp
bool moveTo(const Path &dest) const;
```

**Synopsis**  
Moves this path to the destination. Returns true on success.

**Parameters**

* `dest` – 目标路径，文件或目录应被移动或重命名到此处。

**Return value**

* 若底层移动/重命名操作成功则为 `true`；否则为 `false`。

**In Depth**

`moveTo()` 向操作系统请求将当前 `Path` 引用的文件系统条目移动或重命名到 `dest`。常见失败原因包括：

* 源路径不存在。
* 目标无法创建（例如缺少父目录）。
* 权限不足。
* 目标已存在（具体行为取决于操作系统）。

`Path` 对象本身不会变化；移动成功后，该 `Path` 仍然指向旧位置，而该位置通常已不再存在。

**Example**

```cpp
Path oldPath("C:\\data\\old\\file.txt");
Path newPath("C:\\data\\new\\file.txt");

if (oldPath.moveTo(newPath)) {
  // 文件现在位于 newPath。
}
```

*此示例将文件从一个目录移动到另一个目录。*

---

#### `bool copyTo(const Path &dest) const`

**Signature**

```cpp
bool copyTo(const Path &dest) const;
```

**Synopsis**  
Copies this path to the destination. Returns true on success.

**Parameters**

* `dest` – 目标路径，应将该文件或目录复制到此处。

**Return value**

* 若复制成功则为 `true`；否则为 `false`。

**In Depth**

`copyTo()` 会将源路径内容复制到目标路径：

* 对文件而言，复制其文件数据。
* 对目录而言，行为可能取决于具体实现（例如递归复制目录，或者失败）。

与 `moveTo()` 一样，`Path` 实例本身保持不变；操作仅影响文件系统。

**Example**

```cpp
Path source("C:\\data\\report.txt");
Path backup("C:\\backup\\report.txt");

bool ok = source.copyTo(backup);
```

*此示例创建一个文件的备份副本。*

---

#### `bool deleteFile() const`

**Signature**

```cpp
bool deleteFile() const;
```

**Synopsis**  
Deletes the file. Returns true on success.

**Parameters**

* *（无）*

**Return value**

* 若文件成功删除则为 `true`；否则为 `false`。

**In Depth**

`deleteFile()` 试图删除该路径上的常规文件。如果路径指向目录或特殊对象，操作通常会失败。常见失败原因包括：

* 文件不存在。
* 文件被其他进程打开。
* 权限不足。

如果需要区分“文件不存在”和其他错误，可先调用 `exists()` 或 `isRegularFile()`。

**Example**

```cpp
Path tempFile("C:\\temp\\session.tmp");
if (tempFile.exists()) {
  tempFile.deleteFile();
}
```

*此示例在临时文件存在时将其删除。*

---

#### `bool removeDirectory(bool deleteIfNotEmpty = false) const`

**Signature**

```cpp
bool removeDirectory(bool deleteIfNotEmpty = false) const;
```

**Synopsis**  
Removes the directory. Returns true on success.

**Parameters**

* `deleteIfNotEmpty` – 若为 `false`，通常只有在目录为空时删除才会成功。若为 `true`，实现可以在需要时删除其内容。

**Return value**

* 若目录被删除则为 `true`；否则为 `false`。

**In Depth**

此函数会删除该路径上的目录。当 `deleteIfNotEmpty` 设为 `false`（默认值）时，通常只有在目录为空时删除才会成功。若设为 `true`，实现可视具体情况递归删除目录内容。

同样地，即使目录被删除，`Path` 对象仍表示原来的文本路径。

**Example**

```cpp
Path tempDir("C:\\temp\\myapp");
tempDir.removeDirectory(true);  // 尝试删除目录及其内容
```

*此示例尝试删除临时目录及其所有内容。*

---

#### `bool makeDirectory(bool createParents = true) const`

**Signature**

```cpp
bool makeDirectory(bool createParents = true) const;
```

**Synopsis**  
Creates the directory. Returns true on success.

**Parameters**

* `createParents` – 若为 `true`，则在需要时创建父目录（类似 `mkdir -p`）。若为 `false`，通常要求父目录必须已存在。

**Return value**

* 若目录创建成功或已存在且为目录，则为 `true`；否则为 `false`。

**In Depth**

使用 `makeDirectory()` 来确保该路径处存在目录。通常在程序启动时用于创建配置、日志或缓存目录。

对于诸如 `"C:\\data\\myapp\\logs"` 这样的嵌套目录，`createParents = true` 非常方便。

**Example**

```cpp
Path logsDir("C:\\myapp\\logs");
bool ok = logsDir.makeDirectory(true);
```

*此示例创建一个嵌套的日志目录，并创建所有缺失的父目录。*

---

#### `Path getSymbolicLinkTarget() const`

**Signature**

```cpp
Path getSymbolicLinkTarget() const;
```

**Synopsis**  
Returns the symbolic link target, or empty path if not a link.

**Parameters**

* *（无）*

**Return value**

* 表示符号链接目标的 `Path`；如果当前路径不是符号链接或无法确定目标，则返回“空”的 `Path`。

**In Depth**

如果 `isSymbolicLink()` 返回 `true`，此函数将返回链接所指向的路径。如果该路径不是符号链接（或解析失败），返回的 `Path` 被视为空（其字符串形式为空字符串）。

**Example**

```cpp
Path link("C:\\data\\shortcut.lnk");
Path target = link.getSymbolicLinkTarget();

if (!target.toString().isEmpty()) {
  // target 指向符号链接的目标
}
```

*此示例在可能的情况下，将符号链接解析为其目标路径。*

---

#### `bool setSymbolicLinkTarget(const Path &target) const`

**Signature**

```cpp
bool setSymbolicLinkTarget(const Path &target) const;
```

**Synopsis**  
Creates or updates a symbolic link to the target. Returns true on success.

**Parameters**

* `target` – 此符号链接应引用的目标路径。

**Return value**

* 若符号链接创建或更新成功则为 `true`；否则为 `false`。

**In Depth**

此函数会将当前 `Path` 配置为指向 `target` 的符号链接。根据操作系统和配置，创建符号链接可能需要提升权限。

如果该路径上已经存在符号链接，实现可以将其更新为指向新的目标。

**Example**

```cpp
Path linkPath("C:\\data\\shortcut.lnk");
Path targetPath("C:\\data\\realfile.txt");

bool ok = linkPath.setSymbolicLinkTarget(targetPath);
```

*此示例创建或更新一个符号链接，使其指向某个实际文件。*

---

#### `String getName() const`

**Signature**

```cpp
String getName() const;
```

**Synopsis**  
Returns the filename (last path component).

**Parameters**

* *（无）*

**Return value**

* 包含路径最后一部分的 `String`（例如 `"file.txt"` 或 `"folder"`）。

**In Depth**

`getName()` 返回不含任何父目录的路径基本名。例如：

* `"C:\\folder\\file.txt"` → `"file.txt"`
* `"C:\\folder\\"` → `"folder"`（具体行为取决于实现和尾部分隔符）

当你只需要文件名或目录名用于展示或日志记录时，可使用 `getName()`。

**Example**

```cpp
Path p("C:\\data\\report.txt");
String name = p.getName();  // "report.txt"
```

*此示例从较长路径中提取文件名。*

---

#### `Path getParentDirectory() const`

**Signature**

```cpp
Path getParentDirectory() const;
```

**Synopsis**  
Returns the parent directory path.

**Parameters**

* *（无）*

**Return value**

* 表示包含当前路径的目录的 `Path`。

**In Depth**

`getParentDirectory()` 会移除路径的最后一部分，从而得到其父目录。对于根路径（例如 `"C:\\"`）的具体行为由实现决定；通常根目录的父目录仍为其自身。

**Example**

```cpp
Path p("C:\\data\\report.txt");
Path parent = p.getParentDirectory();  // "C:\\data"
```

*此示例获取文件路径的父目录。*

---

#### `String getExtension() const`

**Signature**

```cpp
String getExtension() const;
```

**Synopsis**  
Returns the file extension without dot (e.g., "txt").

**Parameters**

* *（无）*

**Return value**

* 包含最后一部分扩展名（不含前导点）的 `String`。若没有扩展名，则返回空字符串。

**In Depth**

`getExtension()` 检查路径最后一部分，并找到最后一个点之后的部分。例如：

* `"report.txt"` → `"txt"`
* `"archive.tar.gz"` → `"gz"`
* `"README"` → `""`（无扩展名）

**Example**

```cpp
Path p("C:\\data\\report.txt");
String ext = p.getExtension();  // "txt"
```

*此示例获取文件的扩展名。*

---

#### `String getStem() const`

**Signature**

```cpp
String getStem() const;
```

**Synopsis**  
Returns the filename without extension.

**Parameters**

* *（无）*

**Return value**

* 包含最后一部分且已移除扩展名的 `String`。

**In Depth**

`getStem()` 是 `getExtension()` 的对应函数。以同样的示例为例：

* `"report.txt"` → `"report"`
* `"archive.tar.gz"` → `"archive.tar"`
* `"README"` → `"README"`

这在生成派生文件名（例如从 `report.txt` 生成 `report.bak`）时非常有用。

**Example**

```cpp
Path p("C:\\data\\report.txt");
String stem = p.getStem();  // "report"
```

*此示例获取不带扩展名的文件基本名。*

---

#### `bool hasExtension(const String &ext) const`

**Signature**

```cpp
bool hasExtension(const String &ext) const;
```

**Synopsis**  
Returns true if the file has the given extension (case-insensitive).

**Parameters**

* `ext` – 要检查的扩展名，通常不带点（例如 `"txt"`）。

**Return value**

* 若最后一部分的扩展名与 `ext` 在不区分大小写时匹配，则为 `true`；否则为 `false`。

**In Depth**

此辅助函数简化了扩展名检查。比较是不区分大小写的，所以 `"TXT"` 和 `"txt"` 被视为相同。

**Example**

```cpp
Path p("C:\\data\\report.TXT");
bool isTxt = p.hasExtension("txt");  // true
```

*此示例以大小写不敏感的方式测试文件扩展名。*

---

#### `String toString() const`

**Signature**

```cpp
String toString() const;
```

**Synopsis**  
Returns the path as a string.

**Parameters**

* *（无）*

**Return value**

* 包含路径文本表示的 `String`。

**In Depth**

`toString()` 暴露底层路径字符串，你可以用它进行日志记录、展示，或使用 `String` 操作进行处理。其格式通常是 Windows 风格路径。

**Example**

```cpp
Path p("C:\\data\\file.txt");
String text = p.toString();  // "C:\\data\\file.txt"
```

*此示例将 `Path` 转换为 `String` 以便显示或记录。*

---

#### `bool equals(const Path &other) const`

**Signature**

```cpp
bool equals(const Path &other) const;
```

**Synopsis**  
Returns true if this path equals the other (case-insensitive).

**Parameters**

* `other` – 要比较的路径。

**Return value**

* 若两个路径被认为相等，则为 `true`；否则为 `false`。

**In Depth**

`equals()` 以适合 Windows 文件系统的大小写不敏感方式比较路径。通常会比较两者规范化后的文本表示，不过具体规范化规则由实现决定。

当你希望显式比较路径而不使用运算符时，可以使用此函数。

**Example**

```cpp
Path a("C:\\DATA\\file.txt");
Path b("c:\\data\\file.txt");

bool same = a.equals(b);  // 在大小写不敏感比较下为 true
```

*此示例比较两个仅大小写不同的路径。*

---

#### `bool operator==(const Path &other) const`

**Signature**

```cpp
bool operator==(const Path &other) const;
```

**Synopsis**  
Returns true if this path equals the other (case-insensitive).

**Parameters**

* `other` – 要比较的路径。

**Return value**

* 若两个路径相等，则为 `true`；否则为 `false`。

**In Depth**

该运算符是 `equals(other)` 的惯用简写形式。它允许使用如下表达式：

```cpp
if (a == b) { ... }
```

由于比较是大小写不敏感的，因此 `"C:\\DATA\\file.txt"` 和 `"c:\\data\\FILE.txt"` 会被视为相等。

**Example**

```cpp
Path a("C:\\data\\file.txt");
Path b("C:\\DATA\\FILE.TXT");

if (a == b) {
  // 在忽略大小写时，它们引用相同路径。
}
```

*此示例使用相等运算符比较两个路径。*

---

#### `bool operator!=(const Path &other) const`

**Signature**

```cpp
bool operator!=(const Path &other) const;
```

**Synopsis**  
Returns true if this path does not equal the other.

**Parameters**

* `other` – 要比较的路径。

**Return value**

* 若路径不同则为 `true`；若它们被认为相等则为 `false`。

**In Depth**

该运算符只是 `operator==` 的逻辑取反。当你想在不相等时进行分支时可以使用它。

**Example**

```cpp
Path a("C:\\data\\file.txt");
Path b("C:\\data\\other.txt");

if (a != b) {
  // 这两个路径不同。
}
```

*此示例在两个路径指向不同位置时进行分支。*

---

#### `bool isWithin(const Path &dir) const`

**Signature**

```cpp
bool isWithin(const Path &dir) const;
```

**Synopsis**  
Returns true if this path is within the given directory.

**Parameters**

* `dir` – 要测试包含关系的目录路径。

**Return value**

* 若当前路径在逻辑上位于 `dir` 内部（例如其后代），则为 `true`；否则为 `false`。

**In Depth**

`isWithin()` 对于强制某个文件必须位于特定根目录（如应用数据目录）下非常有用。路径包含关系通常基于路径层级，而非简单的字符串前缀比较（例如 `"C:\\root\\file.txt"` 在 `"C:\\root"` 内，但 `"C:\\rootdir\\file.txt"` 不在其内）。

对 `..` 片段以及尾部斜杠等的规范化规则由实现决定，因此将其视为一种高层次安全检查。

**Example**

```cpp
Path root("C:\\myapp\\data");
Path file("C:\\myapp\\data\\logs\\today.log");

bool inside = file.isWithin(root);  // true
```

*此示例验证文件路径是否在指定的数据目录之内。*

---

#### `List listChildren(bool recursive = false) const`

**Signature**

```cpp
List listChildren(bool recursive = false) const;
```

**Synopsis**  
Returns a list of child path strings.

**Parameters**

* `recursive` – 若为 `false`，则仅返回直接子项。若为 `true`，则递归返回所有后代。

**Return value**

* `List`，其中每个元素为 `String`，表示子项的路径。

**In Depth**

若当前路径为目录，`listChildren()` 会列出其中包含的文件和目录：

* 当 `recursive` 为 `false` 时，仅列出直属条目。
* 当 `recursive` 为 `true` 时，还会包含子目录中的条目，通常使用其完整路径表示。

返回结果中的每个元素都是包含路径的 `String`，而不是 `Path` 对象。需要时你可以将其包装为 `Path`。

如果路径不是目录或不存在，结果通常是空列表。

**Example**

```cpp
Path dir("C:\\myapp\\logs");
List entries = dir.listChildren(false);

int count = entries.length();
// entries[i] 包含每个子项的路径（String）
```

*此示例列出日志目录的直接子项。*

---

#### `String readToString() const`

**Signature**

```cpp
String readToString() const;
```

**Synopsis**  
Reads the entire file as a string.

**Parameters**

* *（无）*

**Return value**

* 包含文件内容的 UTF-8 文本的 `String`。若读取失败，通常返回空字符串。

**In Depth**

`readToString()` 会打开该路径处的文件，读取所有字节，并将它们转换为 `String`。它非常适合配置文件、JSON、小日志或任何文本内容。

对于非常大的文件，此函数会将整个内容读入内存；这种情况下应考虑直接使用 `File` 进行流式处理。

你应先检查 `isRegularFile()` 或 `exists()` 以避免得到出乎意料的结果。

**Example**

```cpp
Path config("C:\\myapp\\config.json");
if (config.isRegularFile()) {
  String json = config.readToString();
}
```

*此示例将配置文件读取到 `String` 中。*

---

#### `Buffer readToBuffer() const`

**Signature**

```cpp
Buffer readToBuffer() const;
```

**Synopsis**  
Reads the entire file as a buffer.

**Parameters**

* *（无）*

**Return value**

* 包含文件原始字节的 `Buffer`。若读取失败，通常返回空缓冲区。

**In Depth**

`readToBuffer()` 类似 `readToString()`，但返回 `Buffer`，更适合处理图像、归档或加密内容等二进制数据。字节不会被解释为文本。

**Example**

```cpp
Path binaryFile("C:\\data\\image.bin");
Buffer data = binaryFile.readToBuffer();
```

*此示例将二进制文件的所有字节读取到 `Buffer` 中。*

---

#### `bool writeFromString(const String &str) const`

**Signature**

```cpp
bool writeFromString(const String &str) const;
```

**Synopsis**  
Writes a string to the file. Returns true on success.

**Parameters**

* `str` – 要写入的文本内容。

**Return value**

* 若文件写入成功则为 `true`；否则为 `false`。

**In Depth**

`writeFromString()` 会以 UTF-8 编码将 `str` 中的字节写入该路径处的文件并覆盖原内容。典型行为包括：

* 若文件不存在，则在可能的情况下创建文件。
* 截断已有内容。

目录问题和权限问题都可能导致写入失败，此时函数返回 `false`。

**Example**

```cpp
Path log("C:\\myapp\\log.txt");
bool ok = log.writeFromString("Application started.\n");
```

*此示例向文件写入一条日志消息，并替换任何已有内容。*

---

#### `bool writeFromBuffer(const Buffer &buf) const`

**Signature**

```cpp
bool writeFromBuffer(const Buffer &buf) const;
```

**Synopsis**  
Writes a buffer to the file. Returns true on success.

**Parameters**

* `buf` – 含有待写入字节的 `Buffer`。

**Return value**

* 成功则为 `true`；失败则为 `false`。

**In Depth**

`writeFromBuffer()` 将原始字节写入文件。适用于不需要文本解释的二进制数据。

与 `writeFromString()` 一样，该操作可能创建或截断文件。

**Example**

```cpp
Buffer data;
// data.append(...);  // 填充缓冲区

Path out("C:\\data\\output.bin");
bool ok = out.writeFromBuffer(data);
```

*此示例将缓冲区中的二进制数据写入文件。*

---

#### `bool appendFromString(const String &str) const`

**Signature**

```cpp
bool appendFromString(const String &str) const;
```

**Synopsis**  
Appends a string to the file. Returns true on success.

**Parameters**

* `str` – 要追加到现有文件内容后的文本。

**Return value**

* 若追加操作成功则为 `true`；否则为 `false`。

**In Depth**

`appendFromString()` 以追加模式打开文件，并将 `str` 的 UTF-8 字节追加到文件末尾。如果文件不存在，实现可能会创建该文件（具体行为取决于底层操作系统和库设计）。

这非常适合日志文件和增量文本输出。

**Example**

```cpp
Path log("C:\\myapp\\log.txt");
log.appendFromString("Next event...\n");
```

*此示例向已有日志文件追加一行文本。*

---

#### `bool appendFromBuffer(const Buffer &buf) const`

**Signature**

```cpp
bool appendFromBuffer(const Buffer &buf) const;
```

**Synopsis**  
Appends a buffer to the file. Returns true on success.

**Parameters**

* `buf` – 其字节应被追加到文件末尾的 `Buffer`。

**Return value**

* 成功则为 `true`；失败则为 `false`。

**In Depth**

此函数会将原始字节追加到文件末尾。其行为类似 `appendFromString()`，但面向二进制数据。

**Example**

```cpp
Path out("C:\\data\\binary.log");
Buffer chunk;
// chunk.append(...);  // 填充缓冲区

out.appendFromBuffer(chunk);
```

*此示例向现有文件追加二进制数据。*

---

#### `static bool ChangeCurrentDirectory(const Path &path)`

**Signature**

```cpp
static bool ChangeCurrentDirectory(const Path &path);
```

**Synopsis**  
Changes the current working directory. Returns true on success.

**Parameters**

* `path` – 表示要设置为当前工作目录的 `Path`。

**Return value**

* 若当前工作目录更改成功则为 `true`；否则为 `false`。

**In Depth**

当前工作目录会影响相对路径的解释方式。更改它可以简化大量使用相对路径的代码，但也会影响进程中所有后续相对路径操作。

在大型应用或库中使用时要格外谨慎，因为其他组件可能依赖该工作目录。

**Example**

```cpp
Path dataDir("C:\\myapp\\data");
if (Path::ChangeCurrentDirectory(dataDir)) {
  Path relative("config.json");
  String contents = relative.readToString();
}
```

*此示例更改工作目录，然后使用相对路径读取文件。*

---

#### `static List ListVolumes()`

**Signature**

```cpp
static List ListVolumes();
```

**Synopsis**  
Returns a list of volume drive letters (e.g., "C:", "D:").

**Parameters**

* *（无）*

**Return value**

* `List`，其中每个元素为 `String`，表示一个卷（例如 `"C:"`、`"D:"`）。

**In Depth**

`ListVolumes()` 会向系统查询可用的卷/驱动器。在展示驱动器选择菜单或在所有驱动器上扫描特定文件时，这很有用。

**Example**

```cpp
List volumes = Path::ListVolumes();
int count = volumes.length();
// volumes[i] 包含诸如 "C:" 或 "D:" 的驱动器名
```

*此示例获取所有被检测到的卷列表。*

---

#### `static Path CreateTemporaryFile(const String &prefix = String(), const String &suffix = String())`

**Signature**

```cpp
static Path CreateTemporaryFile(const String &prefix = String(),
                                const String &suffix = String());
```

**Synopsis**  
Creates a temporary file and returns its path.

**Parameters**

* `prefix` – 文件名的可选前缀（例如 `"myapp_"`）。
* `suffix` – 可选后缀或扩展名（例如 `".tmp"`）。

**Return value**

* 指向新建临时文件的 `Path`。

**In Depth**

此函数会在合适的系统定义位置（通常为临时目录）创建一个唯一的临时文件，并返回其路径。函数返回时，该文件应已存在，可以立即写入数据。

文件何时删除由你自行决定（使用 `deleteFile()`）。

**Example**

```cpp
Path temp = Path::CreateTemporaryFile("myapp_", ".log");
temp.writeFromString("Temporary log content.\n");
// 稍后：temp.deleteFile();
```

*此示例创建一个临时日志文件，写入内容，然后可在之后将其删除。*

---

#### `static Path CreateTemporaryDirectory(const String &prefix = String())`

**Signature**

```cpp
static Path CreateTemporaryDirectory(const String &prefix = String());
```

**Synopsis**  
Creates a temporary directory and returns its path.

**Parameters**

* `prefix` – 目录名的可选前缀。

**Return value**

* 指向新建临时目录的 `Path`。

**In Depth**

此函数会在系统临时区域下创建一个唯一的目录。它适合用于存放中间文件、解压归档或保存可在事后清理的中间结果。

你需要负责删除该目录（若可能包含文件，可使用 `removeDirectory(true)`）。

**Example**

```cpp
Path tempDir = Path::CreateTemporaryDirectory("myapp_");
Path tempFile = Path(tempDir.toString() + "\\data.bin");

tempFile.writeFromBuffer(Buffer());
// 完成后：
tempDir.removeDirectory(true);
```

*此示例创建一个临时目录，用于存放文件，然后删除整个目录树。*

---

#### `static Path GetHomeDirectory()`

**Signature**

```cpp
static Path GetHomeDirectory();
```

**Synopsis**  
Returns the user's home directory path.

**Parameters**

* *（无）*

**Return value**

* 表示当前用户主目录/配置文件目录的 `Path`。

**In Depth**

在 Windows 上，这通常是用户的配置文件目录（例如 `C:\\Users\\Alice`）。它常被用作应用数据、配置或文档的基准位置。

**Example**

```cpp
Path home = Path::GetHomeDirectory();
String homeStr = home.toString();
```

*此示例获取主目录路径并将其转换为字符串。*

---

#### `static Path GetDocumentsDirectory()`

**Signature**

```cpp
static Path GetDocumentsDirectory();
```

**Synopsis**  
Returns the user's documents directory path.

**Parameters**

* *（无）*

**Return value**

* 表示用户“文档”文件夹的 `Path`。

**In Depth**

此函数提供了一种便捷方式来定位用户通常保存可见文档的位置（例如 `C:\\Users\\Alice\\Documents`）。

**Example**

```cpp
Path docs = Path::GetDocumentsDirectory();
Path report(docs.toString() + "\\myapp_report.txt");
```

*此示例在用户文档目录中构建一个文件路径。*

---

#### `static Path GetRoamingAppDirectory()`

**Signature**

```cpp
static Path GetRoamingAppDirectory();
```

**Synopsis**  
Returns the roaming application data directory path.

**Parameters**

* *（无）*

**Return value**

* 表示漫游应用程序数据文件夹的 `Path`。

**In Depth**

漫游应用数据目录适合存放在域环境中应随用户在不同机器间漫游的设置（在许多系统上为 `C:\\Users\\Alice\\AppData\\Roaming`）。

应用程序通常会在该目录下创建自己的子目录。

**Example**

```cpp
Path roaming = Path::GetRoamingAppDirectory();
Path appDir(roaming.toString() + "\\MyApp");
appDir.makeDirectory(true);
```

*此示例在漫游应用数据目录下创建一个按应用划分的文件夹。*

---

#### `static Path GetLocalAppDirectory()`

**Signature**

```cpp
static Path GetLocalAppDirectory();
```

**Synopsis**  
Returns the local application data directory path.

**Parameters**

* *（无）*

**Return value**

* 表示本地（非漫游）应用程序数据文件夹的 `Path`。

**In Depth**

本地应用数据目录通常用于存放与机器绑定的数据，例如缓存或不需要随用户漫游的大型资源（例如 `C:\\Users\\Alice\\AppData\\Local`）。

**Example**

```cpp
Path local = Path::GetLocalAppDirectory();
Path cacheDir(local.toString() + "\\MyApp\\Cache");
cacheDir.makeDirectory(true);
```

*此示例在本地应用数据目录下创建缓存目录。*

---

#### `static Path GetWorkingDirectory()`

**Signature**

```cpp
static Path GetWorkingDirectory();
```

**Synopsis**  
Returns the current working directory path.

**Parameters**

* *（无）*

**Return value**

* 表示进程当前工作目录的 `Path`。

**In Depth**

此函数获取操作系统用于解释相对路径的目录。它是 `ChangeCurrentDirectory()` 的对应函数。

**Example**

```cpp
Path current = Path::GetWorkingDirectory();
String currentStr = current.toString();
```

*此示例获取当前工作目录以用于日志或诊断。*

---

#### `static Path GetCurrentExecutable()`

**Signature**

```cpp
static Path GetCurrentExecutable();
```

**Synopsis**  
Returns the path to the current executable.

**Parameters**

* *（无）*

**Return value**

* 表示正在运行的可执行文件完整路径的 `Path`。

**In Depth**

当应用需要根据自身安装目录定位资源时，此函数尤其有用。你可以对该路径调用 `getParentDirectory()` 以定位可执行文件所在目录。

**Example**

```cpp
Path exe = Path::GetCurrentExecutable();
Path exeDir = exe.getParentDirectory();

Path config(exeDir.toString() + "\\config.json");
```

*此示例定位存放在可执行文件旁边的配置文件。*