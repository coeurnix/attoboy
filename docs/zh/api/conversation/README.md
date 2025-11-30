# Conversation

## 概览

`attoboy::Conversation` 类表示由 `attoboy::AI` 管理的一个**有状态、多轮对话**。与发送单次、无关联的提示不同，你通过 `Conversation` 来：

* 维护一份**持续累积的消息历史**。
* 提出后续问题，并自动包含之前的上下文。
* 检查、修改或分叉历史，以支持更高级的工作流。
* 跟踪每个会话大致的**token 使用量**。

`Conversation` 由一个 `AI` 实例创建：

```cpp
using namespace attoboy;

AI ai("https://example-base-url", "YOUR_API_KEY", "your-model");
Conversation convo = ai.createConversation();
```

从这一刻起，对 `convo.ask(...)` 的所有调用都会使用同一个会话状态，其中包括：

* 配置在 `AI` 上的 system prompt（如果有）。
* 用户与助手消息交替的序列。
* 此会话的 token 使用统计。

### 多轮对话与历史记录

在大多数大语言模型（LLM）API 中，每次请求不仅包含最新的用户消息，还包含到目前为止的**完整对话历史**。这使得模型可以：

* 回溯并引用更早的消息。
* 维持风格与上下文的连贯性。
* 自然地回答后续问题。

`Conversation` 对这一模式进行了抽象：

* 每次调用 `ask()` 都会向内部历史追加一条**用户消息**。
* 模型的回复则会作为一条**助手消息**被追加。
* 你可以通过 `getConversationList()` 获取完整历史，或用 `setConversationList()` 替换它。

历史记录存储为一个由 `String` 组成的 `List`：

* **偶数索引（0, 2, 4, …）**为用户消息。
* **奇数索引（1, 3, 5, …）**为助手消息。

这种结构便于在需要时检查或转换对话内容。

### Token 与使用量跟踪

大多数 LLM 的计费与限制基于 **token**，token 是文本的小片段（平均长度短于一个词）。token 使用量非常重要，因为它会影响性能、成本，有时还会影响延迟。

`Conversation` 提供了三个计数器：

* `getPromptTokensUsed()` – 用于提示（包括以往对话轮次）的 token 数。
* `getResponseTokensUsed()` – 模型生成的回复 token 数。
* `getTotalTokensUsed()` – 提示 token 与回复 token 之和。

这些计数器可以帮助你：

* 监控大致使用量，用于分析或速率限制。
* 实现简单的安全检查，例如“当会话过长时发出警告”。

### 复制与分叉

`Conversation` 实例是指向底层会话状态的**句柄**：

* 拷贝构造和赋值会**共享**同一个底层会话。
* `duplicate()` 会创建一个会话的**分叉副本**，之后在该副本上添加的新消息不会影响原始会话。

当你希望从相同对话历史出发，探索多种可能的“未来”时，这非常有用：

* 原始 `Conversation` 沿一条路径继续。
* 一个副本则通过其关联的 `AI` 探索不同的问题或 system prompt。

### 创建与生命周期

你不能直接构造一个 `Conversation`。其默认构造函数是私有的。相反：

* 调用 `AI::createConversation()` 来获取一个新的 `Conversation`。
* `AI` 实例负责管理配置（模型、base URL、密钥、system prompt 等）。
* `Conversation` 通过 `getAI()` 持有一个指向其 `AI` 的链接。

与其他 attoboy 类型一样，资源管理采用 RAII：

* 当对象离开作用域时，析构函数会释放底层会话资源。
* 拷贝和赋值通过共享引用的方式进行，而不会复制整个历史，从而保持开销低廉。

---

## 参考

下面对 `attoboy::Conversation` 的每个成员进行说明，包括函数签名、概要、参数、返回值、详细行为以及简短示例。

> 所有示例都假定使用 `using namespace attoboy;`。

---

#### `Conversation(const Conversation &other)`

**签名**

```cpp
Conversation(const Conversation &other);
```

**概要**  
创建一个副本（共享底层会话）。

**参数**

* `other` – 已存在的 `Conversation`，其底层状态将被共享。

**返回值**

* *（构造函数；不适用）*

**深入说明**

拷贝构造函数会创建一个新的 `Conversation` 对象，该对象**引用与 `other` 相同的底层会话状态**。这意味着：

* 两个对象看到的是同一份消息历史。
* 对任一对象调用 `ask()` 都会向同一个会话追加消息。
* token 计数器是共享的。

