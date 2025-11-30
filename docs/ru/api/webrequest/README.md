# WebRequest

## Обзор

Класс `attoboy::WebRequest` — основной способ выполнения HTTP-запросов в библиотеке attoboy. Он оборачивает низкоуровневые сетевые API Windows (WinHTTP) и предоставляет высокоуровневый интерфейс, удобный в стиле RAII, для:

* Выполнения HTTP **GET** и **POST** запросов.
* Отправки JSON, бинарных или строковых тел запросов.
* Использования других HTTP-методов, таких как **PUT**, **PATCH** или **DELETE**.
* Управления параметрами запроса и заголовками.
* Скачивания файлов напрямую на диск.

В общих чертах:

* Вы создаёте `WebRequest` с URL (и при необходимости с параметрами запроса и заголовками).
* Вы вызываете одну из функций `doGet`, `doPost` или `doRequest` с необязательным таймаутом.
* Вы получаете объект `WebResponse`, который можно проверить с помощью методов `succeeded()`, `getStatusCode()`, `asString()` или `asJson()`.

Во всех примерах ниже предполагается:

```cpp
using namespace attoboy;
```

### Основы HTTP (для новичков в веб‑программировании)

Если вы новичок в веб‑программировании, полезно понять несколько ключевых идей, которые раскрывает `WebRequest`.

#### URL

URL — это строка, которая указывает клиенту, куда отправить запрос. Например:

* `https://api.example.com/users`
* `https://example.com/search?q=hello`

URL обычно содержит:

* **Схема**: `http` или `https`.
* **Хост**: `api.example.com`.
* **Путь**: `/users`.
* **Строка запроса**: например, `?q=hello`, содержащая пары ключ–значение.

`WebRequest` принимает `String url` и необязательный `Map` параметров запроса. Эти параметры автоматически добавляются к URL в виде строки запроса.

#### Методы HTTP

**Метод** описывает, что вы хотите сделать:

* **GET** — Получить данные. Обычно тело запроса не отправляется.
* **POST** — Отправить данные на сервер (например, JSON, form‑data или бинарные данные).
* **PUT** / **PATCH** — Обновить существующие данные.
* **DELETE** — Удалить данные.

`WebRequest` предоставляет удобные методы для `GET` и нескольких вариантов `POST`, а также универсальный `doRequest()` для пользовательских методов.

#### Заголовки

HTTP‑**заголовки** — это пары ключ–значение, описывающие метаданные запроса:

* Примеры: `Content-Type`, `Authorization`, `Accept`, `User-Agent`.
* В `WebRequest` заголовки передаются как `Map` при создании объекта.

#### Тело запроса

Некоторые методы (особенно `POST`, `PUT`, `PATCH`) включают **тело** запроса:

* Сырые бинарные данные (`Buffer`).
* Текст (`String`).
* JSON‑данные (`Map` или `List`, которые `WebRequest` преобразует в JSON за вас).

`WebRequest` предлагает перегруженные `doPost()` функции для разных типов тела.

#### Ответ, код состояния и ошибки

Ответ сервера — это **HTTP‑ответ**, который содержит:

* **Код состояния** — например, `200` для успеха, `404` для «Не найдено».
* **Заголовки** — метаданные от сервера.
* **Тело** — возвращённые данные (HTML, JSON, бинарные данные и т.д.).

Библиотека представляет это в виде `WebResponse`:

* `response.succeeded()` проверяет, является ли код состояния 2xx.
* `response.getStatusCode()` возвращает код.
* `response.asString()`, `response.asBuffer()` или `response.asJson()` возвращают тело ответа.

#### Таймауты и блокирующее поведение

Все операции `WebRequest` являются **блокирующими**:

* Вызовы, такие как `doGet()` и `doPost()`, не вернут управление до тех пор, пока:

  * запрос не завершится успешно,
  * или не произойдёт ошибка (например, сетевая),
  * или не истечёт **таймаут**.

Таймауты задаются в **миллисекундах**:

* `timeout = -1` (по умолчанию) означает *без таймаута* (ждать сколько потребуется).
* `timeout = 10'000` означает «ждать не более 10 секунд» для завершения операции.

#### Одноразовые запросы

Один экземпляр `WebRequest` предназначен для выполнения **одного** запроса:

* После вызова `doGet`, `doPost` или `doRequest`, `hasCompleted()` вернёт `true`.
* Следует считать такой объект `WebRequest` **одноразовым** и создавать новый объект для следующего вызова.

