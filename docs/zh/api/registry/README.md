# Registry

## 概览

`attoboy::Registry` 类提供一个简单的、符合 RAII 风格的 **Windows 注册表** 接口。它允许你：

* 打开一个已存在的注册表键。
* 创建或删除一个键。
* 读写 **字符串**、**整数** 和 **二进制** 值。
* 枚举值名和子键。

所有操作都通过一个 `Registry` 对象完成，该对象拥有（或共享）一个指向特定键的句柄，例如：

* `HKEY_CURRENT_USER\Software\MyApp`
* `HKEY_LOCAL_MACHINE\Software\Vendor\Product`

该类的设计原则如下：

* **构造** 时打开或引用一个键路径。
* **析构** 时自动关闭底层句柄。
* **拷贝和赋值** 只是共享同一个底层键（轻量级句柄共享）。

这样就可以在无需手动管理句柄的情况下轻松操作注册表。

### Windows 注册表简介

Windows 注册表是一个分层数据库，被 Windows 和许多应用程序用来存储配置数据。关键概念包括：

* **配置单元（Hives）** – 顶层根键，例如：

  * `HKEY_CURRENT_USER`（当前用户的设置）
  * `HKEY_LOCAL_MACHINE`（整台机器的设置）
* **键（Keys）** – 类似于文件系统中的目录。例如：
  `HKEY_CURRENT_USER\Software\MyApp`
* **值（Values）** – 键下的具名条目，每个条目包含：

  * 一个 **名称**（字符串，对于“默认值”可以为空）。
  * 一个 **类型**（字符串、整数、二进制等）。
  * 数据（实际存储的值）。

在 `attoboy::Registry` 中，你通过一个 **单一字符串路径** 指定键，例如：

```cpp
"HKEY_CURRENT_USER\\Software\\MyApp"
```

注意在源代码中使用双反斜杠（`\\`）来表示字面意义上的 `\`。

### 注册表键、值以及默认值

在某个给定键中：

* 每个具名值都有唯一的 **名称** 字符串，比如 `"InstallPath"` 或 `"WindowWidth"`。
* 还可以有一个 **未命名默认值**。在 `attoboy::Registry` 中，通过传入一个 **空名称** (`String()`) 或在提供了默认参数时省略该参数来引用此默认值。

例如：

* 无参数调用 `valueExists()` 会检查默认值是否存在。
* `valueExists("InstallPath")` 检查一个具名值。

### RAII 与句柄共享

`Registry` 类遵循 RAII 原则：

* 当构造一个 `Registry` 对象时，它会打开或引用一个注册表键路径。
* 当对象被销毁时，如果它拥有句柄，则会关闭底层句柄。

拷贝或赋值 `Registry` 对象时：

* 并 **不会** 复制注册表键；所有拷贝对象会 **共享同一个底层句柄**。
* 这样既高效又安全：当最后一个引用超出作用域时，关闭任意一个 `Registry` 实例都会释放共享资源。

因此，你可以放心地按值传递 `Registry` 对象，而不用担心资源泄漏或系统资源重复。

---

## 参考

下面的每一条目都介绍 `attoboy::Registry` 的一个公有构造函数、方法或运算符。每条目包括：

* **Signature** – 头文件中的精确定义。
* **Synopsis** – 原始的一行 Doxygen 注释。
* **Parameters** 和 **Return value** – 参数及返回值说明。
* **In Depth** – 补充细节与注意事项。
* **Example** – 一个简短、贴近实际的用例代码及简要说明。

所有示例均假定：

```cpp
using namespace attoboy;
```

---

#### `Registry(const String &key)`

**Signature**

```cpp
Registry(const String &key);
```

**Synopsis**  
Opens a registry key (e.g., "HKEY_CURRENT_USER\Software\MyApp").

**Parameters**

* `key` – 要打开的完整注册表路径，包括根配置单元。例如：
  `"HKEY_CURRENT_USER\\Software\\MyApp"`。

**Return value**

* *(构造函数；不适用)*

**In Depth**

此构造函数表示一个特定的注册表键路径。在内部，它会打开（或准备打开）与给定字符串对应的注册表键。合法的 `key` 字符串示例包括：

* `HKEY_CURRENT_USER\\Software\\MyApp`
* `HKEY_LOCAL_MACHINE\\Software\\Vendor\\Product`

典型用法模式：

* 调用 `exists()` 检查该键是否已经存在。
* 调用 `create()` 在键不存在时创建它。
* 使用 `setStringValue`、`setIntegerValue` 等在该键下设置各种值。

通常你应选择位于 `HKEY_CURRENT_USER` 或 `HKEY_LOCAL_MACHINE` 之下、且与应用程序相关的子键（例如 `"HKEY_CURRENT_USER\\Software\\MyCompany\\MyApp"`）。

**Example**

```cpp
Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");