这种行为是有意设计且高效的：`Conversation` 是一个轻量级句柄，可以按值传递，而无需复制整份历史。

如果你需要会话的独立分支（例如，希望分别探索两个不同的后续问题），应当改用 `duplicate()`。

**示例**

```cpp
AI ai("https://example-base-url", "KEY", "model");
Conversation original = ai.createConversation();

Conversation shared(original);  // 与 'original' 共享历史

String reply1 = original.ask("Hello!");
String reply2 = shared.ask("What did I just say?");
```

*该示例展示了两个句柄共享同一会话状态；两个 `ask()` 调用都作用在同一份历史上。*

---

#### `~Conversation()`

**签名**

```cpp
~Conversation();
```

**概要**  
销毁会话并释放资源。

**参数**

* *（无）*

**返回值**

* *（析构函数；不适用）*

**深入说明**

当一个 `Conversation` 对象被销毁时，它会释放其对底层会话状态的引用。如果没有其他 `Conversation` 对象共享该状态，库将释放相关资源。

你无需显式调用任何拆卸函数。只需让 `Conversation` 离开作用域或覆盖其变量，RAII 即可自动完成清理。

**示例**

```cpp
{
  AI ai("https://example-base-url", "KEY", "model");
  Conversation convo = ai.createConversation();
  String reply = convo.ask("Temporary question");
} // convo 在此处被销毁；其资源被释放
```

*该示例展示了一个会话被创建，并在离开代码块时自动清理。*

---

#### `Conversation &operator=(const Conversation &other)`

**签名**

```cpp
Conversation &operator=(const Conversation &other);
```

**概要**  
赋值为另一个会话（共享底层会话）。

**参数**

* `other` – `Conversation`，赋值后应与其共享状态。

**返回值**

* 返回 `*this` 的引用，以支持链式赋值。

**深入说明**

赋值运算符会使左侧的 `Conversation` 与 `other` 共享同一个底层会话状态。任何之前关联的会话状态都会被释放。

赋值之后：

* 两个变量引用同一份历史。
* 对任意一个变量调用 `ask()` 都会影响同一个会话。

如果 `*this` 之前关联的是一个独立的会话，在赋值后它将不再被引用，并可能被释放。

**示例**

```cpp
AI ai("https://example-base-url", "KEY", "model");

Conversation convo1 = ai.createConversation();
Conversation convo2 = ai.createConversation();

convo2 = convo1;  // 现在两者共享同一个会话
```

*该示例将一个 `Conversation` 赋值给另一个，使它们共享状态。*

---

#### `String ask(const String &prompt, int timeout = -1)`

**签名**

```cpp
String ask(const String &prompt, int timeout = -1);
```

**概要**  
发送一条消息并返回回复。如出错请检查 isEmpty()。

**参数**

* `prompt` – 要追加到会话并发送给 AI 模型的用户消息。
* `timeout` – 可选的请求超时时间（毫秒）。值为 `-1` 时使用库的默认行为（通常是无限或由模型决定的限制）。

**返回值**

* 包含助手回复的 `String`。
* 若发生错误（例如网络失败或 API 错误），返回的字符串将**为空**；此时应调用 `isEmpty()` 进行检查。

**深入说明**

`ask()` 是通过 `Conversation` 与 AI 交互的主要方式：

1. `prompt` 被作为**用户消息**追加到会话历史中。
2. 完整历史（用户与助手消息）被发送到 AI 后端。
3. AI 的回复被作为**助手消息**追加。
4. 该回复以 `String` 返回。

token 使用计数器（`getPromptTokensUsed()`, `getResponseTokensUsed()`, `getTotalTokensUsed()`）会相应更新。

错误处理是显式的：

* 出错时返回的 `String` 为空（即 `reply.isEmpty()` 返回 `true`）。
* 现有会话历史可能会或可能不会被更新，取决于错误类型；如有需要，你可以通过 `getConversationList()` 与 `setConversationList()` 来检查或重置历史。

`timeout` 参数用于控制调用在等待响应时可以阻塞的最长时间。如果你需要防止长时间阻塞应用程序，可以使用该参数。

**示例**

```cpp
AI ai("https://example-base-url", "KEY", "model");
Conversation convo = ai.createConversation();

String reply = convo.ask("Hello, who are you?");

if (!reply.isEmpty()) {
  // 使用该回复，并根据需要继续发问。
  String followUp = convo.ask("Can you summarize your abilities?");
}
```

