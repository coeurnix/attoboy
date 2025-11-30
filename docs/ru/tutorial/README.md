# Учебник

---

## Добро пожаловать, Программист

Есть особое удовольствие в написании программ, которые просто работают — программы, которые компилируются в мгновение, создают крошечные исполняемые файлы, которыми можно поделиться с кем угодно, и не требуют от пользователей установки чего-то особенного. Именно в такой мир приглашает attoboy.

Возможно, вы писали скрипты на Python или приложения на JavaScript. Может быть, вы слышали, что C++ мощный, но пугающий — полный ловушек управления памятью, загадочного синтаксиса и библиотек, понимание которых требует ученой степени. Мы здесь, чтобы сказать: так быть не обязательно.

Attoboy — это библиотека на C++ с радикальной философией: а что если C++ мог бы быть таким же удобным, как Python, при этом порождая исполняемые файлы меньше большинства изображений? Что если вам никогда не придется думать об выделении памяти, арифметике указателей или десятках конкурирующих способов представления текста? Что если всё просто... работает?

Этот учебник проведет вас по возможностям attoboy. К концу вы будете писать реальные, полезные программы — консольные утилиты, файловые инструменты, веб-клиенты, даже приложения с поддержкой ИИ — всё на чистом, читабельном C++, который компилируется в исполняемые файлы обычно от 10 до 100 килобайт.

Предварительный опыт в C++ не требуется. Если вы понимаете, что такое переменные, функции и циклы, вы готовы. Начнём.

---

## Ваша первая программа на Attoboy

Каждая программа attoboy имеет одинаковую базовую форму:

```cpp
#include <attoboy/attoboy.h>

using namespace attoboy;

void atto_main() {
  Log("Hello, world!");
  Exit();
}
```

Давайте разберёмся, что здесь происходит. Первая строка подключает библиотеку attoboy — этот одиночный заголовок даёт доступ ко всему, о чём мы будем говорить. Вторая строка, `using namespace attoboy;`, позволяет писать `String` вместо `attoboy::String`, делая код чистым и читабельным.

Функция `atto_main()` — это точка входа вашей программы. В отличие от традиционных C++ программ, которые стартуют с функции `main`, программы attoboy начинаются здесь. Не беспокойтесь о причине — просто имейте в виду, что это ваш входной пункт, место, где начинается история вашей программы.

Внутри мы используем `Log()` для вывода сообщения. Это способ attoboy отправлять текст на консоль (или в файл, если вы настроите). Наконец, `Exit()` сообщает программе, что мы закончили. Каждая программа attoboy должна завершаться вызовом `Exit()`.

Вот и всё. Это полная, рабочая программа. После компиляции она создаёт исполняемый файл всего в несколько килобайт, который будет работать на любой машине с Windows без дополнительных файлов или установок.

---

## Строка: ваш постоянный спутник

Если есть один тип, который вы будете использовать чаще других в attoboy, так это `String`. Текст везде в программировании — имена файлов, ввод пользователя, сообщения, форматы данных, сетевое общение — и `String` в attoboy создан, чтобы работать с текстом было приятно.

### Создание строк

Строки можно создавать разными способами:

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

Обратите внимание на прекрасную вещь: `String` в attoboy автоматически конвертирует числа и булевы значения в текст. Никаких функций форматирования, никаких головных болей с приведением типов. Пишете то, что имеете в виду.

### Комбинирование строк

Строки можно соединять оператором `+`, как вы и ожидаете:

```cpp
void atto_main() {
  String first = "Good";
  String second = "morning";
  String message = first + " " + second + "!";
  
  Log(message);  // "Good morning!"
  Exit();
}
```

Но attoboy предлагает ещё более удобный способ. Можно создать строку из множества частей за один раз:

```cpp
void atto_main() {
  String name = "Alice";
  int age = 30;
  
  String bio = String("Name: ", name, ", Age: ", age);
  Log(bio);  // "Name: Alice, Age: 30"
  
  Exit();
}
```

