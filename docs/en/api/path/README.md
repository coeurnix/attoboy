# Path

## Overview

The `attoboy::Path` class represents an **immutable filesystem path** with a rich set of helpers for:

* Inspecting what the path refers to (file, directory, symbolic link, etc.).
* Querying metadata such as size, timestamps, and attributes.
* Performing filesystem operations (create directories, move/copy/delete files).
* Reading and writing files via convenient helpers.
* Discovering system locations (home directory, documents, temp files, etc.).

It is designed for small Windows utilities but abstracts away most low-level details. A `Path` is essentially a **value object** that holds a path string and offers high-level operations around it. The path string itself does not change once the `Path` is constructed; operations like `moveTo()` change the filesystem, not the `Path` instance.

### Filesystem Concepts

If you are less familiar with filesystem terminology, here are the core ideas that `Path` works with:

* **Regular file**: A normal file that stores data (for example, `config.txt`).
* **Directory**: A folder that can contain files and other directories.
* **Named pipe**: A special file-like object used for interprocess communication.
* **Symbolic link (symlink)**: A reference that points to another path. Accessing the symlink usually affects the target.
* **File attributes**: Flags such as “read-only” or “hidden”.
* **Current working directory**: The directory used as the base for relative paths.
* **Volume**: A drive such as `C:` or `D:` on Windows.

The `Path` class lets you navigate, inspect, and manipulate these entities while keeping your code concise and readable.

### Immutability and Value Semantics

`Path` is **immutable** with respect to its textual representation:

* Once you create a `Path`, its internal string does not change.
* Operations such as `moveTo()` or `deleteFile()` act on the filesystem entry referred to by the path but do not change the `Path` object itself.
* Copying and assigning `Path` instances is cheap and safe; they all represent the same underlying path value.

This makes it straightforward to pass paths around your application without worrying about accidental mutation of the stored path string.

---

## Reference

Each entry below covers one public constructor, method, static function, or operator of `attoboy::Path`.

For each member you will find:

* **Signature** – the exact declaration from the header.
* **Synopsis** – the original one-line Doxygen comment.
* **Parameters** and **Return value**.
* **In Depth** – purpose, behavior, caveats, and a short example.

All examples assume:

```cpp
using namespace attoboy;
```

---

### Constructors, Copy, Assignment, and Destruction

#### `Path(const String &pathStr)`

**Signature**

```cpp
Path(const String &pathStr);
```

**Synopsis**
Creates a path from a string.

**Parameters**

* `pathStr` – Textual representation of the path (absolute or relative), typically a Windows-style path such as `"C:\\temp\\file.txt"`.

**Return value**

* *(constructor; not applicable)*

**In Depth**

This is the primary way to construct a `Path`. The string is stored as-is; `Path` does not automatically normalize or canonicalize it. The same textual path will consistently refer to the same filesystem entry when interpreted by the operating system.

You can pass either an explicit `String` or a string literal, thanks to `String`’s constructor:

```cpp
Path p("C:\\temp\\log.txt");  // implicit String construction
```

The path can be:

* **Absolute**, including a drive letter (`"C:\\users\\alice\\file.txt"`).
* **Relative**, interpreted relative to the current working directory (`"logs\\today.txt"`).

**Example**

```cpp
Path configPath("C:\\myapp\\config.json");
bool exists = configPath.exists();
```

*This example creates a path from a literal and checks whether the file exists.*

---

#### `Path(const Path &other)`

**Signature**

```cpp
Path(const Path &other);
```

**Synopsis**
Creates a copy of another path.

**Parameters**

* `other` – Existing `Path` to copy.

**Return value**

* *(constructor; not applicable)*

**In Depth**

The copy constructor creates a new `Path` with identical contents and behavior as `other`. Because `Path` is immutable with respect to its string representation, copies are safe to share and use in different parts of your code.

**Example**

```cpp
Path original("C:\\data\\input.txt");
Path copy(original);

bool same = copy.equals(original);  // true
```

*This example copies a path and confirms that both instances represent the same location.*

---

#### `~Path()`

**Signature**

```cpp
~Path();
```

**Synopsis**
Destroys the path and frees memory.

**Parameters**

* *(none)*

**Return value**

* *(destructor; not applicable)*

**In Depth**

When a `Path` goes out of scope, its destructor releases any internal resources (such as heap memory for the underlying string). No filesystem changes occur automatically; the path is simply no longer referenced by your code.

**Example**

```cpp
{
  Path temp("C:\\temp\\file.tmp");
  // Use temp here...
}
// temp is destroyed here; no automatic file deletion occurs.
```

*This example shows a `Path`’s lifetime ending without affecting the actual file.*

---

#### `Path &operator=(const Path &other)`

**Signature**

```cpp
Path &operator=(const Path &other);
```

**Synopsis**
Assigns another path to this path.

**Parameters**

* `other` – `Path` whose value should be assigned to this instance.

**Return value**

* Reference to `*this`, enabling chained assignments.

**In Depth**

The assignment operator replaces the current path value with that of `other`. After assignment, both `Path` objects represent the same textual path and will behave identically on subsequent operations.

Self-assignment is safe.

**Example**

