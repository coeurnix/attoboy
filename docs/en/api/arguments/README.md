# `attoboy::Arguments` User Guide

## Overview

The `attoboy::Arguments` class is a small, focused **command-line argument parser** for attoboy applications. It provides a structured way to declare:

* **Flags** (boolean switches such as `-v` or `--verbose`),
* **Named parameters** (key–value options such as `-output=log.txt`),
* **Positional parameters** (arguments identified by their position, such as input file names),

and then parse the actual command-line that started your program.

Instead of manually inspecting `argv` and writing custom parsing logic, you can:

1. Define what arguments your program expects.
2. Optionally mark some arguments as **required**.
3. Set human-readable **descriptions** and **help text**.
4. Call `parseArguments()` to validate and retrieve a structured `Map` of values.

`Arguments` uses attoboy’s own `String` and `Map` types, so it fits naturally into the rest of the library.

### Command-line Arguments, Flags, and Parameters

When a program is started from a terminal, it typically receives a list of **command-line arguments**, such as:

```text
mytool -verbose -mode=fast input.txt
```

These arguments usually fall into three categories:

* **Flags** – Boolean options that are either present or absent:

  * Example: `-verbose`, `--dry-run`
  * Often used to turn features on or off.

* **Named parameters** – Options with explicit values:

  * Example: `-mode=fast` or `-output result.json`
  * Used for configuration such as file paths, modes, or thresholds.

* **Positional parameters** – Values that are interpreted based on their position:

  * Example: `input.txt` or `source` and `destination`
  * Used for things like required input files.

`attoboy::Arguments` abstracts the messy details of parsing these from the raw command line:

* It **captures** the program’s command-line arguments when you construct an `Arguments` object.
* You **declare** the flags, parameters, and positional arguments you expect.
* You call **`parseArguments()`**, which validates the input, handles `-h/--help` automatically, and returns a `Map` of parsed values.

### Automatic Help and Required Arguments

When you use `Arguments`, the parser automatically handles:

* `-h` / `--help`: printing a help message (using descriptions you provide) and returning an empty `Map`.
* **Required arguments**: if the user omits a required flag or parameter, `parseArguments()` returns an empty `Map`, so you can abort cleanly instead of running with incomplete configuration.

This helps you:

* Keep your main program logic separate from command-line validation.
* Provide a consistent user experience across tools built with attoboy.

---

## Reference

All examples assume:

```cpp
using namespace attoboy;
```

---

### Construction, Copying, and Destruction

#### `Arguments()`

**Signature**

```cpp
Arguments();
```

**Synopsis**
Creates an Arguments parser and captures command-line arguments.

**Parameters**

* *(none)*

**Return value**

* *(constructor; not applicable)*

**In Depth**

Constructing an `Arguments` object captures the current process’s command-line arguments. Typically, you:

1. Construct the parser near the start of your program.
2. Declare which flags and parameters you expect.
3. Call `parseArguments()` to interpret the captured arguments.

You normally create just one `Arguments` instance per program, but you can create more if needed (for example, to parse sub-commands in different ways).

**Example**

```cpp
Arguments args;

// Declare expected options before parsing
args.addFlag("verbose", "Enable verbose output");
args.addParameter("mode", "Processing mode", "fast");

// Later, parse the captured command line
Map parsed = args.parseArguments();
```

*This example creates an `Arguments` parser, configures expected options, and prepares to parse the command line.*

---

#### `Arguments(const Arguments &other)`

**Signature**

```cpp
Arguments(const Arguments &other);
```

**Synopsis**
Creates a copy of another Arguments parser.

**Parameters**

* `other` – Existing `Arguments` instance to copy.

**Return value**

* *(constructor; not applicable)*

**In Depth**

The copy constructor lets you duplicate a parser configuration, including all defined flags, parameters, positional arguments, and any captured command-line state.

Typical use cases:

* Reusing a base configuration and then modifying the copy for slightly different parsing behavior.
* Passing parser state to helper components that may want to query arguments independently.

**Example**

```cpp
Arguments base;
base.addFlag("verbose", "Enable verbose output");

Arguments copy(base);
// copy now recognizes the same arguments as base
```

*This example copies a configured `Arguments` parser for reuse elsewhere.*

---

#### `~Arguments()`

**Signature**

```cpp
~Arguments();
```

**Synopsis**
Destroys the Arguments parser and frees memory.

**Parameters**

* *(none)*

**Return value**

* *(destructor; not applicable)*

**In Depth**

The destructor releases any internal resources used by the parser (such as allocated metadata or captured argument storage). Because `Arguments` follows RAII, simply letting the object fall out of scope is enough to clean up.

**Example**

```cpp
{
  Arguments args;
  // configure and parse here
} // args is destroyed here; resources are released
```

*This example shows automatic cleanup of the parser when it goes out of scope.*