Этот вариативный конструктор принимает любое количество значений — строки, числа, булевы значения — и бесшовно объединяет их.

### Изучение содержимого строки

Строки знают о себе многое:

```cpp
void atto_main() {
  String text = "Hello, 世界!";
  
  Log("Length: ", text.length());    // 9 символов
  Log("Bytes: ", text.byteLength());   // 14 байт (UTF-8!)
  Log("Empty? ", text.isEmpty());    // ложь
  
  Exit();
}
```

Различие между `length()` и `byteLength()` важно. Attoboy использует кодировку UTF-8 повсеместно, а это значит, что символы вроде китайских иероглифов или эмодзи могут занимать несколько байтов. Метод `length()` считает фактические символы (то, что программисты называют "кодовыми точками"), тогда как `byteLength()` считает сырые байты. Для большинства задач вам понадобится `length()`.

### Поиск и проверка

Строки могут ответить на вопросы о своём содержимом:

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

### Извлечение частей

Можно вытащить части строки:

```cpp
void atto_main() {
  String text = "Hello, World!";
  
  String first = text.at(0);       // "H"
  String last = text.at(-1);       // "!"  (отрицательные индексы отсчитываются с конца)
  String middle = text.substring(0, 5); // "Hello"
  String end = text.substring(7);     // "World!"
  
  Log(first, " ... ", last);
  Log(middle);
  Log(end);
  
  Exit();
}
```

Отрицательные индексы — небольшой подарок: `at(-1)` вернёт последний символ, `at(-2)` — предпоследний и так далее. Этот приём встречается по всему attoboy и будет казаться естественным тем, кто использовал Python.

### Преобразование строк

Строки в attoboy являются неизменяемыми — они не могут быть изменены после создания. Вместо этого методы вроде `upper()` и `replace()` возвращают новые строки:

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

Такая неизменяемость может показаться ограничением, но на самом деле это благо. Вам никогда не придётся гадать, изменила ли какая-то функция вашу строку — нет. Оригинал всегда остаётся в безопасности. Этот подход устраняет целый класс ошибок, которые преследуют программы, написанные в других стилях.

### Разделение и объединение

Часто нужно разбить строку на части или объединить куски в одну строку:

```cpp
void atto_main() {
  String csv = "apple,banana,cherry,date";
  List fruits = csv.split(",");
  
  // fruits теперь содержит ["apple", "banana", "cherry", "date"]
  
  String reunited = " - ".join(fruits);
  Log(reunited);  // "apple - banana - cherry - date"
  
  // Разделить по пробелам
  String sentence = "The quick brown fox";
  List words = sentence.split();
  Log("Word count: ", words.length());  // 4
  
  // Разделить на строки
  String poem = "Roses are red\nViolets are blue";
  List lines = poem.lines();
  Log("Line count: ", lines.length());  // 2
  
  Exit();
}
```

### Шаблонное форматирование

Для более сложной сборки строк attoboy предоставляет форматирование с шаблонами:

```cpp
void atto_main() {
  // Позиционные заполнители
  String template1 = "Hello, {0}! You are {1} years old.";
  String result1 = template1.format(List("Alice", 30));
  Log(result1);  // "Hello, Alice! You are 30 years old."
  
  // Именованные заполнители
  String template2 = "Dear {name}, your order #{order} is ready.";
  String result2 = template2.format(Map("name", "Bob", "order", 12345));
  Log(result2);  // "Dear Bob, your order #12345 is ready."
  
  Exit();
}
```

### Преобразование в числа и обратно

Строки, выглядящие как числа, можно превратить в реальные числа:

```cpp
void atto_main() {
  String numStr = "42";
  String floatStr = "3.14159";
  String boolStr = "yes";
  
  int num = numStr.toInteger();    // 42
  float pi = floatStr.toFloat();   // 3.14159
  bool flag = boolStr.toBool();    // true
  
  // Проверяем перед преобразованием
  String maybeNumber = "hello";
  if (maybeNumber.isNumber()) {
    Log("It's a number!");
  } else {
    Log("Not a number");  // Вот это
  }
  
  Exit();
}
```