```cpp
Path p("C:\\old\\file.txt");
Path q("C:\\new\\file.txt");

p = q;  // p now refers to "C:\\new\\file.txt"
```

*This example reassigns a `Path` variable to refer to a new location.*

---

### Existence and Type Queries

#### `bool exists() const`

**Signature**

```cpp
bool exists() const;
```

**Synopsis**
Returns true if the path exists on the filesystem.

**Parameters**

* *(none)*

**Return value**

* `true` if the path currently refers to an existing filesystem entry; `false` otherwise.

**In Depth**

`exists()` checks whether the path points to anything recognized by the filesystem: a regular file, directory, symbolic link, named pipe, or other special file.

It is a good first step before calling operations like `getSize()`, `readToString()`, or `listChildren()`.

**Example**

```cpp
Path logPath("C:\\logs\\today.log");
if (!logPath.exists()) {
  logPath.writeFromString("Log created.\n");
}
```

*This example creates a new log file only if it does not already exist.*

---

#### `bool isRegularFile() const`

**Signature**

```cpp
bool isRegularFile() const;
```

**Synopsis**
Returns true if the path is a regular file.

**Parameters**

* *(none)*

**Return value**

* `true` if the path exists and refers to a regular file; otherwise `false`.

**In Depth**

This function distinguishes normal data files (for example, text files, binaries) from directories or special types. If the path does not exist, it returns `false`.

Use it when you need to ensure you are dealing with a file before reading or writing its contents.

**Example**

```cpp
Path candidate("C:\\data\\input.txt");
if (candidate.isRegularFile()) {
  String contents = candidate.readToString();
}
```

*This example only reads the file if the path is a regular file.*

---

#### `bool isDirectory() const`

**Signature**

```cpp
bool isDirectory() const;
```

**Synopsis**
Returns true if the path is a directory.

**Parameters**

* *(none)*

**Return value**

* `true` if the path exists and is a directory; otherwise `false`.

**In Depth**

`isDirectory()` checks whether the path points to a directory (folder). Directories can be listed using `listChildren()` and can be created using `makeDirectory()`.

**Example**

```cpp
Path folder("C:\\data");
if (!folder.isDirectory()) {
  folder.makeDirectory(true);
}
```

*This example creates a directory recursively if it does not already exist as a directory.*

---

#### `bool isNamedPipe() const`

**Signature**

```cpp
bool isNamedPipe() const;
```

**Synopsis**
Returns true if the path is a named pipe.

**Parameters**

* *(none)*

**Return value**

* `true` if the path exists and is identified as a named pipe; otherwise `false`.

**In Depth**

Named pipes are used for interprocess communication. This check is useful in specialized scenarios where a path may refer to either a file or a pipe.

**Example**

```cpp
Path p("\\\\.\\pipe\\myapp");
bool pipeExists = p.isNamedPipe();
```

*This example checks whether a specific named pipe exists.*

---

#### `bool isSymbolicLink() const`

**Signature**

```cpp
bool isSymbolicLink() const;
```

**Synopsis**
Returns true if the path is a symbolic link.

**Parameters**

* *(none)*

**Return value**

* `true` if the path exists and is a symbolic link; otherwise `false`.

**In Depth**

Symbolic links (symlinks) are entries that point to other paths. They can refer to files or directories. Use `getSymbolicLinkTarget()` to retrieve the target path.

**Example**

```cpp
Path p("C:\\shortcut.lnk");
if (p.isSymbolicLink()) {
  Path target = p.getSymbolicLinkTarget();
}
```

*This example detects a symbolic link and then obtains its target path.*

---

#### `bool isOther() const`

**Signature**

```cpp
bool isOther() const;
```

**Synopsis**
Returns true if the path is a device or other special file.

**Parameters**

* *(none)*

**Return value**

* `true` if the path refers to a filesystem object that is neither a regular file, directory, named pipe, nor symbolic link; otherwise `false`.

**In Depth**

This function handles remaining kinds of filesystem objects, such as devices or other platform-specific special entries. For most application-level code, you will primarily use `isRegularFile()` and `isDirectory()`.

**Example**

```cpp
Path p("C:\\Device\\SomeSpecialObject");
bool special = p.isOther();
```

*This example checks whether a path refers to a special filesystem object.*

---

### Metadata and Attributes

#### `long long getSize() const`

**Signature**

```cpp
long long getSize() const;
```

**Synopsis**
Returns the file size in bytes (0 if unavailable).

**Parameters**

* *(none)*

**Return value**

* Size in bytes if available; `0` if the size cannot be determined (for example, path does not exist or is not a regular file).

**In Depth**

`getSize()` is primarily meaningful for regular files. For directories or special files, the size may not be meaningful or may be reported as `0`.

To avoid ambiguity, call `isRegularFile()` first when you need a file size.

**Example**

```cpp
Path filePath("C:\\data\\report.pdf");
if (filePath.isRegularFile()) {
  long long bytes = filePath.getSize();
}
```

*This example retrieves the size of a regular file, if it exists.*

---

#### `DateTime getCreatedOn() const`

**Signature**

```cpp
DateTime getCreatedOn() const;
```

**Synopsis**
Returns the creation time.

**Parameters**

* *(none)*

