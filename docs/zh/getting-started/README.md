# 入门指南

---

## 欢迎使用 Attoboy！

Attoboy 是一个“电池全配”的 C++ 库，旨在让构建极小、可移植的 Windows 可执行文件变得像写 Python 脚本一样简单。无论你是 C++ 完全新手，还是希望创建小巧、自包含工具的资深开发者，attoboy 都为你提供了一种简洁、现代且对初学者友好的方式来编写强大的程序。

### Attoboy 有什么特别之处？

- **可执行文件极小**：大多数程序编译后大小为 10–100 KB
- **零依赖**：你的可执行文件可在任意 Windows 机器上运行，无需安装程序或运行时
- **电池全配**：字符串处理、集合、文件 I/O、网络、AI 集成等——全部集中在一个库中
- **新手友好**：受 Python 和现代语言启发的简洁可读 API
- **100% 测试覆盖**：完整的测试覆盖确保可靠性
- **文档完备**：全面的文档与教程助你顺利上手

如果你曾想编写命令行工具、文件工具、Web 客户端，甚至是能编译成比大多数图片还小的 AI 应用程序，那么你来对地方了。

---

## 前置条件

在你构建 attoboy 并开始创建自己的程序之前，需要先安装一些工具。过程很简单，我们会一步步引导你完成。

### 1. C++ 编译器

Attoboy 在 Windows 上支持两套编译器工具链。你只需要安装其中 **一套**（当然也可以两套都装）：

#### 选项 A：Visual Studio（MSVC）——推荐

微软 Visual Studio 的 MSVC 编译器用 attoboy 构建出的可执行文件体积最小。我们推荐使用完全免费的 **Visual Studio 2022 Community Edition**。

**下载并安装：**

