# Console

## Overview

The `attoboy::Console` class is a high-level interface for building rich, interactive command-line applications on Windows. It combines:

* Colored text output.
* Automatic word wrapping and alignment.
* Cursor positioning and screen control.
* Interactive, line-based input with history and tab completion.
* Event-based input (keyboard and mouse).
* Animated progress bars.
* Interactive menus.
* Non-blocking global hotkeys.

The class uses **RAII** (Resource Acquisition Is Initialization) to manage console state:

* When a `Console` object is constructed, it configures the terminal for interactive use (UTF-8 output, mouse, resize handling, etc.).
* When it is destroyed, it restores the original console settings.

Only one `Console` instance should exist at a time. Because `Console` is non-copyable, it is straightforward to enforce this in your code.

### Console Concepts

If you are new to console programming, there are a few key concepts that will be helpful.

#### Console Coordinates

The console screen is a **grid of characters**. You can think of it as a 2D array:

* The horizontal axis is the **X coordinate** (columns).
* The vertical axis is the **Y coordinate** (rows).
* Coordinates are **0-based**, so the top-left corner is `(0, 0)`.

The `Console::width()` and `Console::height()` functions tell you the current size of this grid.

#### Colors and Attributes

The console supports **16 colors** defined by the `ConsoleColor` enum:

* For example, `CON_RED`, `CON_GREEN`, `CON_WHITE`, `CON_BLACK`, etc.

When you print text using `Console::print()` or `Console::println()`, you can pass a **foreground** and **background** color:

* Foreground (`fg`) controls the text color.
* Background (`bg`) controls the background color.

The defaults are:

* `fg = CON_WHITE`
* `bg = CON_BLACK`

#### Text Alignment and Wrapping

The `ConsoleAlign` enum defines how text is positioned:

* `CON_ALIGN_LEFT` (default)
* `CON_ALIGN_CENTER`
* `CON_ALIGN_RIGHT`
* `CON_ALIGN_JUSTIFY`

`Console::printAligned()` and `Console::printWrapped()` use these alignment settings and the available width to lay out your text.

There are also static helpers:

* `Console::Align()` – returns a padded/aligned line.
* `Console::Wrap()` – returns a wrapped multi-line string.

These do not draw anything themselves; they just return `String` values that you can print or manipulate.

#### Interactive Input and Events

`Console::input()` provides line-based input with optional advanced features controlled by a `ConsoleInput` structure:

* Tab completion.
* Input history.
* Password mode (masked input).
* Multi-line mode.

`Console::readEvent()` goes lower level and returns **events** such as key presses and mouse actions as human-readable strings like `"Enter"`, `"Ctrl+C"`, or `"MouseLeft@10,5"`.

#### Progress Bars and Menus

The console can show a persistent **progress bar** at the bottom of the screen. You can update it with `Console::progress()` and hide it with `Console::progressHide()`.

The `Console::menu()` method lets you display an interactive menu, navigate with keyboard or mouse, and returns the selected option index.

#### Global Hotkeys

You can register **global hotkeys** with `Console::onHotkey()`:

* For example `"Ctrl+Q"` or `"Shift+F1"`.
* When pressed, a callback function is invoked asynchronously.
* You can unregister hotkeys with `Console::offHotkey()`.

This enables responsive user interfaces where certain key combinations trigger actions, even while you are waiting for input or updating the screen.

---

## Reference

This section covers every public constructor, destructor, member function, static function, and operator of the `Console` class. For each symbol, you will find:

* **Signature** – exact declaration from the header.
* **Synopsis** – original one-line Doxygen comment.
* **Parameters** – description of each argument.
* **Return value** – description of the return value (if applicable).
* **In Depth** – detailed behavior, usage notes, and caveats.
* A short **example** snippet with a brief caption.

> All examples assume `using namespace attoboy;`.

---

#### `Console::Console()`

**Signature**

```cpp
Console();
```

**Synopsis**
Initializes the console for interactive use.
Enables mouse input, sets up resize handling, and configures UTF-8 output.

**Parameters**

* *(none)*

**Return value**

* *(constructor; not applicable)*

**In Depth**

Constructing a `Console` object:

* Puts the terminal into a mode suitable for interactive UI:

  * UTF-8 output.
  * Mouse event reporting.
  * Handling of console resize, so UI elements can reflow.
* Captures the **previous console state**, so it can be restored in the destructor.

Because only one `Console` instance should exist at a time, it is best to create it once at the beginning of your program’s interactive section and keep it alive until you no longer need console UI.

**Example**

```cpp
Console console;
console.println("Interactive console initialized.");
```