**Return value**

* `DateTime` representing when the filesystem object was created.

**In Depth**

The returned `DateTime` reflects the filesystem’s notion of the creation time. If the path does not exist or metadata cannot be retrieved, the result will reflect an implementation-defined fallback; best practice is to call `exists()` first.

**Example**

```cpp
Path p("C:\\data\\file.txt");
if (p.exists()) {
  DateTime created = p.getCreatedOn();
  String iso = created.toString();
}
```

*This example obtains and formats the creation time of a file.*

---

#### `DateTime getModifiedOn() const`

**Signature**

```cpp
DateTime getModifiedOn() const;
```

**Synopsis**
Returns the last modification time.

**Parameters**

* *(none)*

**Return value**

* `DateTime` indicating the last time the file or directory content was modified.

**In Depth**

This timestamp is typically updated whenever file data or directory entries change. As with other metadata, call `exists()` first to avoid relying on undefined results.

**Example**

```cpp
Path p("C:\\data\\log.txt");
if (p.exists()) {
  DateTime lastModified = p.getModifiedOn();
}
```

*This example retrieves the last modification time of a file.*

---

#### `DateTime getAccessedOn() const`

**Signature**

```cpp
DateTime getAccessedOn() const;
```

**Synopsis**
Returns the last access time.

**Parameters**

* *(none)*

**Return value**

* `DateTime` representing when the file or directory was last accessed.

**In Depth**

This timestamp can indicate when the file was last opened or read, although some systems or settings may disable or delay access-time updates for performance reasons.

**Example**

```cpp
Path p("C:\\data\\file.txt");
if (p.exists()) {
  DateTime lastAccessed = p.getAccessedOn();
}
```

*This example reads the last access time of a file, if available.*

---

#### `bool isReadOnly() const`

**Signature**

```cpp
bool isReadOnly() const;
```

**Synopsis**
Returns true if the file is read-only.

**Parameters**

* *(none)*

**Return value**

* `true` if the file or directory has the read-only attribute set; `false` otherwise.

**In Depth**

The read-only attribute is one of several file attributes on Windows. When `isReadOnly()` returns `true`, attempts to modify or delete the file may fail unless the attribute is cleared or sufficient privileges are used.

For paths that do not exist, this function typically returns `false`.

**Example**

```cpp
Path p("C:\\data\\config.ini");
if (p.isReadOnly()) {
  // Decide how to handle this before writing.
}
```

*This example checks whether a configuration file is marked read-only.*

---

#### `bool setReadOnly(bool isReadOnly = true) const`

**Signature**

```cpp
bool setReadOnly(bool isReadOnly = true) const;
```

**Synopsis**
Sets the read-only attribute. Returns true on success.

**Parameters**

* `isReadOnly` – If `true`, enables the read-only attribute; if `false`, clears it.

**Return value**

* `true` if the attribute was successfully changed; `false` if the operation failed (for example, due to missing file or insufficient permissions).

**In Depth**

This function modifies the read-only attribute of the filesystem entry referred to by this path. The `Path` object itself remains unchanged.

Use this when you need to protect a file from accidental modification, or when you must temporarily clear read-only before writing to a file.

**Example**

```cpp
Path p("C:\\data\\config.ini");

// Make the configuration file writable.
if (p.isReadOnly()) {
  p.setReadOnly(false);
}
```

*This example ensures that a file is writable by clearing the read-only attribute if necessary.*

---

#### `bool isHidden() const`

**Signature**

```cpp
bool isHidden() const;
```

**Synopsis**
Returns true if the file is hidden.

**Parameters**

* *(none)*

**Return value**

* `true` if the filesystem entry has the hidden attribute; `false` otherwise.

**In Depth**

Hidden files and directories are typically not shown by default in file browsers. This function lets you detect such entries.

For non-existing paths, the result is usually `false`.

**Example**

```cpp
Path p("C:\\data\\secret.txt");
bool hidden = p.isHidden();
```

*This example checks whether a file is marked as hidden.*

---

#### `bool setHidden(bool isHidden = true) const`

**Signature**

```cpp
bool setHidden(bool isHidden = true) const;
```

**Synopsis**
Sets the hidden attribute. Returns true on success.

**Parameters**

* `isHidden` – If `true`, sets the hidden attribute; if `false`, clears it.

**Return value**

* `true` if the attribute was changed successfully; `false` otherwise.

**In Depth**

This function changes the hidden attribute on the actual filesystem entry. Use it when you want to hide auxiliary files, cache directories, or internal data from normal directory listings.

**Example**

```cpp
Path cacheDir("C:\\myapp\\cache");
cacheDir.makeDirectory(true);
cacheDir.setHidden(true);
```

*This example creates a cache directory and marks it as hidden.*

---

### Filesystem Modifications

#### `bool moveTo(const Path &dest) const`

**Signature**

```cpp
bool moveTo(const Path &dest) const;
```

**Synopsis**
Moves this path to the destination. Returns true on success.

**Parameters**

* `dest` – Target path that the file or directory should be moved or renamed to.

**Return value**

* `true` if the underlying move/rename operation succeeded; `false` otherwise.

**In Depth**