---

## Справочник

Каждый раздел ниже описывает один публичный конструктор, метод или статическую функцию `attoboy::WebRequest`, включая:

* **Сигнатуру** — точное объявление из заголовочного файла.
* **Синопсис** — оригинальная однострочная пометка Doxygen.
* **Параметры** и **Возвращаемое значение**.
* **Подробнее** — детали, оговорки и пример.

---

#### `WebRequest(const String &url, const Map &params = Map(), const Map &headers = Map())`

**Сигнатура**

```cpp
WebRequest(const String &url, const Map &params = Map(),
           const Map &headers = Map());
```

**Синопсис**
Создаёт запрос. params становятся строкой запроса; headers отправляются с
запросом.

**Параметры**

* `url`
  Базовый URL для запроса, включая схему (`http` или `https`), хост и путь.
  Пример: `"https://api.example.com/users"`.

* `params` *(необязательно)*
  `Map`, пары ключ–значение которого будут добавлены к URL в виде
  параметров строки запроса. Если пустой, дополнительных параметров не добавляется.

* `headers` *(необязательно)*
  `Map` заголовков, отправляемых с запросом. Ключи и значения преобразуются
  в строки. Если пустой, пользовательские заголовки не добавляются.

**Возвращаемое значение**

* *(конструктор; неприменимо)*

**Подробнее**

Этот конструктор настраивает всё необходимое для одного HTTP‑запроса:

* Сохраняет URL.
* Принимает необязательный `Map` **параметров запроса** и преобразует их в
  `?key=value&key2=value2` в URL.
* Принимает необязательный `Map` **заголовков** и добавляет их ко всем методам запроса,
  вызываемым на этом экземпляре `WebRequest`.

Типичное использование параметров:

* **Map параметров** (`params`):
  Ключи и значения обычно являются строками (или типами, из которых можно
  построить `String`). Например,
  `params.put("q", "hello")` станет `?q=hello`. Несколько ключей объединяются через `&`.

* **Map заголовков** (`headers`):
  Тоже представляет собой `Map`. Например,
  `headers.put("Authorization", "Bearer TOKEN")`.

Запрос не отправляется в момент конструирования. Сетевая операция выполняется при вызове `doGet`, `doPost` или `doRequest`.

**Пример**

```cpp
String url("https://api.example.com/search");

// Build query parameters: ?q=attoboy&page=1
Map params;
params.put("q", "attoboy").put("page", 1);

// Build headers: Authorization: Bearer XYZ
Map headers;
headers.put("Authorization", "Bearer XYZ");

WebRequest request(url, params, headers);
WebResponse response = request.doGet(5000);  // 5-second timeout

if (response.succeeded()) {
  String body = response.asString();
  // ... use response body ...
}
```

*В этом примере создаётся GET‑запрос с параметрами строки запроса и пользовательским заголовком, затем он выполняется с таймаутом.*

---

#### `WebRequest(const WebRequest &other)`

**Сигнатура**

```cpp
WebRequest(const WebRequest &other);
```

**Синопсис**
Создаёт копию (разделяет внутренний запрос).

**Параметры**

* `other` – Существующий `WebRequest` для копирования.

**Возвращаемое значение**

* *(конструктор; неприменимо)*

**Подробнее**

Конструктор копирования создаёт новый объект `WebRequest`, который **делит**
внутреннюю реализацию с `other`. Это **поверхностная** копия:

* Конфигурация (URL, params, headers) фактически разделяется.
* Поскольку запросы являются одноразовыми, не стоит использовать это как способ
  выполнить **тот же** запрос дважды. Как только любой из разделяющих экземпляров завершит запрос, все
  копии будут сообщать `hasCompleted() == true`.

Конструктор копирования полезен при передаче `WebRequest` между функциями без передачи владения внутренней реализации.

**Пример**

```cpp
String url("https://example.com/info");
WebRequest base(url);

// Copy the request handle
WebRequest copy(base);

// Either instance can be used to perform the one allowed request.
// After one of them calls doGet/doPost/doRequest, both will reflect completion.
WebResponse response = copy.doGet();
```

*В этом примере копируется `WebRequest`, но внутренняя конфигурация запроса разделяется.*

---

#### `~WebRequest()`

**Сигнатура**

```cpp
~WebRequest();
```

**Синопсис**
Уничтожает запрос и освобождает ресурсы.

