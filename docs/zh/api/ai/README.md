# AI

## 概述

`attoboy::AI` 类是在 attoboy 库中使用远程 AI 模型（大语言模型和 embedding）的主要入口。它充当一个 **兼容 OpenAI 的 API 客户端**，用于：

* **聊天补全（Chat completions）** – 发送提示词并接收模型生成的文本响应。
* **Embedding** – 将文本转换为数值向量，以用于语义相似度。
* **会话（Conversations）** – 管理带历史记录的多轮对话会话。

你可以把 `AI` 看作指向特定 AI 服务的一个 **已配置“句柄”**：它保存 API 端点的基础 URL、你的 API key，以及默认模型名，并提供方法来：

* 调整请求方式（模型名、system prompt、最大 token 数、JSON 模式）。
* 观察使用量和行为（token 计数与结束原因）。
* 执行实际操作（`ask`、`createEmbedding`、`createConversation`）。

### 远程 AI 服务、Base URL 和 API Key

`AI` 类本身并不在本地运行 AI 模型，而是与一个遵循 OpenAI 风格接口的 **远程 HTTP API** 通信。要构造一个 `AI` 实例，你需要提供：

* `baseUrl` – AI 服务的根 URL（例如 `"https://api.openai.com/v1"`）。
* `apiKey` – 用于认证请求的私密密钥。
* `model` – 要调用的模型名称（例如 `"gpt-5-mini"` 或厂商特定变体）。

这些值会被存储在 `AI` 对象内部，并在之后的每次请求中复用。你可以通过 `setModel` 之后再更改模型。

> 将你的 API key 当成密码：不要把它提交到版本库或输出到日志中。

### Prompt、System Prompt 与响应

当你使用 `AI::ask` 时，你会发送一个 **prompt**：

* `prompt` 参数是一个 `String`，其中包含用户的问题、指令或输入文本。
* AI 服务会返回一个 `String` 作为响应（例如答案、解释或补全文本）。

此外，你可以通过 `setSystemPrompt` 设置一个 **system prompt**。system prompt：

* 描述 AI 的整体行为或“角色”（例如：“你是一个简明扼要并用 JSON 回答的助手。”）。
* 适用于该 `AI` 实例的所有后续调用以及从它创建的所有会话。
* 通过向 `setSystemPrompt` 传入空字符串可以清除。

在实践中，这使你可以把 **全局行为**（system prompt）与 **每次请求的内容**（传给 `ask` 或 `Conversation::ask` 的 prompt 文本）分开管理。

### Token、限制与使用量跟踪

AI 服务通常以 **token** 度量使用量，token 是文本的小片段（通常短于一个词）。每次请求会消耗：

* **Prompt tokens** – 来自你的输入（system prompt、消息等）的 token。
* **Response tokens** – 模型在回复中生成的 token。

`AI` 类会在同一个实例上累计跟踪使用情况：

* `getPromptTokensUsed()` – prompt token 总数。
* `getResponseTokensUsed()` – response token 总数。
* `getTotalTokensUsed()` – 上述两者之和。

这种跟踪对于以下场景非常有用：

* 在按 token 计费时监控成本。
* 在响应异常冗长时调试性能问题。

你可以随时通过 `resetTokenTracking()` 重置这些计数器。

你还可以通过 `setMaxTokens(int max)` 控制允许响应使用的最大 token 数。一个典型模式是：

* 先设置一个全局限制（例如 `setMaxTokens(256)`）。
* 使用 `getFinishReason()` 查看响应是因长度被截断（结束原因为 `"length"`），还是自然结束（结束原因为 `"stop"`）。

### JSON 模式

很多 AI 工作流需要生成结构化输出。`AI::setJsonMode(bool)` 可以启用 **JSON 响应模式**：

* 启用后，prompt 和 system prompt 应当指示模型返回合法 JSON。
* AI 客户端会在远程 API 支持的前提下，将请求配置为倾向于 JSON 结构。
* 然后你再将响应体（一个 `String`）按 JSON 解析，可使用你自己的逻辑或其他 attoboy 类型。

在构建命令行工具或以编程方式消费 AI 响应的服务时，这种模式尤其有用。

### Embedding 与语义相似度

`AI::createEmbedding` 方法将文本转换为 `Embedding` 对象：