`moveTo()` requests the operating system to move or rename the filesystem entry referenced by this `Path` to `dest`. Typical reasons for failure include:

* Source does not exist.
* Destination cannot be created (e.g., parent directory missing).
* Permission denied.
* Destination already exists, depending on OS behavior.

The `Path` object itself is not changed; after a successful move, the `Path` still refers to the old location, which will usually no longer exist.

**Example**

```cpp
Path oldPath("C:\\data\\old\\file.txt");
Path newPath("C:\\data\\new\\file.txt");

if (oldPath.moveTo(newPath)) {
  // The file now lives at newPath.
}
```

*This example moves a file from one directory to another.*

---

#### `bool copyTo(const Path &dest) const`

**Signature**

```cpp
bool copyTo(const Path &dest) const;
```

**Synopsis**
Copies this path to the destination. Returns true on success.

**Parameters**

* `dest` – Target path to which the file or directory should be copied.

**Return value**

* `true` if the copy succeeded; `false` otherwise.

**In Depth**

`copyTo()` duplicates the contents of the source path to the destination:

* For files, the file data is copied.
* For directories, behavior may depend on implementation (for example, copying the directory recursively or failing).

As with `moveTo()`, the `Path` instances remain unchanged; the operation affects only the filesystem.

**Example**

```cpp
Path source("C:\\data\\report.txt");
Path backup("C:\\backup\\report.txt");

bool ok = source.copyTo(backup);
```

*This example creates a backup copy of a file.*

---

#### `bool deleteFile() const`

**Signature**

```cpp
bool deleteFile() const;
```

**Synopsis**
Deletes the file. Returns true on success.

**Parameters**

* *(none)*

**Return value**

* `true` if the file was successfully deleted; `false` otherwise.

**In Depth**

`deleteFile()` attempts to remove a regular file at this path. If the path refers to a directory or special object, the operation typically fails. Common failure reasons include:

* File does not exist.
* File is open in another process.
* Insufficient permissions.

Use `exists()` or `isRegularFile()` first if you need to distinguish between “file not present” and other errors.

**Example**

```cpp
Path tempFile("C:\\temp\\session.tmp");
if (tempFile.exists()) {
  tempFile.deleteFile();
}
```

*This example deletes a temporary file if it exists.*

---

#### `bool removeDirectory(bool deleteIfNotEmpty = false) const`

**Signature**

```cpp
bool removeDirectory(bool deleteIfNotEmpty = false) const;
```

**Synopsis**
Removes the directory. Returns true on success.

**Parameters**

* `deleteIfNotEmpty` – If `false`, removal typically succeeds only when the directory is empty. If `true`, the implementation may remove contents as needed.

**Return value**

* `true` if the directory was removed; `false` otherwise.

**In Depth**

This function removes a directory at this path. When `deleteIfNotEmpty` is set to `false` (the default), removal normally succeeds only if the directory is empty. With `true`, the implementation can recursively delete contents, depending on how the library is implemented.

As always, the `Path` object continues to represent the same textual path, even after the directory is removed.

**Example**

```cpp
Path tempDir("C:\\temp\\myapp");
tempDir.removeDirectory(true);  // try to delete the directory and its contents
```

*This example attempts to remove a temporary directory and all its contents.*

---

#### `bool makeDirectory(bool createParents = true) const`

**Signature**

```cpp
bool makeDirectory(bool createParents = true) const;
```

**Synopsis**
Creates the directory. Returns true on success.

**Parameters**

* `createParents` – If `true`, parent directories may be created as needed (similar to `mkdir -p`). If `false`, the operation typically requires the parent directory to already exist.

**Return value**

* `true` if the directory was created successfully or already exists as a directory; `false` otherwise.

**In Depth**

Use `makeDirectory()` to ensure a directory exists at this path. This is often done at startup to create configuration, log, or cache directories.

`createParents = true` is convenient for nested directories, such as `"C:\\data\\myapp\\logs"`.

**Example**

```cpp
Path logsDir("C:\\myapp\\logs");
bool ok = logsDir.makeDirectory(true);
```

*This example creates a nested logs directory, creating any missing parent folders.*

---

### Symbolic Link Operations

#### `Path getSymbolicLinkTarget() const`

**Signature**

```cpp
Path getSymbolicLinkTarget() const;
```

**Synopsis**
Returns the symbolic link target, or empty path if not a link.

**Parameters**

* *(none)*

**Return value**

* `Path` representing the target of the symbolic link, or an “empty” `Path` if this path is not a symbolic link or the target cannot be determined.

**In Depth**

If `isSymbolicLink()` returns `true`, this function returns the path that the link points to. If the path is not a symlink (or if resolution fails), the returned `Path` is considered empty (its string form is an empty string).

**Example**

```cpp
Path link("C:\\data\\shortcut.lnk");
Path target = link.getSymbolicLinkTarget();

if (!target.toString().isEmpty()) {
  // target refers to the symlink's destination
}
```

*This example resolves a symbolic link into its target path, if possible.*

---

#### `bool setSymbolicLinkTarget(const Path &target) const`

**Signature**

```cpp
bool setSymbolicLinkTarget(const Path &target) const;
```

**Synopsis**
Creates or updates a symbolic link to the target. Returns true on success.