**Параметры**

* *(нет)*

**Возвращаемое значение**

* *(деструктор; неприменимо)*

**Подробнее**

Деструктор освобождает любые связанные с запросом ресурсы, такие как:

* Внутренние дескрипторы WinHTTP.
* Выделенная память для конфигурации и состояния.

Вам не нужно вызывать явный метод «закрыть»; просто дайте `WebRequest` выйти из области видимости, и RAII освободит ресурсы.

Уничтожение `WebRequest` не отменяет уже завершённую операцию. Запросы синхронны и завершаются до возврата из метода.

**Пример**

```cpp
{
  WebRequest req("https://example.com/ping");
  WebResponse res = req.doGet();
  // ... process response ...
} // req is destroyed here; underlying resources are freed.
```

*В этом примере показана автоматическая очистка ресурсов при выходе `WebRequest` из области видимости.*

---

#### `WebRequest &operator=(const WebRequest &other)`

**Сигнатура**

```cpp
WebRequest &operator=(const WebRequest &other);
```

**Синопсис**
Присваивает другой запрос (разделяет внутренний запрос).

**Параметры**

* `other` – `WebRequest`, от которого выполняется присваивание.

**Возвращаемое значение**

* Ссылка на `*this`, позволяющая цепочку присваиваний.

**Подробнее**

Оператор присваивания копирования:

* Освобождает ресурсы текущего объекта.
* Делает левый операнд разделяющим внутреннюю реализацию запроса с `other`.

Как и в случае конструктора копирования, это даёт **разделяемое** состояние. Если один из разделяющих экземпляров выполняет запрос, все они увидят `hasCompleted()` как `true` после завершения.

Это полезно, когда вы хотите переназначить переменную так, чтобы она ссылалась на существующий `WebRequest`.

**Пример**

```cpp
WebRequest a("https://example.com/a");
WebRequest b("https://example.com/b");

b = a; // Now b shares the underlying request with a

WebResponse r = b.doGet();
// a.hasCompleted() is now also true.
```

*В этом примере показано переназначение `WebRequest`, в результате чего две переменные разделяют один и тот же внутренний запрос.*

---

#### `WebResponse doGet(int timeout = -1)`

**Сигнатура**

```cpp
WebResponse doGet(int timeout = -1);
```

**Синопсис**
Выполняет HTTP GET‑запрос. timeout в мс (-1 = бесконечность).

**Параметры**

* `timeout` *(необязательно)*
  Максимальное время ожидания операции в миллисекундах:

  * `-1` (по умолчанию): без таймаута (ожидать бесконечно).
  * Любое неотрицательное целое: максимальное количество миллисекунд, после которого библиотека считает запрос истёкшим по таймауту.

**Возвращаемое значение**

* `WebResponse`, представляющий ответ сервера.
  Используйте `response.succeeded()`, `response.getStatusCode()` и другие методы `WebResponse` для его проверки.

**Подробнее**

`doGet()` посылает HTTP **GET** запрос на URL, указанный в конструкторе, с учётом:

* Любых параметров строки запроса из `params`.
* Любых заголовков из `headers`.

Метод:

* Блокирует выполнение до завершения запроса или истечения таймаута.
* Возвращает `WebResponse`, даже в случае ошибок. Например:

  * Сетевые ошибки и таймауты обычно приводят к ответу, где `succeeded()` — `false`, либо тело/контент могут быть пустыми.
  * HTTP‑коды, не относящиеся к 2xx (например, `404`, `500`), всё ещё представлены как объекты `WebResponse`; `succeeded()` будет `false`, но `getStatusCode()` отражает реальный код.

После возврата `doGet()`:

* `hasCompleted()` будет возвращать `true`.
* Не следует вызывать другие методы запроса (`doPost()`, `doRequest()`) на том же экземпляре `WebRequest`.

**Пример**

```cpp
String url("https://api.example.com/status");
WebRequest request(url);

// Wait up to 10 seconds
WebResponse response = request.doGet(10 * 1000);

if (response.succeeded()) {
  String body = response.asString();
  // e.g., parse JSON or inspect text
} else {
  int status = response.getStatusCode();
  // handle error based on status or lack of response
}
```

*В этом примере выполняется GET‑запрос с таймаутом 10 секунд и анализируется результат.*

---

#### `WebResponse doPost(int timeout = -1)`

**Сигнатура**