*该示例开启一个会话，发送问题，通过检测 `isEmpty()` 判断是否出错，然后在相同上下文中提出一个后续问题。*

---

#### `List getConversationList() const`

**签名**

```cpp
List getConversationList() const;
```

**概要**  
返回会话历史（偶数=用户，奇数=助手）。

**参数**

* *（无）*

**返回值**

* 表示会话历史的 `List`，其中元素为 `String` 消息。

  * **偶数索引（0, 2, 4, …）**为用户消息。
  * **奇数索引（1, 3, 5, …）**为助手消息。

**深入说明**

`getConversationList()` 以一种简单、通用的形式暴露内部历史：

* 列表是一个扁平的消息序列。
* 通过索引的奇偶性（偶/奇）即可判断发言角色。
* 消息以 `String` 形式存储。

典型用法包括：

* 记录日志或调试会话。
* 实现自定义对话视图或将历史保存到磁盘。
* 将历史复制出来，通过 `setConversationList()` 为新会话预置上下文。

由于返回的 `List` 是一个独立对象，修改它**不会**自动改变底层会话。若要应用修改，需使用修改后的列表调用 `setConversationList()`。

**示例**

```cpp
AI ai("https://example-base-url", "KEY", "model");
Conversation convo = ai.createConversation();

convo.ask("Hello!");
convo.ask("What can you do?");

List history = convo.getConversationList();
int count = history.length();

// 示例：获取最后一条用户消息（如果存在）。
if (count >= 1 && (count - 1) % 2 == 0) {
  String lastUser = history.at<String>(count - 1);
}
```

*该示例获取历史并展示如何通过索引奇偶性找到最后一条用户消息。*

---

#### `bool setConversationList(const List &list)`

**签名**

```cpp
bool setConversationList(const List &list);
```

**概要**  
替换会话历史。成功返回 true。

**参数**

* `list` – 作为新会话历史的 `List`，其中元素为 `String` 消息。约定为：

  * 偶数索引为用户消息。
  * 奇数索引为助手消息。

**返回值**

* 若成功替换历史，返回 `true`。
* 若无法应用该列表（例如列表格式不正确或对底层实现而言过大），返回 `false`。

**深入说明**

`setConversationList()` 允许你**覆盖**内部会话历史。常见场景包括：

* 从磁盘恢复一个已保存的会话。
* 从预定义的“脚本”消息开始。
* 在继续会话之前先删除敏感消息。

当你调用此函数时：

* 现有历史会被完全替换。
* 后续对 `ask()` 的调用会基于新的历史继续。
* token 计数器可能会重置或重新计算，具体取决于实现；不要假定它们会被保留，除非你在外部自行管理。

你需要自行确保 `list` 构造正确（根据偶/奇索引区分用户/助手）。该函数不会重新解释消息，而是假定你遵循文档约定。

**示例**

```cpp
AI ai("https://example-base-url", "KEY", "model");
Conversation convo = ai.createConversation();

// 构造一个脚本化的会话历史。
List scripted;
scripted.append("User: Please always answer very briefly.");
scripted.append("Assistant: Understood. I will answer briefly.");
scripted.append("User: What is 2+2?");

// 将其应用到会话。
bool ok = convo.setConversationList(scripted);

if (ok) {
  String reply = convo.ask("And what is 3+3?");
}
```

*该示例使用预定义历史为会话预置上下文，然后继续提出新问题。*

---

#### `Conversation duplicate() const`

**签名**

```cpp
Conversation duplicate() const;
```

**概要**  
创建一个此会话的副本以进行分叉。

**参数**

* *（无）*

**返回值**

* 一个新的 `Conversation`，其起始历史与原始会话相同，但之后会独立演化。

**深入说明**

与共享底层会话的拷贝构造与赋值不同，`duplicate()` 会创建一个**分叉**会话：

* 新的 `Conversation` 以相同的消息历史起步。
* 对原始会话和分叉会话调用 `ask()` 会修改**各自独立**的历史。
* 每个分支的 token 跟踪彼此独立。

当你希望在相同上下文起点，从容探索不同后续问题时，适合使用 `duplicate()`。

**示例**

```cpp
AI ai("https://example-base-url", "KEY", "model");
Conversation base = ai.createConversation();

base.ask("Explain recursion in simple terms.");

Conversation branch1 = base.duplicate();
Conversation branch2 = base.duplicate();

String reply1 = branch1.ask("Give me a very short example.");
String reply2 = branch2.ask("Give me a detailed, step-by-step example.");
```