*This example constructs a `Console` and prints a confirmation message.*

---

#### `Console::~Console()`

**Signature**

```cpp
~Console();
```

**Synopsis**
Restores the original console state and settings.

**Parameters**

* *(none)*

**Return value**

* *(destructor; not applicable)*

**In Depth**

When a `Console` object is destroyed:

* Any changes made to console configuration (color modes, input handling, etc.) are undone.
* The console returns to its original state, so other tools or shells are not affected.

This is the RAII pattern: acquiring resources in the constructor and releasing them in the destructor.

**Example**

```cpp
{
  Console console;
  console.println("This UI uses custom console settings.");
}
// After this block, the console state is restored automatically.
```

*This example shows how console settings are automatically cleaned up when the object goes out of scope.*

---

#### `Console::Console(const Console &) = delete`

**Signature**

```cpp
Console(const Console &) = delete;
```

**Synopsis**
Console cannot be copied.

**Parameters**

* *(none; this function is deleted and cannot be called)*

**Return value**

* *(not applicable; deleted function)*

**In Depth**

The copy constructor is **deleted**, which means you cannot copy a `Console`:

* No `Console other = console;`
* No function parameters or return values by value.

This prevents multiple `Console` objects from trying to manage the same underlying console state, which would be error-prone.

If you need to share a `Console`, pass it by reference or pointer instead.

**Example**

```cpp
Console console;

// The following line is NOT allowed and will not compile:
// Console copy = console;  // error: Console copy constructor is deleted
```

*This example illustrates that copying a `Console` instance is not permitted.*

---

#### `Console &Console::operator=(const Console &) = delete`

**Signature**

```cpp
Console &operator=(const Console &) = delete;
```

**Synopsis**
Console cannot be assigned.

**Parameters**

* *(none; this operator is deleted and cannot be called)*

**Return value**

* *(not applicable; deleted function)*

**In Depth**

The copy assignment operator is also **deleted**, so you cannot assign one `Console` to another. This is consistent with the non-copyable design.

If you find yourself wanting to assign consoles, it usually means you should restructure your code to hold a single `Console` object and pass references where needed.

**Example**

```cpp
Console a;
Console b;

// The following line is NOT allowed and will not compile:
// b = a;  // error: Console assignment operator is deleted
```

*This example shows that you cannot assign one `Console` instance to another.*

---

#### `int Console::width() const`

**Signature**

```cpp
int width() const;
```

**Synopsis**
Returns the console width in characters.

**Parameters**

* *(none)*

**Return value**

* Current width of the console (number of character columns).

**In Depth**

`width()` tells you how many characters fit on a single row of the console window. Because the console can be resized by the user, this value may change over time. If you care about layout, call `width()` at the time you render.

Common uses:

* Centering or right-aligning text.
* Designing layout grids.
* Choosing wrapping widths.

**Example**

```cpp
Console console;
int w = console.width();

String msg("Console width: ");
console.println(msg + String(w));
```

*This example reads the console width and prints it as part of a message.*

---

#### `int Console::height() const`

**Signature**

```cpp
int height() const;
```

**Synopsis**
Returns the console height in characters.

**Parameters**

* *(none)*

**Return value**

* Current height of the console (number of character rows).

**In Depth**

`height()` tells you how many text rows fit in the console window. As with `width()`, this value can change if the user resizes the window.

This is helpful when:

* Designing full-screen UIs.
* Placing menus or status bars relative to the bottom of the screen.
* Avoiding output that scrolls unnecessarily.

**Example**

```cpp
Console console;
int h = console.height();

console.println("Console height: " + String(h));
```

*This example prints the current console height in rows.*

---

#### `Console &Console::print(const String &text, ConsoleColor fg = CON_WHITE, ConsoleColor bg = CON_BLACK)`

**Signature**

```cpp
Console &print(const String &text, ConsoleColor fg = CON_WHITE,
               ConsoleColor bg = CON_BLACK);
```

**Synopsis**
Prints text with optional foreground and background colors.

**Parameters**

* `text` – Text to print at the current cursor position.
* `fg` – Foreground text color (default `CON_WHITE`).
* `bg` – Background color (default `CON_BLACK`).

**Return value**

* Reference to `*this`, allowing you to chain multiple calls.

**In Depth**

`print()` writes text to the console without adding a newline at the end. The cursor advances by the number of characters printed.

Color behavior:

* Colors apply to the printed text.
* After printing, the console may remain in the new color state. If you want to revert to a default, explicitly print with your preferred colors.

You can chain calls:

```cpp
console.print("Label: ").print("value", CON_GREEN);
```