* 一个 `Embedding` 是一个高维数值向量（`float` 值），用于表达语义含义。
* 对语义相近的文本，其向量在余弦相似度下会彼此接近。

通常的工作流是：

1. 对一个或多个文本调用 `createEmbedding`。
2. 使用 `Embedding::compare` 计算它们之间的相似度分数。
3. 使用这些分数来做搜索、聚类或排序。

如果 `createEmbedding` 失败，它会返回一个 `getDimensions() == 0` 的 `Embedding`。

### 会话（Conversations）与单次调用（Single-Shot Calls）

与模型交互有两种方式：

1. **单次调用**（`AI::ask`）

   * 你传入一个 prompt 并接收单一响应。
   * 适用于简单工具和一次性问题。

2. **多轮会话**（`AI::createConversation`）

   * 你创建一个绑定到该 `AI` 实例的 `Conversation` 对象。
   * 每次调用 `Conversation::ask` 会将用户消息与助手消息加入历史。
   * 模型在生成新回复时能看到整个对话上下文。

这两种方式共享相同的底层配置（base URL、API key、模型、system prompt、token 设置）。对某个 `AI` 实例的更改会作用于之后的操作以及从该实例新创建的会话。

---

## 参考

下面的每一节都介绍 `attoboy::AI` 的一个公共构造函数、方法或运算符。每节包括：

* **Signature（函数签名）** – 头文件中的精确声明。
* **Synopsis（概要）** – 原始的一行 Doxygen 注释。
* **Parameters（参数）** 和 **Return value（返回值）** 说明。
* **In Depth（深入说明）** – 行为细节、注意事项及使用示例。

> 所有代码示例默认使用 `using namespace attoboy;`。

---

#### `AI(const String &baseUrl, const String &apiKey, const String &model)`

**Signature**

```cpp
AI(const String &baseUrl, const String &apiKey, const String &model);
```

**Synopsis**  
使用基础 URL、API key 和模型名称创建一个 AI 客户端。

**Parameters**

* `baseUrl` – AI 服务的基础 URL（例如 `"https://api.openai.com/v1"`）。
* `apiKey` – 用于认证的私密 API key。
* `model` – 用于聊天和 embedding 调用的默认模型名称。

**Return value**

* *（构造函数；不适用）*

**In Depth**

此构造函数使用与远程服务通信所需的三项核心配置初始化一个 `AI` 客户端：

* **Base URL** – 所有请求都基于此根路径发送。
* **API key** – 通常通过 HTTP 头部传递（例如 `Authorization: Bearer <key>`）。
* **模型名称** – 用于指定服务应运行哪个模型（例如聊天模型与 embedding 模型的不同变体）。

构造完成后，该 `AI` 实例可以在多次请求中复用。你可以之后通过 `setModel` 调整模型，或修改其他行为（system prompt、JSON 模式、最大 token 数），而无需重新构造客户端。

如果配置无效（例如 URL 或 key 错误），诸如 `ask` 和 `createEmbedding` 等请求会失败；你应通过检查返回的 `String` 或 `Embedding` 对象来检测这一点。

**Example**

```cpp
using namespace attoboy;

String baseUrl("https://api.openai.com/v1");
String key("sk-123456");
String model("gpt-5-mini");

AI ai(baseUrl, key, model);
```

*此示例构造了一个 `AI` 客户端，用于与一个假定的远程服务通信，并使用特定模型。*

---

#### `AI(const AI &other)`

**Signature**

```cpp
AI(const AI &other);
```

**Synopsis**  
创建一个副本（共享底层配置）。

**Parameters**

* `other` – 要拷贝的现有 `AI` 实例。

**Return value**

* *（构造函数；不适用）*

**In Depth**

拷贝构造函数会创建一个新的 `AI` 对象，它与 `other` 共享相同的底层配置和内部实现。这是一个 **浅拷贝**：

* Base URL、API key、模型、system prompt 和 token 跟踪配置在逻辑上是共享的。
* 使用任一实例发送请求或调整配置，都会影响底层客户端状态。

当你希望按值传递 `AI` 给辅助代码，同时又不想担心生命周期或昂贵拷贝时，这种方式非常方便。

**Example**

```cpp
using namespace attoboy;

AI original(String("https://api.openai.com/v1"),
            String("sk-123456"),
            String("gpt-5-mini"));

AI copy(original);
```

*此示例创建了第二个 `AI` 句柄，与原始实例共享相同配置。*