if (!reg.exists()) {
  reg.create();
}
```

*此例在当前用户配置单元下打开一个键，并在它尚不存在时创建该键。*

---

#### `Registry(const Registry &other)`

**Signature**

```cpp
Registry(const Registry &other);
```

**Synopsis**  
Creates a copy (shares the underlying key).

**Parameters**

* `other` – 要拷贝的已存在 `Registry` 实例。

**Return value**

* *(构造函数；不适用)*

**In Depth**

拷贝构造函数不会创建一个新的注册表键。相反，它会让新对象与 `other` 共享同一个底层句柄。这样既高效又安全：

* 你可以按值传递 `Registry` 实例。
* 所有拷贝对象都引用同一个键。
* 只有在最后一个引用该键的 `Registry` 对象被销毁时，句柄才会被释放。

通过其中一个拷贝所进行的任何操作（读或写值）都会作用于相同的底层注册表键。

**Example**

```cpp
Registry base("HKEY_CURRENT_USER\\Software\\MyApp");
Registry copy(base);  // shares the same key
```

*此例创建了第二个 `Registry` 对象，它引用与第一个对象相同的注册表键。*

---

#### `~Registry()`

**Signature**

```cpp
~Registry();
```

**Synopsis**  
Closes the key and frees resources.

**Parameters**

* *(无)*

**Return value**

* *(析构函数；不适用)*

**In Depth**

当最后一个引用该键的 `Registry` 对象被销毁时，析构函数会关闭底层注册表键句柄。这体现了 RAII 行为：

* 无需手动清理。
* 不会因为忘记关闭键而产生错误。

**Example**

```cpp
{
  Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");
  // use reg here
} // reg is destroyed here; the key is closed automatically
```

*此例展示了当 `Registry` 对象离开作用域时，该键会被自动关闭。*

---

#### `Registry &operator=(const Registry &other)`

**Signature**

```cpp
Registry &operator=(const Registry &other);
```

**Synopsis**  
Assigns another registry (shares the underlying key).

**Parameters**

* `other` – 其底层键句柄应被共享的 `Registry` 对象。

**Return value**

* 指向 `*this` 的引用，用于支持赋值链式调用。

**In Depth**

赋值运算符的行为如下：

1. 释放 `*this` 当前持有的键句柄（如果有）。
2. 让 `*this` 与 `other` 共享同一个底层句柄。

赋值之后，两个对象都指向同一个键。自赋值是安全的；实现会防止句柄被错误地释放两次。

**Example**

```cpp
Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");
Registry other("HKEY_CURRENT_USER\\Software\\OtherApp");

reg = other;  // reg now refers to the "OtherApp" key
```

*此例将一个 `Registry` 对象重新赋值为引用另一个键。*

---

#### `bool exists() const`

**Signature**

```cpp
bool exists() const;
```

**Synopsis**  
Returns true if the key exists.

**Parameters**

* *(无)*

**Return value**

* 如果指定的注册表键当前存在，则返回 `true`；否则返回 `false`。

**In Depth**

使用 `exists()` 检查构造函数中传入路径所标识的键是否存在于注册表中。这通常是读写值之前的第一步。

该函数不会创建键；它只检测键是否存在。

**Example**

```cpp
Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");