**Parameters**

* `target` – Path that this symlink should refer to.

**Return value**

* `true` if the symbolic link was created or updated successfully; `false` otherwise.

**In Depth**

This function configures the current `Path` as a symbolic link pointing to `target`. Depending on the OS and configuration, creating symlinks may require elevated privileges.

If a symlink already exists at this path, the implementation may update it to point to the new target.

**Example**

```cpp
Path linkPath("C:\\data\\shortcut.lnk");
Path targetPath("C:\\data\\realfile.txt");

bool ok = linkPath.setSymbolicLinkTarget(targetPath);
```

*This example creates or updates a symbolic link to point at a real file.*

---

### Path Components and String Representation

#### `String getName() const`

**Signature**

```cpp
String getName() const;
```

**Synopsis**
Returns the filename (last path component).

**Parameters**

* *(none)*

**Return value**

* `String` containing the last component of the path (for example, `"file.txt"` or `"folder"`).

**In Depth**

`getName()` returns the base name of the path without any parent directories. For example:

* `"C:\\folder\\file.txt"` → `"file.txt"`
* `"C:\\folder\\"` → `"folder"` (depending on exact implementation and trailing separators)

Use `getName()` when you need just the file or directory name for display or logging.

**Example**

```cpp
Path p("C:\\data\\report.txt");
String name = p.getName();  // "report.txt"
```

*This example extracts the filename from a longer path.*

---

#### `Path getParentDirectory() const`

**Signature**

```cpp
Path getParentDirectory() const;
```

**Synopsis**
Returns the parent directory path.

**Parameters**

* *(none)*

**Return value**

* `Path` representing the directory that contains this path.

**In Depth**

`getParentDirectory()` removes the last path component to produce the parent directory. The exact behavior for root paths (such as `"C:\\"`) is implementation-dependent; typically the root is its own parent.

**Example**

```cpp
Path p("C:\\data\\report.txt");
Path parent = p.getParentDirectory();  // "C:\\data"
```

*This example obtains the parent directory of a file path.*

---

#### `String getExtension() const`

**Signature**

```cpp
String getExtension() const;
```

**Synopsis**
Returns the file extension without dot (e.g., "txt").

**Parameters**

* *(none)*

**Return value**

* `String` containing the extension of the last component, without the leading dot. If there is no extension, the returned string is empty.

**In Depth**

`getExtension()` looks at the last path component and finds the portion after the final dot. Examples:

* `"report.txt"` → `"txt"`
* `"archive.tar.gz"` → `"gz"`
* `"README"` → `""` (no extension)

**Example**

```cpp
Path p("C:\\data\\report.txt");
String ext = p.getExtension();  // "txt"
```

*This example retrieves the extension of a file.*

---

#### `String getStem() const`

**Signature**

```cpp
String getStem() const;
```

**Synopsis**
Returns the filename without extension.

**Parameters**

* *(none)*

**Return value**

* `String` containing the last component with its extension removed.

**In Depth**

`getStem()` is the counterpart to `getExtension()`. For the same examples:

* `"report.txt"` → `"report"`
* `"archive.tar.gz"` → `"archive.tar"`
* `"README"` → `"README"`

This is useful for generating variant filenames (for example, `report.txt` to `report.bak`).

**Example**

```cpp
Path p("C:\\data\\report.txt");
String stem = p.getStem();  // "report"
```

*This example obtains the base name of a file without its extension.*

---

#### `bool hasExtension(const String &ext) const`

**Signature**

```cpp
bool hasExtension(const String &ext) const;
```

**Synopsis**
Returns true if the file has the given extension (case-insensitive).

**Parameters**

* `ext` – Extension to check, typically without the dot (for example, `"txt"`).

**Return value**

* `true` if the last component’s extension matches `ext` ignoring case; `false` otherwise.

**In Depth**

This helper simplifies extension checks. The comparison is case-insensitive, so `"TXT"` and `"txt"` are treated the same.

**Example**

```cpp
Path p("C:\\data\\report.TXT");
bool isTxt = p.hasExtension("txt");  // true
```

*This example tests a file’s extension in a case-insensitive way.*

---

#### `String toString() const`

**Signature**

```cpp
String toString() const;
```

**Synopsis**
Returns the path as a string.

**Parameters**

* *(none)*

**Return value**

* `String` containing the textual representation of the path.

**In Depth**

`toString()` exposes the underlying path string, which you can then log, display, or manipulate with `String` operations. The format is typically a Windows-style path.

**Example**

```cpp
Path p("C:\\data\\file.txt");
String text = p.toString();  // "C:\\data\\file.txt"
```

*This example converts a `Path` to a `String` for display or logging.*

---

### Equality and Containment

#### `bool equals(const Path &other) const`

**Signature**

```cpp
bool equals(const Path &other) const;
```

**Synopsis**
Returns true if this path equals the other (case-insensitive).

**Parameters**

* `other` – Path to compare with.

**Return value**

* `true` if the two paths are considered equal; `false` otherwise.

**In Depth**

`equals()` compares paths in a case-insensitive manner appropriate for Windows filesystems. It typically compares the normalized textual representation of both paths, though exact normalization rules are implementation-dependent.