---

#### `~AI()`

**Signature**

```cpp
~AI();
```

**Synopsis**  
销毁 AI 客户端并释放资源。

**Parameters**

* *（无）*

**Return value**

* *（析构函数；不适用）*

**In Depth**

当一个 `AI` 对象被销毁时（例如离开作用域），析构函数会释放与该客户端关联的所有资源：

* 内部句柄与缓冲区。
* 存储在 `AIImpl` 中的网络和配置信息。

由于 `AI` 使用 RAII，你不需要手动关闭网络连接；底层实现会负责清理工作。

如果还有其他 `AI` 实例通过拷贝构造或赋值共享同一实现，那么底层实现会一直存活，直到最后一个句柄被销毁。

**Example**

```cpp
using namespace attoboy;

{
  AI ai(String("https://api.openai.com/v1"),
        String("sk-123456"),
        String("gpt-5-mini"));
  // 在这里使用 ai
} // ai 被销毁；其资源被清理
```

*此示例展示了一个在代码块内部具有自动生命周期的 `AI` 实例。*

---

#### `AI &operator=(const AI &other)`

**Signature**

```cpp
AI &operator=(const AI &other);
```

**Synopsis**  
将另一个 AI 客户端赋值给当前对象（共享底层配置）。

**Parameters**

* `other` – 要赋值来源的现有 `AI` 实例。

**Return value**

* 对 `*this` 的引用，从而支持链式赋值。

**In Depth**

赋值运算符会让左侧的 `AI` 与 `other` 共享相同的内部实现和配置。`*this` 之前关联的任何实现会被释放，任何曾被唯一拥有的资源也可能会被销毁。

赋值后：

* 两个 `AI` 对象引用相同的 base URL、API key、模型、system prompt 和 token 跟踪状态。
* 通过任一句柄发送请求都会使用共享的配置。

自赋值是安全的；实现通常会检测并避免不必要的工作。

**Example**

```cpp
using namespace attoboy;

AI ai1(String("https://api.openai.com/v1"),
       String("sk-1"),
       String("gpt-5-mini"));

AI ai2(String("https://api.other.com/v1"),
       String("sk-2"),
       String("gpt-5-mini"));

ai2 = ai1;
// ai2 现在与 ai1 共享配置
```

*此示例将一个 `AI` 实例重新赋值为共享另一个实例的配置。*

---

#### `AI &setModel(const String &model)`

**Signature**

```cpp
AI &setModel(const String &model);
```

**Synopsis**  
设置模型名称。返回当前 `AI` 以支持链式调用。

**Parameters**

* `model` – 要用于后续请求的新模型名称。

**Return value**

* 对 `*this`（同一 `AI` 对象）的引用，用于流式链式调用。

**In Depth**

`setModel` 会更新该 `AI` 实例及其之后新创建的会话所使用的默认模型。它不会影响已经发出的请求。

常见用途包括：

* 在轻量和重量模型之间切换（例如 `"small-quick"` 和 `"large-accurate"`）。
* 通过多个 `AI` 实例分别使用一个模型做聊天，另一个模型做 embedding。

由于该方法返回 `*this`，你可以链式调用：

```cpp
ai.setModel("gpt-5-mini").setMaxTokens(256);
```

**Example**

```cpp
using namespace attoboy;

AI ai(String("https://api.openai.com/v1"),
      String("sk-123456"),
      String("gpt-5-mini"));

ai.setModel("gpt-4.1-large");
```

*此示例将 AI 客户端切换为在未来调用中使用不同的模型。*

---

#### `AI &setSystemPrompt(const String &prompt)`

**Signature**

```cpp
AI &setSystemPrompt(const String &prompt);
```

**Synopsis**  
设置 system prompt（传入空字符串则清除）。返回当前 `AI` 以支持链式调用。

**Parameters**

* `prompt` – 保存 system prompt 的 `String`，传入空字符串则清除它。

**Return value**

* 对 `*this` 的引用，用于流式链式调用。

**In Depth**

system prompt 提供了影响该 `AI` 实例所有请求行为的 **全局指令**：

* 通常用来描述助手的角色、风格和约束条件。
* 在调用 `ask` 和 `Conversation::ask` 时，会与每个用户 prompt 组合使用。

要设置 system prompt，传入包含指令文本的 `String` 即可。