```cpp
WebResponse doPost(int timeout = -1);
```

**Синопсис**
Выполняет HTTP POST‑запрос без тела.

**Параметры**

* `timeout` *(необязательно)*
  Максимальное время ожидания операции в миллисекундах (`-1` — без таймаута).

**Возвращаемое значение**

* `WebResponse`, представляющий ответ сервера.

**Подробнее**

`doPost()` отправляет HTTP **POST** запрос с:

* Отсутствием явного тела запроса.
* Любыми параметрами из `params`, добавленными в URL.
* Любыми заголовками из `headers`, включёнными в запрос.

Это полезно для конечных точек, которые рассматривают пустой POST (с параметрами в строке запроса или в заголовках) как триггер или команду, а не как отправку данных.

Правила завершения и семантика одноразового использования такие же, как у `doGet()`.

**Пример**

```cpp
String url("https://api.example.com/trigger-job");

// Add ?job=cleanup to the URL
Map params;
params.put("job", "cleanup");

WebRequest request(url, &params, nullptr);
WebResponse response = request.doPost(5000);

if (!response.succeeded()) {
  // handle non-2xx or timeout
}
```

*В этом примере отправляется POST‑запрос без тела, но с параметрами строки запроса.*

---

#### `WebResponse doPost(const Map &json, int timeout = -1)`

**Сигнатура**

```cpp
WebResponse doPost(const Map &json, int timeout = -1);
```

**Синопсис**
Выполняет HTTP POST с JSON‑телом (Map).

**Параметры**

* `json`
  `Map`, представляющий JSON‑объект. Ключи и значения преобразуются в поля JSON‑объекта.

* `timeout` *(необязательно)*
  Максимальное время ожидания операции в миллисекундах (`-1` — без таймаута).

**Возвращаемое значение**

* `WebResponse`, представляющий ответ сервера.

**Подробнее**

Этот перегруженный метод — помощник для JSON:

* `json` map сериализуется в JSON‑строку **объекта**, например:

  * `{"name":"Alice","age":30}`.
* Сериализованный JSON отправляется как **тело** запроса.
* Реализация рассчитана на то, чтобы рассматривать это как JSON POST. Как правило,
  запрос будет содержать соответствующий заголовок `Content-Type`
  (в зависимости от реализации), или вы можете указать его явно в `headers`
  при конструировании `WebRequest`.

Используйте это, когда удалённый API ожидает JSON‑объект в теле POST.

**Пример**

```cpp
String url("https://api.example.com/users");

Map headers;
headers.put("Authorization", "Bearer TOKEN");

Map payload;
payload.put("name", "Alice").put("age", 30);

WebRequest request(url, nullptr, &headers);
WebResponse response = request.doPost(payload, 10000);

if (response.succeeded()) {
  String body = response.asString();
  // server might return created user or status message
}
```

*В этом примере выполняется POST с JSON‑объектом для создания или обновления пользователя.*

---

#### `WebResponse doPost(const List &json, int timeout = -1)`

**Сигнатура**

```cpp
WebResponse doPost(const List &json, int timeout = -1);
```

**Синопсис**
Выполняет HTTP POST с JSON‑телом (List).

**Параметры**

* `json`
  `List`, представляющий JSON‑массив. Элементы преобразуются в записи JSON‑массива.

* `timeout` *(необязательно)*
  Максимальное время ожидания операции в миллисекундах (`-1` — без таймаута).

**Возвращаемое значение**

* `WebResponse`, представляющий ответ сервера.

**Подробнее**

Эта перегрузка является эквивалентом `doPost(const Map&)` для массивов:

* `List` сериализуется в JSON‑**массив**, например:

  * `[1,2,3]`
  * `["one","two","three"]`
  * `[{"id":1},{"id":2}]`, если внутри хранятся значения типа `Map`.

Это полезно, когда API ожидает список значений в теле запроса, например при отправке пачки событий или идентификаторов.

**Пример**

```cpp
String url("https://api.example.com/events/batch");

List events;
events.append("start").append("stop").append("pause");

WebRequest request(url);
WebResponse response = request.doPost(events, 8000);

if (response.succeeded()) {
  // batch accepted
}
```

*В этом примере отправляется JSON‑массив событий в одном POST.*

---

#### `WebResponse doPost(const Buffer &body, int timeout = -1)`

**Сигнатура**

```cpp
WebResponse doPost(const Buffer &body, int timeout = -1);
```