Use this when you want to compare paths explicitly without using operators.

**Example**

```cpp
Path a("C:\\DATA\\file.txt");
Path b("c:\\data\\file.txt");

bool same = a.equals(b);  // true on case-insensitive comparison
```

*This example compares two paths that differ only in letter case.*

---

#### `bool operator==(const Path &other) const`

**Signature**

```cpp
bool operator==(const Path &other) const;
```

**Synopsis**
Returns true if this path equals the other (case-insensitive).

**Parameters**

* `other` – Path to compare with.

**Return value**

* `true` if both paths are equal; `false` otherwise.

**In Depth**

This operator is the idiomatic shorthand for `equals(other)`. It allows expressions like:

```cpp
if (a == b) { ... }
```

Because comparison is case-insensitive, `"C:\\DATA\\file.txt"` and `"c:\\data\\FILE.txt"` are considered equal.

**Example**

```cpp
Path a("C:\\data\\file.txt");
Path b("C:\\DATA\\FILE.TXT");

if (a == b) {
  // These refer to the same path, ignoring case.
}
```

*This example uses the equality operator to compare two paths.*

---

#### `bool operator!=(const Path &other) const`

**Signature**

```cpp
bool operator!=(const Path &other) const;
```

**Synopsis**
Returns true if this path does not equal the other.

**Parameters**

* `other` – Path to compare with.

**Return value**

* `true` if paths differ; `false` if they are considered equal.

**In Depth**

This is simply the logical negation of `operator==`. Use it when you want to branch on inequality.

**Example**

```cpp
Path a("C:\\data\\file.txt");
Path b("C:\\data\\other.txt");

if (a != b) {
  // The two paths are different.
}
```

*This example branches when two paths refer to different locations.*

---

#### `bool isWithin(const Path &dir) const`

**Signature**

```cpp
bool isWithin(const Path &dir) const;
```

**Synopsis**
Returns true if this path is within the given directory.

**Parameters**

* `dir` – Directory path to test containment against.

**Return value**

* `true` if this path logically resides inside `dir` (for example, as a descendant); `false` otherwise.

**In Depth**

`isWithin()` is useful for enforcing that a file lies under a specific root directory, such as an application data folder. Path containment typically accounts for the path hierarchy rather than simple string prefixing (for example, `"C:\\root\\file.txt"` is within `"C:\\root"`, but `"C:\\rootdir\\file.txt"` is not).

Exact normalization rules (such as handling `..` segments and trailing slashes) are implementation-dependent, so treat this as a high-level safety check.

**Example**

```cpp
Path root("C:\\myapp\\data");
Path file("C:\\myapp\\data\\logs\\today.log");

bool inside = file.isWithin(root);  // true
```

*This example verifies that a file path lies inside a designated data directory.*

---

### Directory Listing

#### `List listChildren(bool recursive = false) const`

**Signature**

```cpp
List listChildren(bool recursive = false) const;
```

**Synopsis**
Returns a list of child path strings.

**Parameters**

* `recursive` – If `false`, returns only direct children. If `true`, returns descendants recursively.

**Return value**

* `List` of `String` values, each representing the path of a child item.

**In Depth**

`listChildren()` lists files and directories contained within this path if it is a directory:

* When `recursive` is `false`, only immediate entries are listed.
* When `recursive` is `true`, entries from subdirectories are also included, typically with their full paths.

Each element in the result is a `String` containing a path, not a `Path` object. You can wrap it in `Path` when needed.

If the path is not a directory or does not exist, the result is usually an empty list.

**Example**

```cpp
Path dir("C:\\myapp\\logs");
List entries = dir.listChildren(false);

int count = entries.length();
// entries[i] contains the path of each child as a String
```

*This example lists the immediate children of a logs directory.*

---

### File I/O Helpers

#### `String readToString() const`

**Signature**

```cpp
String readToString() const;
```

**Synopsis**
Reads the entire file as a string.

**Parameters**

* *(none)*

**Return value**

* `String` containing the file’s contents as UTF-8 text. If reading fails, the returned string is typically empty.

**In Depth**

`readToString()` opens the file at this path, reads all bytes, and converts them to a `String`. It is ideal for configuration files, JSON, small logs, or any text content.

For very large files, this will load the entire content into memory; consider using `File` directly for streaming.

You should check `isRegularFile()` or `exists()` first to avoid surprising results.

**Example**

```cpp
Path config("C:\\myapp\\config.json");
if (config.isRegularFile()) {
  String json = config.readToString();
}
```

*This example reads a configuration file into a `String`.*

---

#### `Buffer readToBuffer() const`

**Signature**

```cpp
Buffer readToBuffer() const;
```

**Synopsis**
Reads the entire file as a buffer.

**Parameters**

* *(none)*

**Return value**

* `Buffer` containing the raw file bytes. If reading fails, the buffer is typically empty.

**In Depth**

`readToBuffer()` is similar to `readToString()` but returns a `Buffer`, which is better suited for binary data such as images, archives, or encrypted content. The bytes are not interpreted as text.

**Example**

```cpp
Path binaryFile("C:\\data\\image.bin");
Buffer data = binaryFile.readToBuffer();
```

