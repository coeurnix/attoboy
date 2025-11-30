# Attoboy

一个“电池齐全”的 C++ 库，用于构建微小的 Windows 实用工具。编写简洁、可读的代码，并编译为小于 100KB 的可执行文件。

```cpp
#include <attoboy/attoboy.h>
using namespace attoboy;

void atto_main() {
  Console con;
  String name = con.input("What's your name? ");
  con.println("Hello, " + name + "!", CON_CYAN);
  Exit();
}
```

编译后约为 12KB。无需安装程序，无外部依赖，可在任意 Windows 机器上运行。

## 为什么选择 Attoboy？

大多数 C++ 程序会引入 STL 和 CRT，这会增加数百 KB 的额外体积。attoboy 跳过这些内容，直接与 Windows API 通信，在避免膨胀的同时为你提供一个简洁的接口。

它面向 **C++ 新手**，以及任何在构建命令行工具、文件工具、网络爬虫或其他对可移植性和体积有要求的小程序的开发者。

## 包含哪些内容？

**核心类型：**
- `String` —— 带有类似 Python 方法的 UTF-8 字符串（`split`、`replace`、`trim`、`contains` 等）
- `List` —— 支持混合类型的动态数组，带有 JSON/CSV 支持
- `Map` —— 具有类型灵活性的键值字典
- `Set` —— 支持集合运算的唯一值集合

**文件 I/O：**
- `Path` —— 文件元数据、路径导航和简单读写
- `File` —— 面向流的文件和套接字 I/O
- `Buffer` —— 支持 LZ4 压缩和 ChaCha20 加密的二进制数据

**网络：**
- `WebRequest` / `WebResponse` —— 带 JSON 支持的 HTTP 客户端
- `AI` / `Conversation` / `Embedding` —— 兼容 OpenAI 的 API 集成

**控制台 UI：**
- `Console` —— 颜色、菜单、进度条，带历史记录和 Tab 补全的输入
- `Arguments` —— 支持标志和参数的命令行解析

**系统：**
- `DateTime` —— 毫秒级精度的时间戳
- `Thread` / `Mutex` —— 轻量级线程
- `Subprocess` —— 运行外部程序并进行 I/O 流式交互
- `Registry` —— 访问 Windows 注册表
- `Math` —— 标准数学函数和随机数生成

所有类型都使用 RAII，因此你无需手动管理内存。字符串是不可变的。所有内容都是线程安全的。

## 文档

想了解更多？

- **[快速开始](getting-started/README.md)** —— 安装与入门
- **[教程](tutorial/README.md)** —— 全功能的完整演练
- **[API 参考](api/README.md)** —— 每个类与函数的详细文档

## 环境要求

- **Windows**（生成 32 位可执行文件，适用于所有版本）
- **编译器：** 推荐 MSVC，也支持 MinGW GCC
- **构建系统：** CMake 3.15+
- **测试（可选）：** Python 3.10+

## 许可证

MIT License —— 详情见 [LICENSE](https://github.com/coeurnix/attoboy/blob/main/LICENSE)。

由 [coeurnix](https://github.com/coeurnix) 创建。