**Синопсис**
Выполняет HTTP POST с бинарным телом.

**Параметры**

* `body`
  `Buffer`, содержащий сырые байты для отправки в качестве тела запроса.

* `timeout` *(необязательно)*
  Максимальное время ожидания операции в миллисекундах (`-1` — без таймаута).

**Возвращаемое значение**

* `WebResponse`, представляющий ответ сервера.

**Подробнее**

Эта перегрузка позволяет отправлять произвольные **бинарные** данные:

* Она не налагает никакой конкретной кодировки.
* Вам следует установить соответствующие заголовки (например, `Content-Type` или `Content-Length`)
  через `headers` при создании `WebRequest`.

Типичные случаи использования:

* Загрузка файлов.
* Отправка сжатых или зашифрованных полезных нагрузок.
* Использование собственных протоколов, не основанных на JSON.

**Пример**

```cpp
String url("https://example.com/upload/blob");

// Build a binary buffer (for example, an in-memory file)
Buffer data;
data.append(String("BINARY-DATA"));  // placeholder

Map headers;
headers.put("Content-Type", "application/octet-stream");

WebRequest request(url, nullptr, &headers);
WebResponse response = request.doPost(data, 15000);

if (!response.succeeded()) {
  // handle failure
}
```

*В этом примере отправляются бинарные данные с общим бинарным типом содержимого.*

---

#### `WebResponse doPost(const String &body, int timeout = -1)`

**Сигнатура**

```cpp
WebResponse doPost(const String &body, int timeout = -1);
```

**Синопсис**
Выполняет HTTP POST со строковым телом.

**Параметры**

* `body`
  `String`, содержащая текст для отправки в качестве тела запроса. Это может быть JSON, обычный текст, URL‑закодированные form‑данные или любой другой текстовый протокол.

* `timeout` *(необязательно)*
  Максимальное время ожидания операции в миллисекундах (`-1` — без таймаута).

**Возвращаемое значение**

* `WebResponse`, представляющий ответ сервера.

**Подробнее**

Эта перегрузка предназначена для отправки текстовых полезных нагрузок, когда вам нужен полный контроль над телом:

* Если вы отправляете JSON, вы можете самостоятельно сформировать JSON‑строку (например, используя `String` и `format`) и передать её сюда.
* Если вы отправляете form‑данные в виде `key=value&other=value`, вы можете вручную составить эту строку и отправить её в качестве тела.

Как всегда, укажите соответствующий заголовок `Content-Type` в `headers` при создании `WebRequest`.

**Пример**

```cpp
String url("https://example.com/api/raw");
Map headers;
headers.put("Content-Type", "text/plain; charset=utf-8");

String body("Hello from attoboy!");

WebRequest request(url, nullptr, &headers);
WebResponse response = request.doPost(body, 5000);

if (response.succeeded()) {
  String reply = response.asString();
  // use server's response
}
```

*В этом примере отправляется простой текст в теле POST‑запроса.*

---

#### `WebResponse doRequest(const String &method, const Buffer &body, int timeout = -1)`

**Сигнатура**

```cpp
WebResponse doRequest(const String &method, const Buffer &body,
                      int timeout = -1);
```

**Синопсис**
Выполняет пользовательский HTTP‑метод (PUT, PATCH, DELETE и т.д.).

**Параметры**

* `method`
  HTTP‑метод для использования. Типичные значения: `"PUT"`, `"PATCH"`, `"DELETE"`, `"OPTIONS"` и т.д. Желательно передавать в верхнем регистре, но сервер может принять и другие варианты.

* `body`
  `Buffer`, содержащий сырые байты тела запроса. Для методов типа `DELETE`, которые часто не имеют тела, можно передать пустой буфер.

* `timeout` *(необязательно)*
  Максимальное время ожидания операции в миллисекундах (`-1` — без таймаута).

**Возвращаемое значение**

* `WebResponse`, представляющий ответ сервера.

**Подробнее**

`doRequest()` — самая гибкая операция:

* Вы можете отправить **любой HTTP‑метод**, поддерживаемый сервером.
* Вы сами предоставляете сырые байты тела, получая полный контроль над полезной нагрузкой.

Типичные сценарии:

* **PUT** с JSON‑телом:

  * Сериализуйте JSON в `String`, затем оберните его в `Buffer`:

    * `Buffer body(jsonString);`