*This example reads all bytes of a binary file into a `Buffer`.*

---

#### `bool writeFromString(const String &str) const`

**Signature**

```cpp
bool writeFromString(const String &str) const;
```

**Synopsis**
Writes a string to the file. Returns true on success.

**Parameters**

* `str` – Text content to write.

**Return value**

* `true` if the file was written successfully; `false` otherwise.

**In Depth**

`writeFromString()` overwrites the file at this path with the bytes of `str` in UTF-8 encoding. Typical behavior includes:

* Creating the file if it does not exist (if possible).
* Truncating existing contents.

Directory and permission issues can cause it to fail, in which case it returns `false`.

**Example**

```cpp
Path log("C:\\myapp\\log.txt");
bool ok = log.writeFromString("Application started.\n");
```

*This example writes a log message to a file, replacing any existing content.*

---

#### `bool writeFromBuffer(const Buffer &buf) const`

**Signature**

```cpp
bool writeFromBuffer(const Buffer &buf) const;
```

**Synopsis**
Writes a buffer to the file. Returns true on success.

**Parameters**

* `buf` – `Buffer` containing bytes to write.

**Return value**

* `true` on success; `false` on failure.

**In Depth**

`writeFromBuffer()` writes raw bytes to the file. Use it for binary data where no text interpretation is needed.

As with `writeFromString()`, the operation may create or truncate the file.

**Example**

```cpp
Buffer data;
// data.append(...);  // populate the buffer

Path out("C:\\data\\output.bin");
bool ok = out.writeFromBuffer(data);
```

*This example writes binary data from a buffer to a file.*

---

#### `bool appendFromString(const String &str) const`

**Signature**

```cpp
bool appendFromString(const String &str) const;
```

**Synopsis**
Appends a string to the file. Returns true on success.

**Parameters**

* `str` – Text to append to the existing file contents.

**Return value**

* `true` if the append operation succeeded; `false` otherwise.

**In Depth**

`appendFromString()` opens the file in append mode and adds the UTF-8 bytes of `str` at the end. If the file does not exist, the implementation may create it (behavior depends on the underlying OS and library design).

This is ideal for log files and incremental text output.

**Example**

```cpp
Path log("C:\\myapp\\log.txt");
log.appendFromString("Next event...\n");
```

*This example appends a new line to an existing log file.*

---

#### `bool appendFromBuffer(const Buffer &buf) const`

**Signature**

```cpp
bool appendFromBuffer(const Buffer &buf) const;
```

**Synopsis**
Appends a buffer to the file. Returns true on success.

**Parameters**

* `buf` – `Buffer` whose bytes should be appended to the file.

**Return value**

* `true` on success; `false` on failure.

**In Depth**

This function appends raw bytes to the end of a file. It behaves similarly to `appendFromString()` but for binary data.

**Example**

```cpp
Path out("C:\\data\\binary.log");
Buffer chunk;
// chunk.append(...);  // fill buffer

out.appendFromBuffer(chunk);
```

*This example appends binary data to an existing file.*

---

### Static Utility Functions

#### `static bool ChangeCurrentDirectory(const Path &path)`

**Signature**

```cpp
static bool ChangeCurrentDirectory(const Path &path);
```

**Synopsis**
Changes the current working directory. Returns true on success.

**Parameters**

* `path` – `Path` representing the directory to make the current working directory.

**Return value**

* `true` if the current working directory was changed; `false` otherwise.

**In Depth**

The current working directory affects how relative paths are interpreted. Changing it can simplify code that uses many relative paths, but it also affects all subsequent relative-path operations in the process.

Use this carefully, especially in libraries or larger applications where other components may rely on the working directory.

**Example**

```cpp
Path dataDir("C:\\myapp\\data");
if (Path::ChangeCurrentDirectory(dataDir)) {
  Path relative("config.json");
  String contents = relative.readToString();
}
```

*This example changes the working directory and then reads a file using a relative path.*

---

#### `static List ListVolumes()`

**Signature**

```cpp
static List ListVolumes();
```

**Synopsis**
Returns a list of volume drive letters (e.g., "C:", "D:").

**Parameters**

* *(none)*

**Return value**

* `List` of `String` values, each representing a volume (for example, `"C:"`, `"D:"`).

**In Depth**

`ListVolumes()` queries the system for available volumes/drives. This can be useful when presenting a drive selection menu or scanning all drives for certain files.

**Example**

```cpp
List volumes = Path::ListVolumes();
int count = volumes.length();
// volumes[i] contains drive names like "C:" or "D:"
```

*This example fetches a list of all detected volumes.*

---

#### `static Path CreateTemporaryFile(const String &prefix = String(), const String &suffix = String())`

**Signature**

```cpp
static Path CreateTemporaryFile(const String &prefix = String(),
                                const String &suffix = String());
```

**Synopsis**
Creates a temporary file and returns its path.

**Parameters**

* `prefix` – Optional prefix for the filename (for example, `"myapp_"`).
* `suffix` – Optional suffix or extension (for example, `".tmp"`).

**Return value**

* `Path` to a newly created temporary file.

**In Depth**

This function creates a unique temporary file in a suitable system-defined location (typically the temp directory) and returns the path to it. The file should exist when this function returns, ready for you to write data.