if (!reg.exists()) {
  // Key has not been created yet
}
```

*此例检查一个应用专用键是否已经存在。*

---

#### `bool create()`

**Signature**

```cpp
bool create();
```

**Synopsis**  
Creates the key if it doesn't exist. Returns true on success.

**Parameters**

* *(无)*

**Return value**

* 如果在调用结束后该键存在（无论是新创建的还是之前就存在的），则返回 `true`。
* 如果创建失败（如权限不足），返回 `false`。

**In Depth**

`create()` 用于确保注册表键存在。典型语义：

* 如果键不存在，则创建它。
* 如果键已存在，调用仍视为成功，并保持该键不变。

这使得你可以用一个简单的“确保存在”模式编写代码：

1. 构造一个 `Registry` 对象。
2. 调用一次 `create()`。
3. 在该键下读写值。

根据所使用的配置单元和路径（用户设置 vs. 机器级设置），你可能需要相应权限。

**Example**

```cpp
Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");

// Ensure that the key exists before writing values
if (reg.create()) {
  reg.setStringValue("InstallPath", "C:\\MyApp");
}
```

*此例在需要时创建该键，然后写入一个配置值。*

---

#### `bool remove(bool isRecursive = true)`

**Signature**

```cpp
bool remove(bool isRecursive = true);
```

**Synopsis**  
Removes the key. Returns true on success.

**Parameters**

* `isRecursive` – 若为 `true`，则尝试删除该键及其所有子键。若为 `false`，通常只在键为空时才删除（取决于具体实现）。

**Return value**

* 若成功删除键，返回 `true`。
* 若删除失败（例如权限不足，或在 `isRecursive` 为 `false` 时键非空），返回 `false`。

**In Depth**

`remove()` 删除与当前 `Registry` 对象关联的注册表键。使用时需谨慎：

* 建议只删除由你的应用程序创建的键。
* 避免删除系统或第三方键。
* 当 `isRecursive` 为 `true`（默认）时，子键和值也会被删除，这在应用卸载场景中很方便。

在 `remove()` 成功之后，该键在注册表中不再存在。`Registry` 对象本身仍然有效，但后续诸如 `exists()` 的调用会返回 `false`。

**Example**

```cpp
Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");

// Remove this key and all its subkeys/values
bool ok = reg.remove(true);
```

*此例递归删除一个应用的注册表键，可作为卸载流程的一部分。*

---

#### `bool valueExists(const String &name = String()) const`

**Signature**

```cpp
bool valueExists(const String &name = String()) const;
```

**Synopsis**  
Returns true if the value exists (empty name = default value).

**Parameters**

* `name` – 要检查的值名称。当 `name` 为一个空的 `String()`（默认值）时，表示键的 **默认（未命名）** 值。

**Return value**

* 如果指定名称的值存在于该键下，返回 `true`。
* 否则返回 `false`。

**In Depth**

在读取某个值之前，你可以使用 `valueExists()` 判断它是否已定义：

* 对于 `getIntegerValue()` 尤其有用，因为该函数在“未找到”和实际整数值为 `0` 时都会返回 `0`。
* 对于字符串和二进制值，也可以检查 `get*Value()` 函数返回的对象是否为空，但 `valueExists()` 能更清晰地表达意图。

**Example**

```cpp
Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");

if (reg.valueExists("WindowWidth")) {
  unsigned int width = reg.getIntegerValue("WindowWidth");
}
```

*此例在读取一个整数值前先检查该值是否存在。*

---

#### `String getStringValue(const String &name = String()) const`

**Signature**

```cpp
String getStringValue(const String &name = String()) const;
```

**Synopsis**  
Returns the string value, or empty string if not found.

**Parameters**

* `name` – 要读取的值名称。空的 `String()`（默认）表示默认值。

**Return value**

* 包含值内容的 `String`。
* 如果值不存在或无法表示为字符串，则返回空字符串。

**In Depth**

`getStringValue()` 从注册表读取一个 **字符串类型** 的值。要点如下：

* 返回的 `String` 为值语义，你拥有它的拷贝，可以按需保存。
* 若值不存在（或非字符串类型），函数返回空字符串。
* 可通过 `isEmpty()` 检查是否读到了非空内容，或结合 `valueExists()` 使用以获得更清晰的语义。

**Example**

```cpp
Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");