**Example**

```cpp
Console console;

console.print("OK ", CON_GREEN);
console.print("ERROR ", CON_RED);
console.print("DEFAULT");
```

*This example prints three words with different colors using chained calls.*

---

#### `Console &Console::println(const String &text = "", ConsoleColor fg = CON_WHITE, ConsoleColor bg = CON_BLACK)`

**Signature**

```cpp
Console &println(const String &text = "", ConsoleColor fg = CON_WHITE,
                 ConsoleColor bg = CON_BLACK);
```

**Synopsis**
Prints text followed by a newline.

**Parameters**

* `text` – Text to print (may be empty).
* `fg` – Foreground color (default `CON_WHITE`).
* `bg` – Background color (default `CON_BLACK`).

**Return value**

* Reference to `*this`, allowing method chaining.

**In Depth**

`println()` prints the given text and then moves the cursor to the beginning of the next line. If `text` is empty (the default), it simply prints a blank line.

You can use it for typical “log line” style output:

```cpp
console.println("Starting application...");
```

**Example**

```cpp
Console console;

console.println("Normal line");
console.println("Highlighted line", CON_BLACK, CON_YELLOW);
console.println();  // blank line
console.println("Done.");
```

*This example prints several lines, including a highlighted line and a blank line.*

---

#### `Console &Console::clear()`

**Signature**

```cpp
Console &clear();
```

**Synopsis**
Clears the entire console screen.

**Parameters**

* *(none)*

**Return value**

* Reference to `*this`.

**In Depth**

`clear()` erases all visible content and typically moves the cursor to the top-left corner `(0, 0)`. Use it to:

* Start a fresh frame for a full-screen style interface.
* Remove cluttered output before rendering a new layout.

**Example**

```cpp
Console console;

console.println("This will disappear...");
console.clear();
console.println("Screen cleared.");
```

*This example clears the screen and prints a new message afterwards.*

---

#### `Console &Console::printAligned(const String &text, int width, ConsoleAlign align = CON_ALIGN_LEFT, ConsoleColor fg = CON_WHITE, ConsoleColor bg = CON_BLACK)`

**Signature**

```cpp
Console &printAligned(const String &text, int width,
                      ConsoleAlign align = CON_ALIGN_LEFT,
                      ConsoleColor fg = CON_WHITE,
                      ConsoleColor bg = CON_BLACK);
```

**Synopsis**
Prints text aligned within the specified width.
If width is -1, uses the current console width.

**Parameters**

* `text` – Text to print.
* `width` – Field width in characters, or `-1` to use the console width.
* `align` – Alignment mode (`CON_ALIGN_LEFT`, `CON_ALIGN_CENTER`, `CON_ALIGN_RIGHT`, `CON_ALIGN_JUSTIFY`).
* `fg` – Foreground color (default `CON_WHITE`).
* `bg` – Background color (default `CON_BLACK`).

**Return value**

* Reference to `*this`.

**In Depth**

`printAligned()` handles both **alignment** and **padding**:

* If `width` is positive, the text is aligned within that many columns.
* If `width == -1`, the function uses the current console width.
* Padding is added as needed; overly long text may be truncated or wrapped depending on implementation.

Typical uses:

* Title bars or headers.
* Column-based layouts.
* Centered banners.

**Example**

```cpp
Console console;
int w = console.width();

String title("My Application");
console.printAligned(title, w, CON_ALIGN_CENTER, CON_YELLOW);
console.println();
```

*This example centers a title in the current console width using a bright color.*

---

#### `Console &Console::printWrapped(const String &text, int width = -1, ConsoleColor fg = CON_WHITE, ConsoleColor bg = CON_BLACK)`

**Signature**

```cpp
Console &printWrapped(const String &text, int width = -1,
                      ConsoleColor fg = CON_WHITE,
                      ConsoleColor bg = CON_BLACK);
```

**Synopsis**
Prints text with automatic word wrapping.
If width is -1, uses the current console width.

**Parameters**

* `text` – Text to print; may contain existing line breaks.
* `width` – Wrap width in characters, or `-1` for console width.
* `fg` – Foreground color (default `CON_WHITE`).
* `bg` – Background color (default `CON_BLACK`).

**Return value**

* Reference to `*this`.

**In Depth**

`printWrapped()`:

* Wraps text at **word boundaries** to avoid breaking words in the middle.
* Respects existing line breaks (e.g., already separated paragraphs).
* Uses `width` to determine where to wrap each line.

If `width == -1`, the function uses `Console::width()` at the time of printing. This is useful when the user resizes the console; each call adapts to the current size.

**Example**

