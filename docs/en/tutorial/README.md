# Tutorial
## A Gentle Introduction to Attoboy

---

## Welcome, Programmer

There's a particular joy in writing software that *just works*—programs that compile in the blink of an eye, produce tiny executables you can share with anyone, and don't require your users to install anything special. That's the world attoboy invites you into.

Perhaps you've written Python scripts or JavaScript applications. Maybe you've heard that C++ is powerful but intimidating—full of memory management pitfalls, arcane syntax, and libraries that require advanced degrees to understand. We're here to tell you: it doesn't have to be that way.

Attoboy is a C++ library designed with a radical philosophy: *what if C++ could feel as comfortable as Python, while producing executables smaller than most image files?* What if you never had to think about memory allocation, pointer arithmetic, or the dozens of competing ways to represent text? What if everything just... worked?

This tutorial will take you on a journey through attoboy's capabilities. By the end, you'll be writing real, useful programs—command-line tools, file utilities, web clients, even AI-powered applications—all in clean, readable C++ that compiles to executables typically between 10 and 100 kilobytes.

No prior C++ experience is required. If you understand what variables, functions, and loops are, you're ready. Let's begin.

---

## Your First Attoboy Program

Every attoboy program has the same basic shape:

```cpp
#include <attoboy/attoboy.h>

using namespace attoboy;

void atto_main() {
  Log("Hello, world!");
  Exit();
}
```

Let's understand what's happening here. The first line includes the attoboy library—this single header gives you access to everything we'll discuss. The second line, `using namespace attoboy;`, lets us write `String` instead of `attoboy::String`, keeping our code clean and readable.

The function `atto_main()` is where your program begins. Unlike traditional C++ programs that start with a function called `main`, attoboy programs start here. Don't worry about why—just know that this is your entry point, the place where your program's story begins.

Inside, we use `Log()` to print a message. This is attoboy's way of sending text to the console (or to a file, if you configure it that way). Finally, `Exit()` tells the program we're done. Every attoboy program should end with a call to `Exit()`.

That's it. That's a complete, working program. When compiled, it produces an executable of just a few kilobytes that will run on any Windows machine without requiring any additional files or installations.

---

## The String: Your Constant Companion

If there's one type you'll use more than any other in attoboy, it's `String`. Text is everywhere in programming—file names, user input, messages, data formats, network communication—and attoboy's `String` is designed to make working with text a pleasure.

### Creating Strings

You can create strings in many ways:

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

Notice something wonderful: attoboy's `String` automatically converts numbers and booleans to text. No formatting functions needed, no type conversion headaches. You write what you mean.

### Combining Strings

Strings can be joined together with the `+` operator, just as you might expect:

```cpp
void atto_main() {
  String first = "Good";
  String second = "morning";
  String message = first + " " + second + "!";
  
  Log(message);  // "Good morning!"
  Exit();
}
```

But attoboy offers something even more convenient. You can create a string from multiple pieces in one go:

```cpp
void atto_main() {
  String name = "Alice";
  int age = 30;
  
  String bio = String("Name: ", name, ", Age: ", age);
  Log(bio);  // "Name: Alice, Age: 30"
  
  Exit();
}
```

This variadic constructor accepts any number of values—strings, numbers, booleans—and combines them seamlessly.

### Exploring String Contents

Strings know things about themselves:

```cpp
void atto_main() {
  String text = "Hello, 世界!";
  
  Log("Length: ", text.length());    // 9 characters
  Log("Bytes: ", text.byteLength());   // 14 bytes (UTF-8!)
  Log("Empty? ", text.isEmpty());    // false
  
  Exit();
}
```

That distinction between `length()` and `byteLength()` is important. Attoboy uses UTF-8 encoding throughout, which means characters like Chinese hanzi or emoji might take multiple bytes. The `length()` method counts actual characters (what programmers call "codepoints"), while `byteLength()` counts the raw bytes. For most purposes, you'll want `length()`.

### Searching and Testing

Strings can answer questions about their contents:

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

### Extracting Pieces

You can pull out parts of a string:

```cpp
void atto_main() {
  String text = "Hello, World!";
  
  String first = text.at(0);       // "H"
  String last = text.at(-1);       // "!"  (negative indices count from end)
  String middle = text.substring(0, 5); // "Hello"
  String end = text.substring(7);     // "World!"
  
  Log(first, " ... ", last);
  Log(middle);
  Log(end);
  
  Exit();
}
```

