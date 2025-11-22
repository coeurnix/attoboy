#include "attoboy/attoboy.h"
#include <windows.h>

using namespace attoboy;

static void PrintOutput(const String &s) {
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD written;
  const ATTO_CHAR *str = s.c_str();
  int len = s.length();
  WriteFile(hOut, str, len, &written, nullptr);
  WriteFile(hOut, "\n", 1, &written, nullptr);
}

void atto_main() {
  Arguments args;

  args.addParameter(ATTO_TEXT("test"), ATTO_TEXT("Test name to run"));
  args.addParameter(ATTO_TEXT("path1"), ATTO_TEXT("First path argument"));
  args.addParameter(ATTO_TEXT("path2"), ATTO_TEXT("Second path argument"));
  args.addParameter(ATTO_TEXT("content"), ATTO_TEXT("Content to write"));
  args.addParameter(ATTO_TEXT("attr"), ATTO_TEXT("Attribute to test"));

  Map parsed = args.parseArguments(true);
  if (parsed.isEmpty()) {
    Exit(1);
  }

  String test = parsed.get<String, String>(ATTO_TEXT("test"));

  if (test == ATTO_TEXT("create_temp_file")) {
    Path tempFile = Path::CreateTemporaryFile(ATTO_TEXT("pathtest"));
    PrintOutput(tempFile.getName());
    Exit(tempFile.exists() ? 0 : 1);
  }

  if (test == ATTO_TEXT("create_temp_dir")) {
    Path tempDir = Path::CreateTemporaryDirectory(ATTO_TEXT("pathtest"));
    PrintOutput(tempDir.getName());
    Exit(tempDir.exists() && tempDir.isDirectory() ? 0 : 1);
  }

  if (test == ATTO_TEXT("write_read")) {
    String pathStr = parsed.get<String, String>(ATTO_TEXT("path1"));
    String content = parsed.get<String, String>(ATTO_TEXT("content"));
    Path path(pathStr);

    if (!path.writeFromString(content)) {
      Exit(1);
    }

    String readContent = path.readToString();
    if (readContent == content) {
      PrintOutput(readContent);
      Exit(0);
    }
    Exit(1);
  }

  if (test == ATTO_TEXT("append")) {
    String pathStr = parsed.get<String, String>(ATTO_TEXT("path1"));
    String content = parsed.get<String, String>(ATTO_TEXT("content"));
    Path path(pathStr);

    path.writeFromString(ATTO_TEXT("Initial "));
    path.appendFromString(content);

    String readContent = path.readToString();
    PrintOutput(readContent);
    Exit(readContent == String(ATTO_TEXT("Initial "), content) ? 0 : 1);
  }

  if (test == ATTO_TEXT("exists")) {
    String pathStr = parsed.get<String, String>(ATTO_TEXT("path1"));
    Path path(pathStr);
    Exit(path.exists() ? 0 : 1);
  }

  if (test == ATTO_TEXT("not_exists")) {
    String pathStr = parsed.get<String, String>(ATTO_TEXT("path1"));
    Path path(pathStr);
    Exit(!path.exists() ? 0 : 1);
  }

  if (test == ATTO_TEXT("is_file")) {
    String pathStr = parsed.get<String, String>(ATTO_TEXT("path1"));
    Path path(pathStr);
    Exit(path.isRegularFile() ? 0 : 1);
  }

  if (test == ATTO_TEXT("is_directory")) {
    String pathStr = parsed.get<String, String>(ATTO_TEXT("path1"));
    Path path(pathStr);
    Exit(path.isDirectory() ? 0 : 1);
  }

  if (test == ATTO_TEXT("get_size")) {
    String pathStr = parsed.get<String, String>(ATTO_TEXT("path1"));
    Path path(pathStr);
    PrintOutput(String((int)path.getSize()));
    Exit(0);
  }

  if (test == ATTO_TEXT("get_name")) {
    String pathStr = parsed.get<String, String>(ATTO_TEXT("path1"));
    Path path(pathStr);
    PrintOutput(path.getName());
    Exit(0);
  }

  if (test == ATTO_TEXT("get_extension")) {
    String pathStr = parsed.get<String, String>(ATTO_TEXT("path1"));
    Path path(pathStr);
    PrintOutput(path.getExtension());
    Exit(0);
  }

  if (test == ATTO_TEXT("get_stem")) {
    String pathStr = parsed.get<String, String>(ATTO_TEXT("path1"));
    Path path(pathStr);
    PrintOutput(path.getStem());
    Exit(0);
  }

  if (test == ATTO_TEXT("has_extension")) {
    String pathStr = parsed.get<String, String>(ATTO_TEXT("path1"));
    String ext = parsed.get<String, String>(ATTO_TEXT("path2"));
    Path path(pathStr);
    Exit(path.hasExtension(ext) ? 0 : 1);
  }

  if (test == ATTO_TEXT("get_parent")) {
    String pathStr = parsed.get<String, String>(ATTO_TEXT("path1"));
    Path path(pathStr);
    Path parent = path.getParentDirectory();
    PrintOutput(parent.getName());
    Exit(0);
  }

  if (test == ATTO_TEXT("equals")) {
    String path1Str = parsed.get<String, String>(ATTO_TEXT("path1"));
    String path2Str = parsed.get<String, String>(ATTO_TEXT("path2"));
    Path path1(path1Str);
    Path path2(path2Str);
    Exit(path1.equals(path2) ? 0 : 1);
  }

  if (test == ATTO_TEXT("not_equals")) {
    String path1Str = parsed.get<String, String>(ATTO_TEXT("path1"));
    String path2Str = parsed.get<String, String>(ATTO_TEXT("path2"));
    Path path1(path1Str);
    Path path2(path2Str);
    Exit(!path1.equals(path2) ? 0 : 1);
  }

  if (test == ATTO_TEXT("delete_file")) {
    String pathStr = parsed.get<String, String>(ATTO_TEXT("path1"));
    Path path(pathStr);
    Exit(path.deleteFile() ? 0 : 1);
  }

  if (test == ATTO_TEXT("copy")) {
    String path1Str = parsed.get<String, String>(ATTO_TEXT("path1"));
    String path2Str = parsed.get<String, String>(ATTO_TEXT("path2"));
    Path path1(path1Str);
    Path path2(path2Str);
    Exit(path1.copyTo(path2) ? 0 : 1);
  }

  if (test == ATTO_TEXT("move")) {
    String path1Str = parsed.get<String, String>(ATTO_TEXT("path1"));
    String path2Str = parsed.get<String, String>(ATTO_TEXT("path2"));
    Path path1(path1Str);
    Path path2(path2Str);
    Exit(path1.moveTo(path2) ? 0 : 1);
  }

  if (test == ATTO_TEXT("make_directory")) {
    String pathStr = parsed.get<String, String>(ATTO_TEXT("path1"));
    Path path(pathStr);
    Exit(path.makeDirectory(true) ? 0 : 1);
  }

  if (test == ATTO_TEXT("remove_directory")) {
    String pathStr = parsed.get<String, String>(ATTO_TEXT("path1"));
    Path path(pathStr);
    Exit(path.removeDirectory(false) ? 0 : 1);
  }

  if (test == ATTO_TEXT("remove_directory_recursive")) {
    String pathStr = parsed.get<String, String>(ATTO_TEXT("path1"));
    Path path(pathStr);
    Exit(path.removeDirectory(true) ? 0 : 1);
  }

  if (test == ATTO_TEXT("set_readonly")) {
    String pathStr = parsed.get<String, String>(ATTO_TEXT("path1"));
    String attrStr = parsed.get<String, String>(ATTO_TEXT("attr"));
    Path path(pathStr);
    bool value = (attrStr == ATTO_TEXT("true"));
    Exit(path.setReadOnly(value) ? 0 : 1);
  }

  if (test == ATTO_TEXT("is_readonly")) {
    String pathStr = parsed.get<String, String>(ATTO_TEXT("path1"));
    Path path(pathStr);
    Exit(path.isReadOnly() ? 0 : 1);
  }

  if (test == ATTO_TEXT("set_hidden")) {
    String pathStr = parsed.get<String, String>(ATTO_TEXT("path1"));
    String attrStr = parsed.get<String, String>(ATTO_TEXT("attr"));
    Path path(pathStr);
    bool value = (attrStr == ATTO_TEXT("true"));
    Exit(path.setHidden(value) ? 0 : 1);
  }

  if (test == ATTO_TEXT("is_hidden")) {
    String pathStr = parsed.get<String, String>(ATTO_TEXT("path1"));
    Path path(pathStr);
    Exit(path.isHidden() ? 0 : 1);
  }

  if (test == ATTO_TEXT("list_children")) {
    String pathStr = parsed.get<String, String>(ATTO_TEXT("path1"));
    Path path(pathStr);
    List children = path.listChildren(false);
    PrintOutput(String(children.length()));
    Exit(0);
  }

  if (test == ATTO_TEXT("list_children_recursive")) {
    String pathStr = parsed.get<String, String>(ATTO_TEXT("path1"));
    Path path(pathStr);
    List children = path.listChildren(true);
    PrintOutput(String(children.length()));
    Exit(0);
  }

  if (test == ATTO_TEXT("list_volumes")) {
    List volumes = Path::ListVolumes();
    PrintOutput(String(volumes.length()));
    Exit(volumes.length() > 0 ? 0 : 1);
  }

  if (test == ATTO_TEXT("is_within")) {
    String path1Str = parsed.get<String, String>(ATTO_TEXT("path1"));
    String path2Str = parsed.get<String, String>(ATTO_TEXT("path2"));
    Path path1(path1Str);
    Path path2(path2Str);
    Exit(path1.isWithin(path2) ? 0 : 1);
  }

  LogError(ATTO_TEXT("Unknown test: "), test);
  Exit(1);
}