1. 访问 [Visual Studio 下载页面](https://visualstudio.microsoft.com/downloads/)
2. 下载 **Visual Studio Community 2022**（免费）
3. 运行安装程序
4. 在选择“工作负载”时，勾选 **“使用 C++ 的桌面开发”**（Desktop development with C++）
5. 在右侧的 “Installation details” 中，确保已选择：
   - MSVC v143 - VS 2022 C++ x64/x86 build tools
   - C++ CMake tools for Windows
6. 点击 “Install”，等待安装完成（可能需要 20–30 分钟）

**安装完成后**，你需要使用 “Developer Command Prompt for VS 2022” 或 “x86 Native Tools Command Prompt for VS 2022” 来构建 attoboy。你可以在开始菜单的 “Visual Studio 2022” 下找到它们。

#### 选项 B：MinGW-w64（GCC）

MinGW 提供了 Windows 下的 GCC 编译器。如果你更喜欢 GCC，或需要更轻量级的安装，这是一个不错的选择。

**下载并安装：**

1. 访问 [WinLibs 下载页面](https://winlibs.com/)（推荐的 MinGW 发行版）
2. 下载最新的 **UCRT runtime** 版本（查找 Win32 版本）
3. 将下载的压缩包解压到类似 `C:\mingw32` 的位置
4. 将 `bin` 目录添加到 PATH：
   - 打开 “环境变量”（在开始菜单中搜索 “environment”）
   - 在 “系统变量” 中找到并编辑 “Path”
   - 点击 “新建”，添加 `C:\mingw32\bin`（或你解压到的路径）
   - 点击 “确定” 保存

**验证安装**：打开新的命令提示符并输入：
```
gcc --version
```

你应该能看到版本信息。

### 2. CMake 构建系统

CMake 是一个跨平台构建系统，用来方便地编译 attoboy 及其示例程序。

**下载并安装：**

1. 访问 [CMake 下载页面](https://cmake.org/download/)
2. 下载最新的 **Windows x64 Installer**（例如 `cmake-3.xx.x-windows-x86_64.msi`）
3. 运行安装程序
4. 当提示时，选择 **“Add CMake to the system PATH for all users”**（或当前用户）
5. 完成剩余安装步骤

**验证安装**：打开新的命令提示符并输入：
```
cmake --version
```

你应该能看到版本信息。

### 3.（可选）Python

只有在你希望运行完整测试套件时才需要安装 Python。构建 attoboy 或在你自己的程序中使用它并不需要 Python。

如果你想运行测试：

1. 访问 [Python.org](https://www.python.org/downloads/)
2. 下载 Python 3.10 或更新版本
3. 运行安装程序
4. **重要**：在安装过程中勾选 “Add Python to PATH”
5. 点击 “Install Now”

---

## 构建 Attoboy

现在工具已经安装好了，我们来构建 attoboy。这个过程会编译库并创建一个方便的 “distribution” 目录，其中包含你开始编写自己程序所需的一切。

### 步骤 1：获取源码

如果你还没有这样做，下载或克隆 attoboy 仓库：

```bash
git clone https://github.com/coeurnix/attoboy.git
cd attoboy
```

或者从 GitHub 下载 ZIP 文件并解压。

### 步骤 2：生成构建文件

在 attoboy 目录中打开命令提示符。具体命令取决于你使用的编译器：

#### MSVC（Visual Studio）：

确保你使用的是 **“x86 Native Tools Command Prompt for VS 2022”** 或 **“Developer Command Prompt for VS 2022”**（而不是普通命令提示符）。

```bash
cmake -G "Visual Studio 17 2022" -A Win32 -B build_msvc
```

这会在 `build_msvc` 目录中创建一个 Visual Studio 解决方案。

#### MinGW（GCC）：

```bash
cmake -G "MinGW Makefiles" -B build
```

这会在 `build` 目录中创建 Makefile。

### 步骤 3：构建全部内容

现在编译库、示例程序，并创建发行包：

#### 对于 MSVC：

```bash
cmake --build build_msvc --config Release
```

#### 对于 MinGW：

```bash
cmake --build build
```

这将花费一两分钟。你会看到 CMake 在构建库、所有示例程序，并把它们打包到发行目录中的进度信息。

### 步骤 4：庆祝一下！

如果构建成功，你会看到成功消息。现在编译好的库和示例已经可以使用了。

**构建了什么？**

- **库**：已编译的 attoboy 库（MSVC 为 `attoboy.lib`，MinGW 为 `libattoboy.a`）
- **示例**：15 个示例程序，位于 `build_msvc/examples/` 或 `build/examples/`
- **发行包**：一个开箱即用的发行包，位于 `build_msvc/dist/` 或 `build/dist/`

其中 **dist** 目录很特别——它包含了你编写自己的 attoboy 程序所需的一切，以后无需再碰 CMake！

---

## 使用发行目录（dist）

`dist` 目录（位于 `build_msvc` 或 `build` 内）是一个完整、自包含的包，用于构建 attoboy 程序。目录结构如下：

```
dist/
├── include/
│   └── attoboy/
│       └── attoboy.h       (The complete attoboy library header)
├── lib/
│   └── attoboy.lib         (The compiled library - .lib for MSVC, .a for MinGW)
├── docs/                   (Complete HTML documentation)
└── attobuild.bat           (Magic build script)
```

### attobuild.bat 的魔法

`attobuild.bat` 脚本是编译 attoboy 程序的一个简单而强大的方式。它会自动检测你使用的是 MSVC 还是 MinGW，并使用合适的编译选项来生成小巧且高优化的可执行文件。

### 第一个程序

让我们来创建你的第一个 attoboy 程序。

**步骤 1：** 进入 `dist` 目录：

```bash
cd build_msvc\dist
```
（或对 MinGW 使用 `cd build\dist`）

**步骤 2：** 创建一个名为 `hello.cpp` 的新文件，内容如下：

```cpp
#include <attoboy/attoboy.h>

using namespace attoboy;

void atto_main() {
  Log("Hello from my first attoboy program!");
  Exit();
}
```

**步骤 3：** 使用 attobuild 构建：

```bash
attobuild.bat hello.cpp
```

**步骤 4：** 运行你的程序：

```bash
hello.exe
```

你应该能看到输出：`Hello from my first attoboy program!`

**步骤 5：** 查看文件大小：

```bash
dir hello.exe
```

你的可执行文件应约为 4–5 KB。这是一个完整、自包含的程序，比大多数文本文件还小。

### 构建更复杂的程序

无论 attoboy 程序多复杂，`attobuild.bat` 的用法都是一样的：

```bash
attobuild.bat my_utility.cpp
attobuild.bat calculator.cpp
attobuild.bat web_scraper.cpp
```

只需编写你的代码，运行 `attobuild.bat yourfile.cpp`，你就会得到一个可随意分发的小巧可执行文件。

### 理解构建过程

如果你对 `attobuild.bat` 背后的工作方式感兴趣，可以用文本编辑器打开它。脚本中有详尽注释，展示了生成小体积可执行文件所需的具体编译选项。你可以利用这些信息将 attoboy 集成到自己的 CMake 工程或其他构建系统中。

---

## 运行测试套件（可选）

如果你已安装 Python，并希望验证 attoboy 在你的系统上是否工作正常，可以运行完整测试套件：

### 基础测试（不需要互联网）

```bash
cmake --build build_msvc --config Release --target coverage
```

这会运行所有测试，并为网络和 AI 调用创建一个模拟服务器。最终会显示测试覆盖率统计信息。

### 完整测试（需要 OpenAI API Key）

```bash
set OPENAI_API_KEY=your_api_key_here
cmake --build build_msvc --config Release --target coverage_live
```

这会运行**所有**测试，包括对 OpenAI 的真实 API 调用。请注意，这会产生一小笔费用。

---

## 文档与学习资源

现在环境已经准备就绪，接下来可以从这里继续：

### 教程

**[教程](../tutorial/README.md)** 是学习 attoboy 的最佳起点。它通过清晰的说明和实用示例带你逐步了解库的功能。即使你完全不熟悉 C++，这个教程也能让你迅速上手。

### API 文档

`dist` 目录中的 **docs** 文件夹包含 attoboy 中每个类和函数的完整 API 文档。用浏览器打开 `docs/index.html` 即可查看完整参考。

下面是你会经常使用的一些主要类型概览：

- **String**：不可变 UTF-8 字符串，带有丰富的操作方法
- **List**：可存放混合类型的动态数组
- **Map**：键值映射字典
- **Set**：存放唯一值的集合
- **Path**：文件与目录路径操作
- **File**：文件与套接字 I/O
- **Buffer**：带压缩和加密支持的二进制数据处理
- **DateTime**：时间与日期处理
- **WebRequest/WebResponse**：HTTP 客户端功能
- **AI/Conversation/Embedding**：兼容 OpenAI 的 AI 集成接口
- **Console**：具有颜色、菜单和进度条的丰富控制台 UI
- **Arguments**：命令行参数解析
- **Thread/Mutex**：多线程支持
- **Math**：数学运算与常量
- **Subprocess**：运行外部程序
- **Registry**：访问 Windows 注册表

---

## 获取帮助

如果你遇到问题或有疑问，可以：

1. **先看教程**：大多数常见问题都在其中得到解答
2. **查看示例**：参考展示各种特性的实际工作代码
3. **阅读 API 文档**：每个函数都有详细说明
4. **查看 GitHub Issues**：看看是否有人遇到过类似问题
5. **提交 Issue**：在 GitHub 上报告 bug 或提出问题

---

玩得开心，感谢你使用 Attoboy！

---

*作者：[coeurnix](https://github.com/coeurnix)*

*基于 MIT 协议授权*