```cpp
Console console;

String paragraph(
  "This is a long paragraph that will be automatically wrapped "
  "to fit the console width without breaking words in the middle."
);

console.printWrapped(paragraph);
console.println();
```

*This example prints a paragraph that automatically wraps to the console width.*

---

#### `Console &Console::setCursor(int x, int y)`

**Signature**

```cpp
Console &setCursor(int x, int y);
```

**Synopsis**
Moves the cursor to the specified position (0-based coordinates).

**Parameters**

* `x` – Column position (0 = leftmost).
* `y` – Row position (0 = topmost).

**Return value**

* Reference to `*this`.

**In Depth**

`setCursor()` repositions the text cursor. The next printed character will appear at `(x, y)`.

You should ensure that:

* `0 <= x < width()`
* `0 <= y < height()`

Out-of-range coordinates may be ignored or clamped by the underlying API.

This is often used for:

* Redrawing only parts of the screen.
* Implementing status bars or progress indicators at fixed positions.

**Example**

```cpp
Console console;

console.clear();
console.setCursor(0, 0).println("Top left");
console.setCursor(10, 5).println("At (10,5)");
```

*This example moves the cursor to different positions and prints text there.*

---

#### `Console &Console::showCursor()`

**Signature**

```cpp
Console &showCursor();
```

**Synopsis**
Shows the text cursor.

**Parameters**

* *(none)*

**Return value**

* Reference to `*this`.

**In Depth**

`showCursor()` makes the blinking text cursor visible. This is appropriate when:

* Waiting for user input at the current position.
* Navigating forms or entry fields.

**Example**

```cpp
Console console;

console.hideCursor();
console.println("Working...");
console.showCursor();
console.println("Ready for input.");
```

*This example hides the cursor while doing work, then shows it again when ready for input.*

---

#### `Console &Console::hideCursor()`

**Signature**

```cpp
Console &hideCursor();
```

**Synopsis**
Hides the text cursor.

**Parameters**

* *(none)*

**Return value**

* Reference to `*this`.

**In Depth**

`hideCursor()` removes the visible cursor, which is useful when:

* Showing full-screen dashboards or animations.
* Displaying menus where the cursor position is not meaningful.

Remember to call `showCursor()` before terminating your program or when returning to a standard input prompt so the user is not left without a cursor.

**Example**

```cpp
Console console;

console.hideCursor();
console.println("Rendering a full-screen interface without a blinking cursor...");
```

*This example hides the cursor while rendering an interface.*

---

### `ConsoleInput` Configuration Struct

#### Overview

The `ConsoleInput` struct configures how `Console::input()` behaves. It is a simple, plain-data container with four fields and standard value semantics (constructor, copy, assignment, destructor).

You use `ConsoleInput` when you want to move beyond a basic, single-line prompt and enable features such as:

* **Tab completion** based on a list of candidate strings.
* Persistent **command history** with up/down arrow navigation.
* **Password entry** with masked characters.
* **Multi-line input** with a custom continuation prompt.

---

#### Reference

##### Fields

Each field is public and can be set directly.

---

###### `List ConsoleInput::completions`

**Signature**

```cpp
List completions;
```

**Synopsis**
Tab completion candidates (empty list for no completion).

**Meaning**

* `List` of possible completion strings.
* If empty, tab completion is disabled.

**In Depth**

When `completions` is non-empty, `Console::input()` can offer tab completion using the candidate values in the list. Typical behavior:

* Pressing **Tab** cycles through or suggests completions.
* The exact UI (inline replacement, hints, etc.) follows the library implementation.

**Example**

```cpp
Console console;
ConsoleInput opts;

// Prepare completion candidates:
opts.completions.append("start").append("status").append("stop");

String cmd = console.input("> ", opts);
console.println("Command: " + cmd);
```

*This example enables simple tab completion for three commands: `start`, `status`, and `stop`.*

---

###### `List ConsoleInput::history`

**Signature**

```cpp
List history;
```

**Synopsis**
Command history buffer (read/write, empty list for no history).
History is automatically updated with each input.

**Meaning**

* `List` that stores previous input lines.
* If empty, no history is kept and up/down arrows do not navigate past commands.

**In Depth**

When `history` is non-empty:

* `Console::input()` treats the list as both **input** and **output**:

  * It reads existing entries to provide history navigation with **Up/Down** arrow keys.
  * It appends each successfully entered line to the list automatically.
* You can pre-populate the history before calling `input()`:

  * For example, with default commands or previously saved history.

**Example**

