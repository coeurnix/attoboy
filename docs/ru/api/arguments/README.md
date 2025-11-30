# Arguments

## Overview

Класс `attoboy::Arguments` — небольшой, сфокусированный парсер аргументов командной строки для приложений на attoboy. Он предоставляет структурированный способ объявления:

* **флагов** (булевы переключатели, такие как `-v` или `--verbose`),
* **именованных параметров** (пары ключ–значение, такие как `-output=log.txt`),
* **позиционных параметров** (аргументы, распознаваемые по позиции, например имена входных файлов),

а затем разбора фактической командной строки, с которой был запущен ваш процесс.

Вместо того чтобы вручную проверять `argv` и писать собственную логику разбора, вы можете:

1. Объявить, какие аргументы ожидает ваша программа.
2. Необязательно пометить некоторые аргументы как **обязательные**.
3. Задать читаемые для человека **описания** и **текст помощи**.
4. Вызвать `parseArguments()`, чтобы проверить и получить структурированную `Map` значений.

`Arguments` использует собственные типы attoboy — `String` и `Map`, поэтому он естественно вписывается в остальную часть библиотеки.

### Аргументы командной строки, флаги и параметры

Когда программа запускается из терминала, она обычно получает список **аргументов командной строки**, например:

```text
mytool -verbose -mode=fast input.txt
```

Эти аргументы обычно делятся на три категории:

* **Флаги** — булевы опции, которые либо присутствуют, либо отсутствуют:

  * Пример: `-verbose`, `--dry-run`
  * Часто используются для включения или отключения функций.

* **Именованные параметры** — опции с явными значениями:

  * Пример: `-mode=fast` или `-output result.json`
  * Используются для конфигурации, например путей к файлам, режимов или пороговых значений.

* **Позиционные параметры** — значения, интерпретируемые по их позиции:

  * Пример: `input.txt` или `source` и `destination`
  * Используются для таких вещей, как требуемые входные файлы.

`attoboy::Arguments` абстрагирует сложные детали разбора этих значений из сырой командной строки:

* Он **захватывает** аргументы командной строки процесса при создании объекта `Arguments`.
* Вы **объявляете** флаги, параметры и позиционные аргументы, которые ожидаете.
* Вы вызываете **`parseArguments()`**, который проверяет ввод, автоматически обрабатывает `-h/--help` и возвращает `Map` с разобранными значениями.

### Автоматическая помощь и обязательные аргументы

При использовании `Arguments` парсер автоматически обрабатывает:

* `-h` / `--help`: выводит сообщение помощи (используя описания, которые вы предоставили) и возвращает пустую `Map`.
* **Обязательные аргументы**: если пользователь пропускает обязательный флаг или параметр, `parseArguments()` возвращает пустую `Map`, чтобы вы могли корректно завершить работу вместо запуска с неполной конфигурацией.

Это помогает вам:

* Разделять основную логику программы и проверку командной строки.
* Обеспечивать единообразный пользовательский опыт в инструментах, построенных с помощью attoboy.

---

## Reference

All examples assume:

```cpp
using namespace attoboy;
```

---

#### `Arguments()`

**Signature**

```cpp
Arguments();
```

**Synopsis**
Создаёт парсер Arguments и захватывает аргументы командной строки.

**Parameters**

* *(нет)*

**Return value**

* *(конструктор; неприменимо)*

**In Depth**

При создании объекта `Arguments` захватываются аргументы командной строки текущего процесса. Типичный порядок действий:

1. Создаёте парсер в начале программы.
2. Объявляете, какие флаги и параметры вы ожидаете.
3. Вызываете `parseArguments()`, чтобы интерпретировать захваченные аргументы.

Обычно создаётся один экземпляр `Arguments` на программу, но при необходимости можно создать несколько (например, для разбора под-команд разными способами).

**Example**

```cpp
Arguments args;

// Declare expected options before parsing
args.addFlag("verbose", "Enable verbose output");
args.addParameter("mode", "Processing mode", "fast");

// Later, parse the captured command line
Map parsed = args.parseArguments();
```

*Этот пример создаёт парсер `Arguments`, настраивает ожидаемые опции и готовится к разбору командной строки.*

---

#### `Arguments(const Arguments &other)`

**Signature**

```cpp
Arguments(const Arguments &other);
```

**Synopsis**
Создаёт копию другого парсера Arguments.