---

#### `Arguments &operator=(const Arguments &other)`

**Signature**

```cpp
Arguments &operator=(const Arguments &other);
```

**Synopsis**
Assigns another Arguments parser to this one.

**Parameters**

* `other` – Parser whose configuration and state should be copied.

**Return value**

* Reference to `*this`, allowing assignment chaining.

**In Depth**

Assignment replaces the current parser’s configuration and captured state with those of `other`. After assignment:

* Both objects recognize the same declared arguments.
* Both see the same parsed state (until you call `parseArguments()` again).

Self-assignment is safe.

**Example**

```cpp
Arguments args;
args.addFlag("verbose", "Enable verbose output");

Arguments other;
other = args;  // other now has the same argument definitions
```

*This example assigns one parser’s configuration to another.*

---

### Configuring Expected Arguments

#### `Arguments &addFlag(const String &name, const String &description = String(), bool defaultValue = false, const String &longName = String())`

**Signature**

```cpp
Arguments &addFlag(const String &name, const String &description = String(),
                   bool defaultValue = false,
                   const String &longName = String());
```

**Synopsis**
Adds a boolean flag (-name, --longName, or -name=true/false).

**Parameters**

* `name` – Short name of the flag, used for `-name` (for example, `"verbose"` → `-verbose`).
* `description` – Optional human-readable description shown in the help text.
* `defaultValue` – Default boolean value if the flag is not provided on the command line.
* `longName` – Optional long name used for `--longName` (for example, `"verbose"` → `--verbose`). If empty, only `-name` is supported.

**Return value**

* Reference to `*this`, enabling method chaining.

**In Depth**

Flags are **boolean options** that can be toggled on or off. With `addFlag`, the parser recognizes these forms:

* `-name` – Typically means “set to true”.
* `--longName` – Long version of the same flag (if provided).
* `-name=true` / `-name=false` – Explicitly setting the value.

At parse time:

* The flag’s value is stored as a `String` `"true"` or `"false"` and can be accessed via `getArgument(name)` or `getArgument(longName)` (depending on implementation).
* `defaultValue` determines what `getArgument()` returns if the user does not set the flag, and whether `hasArgument()` reports it as present.

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

*This example declares a `-verbose` / `--verbose` flag with a default of `false` and uses its parsed value to control logging.*

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
Adds a named parameter (-name=value or -name value).

**Parameters**

* `name` – Short name of the parameter (for example, `"output"` → `-output`).
* `description` – Optional human-readable description for help output.
* `defaultValue` – Default string value if the parameter is not provided.
* `longName` – Optional long form (for example, `"output"` → `--output`). If empty, only the short form is used.

**Return value**

* Reference to `*this`, enabling method chaining.

**In Depth**

Named parameters are **key–value options**. The parser typically accepts these forms:

* `-name=value`
* `-name value`
* `--longName=value`
* `--longName value`

The value is stored as a `String` and can be retrieved with `getArgument(name)`.

If the user omits the parameter:

* `getArgument(name)` returns `defaultValue`.
* `hasArgument(name)` returns `true` if there is a default value, even if the user did not supply it.

This is ideal for settings like output paths, modes, or thresholds.

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

*This example declares an `-output` / `--output` parameter with a default file name and reads the resulting value after parsing.*

---

#### `Arguments &addPositionalParameter(const String &name, const String &description = String())`

**Signature**

```cpp
Arguments &addPositionalParameter(const String &name,
                                  const String &description = String());
```

**Synopsis**
Adds a positional parameter (filled in order from remaining args).

**Parameters**

* `name` – Name used to identify this positional parameter in the result map.
* `description` – Optional human-readable description shown in help.

**Return value**

* Reference to `*this`, enabling method chaining.

**In Depth**

Positional parameters represent arguments that are recognized **by position** rather than prefixed with `-` or `--`. After all flags and named parameters are parsed, the parser assigns remaining raw arguments in order to the positional parameters you declare.

For example, if you define:

```text
addPositionalParameter("input");
addPositionalParameter("output");
```

and the user runs:

```text
mytool -mode=fast in.txt out.txt
```

then:

* `"input"` receives `"in.txt"`,
* `"output"` receives `"out.txt"`.

You retrieve these values with `getArgument("input")` and `getArgument("output")`.

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

*This example defines two positional parameters and uses them as source and destination paths.*

---

#### `Arguments &setHelp(const String &helpText)`

**Signature**

```cpp
Arguments &setHelp(const String &helpText);
```

**Synopsis**
Sets custom help text for -h/--help.

**Parameters**

* `helpText` – A `String` containing custom help or usage text to display when `-h` or `--help` is requested.

**Return value**

* Reference to `*this`, enabling method chaining.

**In Depth**

By default, the parser can generate help text based on:

* The arguments you have added,
* Their names and descriptions.