String installPath = reg.getStringValue("InstallPath");
if (!installPath.isEmpty()) {
  // use installPath as a String
}
```

*此例读取一个字符串值，并在确认非空后再使用。*

---

#### `Buffer getBinaryValue(const String &name = String()) const`

**Signature**

```cpp
Buffer getBinaryValue(const String &name = String()) const;
```

**Synopsis**  
Returns the binary value, or empty buffer if not found.

**Parameters**

* `name` – 要读取的二进制值名称。空的 `String()`（默认）表示默认值。

**Return value**

* 包含值数据的 `Buffer`。
* 如果值不存在或不是二进制类型，则返回空的缓冲区。

**In Depth**

`getBinaryValue()` 用于获取一个二进制注册表值。返回的 `Buffer`：

* 是一个独立的值对象，你拥有其数据拷贝，可以按需保存。
* 可通过 `isEmpty()` 或 `length()` 判断该值是否存在。
* 提供 `length()` 以访问二进制数据的长度。

这使得处理二进制数据更加方便，因为你同时拥有数据和其长度信息。

**Example**

```cpp
Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");

Buffer data = reg.getBinaryValue("Signature");
if (!data.isEmpty()) {
  // Interpret data using your own format or protocol.
  int size = data.length();
}
```

*此例读取一个二进制值，并在确认非空后再使用它。*

---

#### `unsigned int getIntegerValue(const String &name = String()) const`

**Signature**

```cpp
unsigned int getIntegerValue(const String &name = String()) const;
```

**Synopsis**  
Returns the integer value (0 if not found).

**Parameters**

* `name` – 要读取的整数值名称。空的 `String()`（默认）表示默认值。

**Return value**

* 存储的整数值。
* 如果值不存在或无法读取为整数，则返回 `0`。

**In Depth**

该函数从注册表读取一个整数值（例如 `REG_DWORD` 类型）。由于在失败时也返回 `0`，因此你无法区分：

* 值缺失，和
* 值合法且确实为 `0`。

要检测值是否存在，请结合 `valueExists()` 使用：

```cpp
if (reg.valueExists("Timeout")) {
  unsigned int timeout = reg.getIntegerValue("Timeout");
}
```

整数值适合用来保存简单计数、标志位或其他数值型配置项。

**Example**

```cpp
Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");

unsigned int timeoutMs = reg.getIntegerValue("TimeoutMs");  // 0 if missing
```

*此例读取一个以毫秒为单位的超时时间；如果该值不存在，则结果为 `0`。*

---

#### `bool setStringValue(const String &name, const String &str)`

**Signature**

```cpp
bool setStringValue(const String &name, const String &str);
```

**Synopsis**  
Sets a string value. Returns true on success.

**Parameters**

* `name` – 要设置的值名称。使用空的 `String()` 可以设置默认值。
* `str` – 包含要写入 UTF-8 文本的 `String`。

**Return value**

* 若成功写入该值，返回 `true`。
* 若写入失败（如权限不足或键无效），返回 `false`。

**In Depth**

`setStringValue()` 将文本值写入注册表。如果已存在同名值：

* 将被新内容覆盖。

如果键本身尚不存在，应该先调用 `create()`。

字符串值通常用于保存路径、标识符或那些自然以文本表示的配置标志。

**Example**

```cpp
Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");