Метод `toBool()` терпим — он распознаёт "true", "yes", "1" и "on" (без учёта регистра) как истинные значения. Всё остальное считается false. Это удобно при разборе конфигурационных файлов или ввода пользователя.

---

## Коллекции: List, Map и Set

Отдельные куски данных полезны, но программы обычно работают с коллекциями — группами связанных элементов. Attoboy предоставляет три мощных типа коллекций, каждый из которых предназначен для разных задач.

### List: упорядоченные коллекции

`List` — это упорядоченная последовательность значений. В отличие от массивов во многих языках, `List` в attoboy может хранить значения разных типов:

```cpp
void atto_main() {
  // Создаём список с начальными значениями
  List groceries("milk", "bread", "eggs", "butter");
  
  Log("Shopping list has ", groceries.length(), " items");
  
  // Доступ по индексу
  String first = groceries.at(0);   // "milk"
  String last = groceries.at(-1);   // "butter"
  Log("First: ", first, ", Last: ", last);
  
  // Добавляем элементы
  groceries.append("cheese");
  groceries.prepend("coffee");
  
  // Можно смешивать типы!
  List mixed(42, "hello", true, 3.14f);
  
  Exit();
}
```

Обратите внимание на синтаксис `groceries.at(0)`. Если предпочитаете, можно также использовать оператор `[]` :

```cpp
void atto_main() {
  List groceries("milk", "bread", "eggs", "butter");
  Log("First: ", groceries[0], ", Last: ", groceries[-1]);
  Exit();
}
```

Списки поддерживают цепочки вызовов — большинство операций возвращают сам список, поэтому можно писать выразительный код:

```cpp
void atto_main() {
  List numbers;
  numbers.append(3).append(1).append(4).append(1).append(5).append(9);
  
  // Сортируем на месте
  numbers.sort();  // Теперь: 1, 1, 3, 4, 5, 9
  
  // Переворачиваем на месте
  numbers.reverse();  // Теперь: 9, 5, 4, 3, 1, 1
  
  // Проверяем содержимое
  if (numbers.contains(5)) {
    Log("Found 5!");
  }
  
  int pos = numbers.find(4);
  Log("4 is at position ", pos);
  
  Exit();
}
```

Можно извлечь части списка:

```cpp
void atto_main() {
  List letters("a", "b", "c", "d", "e");
  
  List middle = letters.slice(1, 4);  // ["b", "c", "d"]
  List fromEnd = letters.slice(-2, letters.length());  // ["d", "e"]
  
  Exit();
}
```

### Map: пары ключ-значение

`Map` связывает ключи со значениями. Это идеально для структурированных данных, конфигурации или любой ситуации, где нужно что-то искать по имени:

```cpp
void atto_main() {
  // Создаём с парами ключ-значение
  Map person("name", "Alice", "age", 30, "city", "Seattle");
  
  // Получаем значения
  String name = person.get("name", "Unknown");
  int age = person.get("age", 0);
  
  Log(name, " is ", age, " years old");
  
  // Проверяем наличие ключей
  if (person.hasKey("email")) {
    Log("Has email");
  } else {
    Log("No email on record");
  }
  
  // Добавляем или обновляем
  person.put("email", "alice@example.com");
  person.put("age", 31);  // Happy birthday!
  
  Exit();
}
```

Можно просмотреть содержимое карты:

```cpp
void atto_main() {
  Map config("debug", true, "timeout", 30, "name", "MyApp");
  
  List allKeys = config.keys();
  List allValues = config.values();
  
  Log("Configuration has ", config.length(), " settings");
  
  Exit();
}
```

### Set: уникальные значения

`Set` — это коллекция, где каждое значение встречается не более одного раза. Дубликаты игнорируются:

```cpp
void atto_main() {
  Set colors("red", "green", "blue", "red", "green");  // Only 3 unique
  
  Log("Unique colors: ", colors.length());  // 3
  
  if (colors.contains("purple")) {
    Log("Has purple");
  } else {
    Log("No purple");
  }
  
  colors.put("purple");  // Now we have it
  colors.remove("red");  // Gone
  
  Exit();
}
```