```cpp
Console console;
ConsoleInput opts;

console.println("Type commands; Up/Down to navigate history, blank to quit.");

while (true) {
  String line = console.input("> ", opts);
  if (line.trim().isEmpty()) {
    break;
  }
  console.println("You entered: " + line);
}

// opts.history now contains all entered lines
```

*This example enables persistent history across multiple input calls, with up/down arrow navigation.*

---

###### `bool ConsoleInput::password`

**Signature**

```cpp
bool password;
```

**Synopsis**
Mask input with asterisks for password entry.

**Meaning**

* If `true`, `Console::input()` masks characters (for example with `*`) as the user types.
* If `false`, input is shown normally.

**In Depth**

Password mode is useful for:

* Authentication prompts.
* Secret tokens or API keys.

Behavior notes:

* The returned string still contains the real characters.
* Only the **display** in the console is masked.
* Combining `password = true` with `history != nullptr` is usually a bad idea:

  * Passwords would be stored in the history list.
  * If you enable `password`, you typically want `history = nullptr`.

**Example**

```cpp
Console console;
ConsoleInput opts;

opts.password = true;   // mask typed characters
opts.history = nullptr; // avoid storing passwords in history

String pwd = console.input("Enter password: ", &opts);
console.println("Password length: " + String(pwd.length()));
```

*This example reads a password with masked input and avoids recording it in history.*

---

###### `bool ConsoleInput::multiline`

**Signature**

```cpp
bool multiline;
```

**Synopsis**
Allow multi-line input (Shift+Enter or Ctrl+Enter inserts newline).

**Meaning**

* If `true`, `Console::input()` allows the user to insert line breaks.
* If `false`, pressing Enter finishes the input line immediately.

**In Depth**

In multiline mode:

* The user can enter multi-line text, for example:

  * Configuration blocks.
  * Script snippets.
  * Long messages or notes.
* Special key combinations (such as **Shift+Enter** or **Ctrl+Enter**) insert a newline rather than finishing input.
* The `continuation` string is used as the prompt for continuation lines.

The final return value from `Console::input()` is a single `String` that may contain `'\n'` characters.

**Example**

```cpp
Console console;
ConsoleInput opts;

opts.multiline = true;
opts.continuation = String("... ");  // prompt for continuation lines

console.println("Enter a multi-line message (finish with plain Enter on an empty line).");
String text = console.input("> ", &opts);

console.println("You entered:");
console.println(text);
```

*This example enables multi-line input with a custom continuation prompt for wrapped lines.*

---

###### `String ConsoleInput::continuation`

**Signature**

```cpp
String continuation;
```

**Synopsis**
Prompt shown for continuation lines in multiline mode.

**Meaning**

* Text used as the prompt when `multiline` is `true` and the user continues to the next line.
* Ignored when `multiline` is `false`.

**In Depth**

When using multiline input:

* The first line uses the `prompt` parameter you pass to `Console::input()`.
* Each subsequent line uses `options->continuation` instead, allowing you to visually distinguish continuation lines from the initial prompt.

A few design tips:

* Choose a short continuation string, such as `"… "` or `"  > "`.
* Avoid long or multi-line prompts; they may interfere with wrapping.

If you leave `continuation` as the default `String()`, the library will use an empty or built-in default prompt for continuation lines (implementation-dependent).

**Example**

```cpp
Console console;
ConsoleInput opts;

opts.multiline = true;
opts.continuation = String("... ");

String text = console.input("> ", &opts);
// Additional lines the user enters will start with "... "
```

*This example sets a custom continuation prompt for multi-line input, making follow-up lines visually distinct.*

---

##### Member Functions

---

###### `ConsoleInput::ConsoleInput()`

**Signature**

```cpp
ConsoleInput();
```

**Synopsis**
Creates default input options (no completion, no history, single-line, visible).

**Parameters**

* *(none)*

**Return value**

* *(constructor; not applicable)*

**In Depth**

The default constructor initializes all fields to sensible defaults:

* `completions = List()` – no tab completion.
* `history = List()` – no command history.
* `password = false` – input is visible.
* `multiline = false` – single-line input only.
* `continuation = String()` – empty continuation prompt.

You can construct a `ConsoleInput` and then customize only the fields you care about.

**Example**

```cpp
ConsoleInput opts;  // all defaults: plain single-line input

opts.password = true;  // enable password masking only
```

*This example constructs the default options and then changes just one field.*

---

###### `ConsoleInput::ConsoleInput(const ConsoleInput &other)`

**Signature**

```cpp
ConsoleInput(const ConsoleInput &other);
```

**Synopsis**
Creates a copy of another ConsoleInput.

**Parameters**