**Parameters**

* `other` – существующий экземпляр `Arguments`, который нужно скопировать.

**Return value**

* *(конструктор; неприменимо)*

**In Depth**

Конструктор копирования позволяет дублировать конфигурацию парсера, включая все определённые флаги, параметры, позиционные аргументы и любое захваченное состояние командной строки.

Типичные сценарии использования:

* Повторное использование базовой конфигурации с последующим изменением копии для немного иного поведения разбора.
* Передача состояния парсера вспомогательным компонентам, которые могут хотеть независимо опрашивать аргументы.

**Example**

```cpp
Arguments base;
base.addFlag("verbose", "Enable verbose output");

Arguments copy(base);
// copy now recognizes the same arguments as base
```

*Этот пример копирует настроенный парсер `Arguments` для повторного использования в другом месте.*

---

#### `~Arguments()`

**Signature**

```cpp
~Arguments();
```

**Synopsis**
Уничтожает парсер Arguments и освобождает память.

**Parameters**

* *(нет)*

**Return value**

* *(деструктор; неприменимо)*

**In Depth**

Деструктор освобождает любые внутренние ресурсы, используемые парсером (например, выделенные метаданные или хранилище захваченных аргументов). Поскольку `Arguments` следует идиоме RAII, достаточно позволить объекту выйти из области видимости, чтобы ресурсы были освобождены.

**Example**

```cpp
{
  Arguments args;
  // configure and parse here
} // args is destroyed here; resources are released
```

*Этот пример показывает автоматическую очистку парсера при выходе объекта из области видимости.*

---

#### `Arguments &operator=(const Arguments &other)`

**Signature**

```cpp
Arguments &operator=(const Arguments &other);
```

**Synopsis**
Присваивает этому объекту конфигурацию другого парсера Arguments.

**Parameters**

* `other` – парсер, конфигурация и состояние которого должны быть скопированы.

**Return value**

* Ссылка на `*this`, позволяющая цепочное присваивание.

**In Depth**

Оператор присваивания заменяет текущую конфигурацию парсера и захваченное состояние состоянием `other`. После присваивания:

* Оба объекта распознают одни и те же объявленные аргументы.
* Оба имеют одинаковое состояние разбора (до следующего вызова `parseArguments()`).

Самоприсваивание безопасно.

**Example**

```cpp
Arguments args;
args.addFlag("verbose", "Enable verbose output");

Arguments other;
other = args;  // other now has the same argument definitions
```

*Этот пример присваивает конфигурацию одного парсера другому.*

---

#### `Arguments &addFlag(const String &name, const String &description = String(), bool defaultValue = false, const String &longName = String())`

**Signature**

```cpp
Arguments &addFlag(const String &name, const String &description = String(),
                   bool defaultValue = false,
                   const String &longName = String());
```

**Synopsis**
Добавляет булев флаг (-name, --longName или -name=true/false).

**Parameters**

* `name` – короткое имя флага, используемое как `-name` (например, `"verbose"` → `-verbose`).
* `description` – необязательное человекочитаемое описание, показываемое в тексте помощи.
* `defaultValue` – значение по умолчанию (boolean), если флаг не указан в командной строке.
* `longName` – необязательное длинное имя, используемое как `--longName` (например, `"verbose"` → `--verbose`). Если пусто, поддерживается только `-name`.

**Return value**

* Ссылка на `*this`, позволяющая цепочку вызовов.

**In Depth**

Флаги — это **булевы опции**, которые могут быть включены или выключены. С помощью `addFlag` парсер распознаёт следующие формы:

* `-name` – обычно означает «установить в true».
* `--longName` – длинная версия того же флага (если указана).
* `-name=true` / `-name=false` – явная установка значения.

При разборе:

* Значение флага сохраняется как `String` `"true"` или `"false"` и может быть доступно через `getArgument(name)` или `getArgument(longName)` (в зависимости от реализации).
* `defaultValue` определяет, что возвращает `getArgument()`, если пользователь не установил флаг, и сообщает `hasArgument()`, считается ли аргумент присутствующим.

**Example**

```cpp
Arguments args;

args.addFlag("verbose", "Enable verbose logging", false, "verbose");

Map parsed = args.parseArguments();
if (parsed.isEmpty()) {
  // help shown or required arguments missing
} else {
  bool verbose = args.getArgument("verbose").toBool();
  if (verbose) {
    LogInfo("Verbose mode enabled");
  }
}
```

