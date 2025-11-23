#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError(ATTO_TEXT("Test failed: "), message);                             \
    errorCount++;                                                              \
  }

void atto_main() {
  Log(ATTO_TEXT("Running Path simple tests..."));

  Path tempFile = Path::CreateTemporaryFile(ATTO_TEXT("pathtest"));
  TEST(tempFile.exists(), ATTO_TEXT("CreateTemporaryFile should create a file"));
  TEST(tempFile.isRegularFile(), ATTO_TEXT("Temporary file should be a regular file"));

  bool writeSuccess = tempFile.writeFromString(ATTO_TEXT("Hello World"));
  TEST(writeSuccess, ATTO_TEXT("writeFromString should succeed"));

  String content = tempFile.readToString();
  TEST(content == ATTO_TEXT("Hello World"), ATTO_TEXT("readToString should return written content"));

  long long size = tempFile.getSize();
  TEST(size > 0, ATTO_TEXT("File size should be greater than 0"));

  String ext = tempFile.getExtension();
  TEST(ext == ATTO_TEXT("tmp"), ATTO_TEXT("Extension should be 'tmp'"));

  bool appendSuccess = tempFile.appendFromString(ATTO_TEXT(" Appended"));
  TEST(appendSuccess, ATTO_TEXT("appendFromString should succeed"));

  String appendedContent = tempFile.readToString();
  TEST(appendedContent == ATTO_TEXT("Hello World Appended"),
       ATTO_TEXT("Content should include appended text"));

  TEST(!tempFile.isReadOnly(), ATTO_TEXT("Test file should not be read-only initially"));
  bool setReadOnlySuccess = tempFile.setReadOnly(true);
  TEST(setReadOnlySuccess, ATTO_TEXT("setReadOnly(true) should succeed"));
  TEST(tempFile.isReadOnly(), ATTO_TEXT("Test file should be read-only"));
  tempFile.setReadOnly(false);
  TEST(!tempFile.isReadOnly(), ATTO_TEXT("Test file should not be read-only after setReadOnly(false)"));

  TEST(!tempFile.isHidden(), ATTO_TEXT("Test file should not be hidden initially"));
  bool setHiddenSuccess = tempFile.setHidden(true);
  TEST(setHiddenSuccess, ATTO_TEXT("setHidden(true) should succeed"));
  TEST(tempFile.isHidden(), ATTO_TEXT("Test file should be hidden"));
  tempFile.setHidden(false);

  String name = tempFile.getName();
  TEST(!name.isEmpty(), ATTO_TEXT("getName should return non-empty string"));

  String stem = tempFile.getStem();
  TEST(!stem.isEmpty(), ATTO_TEXT("getStem should return non-empty string"));

  bool deleteSuccess = tempFile.deleteFile();
  TEST(deleteSuccess, ATTO_TEXT("deleteFile should succeed"));
  TEST(!tempFile.exists(), ATTO_TEXT("File should not exist after deletion"));

  List volumes = Path::ListVolumes();
  TEST(volumes.length() > 0, ATTO_TEXT("ListVolumes should return at least one volume"));

  Path path1(ATTO_TEXT("C:\\test\\file.txt"));
  Path path2(ATTO_TEXT("c:\\test\\file.txt"));
  TEST(path1.equals(path2), ATTO_TEXT("Path comparison should be case-insensitive"));

  Path pathWithExt(ATTO_TEXT("C:\\test\\document.pdf"));
  TEST(pathWithExt.getExtension() == ATTO_TEXT("pdf"), ATTO_TEXT("Extension should be 'pdf'"));
  TEST(pathWithExt.getStem() == ATTO_TEXT("document"), ATTO_TEXT("Stem should be 'document'"));

  Log(ATTO_TEXT("Testing Path equality operators..."));
  Path p1(ATTO_TEXT("C:\\test.txt"));
  Path p2(ATTO_TEXT("C:\\test.txt"));
  Path p3(ATTO_TEXT("c:\\test.txt"));
  Path p4(ATTO_TEXT("C:\\other.txt"));

  TEST(p1 == p2, ATTO_TEXT("operator== should work for identical paths"));
  TEST(p1 == p3, ATTO_TEXT("operator== should be case-insensitive"));
  TEST(!(p1 == p4), ATTO_TEXT("operator== should return false for different paths"));
  TEST(p1 != p4, ATTO_TEXT("operator!= should work for different paths"));

  Log(ATTO_TEXT("Testing File equality operators..."));
  Path tempPath = Path::CreateTemporaryFile();
  File f1(tempPath);
  File f2 = f1;
  File f3(tempPath);

  TEST(f1 == f2, ATTO_TEXT("operator== should work for copied files"));
  TEST(!(f1 == f3), ATTO_TEXT("operator== should return false for different file instances"));
  TEST(f1 != f3, ATTO_TEXT("operator!= should work for different files"));

  f1.close();
  f2.close();
  f3.close();
  tempPath.deleteFile();

  if (errorCount == 0) {
    Log(ATTO_TEXT("All Path simple tests passed!"));
    Exit(0);
  } else {
    LogError(errorCount, ATTO_TEXT(" test(s) failed"));
    Exit(errorCount);
  }
}
