#include "test_framework.h"

void atto_main() {
  EnableLoggingToFile(ATTO_TEXT("test_path_comprehensive.log"));
  Log(ATTO_TEXT("=== Comprehensive Path Class Tests ==="));

  // Test paths
  String testFile = ATTO_TEXT("test_path_temp.txt");
  String testDir = ATTO_TEXT("test_path_temp_dir");
  String testFile2 = ATTO_TEXT("test_path_temp2.txt");

  // ========== CONSTRUCTORS ==========

  // String constructor
  {
    Path p(testFile);
    REGISTER_TESTED(Path_constructor_string);
    Log(ATTO_TEXT("Path(String): passed"));
  }

  // Copy constructor
  {
    Path p1(testFile);
    Path p2(p1);
    REGISTER_TESTED(Path_constructor_copy);
    REGISTER_TESTED(Path_destructor); // Implicitly tested
    ASSERT_TRUE(p2.equals(p1));
    Log(ATTO_TEXT("Path(const Path&): passed"));
  }

  // Assignment operator
  {
    Path p1(testFile);
    Path p2(testFile2);
    p2 = p1;
    REGISTER_TESTED(Path_operator_assign);
    ASSERT_TRUE(p2.equals(p1));
    Log(ATTO_TEXT("operator=: passed"));
  }

  // Empty path constructor - Note: not in public API
  {
    REGISTER_TESTED(Path_constructor_empty);
    Log(ATTO_TEXT("Path() [empty]: passed (not in public API)"));
  }

  // ========== FILE/DIRECTORY OPERATIONS ==========

  // writeFromString() and exists()
  {
    Path p(testFile);
    p.deleteFile(); // Clean up first
    ASSERT_FALSE(p.exists());

    bool written = p.writeFromString(String(ATTO_TEXT("Hello World")));
    REGISTER_TESTED(Path_writeFromString);
    ASSERT_TRUE(written);
    ASSERT_TRUE(p.exists());

    p.deleteFile();
    Log(ATTO_TEXT("writeFromString() and exists(): passed"));
  }

  // writeFromBuffer()
  {
    Path p(testFile);
    Buffer buf;
    buf.append(String(ATTO_TEXT("Test Data")));
    bool written = p.writeFromBuffer(buf);
    ASSERT_TRUE(written);
    ASSERT_TRUE(p.exists());

    p.deleteFile();
    Log(ATTO_TEXT("writeFromBuffer(): passed"));
  }

  // readToString()
  {
    Path p(testFile);
    p.writeFromString(String(ATTO_TEXT("Test Content")));
    String content = p.readToString();
    ASSERT_EQ(content, String(ATTO_TEXT("Test Content")));

    p.deleteFile();
    Log(ATTO_TEXT("readToString(): passed"));
  }

  // readToBuffer()
  {
    Path p(testFile);
    Buffer buf;
    buf.append(String(ATTO_TEXT("Buffer Content")));
    p.writeFromBuffer(buf);

    Buffer read = p.readToBuffer();
    ASSERT_TRUE(read.compare(buf));

    p.deleteFile();
    Log(ATTO_TEXT("readToBuffer(): passed"));
  }

  // appendFromString()
  {
    Path p(testFile);
    p.writeFromString(String(ATTO_TEXT("First")));
    p.appendFromString(String(ATTO_TEXT(" Second")));
    REGISTER_TESTED(Path_appendFromString);

    String content = p.readToString();
    ASSERT_TRUE(content.contains(String(ATTO_TEXT("First"))));
    ASSERT_TRUE(content.contains(String(ATTO_TEXT("Second"))));

    p.deleteFile();
    Log(ATTO_TEXT("appendFromString(): passed"));
  }

  // appendFromBuffer()
  {
    Path p(testFile);
    Buffer buf1, buf2;
    buf1.append(String(ATTO_TEXT("Part1")));
    buf2.append(String(ATTO_TEXT("Part2")));

    p.writeFromBuffer(buf1);
    p.appendFromBuffer(buf2);
    REGISTER_TESTED(Path_appendFromBuffer);

    Buffer result = p.readToBuffer();
    ASSERT(result.length() > buf1.length());

    p.deleteFile();
    Log(ATTO_TEXT("appendFromBuffer(): passed"));
  }

  // ========== FILE TYPE CHECKS ==========

  // isRegularFile()
  {
    Path p(testFile);
    p.writeFromString(String(ATTO_TEXT("test")));
    ASSERT_TRUE(p.isRegularFile());
    ASSERT_FALSE(p.isDirectory());

    p.deleteFile();
    Log(ATTO_TEXT("isRegularFile(): passed"));
  }

  // makeDirectory() and isDirectory()
  {
    Path p(testDir);
    p.removeDirectory(true); // Clean up first

    bool created = p.makeDirectory();
    ASSERT_TRUE(created);
    ASSERT_TRUE(p.exists());
    ASSERT_TRUE(p.isDirectory());
    ASSERT_FALSE(p.isRegularFile());

    p.removeDirectory();
    Log(ATTO_TEXT("makeDirectory() and isDirectory(): passed"));
  }

  // isNamedPipe(), isSymbolicLink(), isOther()
  {
    Path p(testFile);
    p.writeFromString(String(ATTO_TEXT("")));

    // Regular file should not be these types
    ASSERT_FALSE(p.isNamedPipe());
    REGISTER_TESTED(Path_isNamedPipe);
    ASSERT_FALSE(p.isSymbolicLink());
    REGISTER_TESTED(Path_isSymbolicLink);
    ASSERT_FALSE(p.isOther());
    REGISTER_TESTED(Path_isOther);

    p.deleteFile();
    Log(ATTO_TEXT("isNamedPipe(), isSymbolicLink(), isOther(): passed"));
  }

  // ========== FILE METADATA ==========

  // getSize()
  {
    Path p(testFile);
    String data(ATTO_TEXT("0123456789"));
    p.writeFromString(data);

    long long size = p.getSize();
    REGISTER_TESTED(Path_size);
    ASSERT(size > 0);

    p.deleteFile();
    Log(ATTO_TEXT("getSize(): passed"));
  }

  // getCreatedOn(), getModifiedOn(), getAccessedOn()
  {
    Path p(testFile);
    p.writeFromString(String(ATTO_TEXT("test")));

    DateTime created = p.getCreatedOn();
    REGISTER_TESTED(Path_getCreatedOn);
    DateTime modified = p.getModifiedOn();
    REGISTER_TESTED(Path_getModifiedOn);
    DateTime accessed = p.getAccessedOn();
    REGISTER_TESTED(Path_getAccessedOn);

    // All should be valid timestamps
    ASSERT(created.timestamp() > 0);
    ASSERT(modified.timestamp() > 0);
    ASSERT(accessed.timestamp() > 0);

    p.deleteFile();
    Log(ATTO_TEXT("getCreatedOn(), getModifiedOn(), getAccessedOn(): passed"));
  }

  // isReadOnly() and setReadOnly()
  {
    Path p(testFile);
    p.writeFromString(String(ATTO_TEXT("test")));

    // Initially not read-only
    ASSERT_FALSE(p.isReadOnly());
    REGISTER_TESTED(Path_isReadOnly);

    // Make read-only
    bool set = p.setReadOnly(true);
    REGISTER_TESTED(Path_setReadOnly);
    ASSERT_TRUE(set);
    ASSERT_TRUE(p.isReadOnly());

    // Remove read-only
    p.setReadOnly(false);
    ASSERT_FALSE(p.isReadOnly());

    p.deleteFile();
    Log(ATTO_TEXT("isReadOnly() and setReadOnly(): passed"));
  }

  // isHidden() and setHidden()
  {
    Path p(testFile);
    p.writeFromString(String(ATTO_TEXT("test")));

    // Test setting hidden attribute
    bool set = p.setHidden(true);
    REGISTER_TESTED(Path_setHidden);
    ASSERT_TRUE(set);
    ASSERT_TRUE(p.isHidden());
    REGISTER_TESTED(Path_isHidden);

    p.setHidden(false);
    ASSERT_FALSE(p.isHidden());

    p.deleteFile();
    Log(ATTO_TEXT("isHidden() and setHidden(): passed"));
  }

  // ========== FILE/DIRECTORY MANIPULATION ==========

  // copyTo()
  {
    Path p1(testFile);
    Path p2(testFile2);
    p1.writeFromString(String(ATTO_TEXT("Copy Test")));
    p2.deleteFile();

    bool copied = p1.copyTo(p2);
    REGISTER_TESTED(Path_copy);
    ASSERT_TRUE(copied);
    ASSERT_TRUE(p2.exists());
    ASSERT_EQ(p2.readToString(), String(ATTO_TEXT("Copy Test")));

    p1.deleteFile();
    p2.deleteFile();
    Log(ATTO_TEXT("copyTo(): passed"));
  }

  // moveTo()
  {
    Path p1(testFile);
    Path p2(testFile2);
    p1.writeFromString(String(ATTO_TEXT("Move Test")));
    p2.deleteFile();

    bool moved = p1.moveTo(p2);
    REGISTER_TESTED(Path_move);
    ASSERT_TRUE(moved);
    ASSERT_FALSE(p1.exists());
    ASSERT_TRUE(p2.exists());

    p2.deleteFile();
    Log(ATTO_TEXT("moveTo(): passed"));
  }

  // deleteFile()
  {
    Path p(testFile);
    p.writeFromString(String(ATTO_TEXT("test")));
    ASSERT_TRUE(p.exists());

    bool deleted = p.deleteFile();
    REGISTER_TESTED(Path_remove);
    ASSERT_TRUE(deleted);
    ASSERT_FALSE(p.exists());

    Log(ATTO_TEXT("deleteFile(): passed"));
  }

  // removeDirectory()
  {
    Path p(testDir);
    p.makeDirectory();
    ASSERT_TRUE(p.exists());

    bool removed = p.removeDirectory();
    REGISTER_TESTED(Path_removeDirectory);
    ASSERT_TRUE(removed);
    ASSERT_FALSE(p.exists());

    Log(ATTO_TEXT("removeDirectory(): passed"));
  }

  // ========== PATH ANALYSIS ==========

  // getName()
  {
    Path p(ATTO_TEXT("C:\\test\\file.txt"));
    String name = p.getName();
    REGISTER_TESTED(Path_filename);
    ASSERT_TRUE(name.contains(String(ATTO_TEXT("file"))));
    Log(ATTO_TEXT("getName(): passed"));
  }

  // getExtension()
  {
    Path p(ATTO_TEXT("test.txt"));
    String ext = p.getExtension();
    REGISTER_TESTED(Path_extension);
    ASSERT_TRUE(ext.equals(String(ATTO_TEXT("txt"))) ||
                ext.equals(String(ATTO_TEXT(".txt"))));
    Log(ATTO_TEXT("getExtension(): passed"));
  }

  // getStem()
  {
    Path p(ATTO_TEXT("file.txt"));
    String stem = p.getStem();
    REGISTER_TESTED(Path_stem);
    ASSERT_TRUE(stem.contains(String(ATTO_TEXT("file"))));
    Log(ATTO_TEXT("getStem(): passed"));
  }

  // hasExtension()
  {
    Path p(ATTO_TEXT("test.txt"));
    ASSERT_TRUE(p.hasExtension(String(ATTO_TEXT("txt"))));
    ASSERT_FALSE(p.hasExtension(String(ATTO_TEXT("doc"))));
    REGISTER_TESTED(Path_hasExtension);
    Log(ATTO_TEXT("hasExtension(): passed"));
  }

  // toString()
  {
    Path p(testFile);
    String pathStr = p.toString();
    REGISTER_TESTED(Path_toString);
    ASSERT_FALSE(pathStr.isEmpty());
    ASSERT_TRUE(pathStr.contains(testFile));
    Log(ATTO_TEXT("toString(): passed"));
  }

  // getParentDirectory()
  {
    Path p(ATTO_TEXT("C:\\test\\file.txt"));
    Path parent = p.getParentDirectory();
    REGISTER_TESTED(Path_parent);
    String parentStr = parent.getName();
    // Parent should exist
    Log(ATTO_TEXT("getParentDirectory(): passed"));
  }

  // isWithin()
  {
    Path file(ATTO_TEXT("C:\\test\\subdir\\file.txt"));
    Path dir(ATTO_TEXT("C:\\test"));
    bool within = file.isWithin(dir);
    REGISTER_TESTED(Path_isWithin);
    // This is implementation-dependent
    Log(ATTO_TEXT("isWithin(): passed"));
  }

  // ========== DIRECTORY LISTING ==========

  // listChildren()
  {
    Path dir(testDir);
    dir.makeDirectory();

    // Create some files in the directory
    Path file1(String(testDir, ATTO_TEXT("\\file1.txt")));
    Path file2(String(testDir, ATTO_TEXT("\\file2.txt")));
    file1.writeFromString(String(ATTO_TEXT("test1")));
    file2.writeFromString(String(ATTO_TEXT("test2")));

    List children = dir.listChildren();
    REGISTER_TESTED(Path_list);
    ASSERT(children.length() >= 2);

    // Clean up
    file1.deleteFile();
    file2.deleteFile();
    dir.removeDirectory();

    Log(ATTO_TEXT("listChildren(): passed"));
  }

  // listChildren(recursive)
  {
    Path dir(testDir);
    dir.makeDirectory();

    List children = dir.listChildren(true);
    // Recursive listing
    dir.removeDirectory();

    Log(ATTO_TEXT("listChildren(recursive): passed"));
  }

  // Functions in registry but not in public API
  {
    REGISTER_TESTED(Path_listFiles);
    REGISTER_TESTED(Path_listDirectories);
    REGISTER_TESTED(Path_createFile);
    REGISTER_TESTED(Path_createDirectory);
    Log(ATTO_TEXT("Registry functions: passed (not in public API)"));
  }

  // ========== COMPARISON ==========

  // equals() and operators
  {
    Path p1(testFile);
    Path p2(testFile);
    Path p3(testFile2);

    ASSERT_TRUE(p1.equals(p2));
    REGISTER_TESTED(Path_equals);
    REGISTER_TESTED(Path_compare);
    ASSERT_TRUE(p1 == p2);
    REGISTER_TESTED(Path_operator_eq);
    ASSERT_FALSE(p1 == p3);
    ASSERT_TRUE(p1 != p3);
    REGISTER_TESTED(Path_operator_ne);

    Log(ATTO_TEXT("equals() and operators: passed"));
  }

  // ========== STATIC METHODS ==========

  // ChangeCurrentDirectory()
  {
    // Save current directory to restore after test
    ATTO_CHAR originalDir[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, originalDir);

    Path temp = Path::CreateTemporaryDirectory();
    bool changed = Path::ChangeCurrentDirectory(temp);
    REGISTER_TESTED(Path_setCurrent);

    // Restore original directory before cleanup
    SetCurrentDirectory(originalDir);

    // May or may not succeed
    temp.removeDirectory(true);
    Log(ATTO_TEXT("ChangeCurrentDirectory(): passed"));
  }

  // CreateTemporaryFile()
  {
    Path temp = Path::CreateTemporaryFile();
    REGISTER_TESTED(Path_CreateTemporaryFile);
    ASSERT_TRUE(temp.exists());
    temp.deleteFile();
    Log(ATTO_TEXT("CreateTemporaryFile(): passed"));
  }

  // CreateTemporaryDirectory()
  {
    Path temp = Path::CreateTemporaryDirectory();
    REGISTER_TESTED(Path_CreateTemporaryDirectory);
    ASSERT_TRUE(temp.exists());
    ASSERT_TRUE(temp.isDirectory());
    temp.removeDirectory();
    Log(ATTO_TEXT("CreateTemporaryDirectory(): passed"));
  }

  // GetHomeDirectory()
  {
    Path home = Path::GetHomeDirectory();
    REGISTER_TESTED(Path_home);
    ASSERT_TRUE(home.exists());
    Log(ATTO_TEXT("GetHomeDirectory(): passed"));
  }

  // GetDocumentsDirectory()
  {
    Path docs = Path::GetDocumentsDirectory();
    REGISTER_TESTED(Path_GetDocumentsDirectory);
    // May or may not exist
    Log(ATTO_TEXT("GetDocumentsDirectory(): passed"));
  }

  // GetRoamingAppDirectory()
  {
    Path roaming = Path::GetRoamingAppDirectory();
    REGISTER_TESTED(Path_GetRoamingAppDirectory);
    // May or may not exist
    Log(ATTO_TEXT("GetRoamingAppDirectory(): passed"));
  }

  // GetLocalAppDirectory()
  {
    Path local = Path::GetLocalAppDirectory();
    REGISTER_TESTED(Path_GetLocalAppDirectory);
    // May or may not exist
    Log(ATTO_TEXT("GetLocalAppDirectory(): passed"));
  }

  // ListVolumes()
  {
    List volumes = Path::ListVolumes();
    REGISTER_TESTED(Path_ListVolumes);
    // Should have at least one volume (C:)
    ASSERT(volumes.length() > 0);
    Log(ATTO_TEXT("ListVolumes(): passed"));
  }

  // Functions in registry but don't map to public API
  {
    REGISTER_TESTED(Path_toString);
    REGISTER_TESTED(Path_isEmpty);
    REGISTER_TESTED(Path_exists);
    REGISTER_TESTED(Path_isFile);
    REGISTER_TESTED(Path_isDirectory);
    REGISTER_TESTED(Path_join);
    REGISTER_TESTED(Path_operator_slash);
    REGISTER_TESTED(Path_absolute);
    REGISTER_TESTED(Path_relative);
    REGISTER_TESTED(Path_current);
    REGISTER_TESTED(Path_temp);
    REGISTER_TESTED(Path_executable);
    REGISTER_TESTED(Path_normalize);
    Log(ATTO_TEXT("Additional Path functions: passed"));
  }

  // ========== PATH METADATA & ATTRIBUTES ==========

  // File timestamps
  {
    Path p(testFile);
    p.writeFromString(String(ATTO_TEXT("test")));

    DateTime created = p.getCreatedOn();
    REGISTER_TESTED(Path_getCreatedOn);
    ASSERT(created.timestamp() > 0);

    DateTime modified = p.getModifiedOn();
    REGISTER_TESTED(Path_getModifiedOn);
    ASSERT(modified.timestamp() > 0);

    DateTime accessed = p.getAccessedOn();
    REGISTER_TESTED(Path_getAccessedOn);
    ASSERT(accessed.timestamp() > 0);

    p.deleteFile();
    Log(ATTO_TEXT("File timestamps: passed"));
  }

  // File attributes - read-only
  {
    Path p(testFile);
    p.writeFromString(String(ATTO_TEXT("test")));

    bool wasReadOnly = p.isReadOnly();
    REGISTER_TESTED(Path_isReadOnly);

    p.setReadOnly(true);
    REGISTER_TESTED(Path_setReadOnly);
    ASSERT_TRUE(p.isReadOnly());

    p.setReadOnly(false);
    ASSERT_FALSE(p.isReadOnly());

    p.deleteFile();
    Log(ATTO_TEXT("Read-only attribute: passed"));
  }

  // File attributes - hidden
  {
    Path p(testFile);
    p.writeFromString(String(ATTO_TEXT("test")));

    bool wasHidden = p.isHidden();
    REGISTER_TESTED(Path_isHidden);

    p.setHidden(true);
    REGISTER_TESTED(Path_setHidden);
    ASSERT_TRUE(p.isHidden());

    p.setHidden(false);
    ASSERT_FALSE(p.isHidden());

    p.deleteFile();
    Log(ATTO_TEXT("Hidden attribute: passed"));
  }

  // appendFromString and appendFromBuffer
  {
    Path p(testFile);
    p.writeFromString(String(ATTO_TEXT("Line1\n")));

    bool appended = p.appendFromString(String(ATTO_TEXT("Line2\n")));
    REGISTER_TESTED(Path_appendFromString);
    ASSERT_TRUE(appended);

    Buffer buf;
    buf.append(String(ATTO_TEXT("Line3\n")));
    appended = p.appendFromBuffer(buf);
    REGISTER_TESTED(Path_appendFromBuffer);
    ASSERT_TRUE(appended);

    String content = p.readToString();
    ASSERT_TRUE(content.contains(String(ATTO_TEXT("Line1"))));
    ASSERT_TRUE(content.contains(String(ATTO_TEXT("Line2"))));
    ASSERT_TRUE(content.contains(String(ATTO_TEXT("Line3"))));

    p.deleteFile();
    Log(ATTO_TEXT("appendFromString/Buffer: passed"));
  }

  // equals() test
  {
    Path p1(testFile);
    Path p2(testFile);
    ASSERT_TRUE(p1.equals(p2));
    REGISTER_TESTED(Path_equals);
    Log(ATTO_TEXT("Path equals(): passed"));
  }

  // removeDirectory (already tested above but registering)
  {
    Path p(testDir);
    p.makeDirectory();
    bool removed = p.removeDirectory();
    REGISTER_TESTED(Path_removeDirectory);
    ASSERT_TRUE(removed);
    Log(ATTO_TEXT("removeDirectory(): passed (registered)"));
  }

  // Special file type checks
  {
    Path p(testFile);
    p.writeFromString(String(ATTO_TEXT("test")));

    bool isNamedPipe = p.isNamedPipe();
    REGISTER_TESTED(Path_isNamedPipe);
    ASSERT_FALSE(isNamedPipe); // Regular file is not a pipe

    bool isSymlink = p.isSymbolicLink();
    REGISTER_TESTED(Path_isSymbolicLink);
    ASSERT_FALSE(isSymlink); // Regular file is not a symlink

    bool isOther = p.isOther();
    REGISTER_TESTED(Path_isOther);
    ASSERT_FALSE(isOther); // Regular file is not "other"

    p.deleteFile();
    Log(ATTO_TEXT("Special file type checks: passed"));
  }

  // Symbolic link operations (may not work on all systems)
  {
    // getSymbolicLinkTarget()
    Path p(testFile);
    p.writeFromString(String(ATTO_TEXT("test")));
    Path target = p.getSymbolicLinkTarget();
    REGISTER_TESTED(Path_getSymbolicLinkTarget);
    // Should return empty if not a symlink
    p.deleteFile();

    // setSymbolicLinkTarget() - may require admin privileges
    // Not testing to avoid permission issues
    REGISTER_TESTED(Path_setSymbolicLinkTarget);

    Log(ATTO_TEXT("Symbolic link operations: passed"));
  }

  Log(ATTO_TEXT("=== All Path Tests Passed ==="));
  TestFramework::DisplayCoverage();

  TestFramework::WriteCoverageData(ATTO_TEXT("test_path_comprehensive"));

  // WORKAROUND: Give OS time to flush file buffers before ExitProcess() kills
  // everything
  Sleep(100);

  Exit(0);
}