if (reg.create()) {
  reg.setStringValue("InstallPath", "C:\\MyApp");
}
```

*此例确保键存在，然后写入一个字符串设置。*

---

#### `bool setBinaryValue(const String &name, const Buffer &buf)`

**Signature**

```cpp
bool setBinaryValue(const String &name, const Buffer &buf);
```

**Synopsis**  
Sets a binary value. Returns true on success.

**Parameters**

* `name` – 要设置的二进制值名称。
* `buf` – 其原始字节将被存储为该值的 `Buffer`。

**Return value**

* 若成功写入该值，返回 `true`。
* 若写入失败，返回 `false`。

**In Depth**

`setBinaryValue()` 将任意二进制数据存储到注册表中。该值的类型对应于某种二进制注册表类型（例如 `REG_BINARY`）。

典型用例包括：

* 缓存小型二进制数据块（签名、校验和）。
* 存储应用程序能够自行解码的固定格式数据。

**Example**

```cpp
unsigned char signatureBytes[4] = {0xDE, 0xAD, 0xBE, 0xEF};
Buffer sigBuf(signatureBytes, 4);

Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");
if (reg.create()) {
  reg.setBinaryValue("Signature", sigBuf);
}
```

*此例向注册表写入一个小型二进制签名值。*

---

#### `bool setIntegerValue(const String &name, unsigned int num)`

**Signature**

```cpp
bool setIntegerValue(const String &name, unsigned int num);
```

**Synopsis**  
Sets an integer value. Returns true on success.

**Parameters**

* `name` – 要设置的整数值名称。
* `num` – 要存储的无符号整数。

**Return value**

* 若成功写入该值，返回 `true`。
* 若写入失败，返回 `false`。

**In Depth**

`setIntegerValue()` 在当前键下写入一个数值（例如 `REG_DWORD`）。它会覆盖任何同名的既有值。

整数值适合用于存储计数、尺寸、标志、功能开关或其他数值配置。

**Example**

```cpp
Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");
if (reg.create()) {
  reg.setIntegerValue("WindowWidth", 1280);
  reg.setIntegerValue("WindowHeight", 720);
}
```

*此例将窗口尺寸以整数设置方式存储。*

---

#### `bool deleteValue(const String &name = String())`

**Signature**

```cpp
bool deleteValue(const String &name = String());
```

**Synopsis**  
Deletes a value. Returns true on success.

**Parameters**

* `name` – 要删除的注册表值名称。空的 `String()`（默认）表示默认值。

**Return value**

* 如果在调用结束时该值已被删除或原本就不存在（如何视作成功由实现决定），返回 `true`。
* 若删除失败（如权限不足），返回 `false`。

**In Depth**

`deleteValue()` 在不删除键本身的前提下，从键中移除单个值。可在以下场景中使用：

* 某个设置已不再需要。
* 你希望通过删除存储值来恢复到程序内置的默认设置。

需注意区分删除 **值**（键内部）与通过 `remove()` 删除整个 **键**。

**Example**

```cpp
Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");
// Remove an obsolete configuration value
reg.deleteValue("OldSetting");
```

*此例从应用的注册表键中删除了一个过时的具名值。*

---

#### `List listValueNames() const`

**Signature**

```cpp
List listValueNames() const;
```

**Synopsis**  
Returns a list of all value names.

**Parameters**

* *(无)*

**Return value**

* 一个 `List`，其中包含当前键下所有值的名称。每个元素通常是一个 `String`。

**In Depth**

`listValueNames()` 允许你发现当前键下定义了哪些值。这对于以下用途很有帮助：

* 调试和检查工具。
* 通用配置编辑器。
* “导入全部” 类场景。

默认（未命名）值的处理方式取决于具体实现；它可能作为空字符串名称出现，或被特殊处理。

你可以将其与 `getStringValue`、`getIntegerValue` 或 `getBinaryValue` 结合使用，以遍历所有值。

**Example**

```cpp
Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");

List names = reg.listValueNames();
for (int i = 0; i < names.length(); ++i) {
  String name = names.at<String>(i);
  // For each value name, you could read or display it.
}
```

*此例枚举了某个键下的所有值名并对其进行遍历。*

---

#### `List listSubkeys() const`

**Signature**

```cpp
List listSubkeys() const;
```

**Synopsis**  
Returns a list of all subkey names.

**Parameters**

* *(无)*

**Return value**

* 一个 `List`，其中每个 `String` 表示当前键下某个子键的名称。

**In Depth**

`listSubkeys()` 提供当前键下子键的名称。这使你可以：

* 发现动态创建的子键（例如按用户配置文件或配置名称创建的子键）。
* 在配置工具中构建树形导航。
* 枚举同一根键下所有某类配置的实例。

结合构造函数，你可以递归遍历某个注册表子树：

```cpp
List subs = reg.listSubkeys();
for (int i = 0; i < subs.length(); ++i) {
  String childName = subs.at<String>(i);
  Registry child("HKEY_CURRENT_USER\\Software\\MyApp\\" + childName);
  // Work with child...
}
```

**Example**

```cpp
Registry reg("HKEY_CURRENT_USER\\Software\\MyApp");

List subkeys = reg.listSubkeys();
for (int i = 0; i < subkeys.length(); ++i) {
  String childName = subkeys.at<String>(i);
  // childName is the name of a subkey under MyApp
}
```