Negative indices are a small gift—`at(-1)` gives you the last character, `at(-2)` the second-to-last, and so on. This pattern appears throughout attoboy and will feel natural if you've used Python.

### Transforming Strings

Strings in attoboy are *immutable*—they cannot be changed once created. Instead, methods like `upper()` and `replace()` return new strings:

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

This immutability might seem restrictive, but it's actually a blessing. You never have to wonder whether some function modified your string—it didn't. The original is always safe. This pattern eliminates an entire category of bugs that plague programs written in other styles.

### Splitting and Joining

Often you need to break a string into pieces or combine pieces into a string:

```cpp
void atto_main() {
  String csv = "apple,banana,cherry,date";
  List fruits = csv.split(",");
  
  // fruits is now ["apple", "banana", "cherry", "date"]
  
  String reunited = " - ".join(fruits);
  Log(reunited);  // "apple - banana - cherry - date"
  
  // Split by whitespace
  String sentence = "The quick brown fox";
  List words = sentence.split();
  Log("Word count: ", words.length());  // 4
  
  // Split into lines
  String poem = "Roses are red\nViolets are blue";
  List lines = poem.lines();
  Log("Line count: ", lines.length());  // 2
  
  Exit();
}
```

### Formatting Templates

For more complex string assembly, attoboy provides template formatting:

```cpp
void atto_main() {
  // Positional placeholders
  String template1 = "Hello, {0}! You are {1} years old.";
  String result1 = template1.format(List("Alice", 30));
  Log(result1);  // "Hello, Alice! You are 30 years old."
  
  // Named placeholders
  String template2 = "Dear {name}, your order #{order} is ready.";
  String result2 = template2.format(Map("name", "Bob", "order", 12345));
  Log(result2);  // "Dear Bob, your order #12345 is ready."
  
  Exit();
}
```

### Converting To and From Numbers

Strings that look like numbers can become actual numbers:

```cpp
void atto_main() {
  String numStr = "42";
  String floatStr = "3.14159";
  String boolStr = "yes";
  
  int num = numStr.toInteger();    // 42
  float pi = floatStr.toFloat();   // 3.14159
  bool flag = boolStr.toBool();    // true
  
  // Check before converting
  String maybeNumber = "hello";
  if (maybeNumber.isNumber()) {
    Log("It's a number!");
  } else {
    Log("Not a number");  // This one
  }
  
  Exit();
}
```

The `toBool()` method is forgiving—it recognizes "true", "yes", "1", and "on" (case-insensitive) as true values. Everything else is false. This is handy when parsing configuration files or user input.

---

## Collecting Things: List, Map, and Set

Individual pieces of data are useful, but programs typically work with *collections*—groups of related items. Attoboy provides three powerful collection types, each designed for different purposes.

### The List: Ordered Collections

A `List` is an ordered sequence of values. Unlike arrays in many languages, an attoboy `List` can hold values of different types:

```cpp
void atto_main() {
  // Create a list with initial values
  List groceries("milk", "bread", "eggs", "butter");
  
  Log("Shopping list has ", groceries.length(), " items");
  
  // Access by index
  String first = groceries.at(0);   // "milk"
  String last = groceries.at(-1);   // "butter"
  Log("First: ", first, ", Last: ", last);
  
  // Add items
  groceries.append("cheese");
  groceries.prepend("coffee");
  
  // Mixed types are fine!
  List mixed(42, "hello", true, 3.14f);
  
  Exit();
}
```

Notice the syntax `groceries.at(0)`. If you prefer, you can also use the `[]` operator :

```cpp
void atto_main() {
  List groceries("milk", "bread", "eggs", "butter");
  Log("First: ", groceries[0], ", Last: ", groceries[-1]);
  Exit();
}
```

Lists support method chaining—most operations return the list itself, so you can write fluent code:

```cpp
void atto_main() {
  List numbers;
  numbers.append(3).append(1).append(4).append(1).append(5).append(9);
  
  // Sort in place
  numbers.sort();  // Now: 1, 1, 3, 4, 5, 9
  
  // Reverse in place
  numbers.reverse();  // Now: 9, 5, 4, 3, 1, 1
  
  // Check contents
  if (numbers.contains(5)) {
    Log("Found 5!");
  }
  
  int pos = numbers.find(4);
  Log("4 is at position ", pos);
  
  Exit();
}
```

You can extract portions of a list:

```cpp
void atto_main() {
  List letters("a", "b", "c", "d", "e");
  
  List middle = letters.slice(1, 4);  // ["b", "c", "d"]
  List fromEnd = letters.slice(-2, letters.length());  // ["d", "e"]
  
  Exit();
}
```

### The Map: Key-Value Pairs

A `Map` associates keys with values. It's perfect for structured data, configuration, or any situation where you need to look things up by name:

```cpp
void atto_main() {
  // Create with key-value pairs
  Map person("name", "Alice", "age", 30, "city", "Seattle");
  
  // Retrieve values
  String name = person.get("name", "Unknown");
  int age = person.get("age", 0);
  
  Log(name, " is ", age, " years old");
  
  // Check for keys
  if (person.hasKey("email")) {
    Log("Has email");
  } else {
    Log("No email on record");
  }
  
  // Add or update
  person.put("email", "alice@example.com");
  person.put("age", 31);  // Happy birthday!
  
  Exit();
}
```

You can inspect a map's contents:

```cpp
void atto_main() {
  Map config("debug", true, "timeout", 30, "name", "MyApp");
  
  List allKeys = config.keys();
  List allValues = config.values();
  
  Log("Configuration has ", config.length(), " settings");
  
  Exit();
}
```

### The Set: Unique Values

A `Set` is a collection where each value appears at most once. Duplicates are silently ignored:

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

Sets support mathematical operations:

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

### JSON and CSV

Collections integrate naturally with common data formats:

```cpp
void atto_main() {
  // Create data
  Map user("name", "Bob", "age", 25);
  
  // Convert to JSON
  String json = user.toJSONString();
  Log(json);  // {"name":"Bob","age":25}
  
  // Parse JSON back
  Map parsed = Map::FromJSONString(json);
  Log("Name from JSON: ", parsed.get("name", ""));
  
  // CSV for tabular data
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

## Working with Files

Programs that can't interact with the filesystem are limited in what they can accomplish. Attoboy makes file operations straightforward with two cooperating types: `Path` for file metadata and navigation, and `File` for reading and writing content.

### The Path: Navigating the Filesystem

A `Path` represents a location in the filesystem. It can tell you about what exists there and help you manipulate files:

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

Paths can reveal detailed information about files:

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

### Reading and Writing Files

For simple operations, `Path` has convenience methods:

```cpp
void atto_main() {
  Path configPath("settings.txt");
  
  // Write a string to a file
  configPath.writeFromString("theme=dark\nfontSize=14\n");
  
  // Read it back
  String contents = configPath.readToString();
  Log("Config contents:\n", contents);
  
  // Append more
  configPath.appendFromString("language=en\n");
  
  Exit();
}
```

### Listing Directory Contents

You can explore what's inside a directory:

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

### Creating Directories and Moving Files

```cpp
void atto_main() {
  Path newDir("my_project");
  
  // Create directory (creates parents if needed)
  if (newDir.makeDirectory()) {
    Log("Created ", newDir.toString());
  }
  
  // Create a file inside
  Path readme(newDir.toString() + "\\README.txt");
  readme.writeFromString("# My Project\n\nWelcome!");
  
  // Copy it
  Path backup(newDir.toString() + "\\README.backup.txt");
  readme.copyTo(backup);
  
  // Move/rename
  Path newName(newDir.toString() + "\\README.md");
  readme.moveTo(newName);
  
  Exit();
}
```

### Temporary Files

When you need scratch space:

```cpp
void atto_main() {
  Path tempFile = Path::CreateTemporaryFile("myapp_", ".tmp");
  Log("Temporary file: ", tempFile.toString());
  
  tempFile.writeFromString("Temporary data here");
  
  // Don't forget to clean up when done
  tempFile.deleteFile();
  
  Exit();
}
```

### The File Class: Streaming I/O

For more control over reading and writing—especially for large files or network connections—use the `File` class:

```cpp
void atto_main() {
  Path logPath("application.log");
  
  File logFile(logPath);
  if (logFile.isValid()) {
    logFile.write("Application started\n");
    logFile.write("Performing initialization...\n");
    logFile.flush();  // Ensure data is written to disk
  }
  
  // Read back
  logFile.setPosition(0);  // Go back to beginning
  String contents = logFile.readAllToString();
  Log("Log contents: ", contents);
  
  logFile.close();
  
  Exit();
}
```

---

## Command-Line Arguments

Most useful programs accept input from the command line. Attoboy's `Arguments` class makes parsing arguments painless:

```cpp
void atto_main() {
  Arguments args;
  
  // Define what we accept
  args.addFlag("v", "Enable verbose output", false, "verbose");
  args.addFlag("h", "Show help", false, "help");
  args.addParameter("o", "Output file path", "output.txt", "output");
  args.addPositionalParameter("input", "Input file to process");
  
  // Parse
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

With this setup, your program automatically handles:
- `myprogram.exe -h` or `myprogram.exe --help` (shows help)
- `myprogram.exe -v --output=result.txt input.txt`
- `myprogram.exe input.txt -o output.txt --verbose`

The arguments can appear in any order, with flags and parameters using either short (`-v`) or long (`--verbose`) forms.

---

## Telling Time

The `DateTime` class handles timestamps with millisecond precision:

```cpp
void atto_main() {
  // Current time
  DateTime now;
  Log("Current time: ", now.toString());
  
  // From ISO string
  DateTime birthday("2000-01-15T08:30:00.000Z");
  
  // Difference (in milliseconds)
  long long ageMs = now.diff(birthday);
  long long ageYears = ageMs / (1000LL * 60 * 60 * 24 * 365);
  Log("Approximate age: ", ageYears, " years");
  
  // Add time
  DateTime tomorrow = DateTime(now);
  tomorrow.add(24 * 60 * 60 * 1000);  // Add 24 hours in milliseconds
  Log("Tomorrow: ", tomorrow.toString());
  
  Exit();
}
```

---

## Binary Data and the Buffer

While `String` is for text, `Buffer` handles raw binary data—file contents, encrypted data, network packets, and the like:

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

### Compression

Buffers can be compressed using LZ4, which is extremely fast:

```cpp
void atto_main() {
  // Create some repetitive data
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

### Encryption

Buffers support ChaCha20 encryption, a modern, secure cipher:

```cpp
void atto_main() {
  String secret = "This is my secret message!";
  Buffer plaintext(secret);
  
  // Key must be at least 32 bytes, nonce at least 12 bytes
  String key = "my-32-byte-encryption-key-here!!";
  String nonce = "unique-nonce";
  
  // Encrypt
  Buffer ciphertext = plaintext.crypt(key, nonce);
  Log("Encrypted (base64): ", ciphertext.toBase64());
  
  // Decrypt (same operation with same key/nonce)
  Buffer decrypted = ciphertext.crypt(key, nonce);
  Log("Decrypted: ", decrypted.toString());
  
  Exit();
}
```

### Base64 Encoding

For transmitting binary data as text:

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

## Reaching Out: Web Requests

Attoboy makes HTTP requests simple:

```cpp
void atto_main() {
  // Simple GET request
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

### POST Requests

Sending data to a server:

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

### Downloading Files

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

## Conversing with AI

One of attoboy's most powerful features is built-in support for AI language models through any OpenAI-compatible API:

```cpp
void atto_main() {
  // Connect to an AI service
  AI ai(
    "https://api.openai.com/v1",       // Base URL
    GetEnv("OPENAI_API_KEY"),        // API key from environment
    "gpt-5-mini"                 // Model name
  );
  
  // Set a system prompt (optional)
  ai.setSystemPrompt("You are a helpful assistant who gives concise answers.");
  
  // Ask a question
  String answer = ai.ask("What is the capital of France?");
  
  if (!answer.isEmpty()) {
    Log("AI says: ", answer);
  } else {
    Log("Error getting response");
  }
  
  Exit();
}
```

### Multi-Turn Conversations

For back-and-forth dialogue:

```cpp
void atto_main() {
  AI ai("https://api.openai.com/v1", GetEnv("OPENAI_API_KEY"), "gpt-5-mini");
  Conversation chat = ai.createConversation();
  
  Log(chat.ask("Hi! I'm learning about astronomy."));
  Log(chat.ask("What's the closest star to Earth?"));
  Log(chat.ask("How long would it take to travel there?"));
  
  // The AI remembers the context of the conversation
  
  Exit();
}
```

### Embeddings for Semantic Search

Embeddings convert text into numerical vectors that capture meaning, enabling semantic similarity comparisons:

```cpp
void atto_main() {
  AI ai("https://api.openai.com/v1", GetEnv("OPENAI_API_KEY"), "text-embedding-3-small");
  
  Embedding dogEmbed = ai.createEmbedding("dog");
  Embedding puppyEmbed = ai.createEmbedding("puppy");
  Embedding carEmbed = ai.createEmbedding("car");
  
  float dogPuppy = dogEmbed.compare(puppyEmbed);  // High similarity
  float dogCar = dogEmbed.compare(carEmbed);    // Low similarity
  
  Log("dog vs puppy similarity: ", dogPuppy);
  Log("dog vs car similarity: ", dogCar);
  
  Exit();
}
```

---

## Building Console Interfaces

For interactive command-line applications, attoboy provides a rich `Console` class:

```cpp
void atto_main() {
  Console con;
  
  // Colored output
  con.println("Welcome!", CON_CYAN);
  con.print("Status: ", CON_WHITE);
  con.println("OK", CON_GREEN);
  
  // Formatted output
  con.printWrapped("This is a long paragraph that will automatically "
    "wrap to fit the console width, making it easy to display "
    "large amounts of text neatly.", -1, CON_GRAY);
  
  Exit();
}
```

### Interactive Input

The `Console` provides sophisticated input handling:

```cpp
void atto_main() {
  Console con;
  
  // Simple input
  String name = con.input("Enter your name: ");
  con.println(String("Hello, ", name, "!"), CON_YELLOW);
  
  // Password input (masked with asterisks)
  ConsoleInput passOptions;
  passOptions.password = true;
  String password = con.input("Password: ", passOptions);
  
  // Input with history and tab completion
  ConsoleInput shellOptions;
  shellOptions.completions = List("help", "exit", "status", "run", "config");
  
  String command = con.input("> ", shellOptions);
  
  Exit();
}
```

### Progress Bars

For long-running operations:

```cpp
void atto_main() {
  Console con;
  
  con.println("Processing files...");
  
  for (int i = 0; i <= 100; i++) {
    con.progress(i / 100.0f, String("Progress: ", i, "%"));
    Sleep(50);  // Simulate work
  }
  
  con.progressHide();
  con.println("Done!", CON_GREEN);
  
  Exit();
}
```

### Interactive Menus

```cpp
void atto_main() {
  Console con;
  
  List options("Start new game", "Load saved game", "Options", "Exit");
  int choice = con.menu(options, "Main Menu");
  
  if (choice == 0) {
    con.println("Starting new game...");
  } else if (choice == 3 || choice == -1) {  // -1 if user pressed Escape
    con.println("Goodbye!");
  }
  
  Exit();
}
```

---

## Running Other Programs

Sometimes you need to launch external programs. The `Subprocess` class handles this:

```cpp
void atto_main() {
  // Run and wait
  int exitCode = Subprocess::Run(Path("C:\\Windows\\notepad.exe"));
  Log("Notepad exited with code ", exitCode);
  
  // Capture output
  String output = Subprocess::Capture(Path("C:\\Windows\\System32\\ipconfig.exe"), "/all");
  Log("IP Configuration:\n", output);
  
  // Fire and forget
  Subprocess::Start(Path("C:\\Windows\\explorer.exe"), "C:\\");
  
  Exit();
}
```

For interactive communication with a subprocess:

```cpp
void atto_main() {
  Subprocess proc(Path("C:\\Windows\\System32\\cmd.exe"));
  
  // Send commands
  proc.write("echo Hello from attoboy!\r\n");
  proc.write("exit\r\n");
  
  // Wait and read output
  proc.wait();
  String output = proc.readToString();
  Log("Output:\n", output);
  
  Exit();
}
```

---

## Threads for Parallel Work

When you need to do multiple things simultaneously:

```cpp
void *backgroundTask(void *arg) {
  String name = *((String *)arg);
  Log("Background task started: ", name);
  Sleep(2000);  // Simulate work
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

For protecting shared data:

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
  
  Log("Final counter value: ", sharedCounter);  // Safely 2000
  
  Exit();
}
```

---

## Mathematical Operations

The `Math` class provides all the standard mathematical functions:

```cpp
void atto_main() {
  // Constants
  Log("Pi: ", Math::PI);
  Log("E: ", Math::E);
  
  // Random numbers
  Log("Random int: ", Math::Random());
  Log("Random float [0,1): ", Math::RandomFloat());
  Log("Random in range [1,10): ", Math::RandomRange(1, 10));
  Log("Random bool: ", Math::RandomBool());
  
  // Common operations
  Log("Abs(-5): ", Math::Abs(-5));
  Log("Min(3,7): ", Math::Min(3, 7));
  Log("Max(3,7): ", Math::Max(3, 7));
  Log("Clamp(15, 0, 10): ", Math::Clamp(15, 0, 10));  // 10
  
  // Trigonometry (radians)
  float angle = Math::PI / 4;  // 45 degrees
  Log("Sin(45°): ", Math::Sin(angle));
  Log("Cos(45°): ", Math::Cos(angle));
  
  // Powers and roots
  Log("Sqrt(16): ", Math::Sqrt(16.0f));
  Log("Pow(2, 10): ", Math::Pow(2.0f, 10.0f));
  
  // Rounding
  Log("Floor(3.7): ", Math::Floor(3.7f));  // 3
  Log("Ceil(3.2): ", Math::Ceil(3.2f));    // 4
  Log("Round(3.5): ", Math::Round(3.5f));  // 4
  
  Exit();
}
```

---

## Logging

We've been using `Log()` throughout this tutorial, but attoboy provides a more sophisticated logging system:

```cpp
void atto_main() {
  // Different log levels
  LogDebug("This is a debug message");
  LogInfo("This is an info message");
  LogWarning("This is a warning");
  LogError("This is an error");
  
  // Log to a file instead of console
  EnableLoggingToFile("app.log");
  
  LogInfo("This goes to the file");
  LogInfo("So does this");
  
  // Back to console
  EnableLoggingToConsole();
  LogInfo("Back to console");
  
  Exit();
}
```

Log levels are controlled at compile time for efficiency. By default, `LogInfo`, `LogWarning`, and `LogError` are enabled, while `LogDebug` requires defining `ATTOBOY_LOG_DEBUG_ENABLE` before including the header.

---

## Environment and System Information

```cpp
void atto_main() {
  // Environment variables
  String path = GetEnv("PATH");
  Log("PATH has ", path.length(), " characters");
  
  SetEnv("MY_APP_CONFIG", "production");
  
  // User information
  Log("User: ", GetUserName());
  Log("Display name: ", GetUserDisplayName());
  
  // Process ID
  Log("Process ID: ", GetProcessId());
  
  // Pausing
  Log("Waiting 2 seconds...");
  Sleep(2000);
  Log("Done waiting!");
  
  Exit();
}
```

---

## Windows Registry

For Windows-specific configuration storage:

```cpp
void atto_main() {
  Registry myAppSettings("HKEY_CURRENT_USER\\Software\\MyApp");
  
  // Create if needed
  if (!myAppSettings.exists()) {
    myAppSettings.create();
  }
  
  // Store values
  myAppSettings.setStringValue("lastUser", "alice");
  myAppSettings.setIntegerValue("windowWidth", 1024);
  myAppSettings.setIntegerValue("windowHeight", 768);
  
  // Read them back
  String user = myAppSettings.getStringValue("lastUser");
  int width = myAppSettings.getIntegerValue("windowWidth");
  
  Log("Last user was ", user, " with window ", width, " pixels wide");
  
  Exit();
}
```

---

## A Complete Example: A Mini Note-Taking App

Let's put several concepts together into a small but useful application:

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

This tiny application demonstrates file I/O, JSON persistence, console interaction with tab completion, colored output, and a simple command loop. When compiled, it produces an executable around 35 kilobytes that stores notes persistently and runs on any Windows system.

---

## Closing Thoughts

You've now seen the breadth of what attoboy offers: from basic string manipulation to file I/O, from web requests to AI integration, from console UIs to threading. The library was designed with a simple philosophy: writing C++ should feel natural and safe, and the resulting programs should be tiny enough to share effortlessly.

As you continue exploring, remember a few principles that will serve you well:

**Embrace immutability.** When a `String` method returns a new string rather than modifying the original, that's a feature. Your data stays predictable.

**Trust the RAII pattern.** When you create a `File`, `Console`, or `Thread`, it cleans up after itself when it goes out of scope. You don't need to remember to close things or free memory—attoboy handles it.

**Chain your methods.** Many attoboy methods return the object itself, letting you write expressive chains like `list.append(1).append(2).sort().reverse()`.

**Check your results.** Methods like `isEmpty()`, `isValid()`, and `succeeded()` help you handle errors gracefully.

**Start simple.** Your first programs don't need threading or AI or network requests. Start with strings and files, then expand as your needs grow.

The tiny executables you create with attoboy are complete, self-contained programs. No installers, no runtime dependencies, no compatibility concerns. Just small, fast, capable software that you can share with anyone.

Welcome to attoboy. We think you're going to enjoy it here.