*Этот пример объявляет флаг `-verbose` / `--verbose` со значением по умолчанию `false` и использует разобранное значение для управления логированием.*

---

#### `Arguments &addParameter(const String &name, const String &description = String(), const String &defaultValue = String(), const String &longName = String())`

**Signature**

```cpp
Arguments &addParameter(const String &name,
                        const String &description = String(),
                        const String &defaultValue = String(),
                        const String &longName = String());
```

**Synopsis**
Добавляет именованный параметр (-name=value или -name value).

**Parameters**

* `name` – короткое имя параметра (например, `"output"` → `-output`).
* `description` – необязательное человекочитаемое описание для вывода помощи.
* `defaultValue` – значение по умолчанию (строка), если параметр не указан.
* `longName` – необязательная длинная форма (например, `"output"` → `--output`). Если пусто, используется только короткая форма.

**Return value**

* Ссылка на `*this`, позволяющая цепочку вызовов.

**In Depth**

Именованные параметры — это **опции ключ–значение**. Парсер обычно принимает следующие формы:

* `-name=value`
* `-name value`
* `--longName=value`
* `--longName value`

Значение сохраняется как `String` и может быть получено с помощью `getArgument(name)`.

Если пользователь опустил параметр:

* `getArgument(name)` возвращает `defaultValue`.
* `hasArgument(name)` возвращает `true`, если задано значение по умолчанию, даже если пользователь его не предоставил.

Это удобно для настроек, таких как пути вывода, режимы или пороги.

**Example**

```cpp
Arguments args;

args.addParameter("output",
                  "Output file path",
                  "result.txt",
                  "output");

Map parsed = args.parseArguments();
if (parsed.isEmpty()) {
  // help or validation failure
} else {
  String outputPath = args.getArgument("output");
  LogInfo("Writing results to ", outputPath);
}
```

*Этот пример объявляет параметр `-output` / `--output` с именем файла по умолчанию и считывает полученное значение после разбора.*

---

#### `Arguments &addPositionalParameter(const String &name, const String &description = String())`

**Signature**

```cpp
Arguments &addPositionalParameter(const String &name,
                                  const String &description = String());
```

**Synopsis**
Добавляет позиционный параметр (заполняется по порядку из оставшихся аргументов).

**Parameters**

* `name` – имя, используемое для идентификации этого позиционного параметра в результирующей карте.
* `description` – необязательное человекочитаемое описание, показываемое в помощи.

**Return value**

* Ссылка на `*this`, позволяющая цепочку вызовов.

**In Depth**

Позиционные параметры представляют собой аргументы, которые распознаются **по позиции**, а не с префиксом `-` или `--`. После разбора всех флагов и именованных параметров парсер присваивает оставшиеся сырые аргументы по порядку объявленным позиционным параметрам.

Например, если вы определите:

```text
addPositionalParameter("input");
addPositionalParameter("output");
```

и пользователь запустит:

```text
mytool -mode=fast in.txt out.txt
```

то:

* `"input"` получит `"in.txt"`,
* `"output"` получит `"out.txt"`.

Вы получаете эти значения через `getArgument("input")` и `getArgument("output")`.

**Example**

```cpp
Arguments args;

args.addPositionalParameter("input",  "Input file");
args.addPositionalParameter("output", "Output file");

Map parsed = args.parseArguments();
if (parsed.isEmpty()) {
  // help or required arguments missing
} else {
  String input  = args.getArgument("input");
  String output = args.getArgument("output");
  LogInfo("Copying from ", input, " to ", output);
}
```

*Этот пример определяет два позиционных параметра и использует их как пути источника и назначения.*

---

#### `Arguments &setHelp(const String &helpText)`

**Signature**

```cpp
Arguments &setHelp(const String &helpText);
```

**Synopsis**
Устанавливает пользовательский текст помощи для `-h/--help`.

**Parameters**

* `helpText` – `String`, содержащая пользовательский текст помощи или описание использования, отображаемое при запросе `-h` или `--help`.

**Return value**

* Ссылка на `*this`, позволяющая цепочку вызовов.

**In Depth**

По умолчанию парсер может генерировать текст помощи на основе:

* Добавленных аргументов,
* Их имён и описаний.

`setHelp()` позволяет переопределить или дополнить это собственным содержимым. Когда пользователь передаёт `-h` или `--help`, `parseArguments()`:

* Выводит текст помощи (возможно ваш пользовательский), и
* Возвращает пустую `Map`, чтобы вы могли прервать нормальный путь выполнения.

**Example**

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
  // Help was printed or validation failed; exit the program.
}
```

*Этот пример задаёт пользовательское сообщение об использовании, которое показывается при запросе помощи.*

---

#### `Arguments &requireArgument(const String &name)`

**Signature**

```cpp
Arguments &requireArgument(const String &name);
```

**Synopsis**
Помечает аргумент как обязательный.

**Parameters**

* `name` – имя аргумента, который нужно пометить как обязательный. Это может быть имя флага, именованного или позиционного параметра.

**Return value**

* Ссылка на `*this`, позволяющая цепочку вызовов.

**In Depth**

`requireArgument()` сообщает парсеру, что определённый аргумент **обязан** быть предоставлен пользователем. Если аргумент отсутствует при вызове `parseArguments()`:

* Парсер считает это ошибкой.
* Возвращает пустую `Map`.
* Может напечатать сообщение об ошибке и помощь, чтобы уведомить пользователя (в зависимости от реализации).

Вы можете вызывать его цепочкой, чтобы пометить несколько аргументов как обязательные.

**Example**

```cpp
Arguments args;

args.addPositionalParameter("input", "Input file");
args.addParameter("mode", "Processing mode", "fast");
args.requireArgument("input");  // input is mandatory

Map parsed = args.parseArguments();
if (parsed.isEmpty()) {
  // Either help was shown or "input" was missing; abort.
} else {
  String input = args.getArgument("input");
  String mode  = args.getArgument("mode");
  LogInfo("Processing ", input, " in mode ", mode);
}
```

*Этот пример требует, чтобы позиционный параметр `input` был обязательно предоставлен.*

---

#### `String getArgument(const String &name) const`

**Signature**

```cpp
String getArgument(const String &name) const;
```

**Synopsis**
Возвращает значение аргумента (флаги возвращают "true" или "false").

**Parameters**

* `name` – имя аргумента, значение которого вы хотите получить. Это то же имя, которое использовалось в `addFlag`, `addParameter` или `addPositionalParameter`.

**Return value**

* `String`, представляющая значение аргумента:

  * Флаги: `"true"` или `"false"`.
  * Именованные параметры: переданное значение или значение по умолчанию.
  * Позиционные параметры: значение на данной позиции, если оно было предоставлено.

**In Depth**

`getArgument()` — основной способ получить разобранные значения аргументов после вызова `parseArguments()`.

Поведение:

* Если именованный параметр не предоставлен в командной строке, но имеет `defaultValue`, возвращается это значение по умолчанию.
* Если флаг не предоставлен, но имеет `defaultValue`, возвращается соответствующий булевый текст (`"true"` или `"false"`).
* Если параметр отсутствует и не имеет значения по умолчанию, поведение определяется реализацией; обычно возвращается пустая строка.

Чтобы избежать неоднозначности между «отсутствует» и «присутствует с пустым значением», можно комбинировать `getArgument()` с `hasArgument()`.

**Example**

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

*Этот пример получает значения флага, параметра и позиционного аргумента с помощью `getArgument()`.*

---

#### `bool hasArgument(const String &name) const`

**Signature**

```cpp
bool hasArgument(const String &name) const;
```

**Synopsis**
Возвращает true, если аргумент был установлен или имеет значение по умолчанию.

**Parameters**

* `name` – имя проверяемого аргумента.

**Return value**

* `true`, если пользователь предоставил аргумент или если для него задано значение по умолчанию.
* `false` в противном случае.

**In Depth**

`hasArgument()` сообщает, доступен ли аргумент **логически** после разбора. Это включает в себя:

* Аргументы, явно установленные в командной строке.
* Аргументы, у которых настроены значения по умолчанию (для именованных параметров и флагов).

Это особенно полезно, когда:

* Нужно отличать «аргумент не входит в конфигурацию» от «аргумент существует, но может быть пустым».
* Нужно знать, поступило ли значение из конфигурации или оно подразумевается.

Обратите внимание, что обязательный аргумент, который отсутствует, заставит `parseArguments()` вернуть пустую `Map`, поэтому обычно проверяют пустоту результата перед вызовом `hasArgument()`.

**Example**

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

*Этот пример проверяет, логически ли доступен параметр, а затем считывает его.*

---

#### `Map parseArguments(bool suppressHelp = false)`

**Signature**

```cpp
Map parseArguments(bool suppressHelp = false);
```

**Synopsis**
Разбирает аргументы и возвращает `Map`. Пустой, если показана помощь или отсутствуют обязательные аргументы.

**Parameters**

* `suppressHelp` – если `false` (по умолчанию), парсер автоматически показывает помощь и сообщения об ошибках, когда это уместно (например, при `-h` / `--help` или при отсутствии обязательных аргументов). Если `true`, разбор выполняется и возвращается результат без автоматического показа помощи; вы сами обрабатываете помощь и ошибки.

**Return value**

* `Map`, содержащая разобранные аргументы, или **пустая карта**, если:

  * Запрошена помощь (`-h` или `--help`), или
  * Отсутствует один или несколько обязательных аргументов, или
  * Произошла другая ошибка разбора (в зависимости от реализации).

**In Depth**

`parseArguments()` — центральная операция:

1. Она интерпретирует захваченные аргументы командной строки, используя конфигурацию, которую вы построили с помощью `addFlag`, `addParameter`, `addPositionalParameter` и `requireArgument`.
2. Валидирует ввод (обязательные аргументы, формат значений и т. п.).
3. Либо:

   * Возвращает непустую `Map` разобранных значений, либо
   * Возвращает пустую `Map` и (если `suppressHelp` не равно `true`) отображает помощь и/или сообщения об ошибках.

Возвращаемая `Map` часто используется для итерации или внешних API, но в большинстве программ вы в основном используете `getArgument()` и `hasArgument()` для удобства.

Типичный шаблон использования:

```cpp
Map parsed = args.parseArguments();
if (parsed.isEmpty()) {
  // Do not continue; user needs help or must fix input.
  return;
}
// Safely use arguments here.
```

Если вы передаёте `suppressHelp = true`, вы сами можете обнаруживать запросы помощи, анализируя аргументы и выводя пользовательские сообщения по необходимости.

**Example**

```cpp
Arguments args;