*该示例从共享的初始会话创建两个分支，并在每个分支上提出不同的后续问题。*

---

#### `AI getAI() const`

**签名**

```cpp
AI getAI() const;
```

**概要**  
返回管理此会话的 AI 实例。

**参数**

* *（无）*

**返回值**

* 一个 `AI` 对象，与创建该 `Conversation` 的原始 `AI` 共享底层配置。

**深入说明**

`getAI()` 使你可以访问与此 `Conversation` 关联的 `AI` 实例。当你需要：

* 检查配置（模型名称、base URL、system prompt 等）。
* 调整设置（例如 `setMaxTokens()` 或 `setJsonMode()`）。
* 使用相同 AI 配置创建更多会话。

时会很有用。

返回的 `AI` 与创建该会话的原始 `AI` 共享底层配置。通过该 `AI` 实例进行的配置更改，会在共享它的各个会话间生效。

**示例**

```cpp
AI ai("https://example-base-url", "KEY", "model");
Conversation convo = ai.createConversation();

// 从会话中取回 AI。
AI associated = convo.getAI();

// 修改配置（例如，开启 JSON 模式）。
associated.setJsonMode(true);

String reply = convo.ask("Respond with JSON describing a cat.");
```

*该示例从会话中取回管理它的 `AI`，更新配置，然后再提出问题。*

---

#### `int getPromptTokensUsed() const`

**签名**

```cpp
int getPromptTokensUsed() const;
```

**概要**  
返回已使用的提示 token 数。

**参数**

* *（无）*

**返回值**

* 表示此会话中用于提示的 token 数的整数（由具体实现统计）。

**深入说明**

该计数器表示作为**输入**发送给模型的 token 累积数量：

* 包含 system prompt（如果有）。
* 包含所有参与请求的用户和助手消息。

可用它来：

* 监控每个会话的使用量。
* 实现简单限制（例如，在到达某个 token 阈值后结束或重置会话）。

具体计数方式取决于底层模型与 API；应将其视为近似但有用的指标，而非精确计费数值。

**示例**

```cpp
AI ai("https://example-base-url", "KEY", "model");
Conversation convo = ai.createConversation();

convo.ask("Hello!");
convo.ask("Tell me a story.");

int promptTokens = convo.getPromptTokensUsed();
```

*该示例在若干轮对话后获取已使用的提示 token 数。*

---

#### `int getResponseTokensUsed() const`

**签名**

```cpp
int getResponseTokensUsed() const;
```

**概要**  
返回已使用的回复 token 数。

**参数**

* *（无）*

**返回值**

* 表示在此会话中模型作为**回复**生成的 token 数的整数。

**深入说明**

该计数器跟踪模型在所有 `ask()` 调用中作为输出产生的 token 总数。

它可以帮助你：

* 估算助手回复有多“啰嗦”。
* 根据生成内容总量实现相应策略。

**示例**

```cpp
AI ai("https://example-base-url", "KEY", "model");
Conversation convo = ai.createConversation();

convo.ask("Explain tokens.");
convo.ask("Summarize your explanation in one sentence.");

int responseTokens = convo.getResponseTokensUsed();
```

*该示例检查到目前为止生成的输出 token 数量。*

---

#### `int getTotalTokensUsed() const`

**签名**

```cpp
int getTotalTokensUsed() const;
```

**概要**  
返回总 token 使用量。

**参数**

* *（无）*

**返回值**

* 等于 `getPromptTokensUsed() + getResponseTokensUsed()` 的整数。

**深入说明**

这是此会话的**整体 token 使用量**，包含提示与回复两部分。它是最方便的单一指标，可用于：

* 针对每个会话进行预算控制。
* 决定何时截断或重置较长的历史。
* 构建简易监控面板。

其关系为：

```cpp
getTotalTokensUsed() == getPromptTokensUsed() + getResponseTokensUsed();
```

**示例**

```cpp
AI ai("https://example-base-url", "KEY", "model");
Conversation convo = ai.createConversation();

convo.ask("Hello!");
convo.ask("What can you do?");
convo.ask("Give me three bullet points.");

int total = convo.getTotalTokensUsed();

if (total > 4000) {
  // 考虑截断或重置会话。
}
```

*该示例通过总 token 数来判断会话是否可能已经变得过大。*