* `other` – Existing `ConsoleInput` to copy.

**Return value**

* *(constructor; not applicable)*

**In Depth**

The copy constructor duplicates all configuration fields:

* `List` fields (`completions`, `history`) are deep copied:
  * The original and the copy have their own separate copies of all the elements.
* Value fields (`password`, `multiline`, `continuation`) are copied by value.

Use this when you want to start from an existing configuration and tweak it slightly.

**Example**

```cpp
ConsoleInput base;
base.multiline = true;

ConsoleInput derived(base);
derived.password = true;  // separate configuration for another prompt
```

*This example copies a base configuration and then adjusts one flag in the copy.*

---

###### `ConsoleInput::~ConsoleInput()`

**Signature**

```cpp
~ConsoleInput();
```

**Synopsis**
Destroys the ConsoleInput.

**Parameters**

* *(none)*

**Return value**

* *(destructor; not applicable)*

**In Depth**

The destructor releases any internal resources owned by `ConsoleInput` (primarily the `String continuation` field). It does **not** free the `List` instances pointed to by `completions` or `history`; those remain your responsibility.

This makes `ConsoleInput` cheap to construct and destroy and safe to store on the stack.

**Example**

```cpp
{
  ConsoleInput opts;
  opts.password = true;
  // use opts...
}
// opts is destroyed here; continuation is cleaned up automatically
```

*This example shows a `ConsoleInput` used in a local scope.*

---

###### `ConsoleInput &ConsoleInput::operator=(const ConsoleInput &other)`

**Signature**

```cpp
ConsoleInput &operator=(const ConsoleInput &other);
```

**Synopsis**
Assigns another ConsoleInput to this one.

**Parameters**

* `other` – Source configuration to copy from.

**Return value**

* Reference to `*this`, allowing assignment chaining.

**In Depth**

The assignment operator replaces the current configuration with the contents of `other`:

* `List` fields now point to copies of the same `List` instances as `other`.
* Boolean fields and `continuation` are copied by value.

Self-assignment is safe.

**Example**

```cpp
ConsoleInput a;
ConsoleInput b;

a.password = true;
a.multiline = true;

b = a;  // b now has the same configuration as a
```

*This example copies configuration from one `ConsoleInput` to another using assignment.*

---
---

#### `String Console::input(const String &prompt = "", const ConsoleInput &options = ConsoleInput())`

**Signature**

```cpp
String input(const String &prompt = "",
             const ConsoleInput &options = ConsoleInput());
```

**Synopsis**
Reads a line of input with optional prompt and advanced options.
Supports up/down arrow history, tab completion with hints, and more.
Returns empty string if input is cancelled (Ctrl+C).

**Parameters**

* `prompt` – Text shown before the input area (for example `"> "`).
* `options` – `ConsoleInput` configuration structure. Defaults to a default-constructed `ConsoleInput` if not provided.

**Return value**

* A `String` containing the user's input line, or an empty `String` if the input was cancelled (for example via `Ctrl+C`).

**In Depth**

`input()` is the high-level input function that supports:

* Prompt text (for example `"> "`).
* Optional **history**, if `options.history` is provided:

  * Up/down arrows to navigate previous commands.
* Optional **tab completion**, if `options.completions` is provided:

  * Suggestions based on a list of candidate strings.
* **Password mode**, when `options.password` is true:

  * Input characters are masked (for example with `'*'`).
* **Multi-line mode**, when `options.multiline` is true:

  * `Shift+Enter` or `Ctrl+Enter` inserts a newline.
  * The `ConsoleInput::continuation` string is used as the prompt for subsequent lines.

When `options` is default-constructed, the function uses reasonable defaults:

* No history.
* No tab completion.
* Single-line, visible input.

Check for an empty result to detect cancellation.

**Example**

```cpp
Console console;
ConsoleInput opts;

// Enable simple history:
List history;
opts.history = &history;

// Prompt for a command:
String line = console.input("> ", opts);

if (!line.isEmpty()) {
  console.println("You typed: " + line);
}
```

*This example reads a line of input with history support and echoes it back.*

---

#### `String Console::readEvent()`

**Signature**

```cpp
String readEvent();
```

**Synopsis**
Reads a single key press or mouse event (blocking).
Returns a descriptive name: "A", "Enter", "Space", "Ctrl+C", "Alt+X",
"F1", "MouseLeft@x,y", "MouseRight@x,y", "MouseMove@x,y", etc.

**Parameters**

* *(none)*

**Return value**

* A `String` describing the event in a readable format.

**In Depth**

`readEvent()` provides low-level, event-based input:

* It blocks until a key or mouse event occurs.
* The returned string describes the event:

  * `"A"`, `"b"`, `"Enter"`, `"Space"`.
  * `"Ctrl+C"`, `"Alt+X"`, `"Shift+F1"`.
  * Mouse events like `"MouseLeft@10,5"` or `"MouseMove@20,3"`.

This is useful for:

* Custom input handling loops.
* Interactive menus or games.
* Handling mouse clicks on specific screen regions.

You can parse the returned string to interpret the event type and coordinates.

**Example**

```cpp
Console console;

console.println("Press any key or click the mouse. Press Esc to quit.");

while (true) {
  String event = console.readEvent();
  console.println("Event: " + event);

  if (event == "Escape") {
    break;
  }
}
```

*This example prints every event (key or mouse) until the user presses Escape.*

---

#### `Console &Console::progress(float percent, const String &label = "")`

**Signature**

```cpp
Console &progress(float percent, const String &label = "");
```

**Synopsis**
Shows or updates an animated progress bar at the bottom of the console.
percent: 0.0 to 1.0 for determinate, or -1.0 for indeterminate spinner.
The bar animates smoothly between values and resizes with the console.

**Parameters**

* `percent` – Progress value:

  * Between `0.0` and `1.0` for determinate progress.
  * `-1.0` for an indeterminate spinner (unknown progress).
* `label` – Optional text label shown alongside the bar.

**Return value**

* Reference to `*this`.

**In Depth**

`progress()` manages a persistent bar at the bottom of the console:

* It reserves space and updates only the bar area, leaving your main UI intact.
* When `percent` increases from one call to the next, the bar animates smoothly.
* If the console is resized, the bar layout adapts to the new width.
* Use `percent == -1.0f` when you do not know the duration and just want an activity indicator.

You can call `progress()` repeatedly as your task advances.

**Example**

```cpp
Console console;

for (int i = 0; i <= 100; ++i) {
  float p = i / 100.0f;
  console.progress(p, "Processing...");
}
console.progressHide();
console.println("Done.");
```

*This example simulates a progressing task and updates the progress bar accordingly.*

---

#### `Console &Console::progressHide()`

**Signature**

```cpp
Console &progressHide();
```

**Synopsis**
Hides the progress bar and restores the line.

**Parameters**

* *(none)*

**Return value**

* Reference to `*this`.

**In Depth**

After calling `progress()` one or more times, use `progressHide()` to:

* Remove the progress bar from the screen.
* Restore any underlying content or free the line for further output.

It is good practice to call this when your operation completes or is cancelled.

**Example**

```cpp
Console console;

console.progress(-1.0f, "Working...");
// ... do some work ...
console.progressHide();
console.println("Operation finished.");
```

*This example shows an indeterminate progress indicator and then hides it.*

---

#### `int Console::menu(const List &options, const String &title = "")`

**Signature**

```cpp
int menu(const List &options, const String &title = "");
```

**Synopsis**
Shows an interactive menu and returns the selected index (0-based).
Returns -1 if cancelled (Escape key).
Supports arrow keys, number keys (1-9), mouse hover/click, and
first-letter jump.

**Parameters**

* `options` – `List` of menu entries; each element is converted to `String`.
* `title` – Optional title displayed above the menu.

**Return value**

* Selected option index (0-based), or `-1` if the user cancels (for example by pressing Escape).

**In Depth**

`menu()` manages the entire interaction:

* Renders the menu with highlighting.
* Supports various input methods:

  * Arrow keys to move up and down.
  * Number keys (`1`–`9`) to jump directly to options.
  * Mouse hover and click to select.
  * First-letter jump: pressing a letter jumps to the next option starting with that letter.
* Returns when the user selects an item or cancels.

You can call it multiple times to show different menus in sequence.

**Example**

```cpp
Console console;

List options;
options.append("Start").append("Settings").append("Quit");

int choice = console.menu(options, "Main Menu");

if (choice == 0) {
  console.println("Starting...");
} else if (choice == 1) {
  console.println("Opening settings...");
} else if (choice == 2) {
  console.println("Quitting...");
} else {
  console.println("Menu cancelled.");
}
```

*This example displays a simple main menu and handles the user’s choice.*

---

#### `Console &Console::onHotkey(const String &key, void (*callback)(void *), void *arg = nullptr)`

**Signature**

```cpp
Console &onHotkey(const String &key, void (*callback)(void *),
                  void *arg = nullptr);
```

**Synopsis**
Registers a non-blocking global hotkey callback.
Key format examples: "Ctrl+Q", "Alt+X", "Shift+F1", "F12", "Ctrl+Shift+S".
The callback is invoked asynchronously when the key is pressed.