要清除 system prompt，传入一个空字符串。调用后，客户端将恢复为没有 system prompt 的状态。

**Example**

```cpp
using namespace attoboy;

AI ai(String("https://api.openai.com/v1"),
      String("sk-123456"),
      String("gpt-5-mini"));

String sys(
  "You are a helpful assistant that answers concisely.\n"
  "Use simple language and avoid technical jargon."
);

ai.setSystemPrompt(sys);
```

*此示例配置了一个 system prompt，用以影响 AI 对所有未来请求的回复方式。*

---

#### `AI &setMaxTokens(int max = -1)`

**Signature**

```cpp
AI &setMaxTokens(int max = -1);
```

**Synopsis**  
设置响应的最大 token 数（-1 使用模型默认值）。返回当前 `AI` 以支持链式调用。

**Parameters**

* `max` – 模型允许生成的最大响应 token 数。

  * `-1` 表示使用模型的默认限制（不显式覆盖）。

**Return value**

* 对 `*this` 的引用，用于流式链式调用。

**In Depth**

`setMaxTokens` 用于控制生成响应的 **最大长度**：

* 较小的值可以减少成本和延迟，但可能截断较长的答案。
* 较大的值允许更详细的响应，但可能更慢且更昂贵。

当响应因该限制被截断时，`getFinishReason()` 通常会返回 `"length"`。当模型自然停止时，它会返回 `"stop"` 或其他原因。

此设置适用于：

* `ask`
* `Conversation::ask`
* 某些情况下也可能影响 embedding 请求（依具体远程 API 而定），但主要用于聊天补全。

**Example**

```cpp
using namespace attoboy;

AI ai(String("https://api.openai.com/v1"),
      String("sk-123456"),
      String("gpt-5-mini"));

ai.setMaxTokens(128);  // 将响应限制在 128 个 token 以内
```

*此示例设置了一个 token 限制，以保持响应简短且可预测。*

---

#### `AI &setJsonMode(bool isJsonMode = false)`

**Signature**

```cpp
AI &setJsonMode(bool isJsonMode = false);
```

**Synopsis**  
启用/禁用 JSON 响应模式。返回当前 `AI` 以支持链式调用。

**Parameters**

* `isJsonMode` – 传入 `true` 启用 JSON 响应模式；`false` 则禁用。

**Return value**

* 对 `*this` 的引用，用于流式链式调用。

**In Depth**

启用 JSON 模式时：

* `AI` 客户端会配置请求，使模型预期返回某种 JSON 结构。
* 你的 prompt 和 system prompt 应当明确指示模型返回合法 JSON（例如单个 JSON 对象或数组）。

该模式特别适用于：

* 使用 attoboy 的 `Map`/`List` 在 JSON 解析后对响应进行程序化处理的工具。
* 依赖结构化输出的工作流（例如分类标签、配置对象）。

JSON 模式不会自动解析响应；`ask` 依然返回一个 `String`。你需要自行把该字符串解析成结构化数据。

**Example**

```cpp
using namespace attoboy;

AI ai(String("https://api.openai.com/v1"),
      String("sk-123456"),
      String("gpt-5-mini"));

ai.setJsonMode(true);
```

*此示例启用了 JSON 模式，使后续响应预期为 JSON 格式。*

---

#### `String getModel() const`

**Signature**

```cpp
String getModel() const;
```

**Synopsis**  
返回当前模型名称。

**Parameters**

* *（无）*

**Return value**

* 包含当前为该 `AI` 配置的模型名称的 `String`。

**In Depth**

`getModel()` 可以让你查看客户端当前正在使用哪个模型。它在以下场景中很有用：

* 调试（在请求前记录配置）。
* 构建需要展示或报告模型选择的工具。

返回的 `String` 是内部模型名称的不可变副本。

**Example**

```cpp
using namespace attoboy;

AI ai(String("https://api.openai.com/v1"),
      String("sk-123456"),
      String("gpt-5-mini"));

String currentModel = ai.getModel();  // "gpt-5-mini"
```

*此示例获取当前模型名称，用于日志或展示。*

---

#### `String getSystemPrompt() const`

**Signature**

```cpp
String getSystemPrompt() const;
```

**Synopsis**  
返回 system prompt。通过 `isEmpty()` 检查是否设置。

**Parameters**

* *（无）*

**Return value**