`setHelp()` lets you override or supplement this with your own help content. When the user passes `-h` or `--help`, `parseArguments()`:

* Displays the help text (which may be your custom text), and
* Returns an empty `Map` so you can abort the normal execution path.

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

*This example sets a custom usage message that is shown when the user requests help.*

---

#### `Arguments &requireArgument(const String &name)`

**Signature**

```cpp
Arguments &requireArgument(const String &name);
```

**Synopsis**
Marks an argument as required.

**Parameters**

* `name` – Name of the argument to mark as required. This can be the name of a flag, named parameter, or positional parameter.

**Return value**

* Reference to `*this`, enabling method chaining.

**In Depth**

`requireArgument()` tells the parser that a particular argument **must** be supplied by the user. If the argument is missing when `parseArguments()` is called:

* The parser considers this an error.
* It returns an empty `Map`.
* It may print an error and help message to inform the user (depending on implementation).

You can chain calls to mark several arguments as required.

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

*This example enforces that the `input` positional parameter must be supplied.*

---

### Querying Parsed Arguments

#### `String getArgument(const String &name) const`

**Signature**

```cpp
String getArgument(const String &name) const;
```

**Synopsis**
Returns the value of the argument (flags return "true" or "false").

**Parameters**

* `name` – Name of the argument whose value you want. This is the same name used in `addFlag`, `addParameter`, or `addPositionalParameter`.

**Return value**

* A `String` representing the argument’s value:

  * Flags: `"true"` or `"false"`.
  * Named parameters: the provided value or the default.
  * Positional parameters: the value at that position, if supplied.

**In Depth**

`getArgument()` is the primary way to retrieve parsed argument values after calling `parseArguments()`.

Behavior:

* If a named parameter is not provided on the command line but has a `defaultValue`, that default is returned.
* If a flag is not provided but has a `defaultValue`, the corresponding boolean text is returned (`"true"` or `"false"`).
* If a parameter is missing and has no default, the behavior is implementation-defined; typically, it returns an empty string.

To avoid ambiguity between “missing” and “present with empty value,” you can combine `getArgument()` with `hasArgument()`.

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

*This example retrieves flag, parameter, and positional values using `getArgument()`.*

---

#### `bool hasArgument(const String &name) const`

**Signature**

```cpp
bool hasArgument(const String &name) const;
```

**Synopsis**
Returns true if the argument was set or has a default value.

**Parameters**

* `name` – Name of the argument to check.

**Return value**

* `true` if the user provided the argument or if it has a default value.
* `false` otherwise.

**In Depth**

`hasArgument()` tells you whether an argument is **logically available** after parsing. This includes:

* Arguments explicitly set on the command line.
* Arguments with configured default values (for named parameters and flags).

This is particularly useful when:

* You want to distinguish “argument is not part of the configuration” from “argument exists but may be empty”.
* You need to know whether a value came from config or was implied.

Note that a required argument that is not present causes `parseArguments()` to return an empty `Map`, so you usually check for an empty result before calling `hasArgument()`.

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

*This example checks whether a parameter is logically available and then reads it.*

---

### Parsing the Command Line

#### `Map parseArguments(bool suppressHelp = false)`

**Signature**

```cpp
Map parseArguments(bool suppressHelp = false);
```

**Synopsis**
Parses arguments and returns a Map. Empty if help shown or required missing.

**Parameters**

* `suppressHelp` – If `false` (default), the parser automatically shows help and error messages when appropriate (for example, for `-h` / `--help` or missing required arguments). If `true`, it performs the parsing and returns the result without automatically displaying help; you can handle help and errors yourself.

**Return value**

* A `Map` containing parsed arguments, or an **empty map** if:

  * Help was requested (`-h` or `--help`), or
  * One or more required arguments are missing, or
  * Another parsing error occurs (depending on implementation).

**In Depth**

`parseArguments()` is the central operation:

1. It interprets the captured command-line arguments using the configuration you built with `addFlag`, `addParameter`, `addPositionalParameter`, and `requireArgument`.
2. It validates the input (required arguments, format of values, etc.).
3. It either:

   * Returns a non-empty `Map` of parsed values, or
   * Returns an empty `Map` and (unless `suppressHelp` is `true`) displays help and/or error messages.

The returned `Map` is often used for iteration or external APIs, but in most programs you primarily use `getArgument()` and `hasArgument()` for convenience.

Typical usage pattern:

```cpp
Map parsed = args.parseArguments();
if (parsed.isEmpty()) {
  // Do not continue; user needs help or must fix input.
  return;
}
// Safely use arguments here.
```

If you pass `suppressHelp = true`, you can detect help requests yourself by inspecting arguments and printing custom messages if desired.

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

*This example demonstrates a typical full flow: configuring arguments, enforcing a required parameter, parsing the command line, and then using the resulting values.*