You are responsible for deleting the file when you no longer need it (using `deleteFile()`).

**Example**

```cpp
Path temp = Path::CreateTemporaryFile("myapp_", ".log");
temp.writeFromString("Temporary log content.\n");
// Later: temp.deleteFile();
```

*This example creates a temporary log file, writes to it, and later can delete it.*

---

#### `static Path CreateTemporaryDirectory(const String &prefix = String())`

**Signature**

```cpp
static Path CreateTemporaryDirectory(const String &prefix = String());
```

**Synopsis**
Creates a temporary directory and returns its path.

**Parameters**

* `prefix` – Optional prefix for the directory name.

**Return value**

* `Path` to a newly created temporary directory.

**In Depth**

This function creates a unique directory under the system’s temporary area. It is useful for staging files, extracting archives, or holding intermediate results that can be cleaned up afterward.

You are responsible for removing the directory (using `removeDirectory(true)` if it may contain files).

**Example**

```cpp
Path tempDir = Path::CreateTemporaryDirectory("myapp_");
Path tempFile = Path(tempDir.toString() + "\\data.bin");

tempFile.writeFromBuffer(Buffer());
// When done:
tempDir.removeDirectory(true);
```

*This example creates a temporary directory, uses it to store a file, and then removes the entire directory tree.*

---

#### `static Path GetHomeDirectory()`

**Signature**

```cpp
static Path GetHomeDirectory();
```

**Synopsis**
Returns the user's home directory path.

**Parameters**

* *(none)*

**Return value**

* `Path` representing the current user’s home/profile directory.

**In Depth**

On Windows, this is typically the user’s profile directory (for example, `C:\\Users\\Alice`). It is a common base for application data, configuration, or documents.

**Example**

```cpp
Path home = Path::GetHomeDirectory();
String homeStr = home.toString();
```

*This example obtains the home directory path and converts it to a string.*

---

#### `static Path GetDocumentsDirectory()`

**Signature**

```cpp
static Path GetDocumentsDirectory();
```

**Synopsis**
Returns the user's documents directory path.

**Parameters**

* *(none)*

**Return value**

* `Path` representing the user’s “Documents” folder.

**In Depth**

This is a convenient way to locate where user-visible documents are typically stored (for example, `C:\\Users\\Alice\\Documents`).

**Example**

```cpp
Path docs = Path::GetDocumentsDirectory();
Path report(docs.toString() + "\\myapp_report.txt");
```

*This example builds a file path in the user’s documents directory.*

---

#### `static Path GetRoamingAppDirectory()`

**Signature**

```cpp
static Path GetRoamingAppDirectory();
```

**Synopsis**
Returns the roaming application data directory path.

**Parameters**

* *(none)*

**Return value**

* `Path` representing the roaming application data folder.

**In Depth**

The roaming app data directory is a suitable place for settings that should follow the user between machines in a domain environment (for example, `C:\\Users\\Alice\\AppData\\Roaming` on many systems).

Applications often create a subdirectory here for their own data.

**Example**

```cpp
Path roaming = Path::GetRoamingAppDirectory();
Path appDir(roaming.toString() + "\\MyApp");
appDir.makeDirectory(true);
```

*This example creates a per-application folder under the roaming app data directory.*

---

#### `static Path GetLocalAppDirectory()`

**Signature**

```cpp
static Path GetLocalAppDirectory();
```

**Synopsis**
Returns the local application data directory path.

**Parameters**

* *(none)*

**Return value**

* `Path` representing the local (non-roaming) application data folder.

**In Depth**

The local app data directory is usually used for machine-specific data such as caches or large resources that do not need to roam with the user (for example, `C:\\Users\\Alice\\AppData\\Local`).

**Example**

```cpp
Path local = Path::GetLocalAppDirectory();
Path cacheDir(local.toString() + "\\MyApp\\Cache");
cacheDir.makeDirectory(true);
```

*This example creates a cache directory under the local app data folder.*

---

#### `static Path GetWorkingDirectory()`

**Signature**

```cpp
static Path GetWorkingDirectory();
```

**Synopsis**
Returns the current working directory path.

**Parameters**

* *(none)*

**Return value**

* `Path` representing the process's current working directory.

**In Depth**

This function retrieves the directory that the operating system uses as the base for relative paths. It is the counterpart to `ChangeCurrentDirectory()`.

**Example**

```cpp
Path current = Path::GetWorkingDirectory();
String currentStr = current.toString();
```

*This example obtains the current working directory for logging or diagnostics.*

---

#### `static Path GetCurrentExecutable()`

**Signature**

```cpp
static Path GetCurrentExecutable();
```

**Synopsis**
Returns the path to the current executable.

**Parameters**

* *(none)*

**Return value**

* `Path` representing the full path of the running executable file.

**In Depth**

This is particularly useful when your application needs to locate resources relative to its own installation directory. You can call `getParentDirectory()` on this path to locate the executable’s directory.

**Example**

```cpp
Path exe = Path::GetCurrentExecutable();
Path exeDir = exe.getParentDirectory();

Path config(exeDir.toString() + "\\config.json");
```

*This example locates a configuration file stored next to the executable.*