* 包含当前 system prompt 的 `String`，如果未设置则为一个空字符串。

**In Depth**

`getSystemPrompt()` 会返回最近一次通过 `setSystemPrompt` 配置的内容；如果从未设置或已经被清除，则返回空字符串。

由于空字符串本身也是合法的 prompt，你应对返回值调用 `isEmpty()` 来区分“未设置任何 prompt”和“显式设置为空”。

**Example**

```cpp
using namespace attoboy;

AI ai(String("https://api.openai.com/v1"),
      String("sk-123456"),
      String("gpt-5-mini"));

String sys = ai.getSystemPrompt();
if (sys.isEmpty()) {
  // 当前未配置 system prompt
}
```

*此示例检查是否已配置 system prompt。*

---

#### `String getBaseUrl() const`

**Signature**

```cpp
String getBaseUrl() const;
```

**Synopsis**  
返回 base URL。

**Parameters**

* *（无）*

**Return value**

* 包含该 `AI` 客户端 base URL 的 `String`。

**In Depth**

`getBaseUrl()` 用于获取该 `AI` 实例当前配置使用的基础端点。这在以下场景中很有用：

* 调试多环境设置（例如测试环境与生产环境）。
* 在诊断时记录配置信息。

**Example**

```cpp
using namespace attoboy;

AI ai(String("https://api.openai.com/v1"),
      String("sk-123456"),
      String("gpt-5-mini"));

String url = ai.getBaseUrl();  // "https://api.openai.com/v1"
```

*此示例查询 base URL 以确认客户端正在访问哪个环境。*

---

#### `String getAPIKey() const`

**Signature**

```cpp
String getAPIKey() const;
```

**Synopsis**  
返回 API key。

**Parameters**

* *（无）*

**Return value**

* 包含 API key 的 `String`。

**In Depth**

`getAPIKey()` 会返回存储在该 `AI` 实例中的 API key。其主要用途包括：

* 确认客户端已经配置了 **某个** key。
* 将该 key 传递给其他可能需要它的 API。

由于 API key 非常敏感，你应避免直接记录到日志中。如果确实必须记录某些信息，建议对 key 进行掩码或截断后再输出。

**Example**

```cpp
using namespace attoboy;

AI ai(String("https://api.openai.com/v1"),
      String("sk-123456"),
      String("gpt-5-mini"));

String key = ai.getAPIKey();
// 避免在日志中直接打印 key。
```

*此示例获取 API key，并提醒要将其视为敏感信息。*

---

#### `int getPromptTokensUsed() const`

**Signature**

```cpp
int getPromptTokensUsed() const;
```

**Synopsis**  
返回累计使用的 prompt token 数。

**Parameters**

* *（无）*

**Return value**

* 自该 `AI` 实例创建或上次调用 `resetTokenTracking()` 以来消耗的 prompt token 总数。

**In Depth**

每当你调用 `ask` 或使用从该 `AI` 创建的 `Conversation` 时，prompt token 会被统计并累加到该总数中。prompt token 通常包括：

* system prompt 的 token。
* 会话或消息历史。
* 新输入的 prompt。

使用该值可以监控你在一段时间内向模型发送了多少输入。

**Example**

```cpp
using namespace attoboy;

AI ai(String("https://api.openai.com/v1"),
      String("sk-123456"),
      String("gpt-5-mini"));

ai.ask("Hello!", -1);
int promptTokens = ai.getPromptTokensUsed();
```

*此示例在发起请求后获取累计的 prompt token 计数。*

---

#### `int getResponseTokensUsed() const`

**Signature**

```cpp
int getResponseTokensUsed() const;
```

**Synopsis**  
返回累计使用的 response token 数。

**Parameters**

* *（无）*

**Return value**

* 自该 `AI` 实例创建或上次调用 `resetTokenTracking()` 以来，模型生成的 response token 总数。

**In Depth**

response token 衡量模型在响应你的请求时生成了多少 **输出**。包括：

* 来自 `ask` 的单次响应。
* 来自 `Conversation::ask` 的多轮响应。

这个计数对于监控和限制整体输出量很有帮助。

**Example**

```cpp
using namespace attoboy;

AI ai(String("https://api.openai.com/v1"),
      String("sk-123456"),
      String("gpt-5-mini"));

ai.ask("Explain RAII briefly.", -1);
int responseTokens = ai.getResponseTokensUsed();
```