args.addFlag("verbose", "Enable verbose output");
args.addParameter("mode", "Processing mode", "fast");
args.addPositionalParameter("input", "Input file");
args.requireArgument("input");

Map parsed = args.parseArguments();
// If help was requested or "input" is missing, parsed will be empty.
if (parsed.isEmpty()) {
  // Abort program logic; user either saw help or needs to fix arguments.
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

*Этот пример демонстрирует типичный полный поток: настройка аргументов, принуждение обязательного параметра, разбор командной строки и дальнейшее использование полученных значений.*

---

#### `List getAllArguments() const`

**Signature**

```cpp
List getAllArguments() const;
```

**Synopsis**
Возвращает список всех сырых аргументов командной строки (включая `argv[0]`).

**Parameters**

* *(нет)*

**Return value**

* `List` из значений `String`, содержащая все аргументы командной строки, захваченные объектом `Arguments`:

  * Индекс `0` соответствует `argv[0]` (обычно имя программы).
  * Последующие элементы соответствуют каждому токену аргумента в порядке, в котором операционная система передала их процессу.

**In Depth**

В то время как `parseArguments()` даёт вам валидационный, структурированный вид командной строки, `getAllArguments()` показывает **сырые** аргументы ровно так, как они были захвачены при создании экземпляра `Arguments`.

Это полезно, когда вы:

* Хотите записать или зафиксировать полную командную строку для отладки или диагностики.
* Нужно реализовать собственную логику разбора для особых случаев, не покрываемых `addFlag`, `addParameter` или `addPositionalParameter`.
* Хотите сравнить вашу высокоуровневую разобранную конфигурацию с оригинальным `argv`, полученным процессом.

Возвращаемый `List` независим от разобранной `Map`; вызов `parseArguments()` не изменяет список сырых аргументов. Как правило, каждая запись в списке соответствует одному элементу традиционного массива `argv[]` в C или C++.

**Example**

```cpp
Arguments args;

// Inspect or log the raw command line
List all = args.getAllArguments();

for (int i = 0; i < all.size(); ++i) {
  LogInfo("argv[", i, "] = ", all[i]);
}
```

*Этот пример получает все сырые аргументы командной строки, включая имя программы, и логирует их в порядке для отладки или аудита.*