* **PATCH** для частичных обновлений.
* **DELETE** с телом или без тела, в зависимости от API.

Не забудьте установить заголовки, такие как `Content-Type`, с помощью `headers`, переданных в конструктор.

**Пример**

```cpp
String url("https://api.example.com/users/42");
Map headers;
headers.put("Content-Type", "application/json");

// Build JSON body as a string, then convert to a buffer
String jsonBody("{\"active\":false}");
Buffer body(jsonBody);

WebRequest request(url, nullptr, &headers);

// Perform a PATCH request with JSON body
WebResponse response = request.doRequest("PATCH", body, 10000);

if (response.succeeded()) {
  // user deactivated successfully
}
```

*В этом примере отправляется пользовательский PATCH‑запрос с JSON‑телом.*

---

#### `String getUrl() const`

**Сигнатура**

```cpp
String getUrl() const;
```

**Синопсис**
Возвращает URL запроса.

**Параметры**

* *(нет)*

**Возвращаемое значение**

* `String`, содержащая полный URL для этого запроса, включая любую строку запроса, сформированную из карты параметров.

**Подробнее**

`getUrl()` возвращает фактический URL, сохранённый в `WebRequest`. После конструирования:

* Обычно включает:

  * Базовый URL, который вы указали.
  * Любые параметры строки запроса, добавленные из `params`.

Вы можете вызвать этот метод до или после выполнения запроса, чтобы убедиться, что параметры применены как ожидалось.

**Пример**

```cpp
Map params;
params.put("q", "attoboy");

WebRequest request("https://example.com/search", &params, nullptr);
String fullUrl = request.getUrl();
// e.g., "https://example.com/search?q=attoboy"
```

*В этом примере проверяется итоговый URL, который `WebRequest` будет использовать для запроса.*

---

#### `Map getParams() const`

**Сигнатура**

```cpp
Map getParams() const;
```

**Синопсис**
Возвращает карту параметров строки запроса.

**Параметры**

* *(нет)*

**Возвращаемое значение**

* `Map`, содержащая параметры запроса, связанные с этим запросом.

**Подробнее**

`getParams()` возвращает `Map` параметров строки запроса, используемых `WebRequest`. Это позволяет вам:

* Отлаживать или логировать, какие параметры прикреплены к запросу.
* Скопировать их в другой запрос.
* Просмотреть или изменить их (создав новый `WebRequest` с возвращённой картой, поскольку метод возвращает `Map` по значению).

Возвращённая `Map` не является живым представлением; изменение её **не** изменяет исходный `WebRequest`. Чтобы изменить параметры, создайте новый `WebRequest` с обновлённой картой.

**Пример**

```cpp
Map params;
params.put("page", 2).put("limit", 50);

WebRequest request("https://example.com/users", &params, nullptr);

Map copy = request.getParams();
// copy can be logged, inspected, or reused when constructing a new request
```

*В этом примере извлекаются параметры, используемые в запросе, для проверки или повторного использования.*

---

#### `Map getHeaders() const`

**Сигнатура**

```cpp
Map getHeaders() const;
```

**Синопсис**
Возвращает карту заголовков.

**Параметры**

* *(нет)*

**Возвращаемое значение**

* `Map`, содержащая заголовки, связанные с этим запросом.

**Подробнее**

`getHeaders()` возвращает `Map` пользовательских заголовков, переданных при создании. Это полезно для:

* Отладки конфигурации заголовков.
* Копирования заголовков в другой запрос.
* Логирования настроек аутентификации или типа содержимого (осторожно, не включая секреты в логах).

Как и в случае с `getParams()`, изменение возвращённой `Map` **не** повлияет на существующий `WebRequest`. Чтобы изменить заголовки, создайте новый `WebRequest` с обновлённой картой заголовков.

**Пример**

```cpp
Map headers;
headers.put("Authorization", "Bearer TOKEN")
       .put("Accept", "application/json");

WebRequest request("https://example.com/data", nullptr, &headers);

Map copy = request.getHeaders();
// copy["Authorization"], copy["Accept"], etc.
```

*В этом примере извлекаются заголовки запроса для инспекции или повторного использования.*

---

#### `bool hasCompleted() const`

**Сигнатура**

```cpp
bool hasCompleted() const;
```

**Синопсис**
Возвращает true, если запрос был завершён (может быть выполнен только один раз).

**Параметры**

* *(нет)*

**Возвращаемое значение**