**Parameters**

* `key` – Hotkey description string, such as `"Ctrl+Q"` or `"F12"`.
* `callback` – Function pointer to a callback that takes a `void *` argument.
* `arg` – Optional user data pointer passed to the callback.

**Return value**

* Reference to `*this`.

**In Depth**

`onHotkey()` lets you register **global hotkeys** that:

* Are monitored in the background.
* Trigger a callback when the key combination is pressed.
* Do not block your main thread or UI loop.

This is useful for:

* Emergency exit keys (`"Ctrl+Q"`).
* Toggling views.
* Pausing/resuming operations.

Important considerations:

* Callbacks run asynchronously; avoid long-running or blocking work inside them.
* If you modify shared data from a callback, ensure proper synchronization (for example using `Mutex` from the library).

**Example**

```cpp
void QuitCallback(void *arg) {
  Console *console = (Console *)arg;
  console->println("Quit hotkey pressed.");
  // You might set a global flag here to stop the main loop.
}

Console console;

console.onHotkey("Ctrl+Q", QuitCallback, &console);
console.println("Press Ctrl+Q to trigger the hotkey (and then exit).");

// In a real program, you would now run some loop or UI logic.
```

*This example registers a Ctrl+Q hotkey that prints a message when pressed.*

---

#### `Console &Console::offHotkey(const String &key)`

**Signature**

```cpp
Console &offHotkey(const String &key);
```

**Synopsis**
Unregisters a previously registered hotkey.

**Parameters**

* `key` – Hotkey description string, exactly as used in `onHotkey()`.

**Return value**

* Reference to `*this`.

**In Depth**

`offHotkey()` disables a hotkey so it no longer triggers its callback. You should use this:

* Before destroying the `Console`, if you want to stop listening for additional events.
* When a feature or mode that uses a hotkey is turned off.

If the key is not registered, the call is typically a no-op.

**Example**

```cpp
Console console;

console.onHotkey("Ctrl+Q", nullptr);  // Example registration (callback omitted)
console.offHotkey("Ctrl+Q");          // Disable the hotkey again
```

*This example shows how to unregister a hotkey by key name.*

---

#### `static String Console::Align(const String &text, int width, ConsoleAlign align = CON_ALIGN_LEFT)`

**Signature**

```cpp
static String Align(const String &text, int width,
                    ConsoleAlign align = CON_ALIGN_LEFT);
```

**Synopsis**
Returns text padded and aligned to the specified width.
Useful for building formatted output or tables.

**Parameters**

* `text` – Text to align.
* `width` – Target width in characters.
* `align` – Alignment mode (`CON_ALIGN_LEFT`, `CON_ALIGN_CENTER`, `CON_ALIGN_RIGHT`, `CON_ALIGN_JUSTIFY`).

**Return value**

* A `String` containing the aligned and padded text.

**In Depth**

`Align()` is a **pure formatting helper**:

* It does **not** print to the console.
* It returns a single line with padding added as needed.
* If the text is longer than `width`, it may be truncated (implementation-dependent) or left as-is; design your widths accordingly.

This is useful for producing aligned columns or formatting text before printing or logging.

**Example**

```cpp
Console console;

String label = Console::Align("Name", 10, CON_ALIGN_LEFT);
String value = Console::Align("Alice", 10, CON_ALIGN_RIGHT);

console.println(label + value);
// e.g., "Name          Alice"
```

*This example builds a formatted line with a left-aligned label and right-aligned value.*

---

#### `static String Console::Wrap(const String &text, int width)`

**Signature**

```cpp
static String Wrap(const String &text, int width);
```

**Synopsis**
Returns text word-wrapped to fit the specified width.
Preserves existing line breaks and wraps at word boundaries.

**Parameters**

* `text` – Original paragraph(s) to wrap.
* `width` – Maximum line width in characters.

**Return value**

* A `String` with embedded newlines inserted to ensure each line fits within `width`.

**In Depth**

`Wrap()` takes a long string and returns a version:

* With newlines inserted between words to keep line lengths within `width`.
* Preserving existing line breaks, so you can wrap multi-paragraph text.

Because it returns a `String`, you can:

* Print it directly with `println()` or `printWrapped()`.
* Further split it with `lines()` or `split()`.

**Example**

```cpp
Console console;

String text(
  "This is some long text that we want to wrap to a fixed width "
  "before printing it."
);

String wrapped = Console::Wrap(text, 40);
console.println(wrapped);
```

*This example uses `Console::Wrap()` to format a paragraph to a fixed width before printing.*