*此示例查询模型到目前为止生成的 token 总数。*

---

#### `int getTotalTokensUsed() const`

**Signature**

```cpp
int getTotalTokensUsed() const;
```

**Synopsis**  
返回累计使用的 token 总数。

**Parameters**

* *（无）*

**Return value**

* 该 `AI` 实例消耗的 prompt 和 response token 之和。

**In Depth**

`getTotalTokensUsed()` 是以下表达式的便捷封装：

```cpp
getPromptTokensUsed() + getResponseTokensUsed()
```

当你只关心总使用量（例如用于粗略估算成本）时，可以使用该方法。

**Example**

```cpp
using namespace attoboy;

AI ai(String("https://api.openai.com/v1"),
      String("sk-123456"),
      String("gpt-5-mini"));

ai.ask("Give me a short poem.", -1);

int totalTokens = ai.getTotalTokensUsed();
```

*此示例获取 prompt 与 response token 使用量的总和。*

---

#### `void resetTokenTracking()`

**Signature**

```cpp
void resetTokenTracking();
```

**Synopsis**  
将所有 token 计数器重置为零。

**Parameters**

* *（无）*

**Return value**

* *（void；不适用）*

**In Depth**

调用 `resetTokenTracking()` 会将以下计数全部设为 0：

* `getPromptTokensUsed()` → 0
* `getResponseTokensUsed()` → 0
* `getTotalTokensUsed()` → 0

这在以下场景中很有用：

* 需要开始一个新的计费周期（例如每次命令执行或每个用户会话）。
* 在测量单次操作的使用量之前，丢弃之前的计数。

重置只会影响未来的调用；一旦重置，之前的使用统计将无法恢复。

**Example**

```cpp
using namespace attoboy;

AI ai(String("https://api.openai.com/v1"),
      String("sk-123456"),
      String("gpt-5-mini"));

ai.ask("First request.", -1);
ai.resetTokenTracking();
ai.ask("Second request.", -1);

int tokensForSecond = ai.getTotalTokensUsed();
```

*此示例在第一次请求后重置 token 跟踪，并仅测量第二次请求的使用量。*

---

#### `String getFinishReason() const`

**Signature**

```cpp
String getFinishReason() const;
```

**Synopsis**  
返回上一次调用的结束原因（例如 `"stop"`、`"length"`）。

**Parameters**

* *（无）*

**Return value**

* 一个 `String`，描述上一次模型调用为何停止生成输出。

**In Depth**

`getFinishReason()` 提供了关于上一次 `ask` 或 `Conversation::ask` 如何结束的线索。常见值包括：

* `"stop"` – 模型自然结束（例如遇到结束标记或满足约束）。
* `"length"` – 模型因为达到最大 token 限制（`setMaxTokens` 或服务限制）而被停止。
* 其他服务特定的值（例如 `"content_filter"`、`"tool_calls"` 等）。

如果尚未发起过任何请求，结束原因可能是空字符串或某个默认值，具体取决于实现。

**Example**

```cpp
using namespace attoboy;

AI ai(String("https://api.openai.com/v1"),
      String("sk-123456"),
      String("gpt-5-mini"));

ai.setMaxTokens(16);
String answer = ai.ask("Explain RAII in detail.", -1);

String reason = ai.getFinishReason();
// 例如，如果解释被截断，则可能为 "length"
```

*此示例检查响应是否因 token 限制而被截断。*

---

#### `String ask(const String &prompt, int timeout = -1)`

**Signature**

```cpp
String ask(const String &prompt, int timeout = -1);
```

**Synopsis**  
发送单个 prompt 并返回响应。出错时通过 `isEmpty()` 检查。

**Parameters**

* `prompt` – 传递给模型的用户输入或指令文本。
* `timeout` – 超时时间（毫秒）。

  * `-1` 表示“使用模型或客户端默认值”（通常被视为无限或已配置的默认超时）。

**Return value**

* 成功时返回包含模型响应文本的 `String`。
* 出错时返回空 `String`（例如网络故障、配置无效或服务错误）。

**In Depth**

`ask` 是与 AI 交互的最简单方式：

* 它会使用当前配置构造单次请求：

  * base URL、API key、模型。
  * system prompt（如果有）。
  * 最大 token 数和 JSON 模式。