* `true`, если этот `WebRequest` уже выполнил метод запроса (`doGet`, `doPost` или `doRequest`) и эта операция завершилась.
* `false`, если запрос ещё не был выполнен.

**Подробнее**

`hasCompleted()` отражает **одноразовую** природу `WebRequest`:

* Как только вызов `doGet`, любой перегрузки `doPost` или `doRequest` завершится (успешно или с ошибкой), `hasCompleted()` будет возвращать `true`.
* После завершения не следует вызывать другой метод запроса на том же экземпляре. Создайте новый `WebRequest`.

Эта функция особенно полезна, когда у вас есть разделяющие или скопированные объекты `WebRequest` и нужно узнать, был ли уже выполнен какой‑то из них.

**Пример**

```cpp
WebRequest req("https://example.com/ping");

if (!req.hasCompleted()) {
  WebResponse res = req.doGet();
}

if (req.hasCompleted()) {
  // A request was performed (successfully or not); this instance is “used”
}
```

*В этом примере проверяется, использовался ли `WebRequest`, прежде чем вызвать `doGet()`.*

---

#### `static bool Download(const String &url, const String &savePath, const Map &params = Map(), const Map &headers = Map(), bool overwrite = true, int timeout = -1)`

**Сигнатура**

```cpp
static bool Download(const String &url, const String &savePath,
                     const Map &params = Map(),
                     const Map &headers = Map(), bool overwrite = true,
                     int timeout = -1);
```

**Синопсис**
Скачивает файл по URL на диск. Возвращает true при успехе.

**Параметры**

* `url`
  URL ресурса для скачивания. Эквивалентен `url`, передаваемому в конструктор `WebRequest`.

* `savePath`
  Путь (как `String`) к файлу на диске, куда должны быть записаны загруженные данные. Путь должен быть корректен для текущего пользователя и системы.

* `params` *(необязательно)*
  `Map` параметров строки запроса, добавляемых к URL. Если пустой, дополнительных параметров не добавляется.

* `headers` *(необязательно)*
  `Map` заголовков для отправки с запросом. Если пустой, пользовательские заголовки не добавляются.

* `overwrite` *(необязательно)*
  Если `true` (по умолчанию), существующий файл по `savePath` может быть перезаписан. Если `false`, существующие файлы должны приводить к ошибке (зависит от реализации).

* `timeout` *(необязательно)*
  Максимальное время ожидания для всего скачивания в миллисекундах (`-1` — без таймаута).

**Возвращаемое значение**

* `true`, если скачивание успешно завершилось и файл был записан в `savePath`.
* `false`, если произошла ошибка (сетевая ошибка, таймаут, проблема с правами и т.д.).

**Подробнее**

`Download()` — это **вспомогательная** функция, инкапсулирующая распространённый шаблон:

1. Строит `WebRequest` для заданных `url`, `params` и `headers`.
2. Выполняет запрос (обычно `GET`).
3. Записывает тело ответа напрямую на диск по `savePath`.

Она упрощает задачу скачивания, чтобы вам не нужно было:

* Вручную вызывать `doGet()`.
* Извлекать тело как `Buffer` или `String`.
* Открывать `File` и самому записывать данные.

Поведенческие замечания:

* Если `overwrite` равно `false` и целевой файл уже существует, скачивание должно завершиться с ошибкой и вернуть `false` (рассматривать как «файл уже присутствует»).
* Вы отвечаете за то, чтобы директория для `savePath` существовала и у вас были права на запись.
* Функция блокирует выполнение и подчиняется указанному таймауту.

**Пример**

```cpp
String url("https://example.com/files/report.pdf");

// Build query parameters: ?version=latest
Map params;
params.put("version", "latest");

// Set a custom header
Map headers;
headers.put("User-Agent", "attoboy-downloader");

// Decide where to save the file
Path destPath = Path::GetDocumentsDirectory()
                    .getParentDirectory()
                    .getParentDirectory(); // example chaining (if desired)
// In practice, build a proper path string:
String savePath("C:\\Users\\User\\Documents\\report.pdf");

bool ok = WebRequest::Download(url, savePath, params, headers,
                               true,   // overwrite if exists
                               30000); // 30-second timeout

if (!ok) {
  // handle download failure
}
```

*В этом примере скачивается удалённый PDF в указанный путь с параметрами строки запроса, пользовательским заголовком, разрешением перезаписи и таймаутом 30 секунд.*