Множества поддерживают математические операции:

```cpp
void atto_main() {
  Set a(1, 2, 3, 4);
  Set b(3, 4, 5, 6);
  
  Set both = a.duplicate();
  both.intersect(b);  // 3, 4 (in both)
  
  Set either = a.duplicate();
  either.setUnion(b);  // 1, 2, 3, 4, 5, 6 (in either)
  
  Set onlyA = a.duplicate();
  onlyA.subtract(b);  // 1, 2 (in a but not b)
  
  Exit();
}
```

### JSON и CSV

Коллекции органично интегрируются с распространёнными форматами данных:

```cpp
void atto_main() {
  // Создаём данные
  Map user("name", "Bob", "age", 25);
  
  // Преобразуем в JSON
  String json = user.toJSONString();
  Log(json);  // {"name":"Bob","age":25}
  
  // Разбираем JSON обратно
  Map parsed = Map::FromJSONString(json);
  Log("Name from JSON: ", parsed.get("name", ""));
  
  // CSV для табличных данных
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

## Работа с файлами

Программы, которые не умеют взаимодействовать с файловой системой, ограничены в том, что могут сделать. Attoboy упрощает файловые операции с помощью двух кооперативных типов: `Path` для метаданных и навигации по файловой системе и `File` для чтения и записи содержимого.

### Path: навигация по файловой системе

`Path` представляет местоположение в файловой системе. Он может рассказать, что там находится, и помочь манипулировать файлами:

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

Пути могут предоставить детальную информацию о файлах:

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

### Чтение и запись файлов

Для простых операций у `Path` есть удобные методы:

```cpp
void atto_main() {
  Path configPath("settings.txt");
  
  // Записываем строку в файл
  configPath.writeFromString("theme=dark\nfontSize=14\n");
  
  // Читаем обратно
  String contents = configPath.readToString();
  Log("Config contents:\n", contents);
  
  // Дописываем ещё
  configPath.appendFromString("language=en\n");
  
  Exit();
}
```

### Перечисление содержимого каталога

Можно исследовать, что находится внутри каталога:

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

### Создание каталогов и перемещение файлов

```cpp
void atto_main() {
  Path newDir("my_project");
  
  // Создаём каталог (при необходимости создаёт родительские)
  if (newDir.makeDirectory()) {
    Log("Created ", newDir.toString());
  }
  
  // Создаём файл внутри
  Path readme(newDir.toString() + "\\README.txt");
  readme.writeFromString("# My Project\n\nWelcome!");
  
  // Копируем его
  Path backup(newDir.toString() + "\\README.backup.txt");
  readme.copyTo(backup);
  
  // Перемещаем/переименовываем
  Path newName(newDir.toString() + "\\README.md");
  readme.moveTo(newName);
  
  Exit();
}
```

### Временные файлы

Когда вам нужно место для промежуточных данных:

```cpp
void atto_main() {
  Path tempFile = Path::CreateTemporaryFile("myapp_", ".tmp");
  Log("Temporary file: ", tempFile.toString());
  
  tempFile.writeFromString("Temporary data here");
  
  // Не забудьте удалить файл, когда закончите
  tempFile.deleteFile();
  
  Exit();
}
```

### Класс File: потоковый ввод/вывод

Для более тонкого контроля чтения и записи — особенно для больших файлов или сетевых подключений — используйте класс `File`:

```cpp
void atto_main() {
  Path logPath("application.log");
  
  File logFile(logPath);
  if (logFile.isValid()) {
    logFile.write("Application started\n");
    logFile.write("Performing initialization...\n");
    logFile.flush();  // Гарантирует запись данных на диск
  }
  
  // Читаем обратно
  logFile.setPosition(0);  // Возвращаемся в начало
  String contents = logFile.readAllToString();
  Log("Log contents: ", contents);
  
  logFile.close();
  
  Exit();
}
```

---

## Аргументы командной строки

Большинство полезных программ принимают ввод из командной строки. Класс `Arguments` в attoboy делает разбор аргументов простым:

```cpp
void atto_main() {
  Arguments args;
  
  // Описываем, какие аргументы принимаем
  args.addFlag("v", "Enable verbose output", false, "verbose");
  args.addFlag("h", "Show help", false, "help");
  args.addParameter("o", "Output file path", "output.txt", "output");
  args.addPositionalParameter("input", "Input file to process");
  
  // Разбираем аргументы
  Map parsed = args.parseArguments();
  
  if (parsed.isEmpty()) {
    Exit(1);  // User asked for help or required args missing
  }
  
  bool verbose = parsed.get("v", "false").toBool();
  String outputPath = parsed.get("o", "");
  String inputPath = parsed.get("input", "");
  
  if (verbose) {
    Log("Processing ", inputPath, " -> ", outputPath);
  }
  
  // ... do the actual work ...
  
  Exit();
}
```

С такой конфигурацией ваша программа автоматически поддерживает:

* `myprogram.exe -h` или `myprogram.exe --help` (показывает помощь)
* `myprogram.exe -v --output=result.txt input.txt`
* `myprogram.exe input.txt -o output.txt --verbose`

Аргументы могут появляться в любом порядке, флаги и параметры принимают как короткую (`-v`), так и длинную (`--verbose`) формы.

---

## Работа со временем

Класс `DateTime` работает с метками времени с точностью до миллисекунд:

```cpp
void atto_main() {
  // Текущее время
  DateTime now;
  Log("Current time: ", now.toString());
  
  // Из строки в формате ISO
  DateTime birthday("2000-01-15T08:30:00.000Z");
  
  // Разница (в миллисекундах)
  long long ageMs = now.diff(birthday);
  long long ageYears = ageMs / (1000LL * 60 * 60 * 24 * 365);
  Log("Approximate age: ", ageYears, " years");
  
  // Прибавляем время
  DateTime tomorrow = DateTime(now);
  tomorrow.add(24 * 60 * 60 * 1000);  // Add 24 hours in milliseconds
  Log("Tomorrow: ", tomorrow.toString());
  
  Exit();
}
```

---

## Бинарные данные и Buffer

Если `String` для текста, то `Buffer` обрабатывает сырые бинарные данные — содержимое файлов, зашифрованные данные, сетевые пакеты и тому подобное:

```cpp
void atto_main() {
  // Create from string
  Buffer data("Hello, binary world!");
  Log("Buffer size: ", data.length(), " bytes");
  
  // Convert back
  String text = data.toString();
  Log("As string: ", text);
  
  Exit();
}
```

### Сжатие

Buffers можно сжимать с помощью LZ4 — крайне быстрой библиотеки:

```cpp
void atto_main() {
  // Создаём немного повторяющихся данных
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

### Шифрование

Buffers поддерживают шифрование ChaCha20 — современный, безопасный шифр:

```cpp
void atto_main() {
  String secret = "This is my secret message!";
  Buffer plaintext(secret);
  
  // Key must be at least 32 bytes, nonce at least 12 bytes
  String key = "my-32-byte-encryption-key-here!!";
  String nonce = "unique-nonce";
  
  // Шифруем
  Buffer ciphertext = plaintext.crypt(key, nonce);
  Log("Encrypted (base64): ", ciphertext.toBase64());
  
  // Расшифровываем (та же операция с тем же ключом/nonce)
  Buffer decrypted = ciphertext.crypt(key, nonce);
  Log("Decrypted: ", decrypted.toString());
  
  Exit();
}
```

### Base64-кодирование

Для передачи бинарных данных в виде текста:

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

## Обращение в сеть: Web-запросы

Attoboy делает HTTP-запросы простыми:

```cpp
void atto_main() {
  // Простой GET-запрос
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

### POST-запросы

Отправка данных на сервер:

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

### Загрузка файлов

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

## Общение с ИИ

Одна из самых мощных возможностей attoboy — встроенная поддержка языковых моделей ИИ через любой API, совместимый с OpenAI:

```cpp
void atto_main() {
  // Подключаемся к сервису ИИ
  AI ai(
    "https://api.openai.com/v1",       // Base URL
    GetEnv("OPENAI_API_KEY"),        // API key from environment
    "gpt-5-mini"                 // Model name
  );
  
  // Задаём системный промпт (необязательно)
  ai.setSystemPrompt("You are a helpful assistant who gives concise answers.");
  
  // Задаём вопрос
  String answer = ai.ask("What is the capital of France?");
  
  if (!answer.isEmpty()) {
    Log("AI says: ", answer);
  } else {
    Log("Error getting response");
  }
  
  Exit();
}
```

### Многократный диалог

Для диалога с сохранением контекста:

```cpp
void atto_main() {
  AI ai("https://api.openai.com/v1", GetEnv("OPENAI_API_KEY"), "gpt-5-mini");
  Conversation chat = ai.createConversation();
  
  Log(chat.ask("Hi! I'm learning about astronomy."));
  Log(chat.ask("What's the closest star to Earth?"));
  Log(chat.ask("How long would it take to travel there?"));
  
  // ИИ запоминает контекст разговора
  
  Exit();
}
```

### Эмбеддинги для семантического поиска

Эмбеддинги преобразуют текст в числовые векторы, которые захватывают смысл, позволяя сравнивать семантическую близость:

```cpp
void atto_main() {
  AI ai("https://api.openai.com/v1", GetEnv("OPENAI_API_KEY"), "text-embedding-3-small");
  
  Embedding dogEmbed = ai.createEmbedding("dog");
  Embedding puppyEmbed = ai.createEmbedding("puppy");
  Embedding carEmbed = ai.createEmbedding("car");
  
  float dogPuppy = dogEmbed.compare(puppyEmbed);  // Высокое сходство
  float dogCar = dogEmbed.compare(carEmbed);    // Низкое сходство
  
  Log("dog vs puppy similarity: ", dogPuppy);
  Log("dog vs car similarity: ", dogCar);
  
  Exit();
}
```

---

## Создание консольных интерфейсов

Для интерактивных командных приложений attoboy предоставляет богатый класс `Console`:

```cpp
void atto_main() {
  Console con;
  
  // Цветной вывод
  con.println("Welcome!", CON_CYAN);
  con.print("Status: ", CON_WHITE);
  con.println("OK", CON_GREEN);
  
  // Форматированный вывод
  con.printWrapped("This is a long paragraph that will automatically "
    "wrap to fit the console width, making it easy to display "
    "large amounts of text neatly.", -1, CON_GRAY);
  
  Exit();
}
```

### Интерактивный ввод

`Console` предоставляет продвинутую обработку ввода:

```cpp
void atto_main() {
  Console con;
  
  // Простой ввод
  String name = con.input("Enter your name: ");
  con.println(String("Hello, ", name, "!"), CON_YELLOW);
  
  // Ввод пароля (маскируется звёздочками)
  ConsoleInput passOptions;
  passOptions.password = true;
  String password = con.input("Password: ", passOptions);
  
  // Ввод с историей и автодополнением по Tab
  ConsoleInput shellOptions;
  shellOptions.completions = List("help", "exit", "status", "run", "config");
  
  String command = con.input("> ", shellOptions);
  
  Exit();
}
```

### Индикаторы прогресса

Для долгих операций:

```cpp
void atto_main() {
  Console con;
  
  con.println("Processing files...");
  
  for (int i = 0; i <= 100; i++) {
    con.progress(i / 100.0f, String("Progress: ", i, "%"));
    Sleep(50);  // Имитируем работу
  }
  
  con.progressHide();
  con.println("Done!", CON_GREEN);
  
  Exit();
}
```

### Интерактивные меню

```cpp
void atto_main() {
  Console con;
  
  List options("Start new game", "Load saved game", "Options", "Exit");
  int choice = con.menu(options, "Main Menu");
  
  if (choice == 0) {
    con.println("Starting new game...");
  } else if (choice == 3 || choice == -1) {  // -1 если пользователь нажал Escape
    con.println("Goodbye!");
  }
  
  Exit();
}
```

---

## Запуск внешних программ

Иногда нужно запустить внешние программы. Класс `Subprocess` справляется с этим:

```cpp
void atto_main() {
  // Запускаем и ждём завершения
  int exitCode = Subprocess::Run(Path("C:\\Windows\\notepad.exe"));
  Log("Notepad exited with code ", exitCode);
  
  // Захватываем вывод
  String output = Subprocess::Capture(Path("C:\\Windows\\System32\\ipconfig.exe"), "/all");
  Log("IP Configuration:\n", output);
  
  // Запускаем и не ждём
  Subprocess::Start(Path("C:\\Windows\\explorer.exe"), "C:\\");
  
  Exit();
}
```

Для интерактивного общения с подпроцессом:

```cpp
void atto_main() {
  Subprocess proc(Path("C:\\Windows\\System32\\cmd.exe"));
  
  // Отправляем команды
  proc.write("echo Hello from attoboy!\r\n");
  proc.write("exit\r\n");
  
  // Ждём и читаем вывод
  proc.wait();
  String output = proc.readToString();
  Log("Output:\n", output);
  
  Exit();
}
```

---

## Потоки для параллельной работы

Когда нужно делать несколько вещей одновременно:

```cpp
void *backgroundTask(void *arg) {
  String name = *((String *)arg);
  Log("Background task started: ", name);
  Sleep(2000);  // Имитируем работу
  Log("Background task finished: ", name);
  return nullptr;
}

void atto_main() {
  String taskName = "DataProcessor";
  Thread worker(backgroundTask, &taskName);
  
  Log("Main thread continues...");
  Sleep(1000);
  Log("Main thread waiting for worker...");
  
  worker.await();  // Wait for completion
  Log("All done!");
  
  Exit();
}
```

Для защиты общих данных:

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
  
  Log("Final counter value: ", sharedCounter);  // Гарантированно 2000
  
  Exit();
}
```

---

## Математические операции

Класс `Math` предоставляет все стандартные математические функции:

```cpp
void atto_main() {
  // Константы
  Log("Pi: ", Math::PI);
  Log("E: ", Math::E);
  
  // Случайные числа
  Log("Random int: ", Math::Random());
  Log("Random float [0,1): ", Math::RandomFloat());
  Log("Random in range [1,10): ", Math::RandomRange(1, 10));
  Log("Random bool: ", Math::RandomBool());
  
  // Обычные операции
  Log("Abs(-5): ", Math::Abs(-5));
  Log("Min(3,7): ", Math::Min(3, 7));
  Log("Max(3,7): ", Math::Max(3, 7));
  Log("Clamp(15, 0, 10): ", Math::Clamp(15, 0, 10));  // 10
  
  // Тригонометрия (в радианах)
  float angle = Math::PI / 4;  // 45 degrees
  Log("Sin(45°): ", Math::Sin(angle));
  Log("Cos(45°): ", Math::Cos(angle));
  
  // Степени и корни
  Log("Sqrt(16): ", Math::Sqrt(16.0f));
  Log("Pow(2, 10): ", Math::Pow(2.0f, 10.0f));
  
  // Округление
  Log("Floor(3.7): ", Math::Floor(3.7f));  // 3
  Log("Ceil(3.2): ", Math::Ceil(3.2f));    // 4
  Log("Round(3.5): ", Math::Round(3.5f));  // 4
  
  Exit();
}
```

---

## Логирование

Мы использовали `Log()` на протяжении всего этого учебника, но attoboy предоставляет более продвинутую систему логирования:

```cpp
void atto_main() {
  // Разные уровни логирования
  LogDebug("This is a debug message");
  LogInfo("This is an info message");
  LogWarning("This is a warning");
  LogError("This is an error");
  
  // Пишем лог в файл вместо консоли
  EnableLoggingToFile("app.log");
  
  LogInfo("This goes to the file");
  LogInfo("So does this");
  
  // Возвращаемся к консоли
  EnableLoggingToConsole();
  LogInfo("Back to console");
  
  Exit();
}
```

Уровни логов контролируются на этапе компиляции для эффективности. По умолчанию включены `LogInfo`, `LogWarning` и `LogError`, тогда как `LogDebug` требует определения `ATTOBOY_LOG_DEBUG_ENABLE` до подключения заголовка.

---

## Информация об окружении и системе

```cpp
void atto_main() {
  // Переменные окружения
  String path = GetEnv("PATH");
  Log("PATH has ", path.length(), " characters");
  
  SetEnv("MY_APP_CONFIG", "production");
  
  // Информация о пользователе
  Log("User: ", GetUserName());
  Log("Display name: ", GetUserDisplayName());
  
  // Идентификатор процесса
  Log("Process ID: ", GetProcessId());
  
  // Пауза
  Log("Waiting 2 seconds...");
  Sleep(2000);
  Log("Done waiting!");
  
  Exit();
}
```

---

## Реестр Windows

Для хранения конфигурации, специфичной для Windows:

```cpp
void atto_main() {
  Registry myAppSettings("HKEY_CURRENT_USER\\Software\\MyApp");
  
  // Создаём при необходимости
  if (!myAppSettings.exists()) {
    myAppSettings.create();
  }
  
  // Сохраняем значения
  myAppSettings.setStringValue("lastUser", "alice");
  myAppSettings.setIntegerValue("windowWidth", 1024);
  myAppSettings.setIntegerValue("windowHeight", 768);
  
  // Читаем их обратно
  String user = myAppSettings.getStringValue("lastUser");
  int width = myAppSettings.getIntegerValue("windowWidth");
  
  Log("Last user was ", user, " with window ", width, " pixels wide");
  
  Exit();
}
```

---

## Полный пример: мини-записная книжка

Давайте соберём несколько концепций вместе в небольшом, но полезном приложении:

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

Это крошечное приложение демонстрирует работу с файлами, сохранение в JSON, взаимодействие с консолью с автодополнением, цветной вывод и простой цикл команд. После компиляции оно создаёт исполняемый файл около 35 килобайт, который сохраняет заметки и запускается на любой системе Windows.

---

## Заключительные мысли

Теперь вы увидели широту возможностей attoboy: от базовой работы со строками до файлового ввода/вывода, от веб-запросов до интеграции с ИИ, от консольных интерфейсов до потоков. Библиотека создана с простой философией: писать на C++ должно быть естественно и безопасно, а порождаемые программы должны быть достаточно маленькими, чтобы ими можно было легко делиться.

По мере дальнейшего изучения помните несколько принципов, которые вам пригодятся:

* Примите неизменяемость. Когда метод `String` возвращает новую строку вместо изменения оригинала — это преимущество. Ваши данные остаются предсказуемыми.
* Доверяйте паттерну RAII. Когда вы создаёте `File`, `Console` или `Thread`, он сам убирает за собой при выходе из области видимости. Вам не нужно помнить о закрытии ресурсов или освобождении памяти — attoboy справится.
* Цепляйте методы. Многие методы attoboy возвращают сам объект, позволяя писать выразительные цепочки вроде `list.append(1).append(2).sort().reverse()`.
* Проверяйте результаты. Методы вроде `isEmpty()`, `isValid()` и `succeeded()` помогут обрабатывать ошибки аккуратно.
* Начинайте с простого. Первые программы не обязаны использовать потоки, ИИ или сетевые запросы. Начните со строк и файлов, затем расширяйте функциональность по мере необходимости.

Крошечные исполняемые файлы, которые вы создадите с помощью attoboy, — это полные, автономные программы. Никаких инсталляторов, никаких зависимостей времени выполнения, никаких проблем совместимости. Просто маленькие, быстрые и способные приложения, которыми можно поделиться с кем угодно.

Добро пожаловать в attoboy. Мы уверены, что вам здесь понравится.