* 它会等待响应（或直到达到 `timeout`）。
* 它把模型的回复作为 `String` 返回。

典型的错误检测模式：

* 如果 `ask` 返回空字符串，你应视为失败，并进行相应处理（例如记录日志或重试）。
* 使用 `getFinishReason()` 查看请求是如何结束的。
* 使用 token 跟踪方法衡量使用量。

**超时行为：**

* 正的 `timeout` 值限制客户端等待响应的最长时间。
* `-1` 则将超时行为交由实现和远程服务决定。

**Example**

```cpp
using namespace attoboy;

AI ai(String("https://api.openai.com/v1"),
      String("sk-123456"),
      String("gpt-5-mini"));

String response = ai.ask("Give me a one-line definition of RAII.", 5000);

if (!response.isEmpty()) {
  // 使用该响应，例如打印或记录日志
}
```

*此示例发送一个带 5 秒超时的 prompt，并通过检查空响应来检测错误。*

---

#### `Embedding createEmbedding(const String &str, int dimensions = -1, int timeout = -1)`

**Signature**

```cpp
Embedding createEmbedding(const String &str, int dimensions = -1,
                          int timeout = -1);
```

**Synopsis**  
创建一个 embedding 向量。出错时检查 `getDimensions() == 0`。

**Parameters**

* `str` – 要进行 embedding 的文本（希望捕获其语义含义的输入）。
* `dimensions` – 期望的 embedding 维度数：

  * `-1` 表示由模型自行选择默认维度。
  * 正数表示请求特定长度（前提是服务支持）。
* `timeout` – 请求的超时时间（毫秒，`-1` 表示使用默认行为）。

**Return value**

* 一个表示向量的 `Embedding` 对象。
* 出错时返回 `Embedding`，且其 `getDimensions() == 0`。

**In Depth**

Embedding 会把文本转换为数值向量，用于表达语义相似度：

* 语义相似的文本会产生余弦相似度接近 `1.0` 的 embedding。
* 语义不相似的文本会产生较低或负的相似度。

调用 `createEmbedding` 后：

* 检查 `embedding.getDimensions()`：

  * `> 0` – 成功。
  * `0` – 出错（配置无效、网络问题或服务错误）。
* 使用 `Embedding::compare` 计算两个 embedding 之间的相似度。

`dimensions` 参数允许在服务支持多种向量长度时控制向量大小。较小的向量更快且占用更少内存；较大的向量可能更精确。

**Example**

```cpp
using namespace attoboy;

AI ai(String("https://api.openai.com/v1"),
      String("sk-123456"),
      String("text-embedding-model"));

Embedding e1 = ai.createEmbedding("Hello world", -1, 5000);
Embedding e2 = ai.createEmbedding("Hi there", -1, 5000);

if (e1.getDimensions() > 0 && e2.getDimensions() > 0) {
  float similarity = e1.compare(e2);
  // similarity 接近 1.0 表示文本在语义上非常相似
}
```

*此示例为两个短语创建 embedding，并使用余弦相似度进行比较。*

---

#### `Conversation createConversation()`

**Signature**

```cpp
Conversation createConversation();
```

**Synopsis**  
创建一个新的多轮对话。

**Parameters**

* *（无）*

**Return value**

* 一个与该 `AI` 实例关联的 `Conversation` 对象。

**In Depth**

`createConversation` 会构造一个 `Conversation`，它：

* 使用与创建它的 `AI` 实例相同的 base URL、API key、模型、system prompt、最大 token 数和 JSON 模式。
* 维护自己独立的消息历史：

  * 每次调用 `Conversation::ask` 都会把用户消息和助手回复加入会话。
  * 模型在生成新回复时会看到整个历史记录。

当你需要：

* 有状态的交互，使每个答案依赖此前的问题。
* 在应用中维护类似聊天的界面。

时，应使用 `Conversation`。

你可以从同一个 `AI` 实例创建多个会话；每个会话都有自己独立的历史。

**Example**

```cpp
using namespace attoboy;

AI ai(String("https://api.openai.com/v1"),
      String("sk-123456"),
      String("gpt-5-mini"));

Conversation conv = ai.createConversation();

String reply1 = conv.ask("Hello, who are you?", -1);
String reply2 = conv.ask("Can you remind me what I just asked?", -1);
```

*此示例创建了一个会话，并进行两轮对话，其中第二个问题依赖于第一个问题。*