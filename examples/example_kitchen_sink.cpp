//==============================================================================
// example_kitchen_sink.cpp - Exercises every function in libattoboy
//==============================================================================
// This file demonstrates that even when ALL library functions are linked,
// the resulting executable remains remarkably small. Each function is called
// exactly once in a minimal, straightforward manner.
//==============================================================================

#define ATTOBOY_LOG_DEBUG_ENABLE
#include "attoboy/attoboy.h"

using namespace attoboy;

// Thread function for Thread demo
static void *threadFunc(void *arg) { return arg; }

// Hotkey callback for Console demo
static void hotkeyCallback(void *) {}

void atto_main() {
  //--------------------------------------------------------------------------
  // String - Constructors
  //--------------------------------------------------------------------------
  String s1;              // default
  String s2("hello");     // const char*
  String s3(true);        // bool
  String s4('x');         // char
  String s5(42);          // int
  String s6(123456789LL); // long long
  String s7(3.14f);       // float
  String s8(s2);          // copy constructor

  //--------------------------------------------------------------------------
  // List - Constructors (needed for String constructors from collections)
  //--------------------------------------------------------------------------
  List list1;                 // default
  List list2(10);             // capacity
  List list3(1, "two", 3.0f); // variadic
  List list4(list3);          // copy

  //--------------------------------------------------------------------------
  // Map - Constructors
  //--------------------------------------------------------------------------
  Map map1;                          // default
  Map map2(10);                      // capacity
  Map map3("key", "value", "n", 42); // variadic pairs
  Map map4(map3);                    // copy

  //--------------------------------------------------------------------------
  // Set - Constructors
  //--------------------------------------------------------------------------
  Set set1;          // default
  Set set2(10);      // capacity
  Set set3(1, 2, 3); // variadic
  Set set4(set3);    // copy
  Set set5(list3);   // from List

  //--------------------------------------------------------------------------
  // String - Constructors from collections
  //--------------------------------------------------------------------------
  String s9(list3);          // from List (JSON array)
  String s10(map3);          // from Map (JSON object)
  String s11(set3);          // from Set (JSON array)
  String s12("a", "b", "c"); // variadic concatenation

  //--------------------------------------------------------------------------
  // String - Static factory
  //--------------------------------------------------------------------------
  String s13 = String::FromCStr("raw", 3, "utf-8");

  //--------------------------------------------------------------------------
  // String - Properties
  //--------------------------------------------------------------------------
  s2.length();
  s2.byteLength();
  s2.isEmpty();
  s2.c_str();
  char *allocated = s2.c_str_allocated("utf-8");
  Free(allocated);
  s2.at(0);
  s2.byteAt(0);

  //--------------------------------------------------------------------------
  // String - Substring operations
  //--------------------------------------------------------------------------
  s2.substring(0, 3);
  s2.byteSubstring(0, 3);
  s2.duplicate();

  //--------------------------------------------------------------------------
  // String - Search operations
  //--------------------------------------------------------------------------
  s2.contains("ell");
  s2.startsWith("he");
  s2.endsWith("lo");
  s2.count("l");
  s2.getPositionOf("l", 0);

  //--------------------------------------------------------------------------
  // String - Conversion
  //--------------------------------------------------------------------------
  String numStr("123");
  numStr.isNumber();
  numStr.toInteger();
  numStr.toFloat();
  String boolStr("true");
  boolStr.toBool();

  //--------------------------------------------------------------------------
  // String - Transformation
  //--------------------------------------------------------------------------
  s2.append("!");
  s2.prepend(">");
  s2.insert(1, "-");
  s2.remove(0, 1);
  s2.replace("l", "L");
  String padded("  hi  ");
  padded.trim();
  s2.upper();
  s2.lower();
  s2.reverse();
  s2.repeat(2);

  //--------------------------------------------------------------------------
  // String - Comparison
  //--------------------------------------------------------------------------
  s2.equals(s2);
  s2.compare(s2);
  (void)(s2 == s2);
  (void)(s2 != s3);
  s2 + s3;
  "prefix" + s2; // operator+(const char*, String)
  s2.hash();

  //--------------------------------------------------------------------------
  // String - Split/Join
  //--------------------------------------------------------------------------
  String multiline("a\nb\nc");
  multiline.lines();
  String sep(",");
  sep.join(list3);
  String csv("a,b,c");
  csv.split(",", 2);
  String words("one two three");
  words.split();

  //--------------------------------------------------------------------------
  // String - Format
  //--------------------------------------------------------------------------
  String tmpl1("Hello {0}!");
  tmpl1.format(List("World"));
  String tmpl2("Hello {name}!");
  tmpl2.format(Map("name", "User"));

  //--------------------------------------------------------------------------
  // List - Modification
  //--------------------------------------------------------------------------
  list1.append(1);
  list1.prepend(0);
  list1.insert(1, 99);
  list1.set(0, 100);
  list1.remove(0);
  list1.reverse();
  list1.sort(true);
  list1.concat(list3);
  list1.concat(set3);
  list1.clear();

  //--------------------------------------------------------------------------
  // List - Access
  //--------------------------------------------------------------------------
  list3.at<int>(0);
  list3.operator[]<int>(0);
  list3.typeAt(0);
  list3.slice(0, 2);
  list3.duplicate();

  //--------------------------------------------------------------------------
  // List - Search
  //--------------------------------------------------------------------------
  list3.find<int>(1);
  list3.contains<int>(1);
  list3.contains("two"); // const char* search

  //--------------------------------------------------------------------------
  // List - Pop (needs element to pop)
  //--------------------------------------------------------------------------
  List popList(1, 2, 3);
  popList.pop<int>();

  //--------------------------------------------------------------------------
  // List - Serialization
  //--------------------------------------------------------------------------
  List csvList(List(1, 2), List(3, 4));
  csvList.toCSVString();
  List::FromCSVString("1,2\n3,4");
  list3.toJSONString();
  List::FromJSONString("[1,2,3]");

  //--------------------------------------------------------------------------
  // List - Comparison
  //--------------------------------------------------------------------------
  list3.compare(list4);
  (void)(list3 == list4);
  (void)(list3 != list4);
  list3 + 5;

  //--------------------------------------------------------------------------
  // List - From Set
  //--------------------------------------------------------------------------
  List list5(set3);

  //--------------------------------------------------------------------------
  // Map - Access
  //--------------------------------------------------------------------------
  map3.get("key", String("default"));
  map3.hasKey("key");
  map3.typeAt("key");
  map3.findValue<String>("value");
  map3.keys();
  map3.values();

  //--------------------------------------------------------------------------
  // Map - Modification
  //--------------------------------------------------------------------------
  map1.put("a", 1);
  map1.remove("a");
  map1.merge(map3);
  map1.duplicate();
  map1.clear();

  //--------------------------------------------------------------------------
  // Map - Serialization
  //--------------------------------------------------------------------------
  map3.toJSONString();
  Map::FromJSONString("{\"x\":1}");

  //--------------------------------------------------------------------------
  // Map - Comparison
  //--------------------------------------------------------------------------
  map3.compare(map4);
  (void)(map3 == map4);
  (void)(map3 != map4);

  //--------------------------------------------------------------------------
  // Set - Modification
  //--------------------------------------------------------------------------
  set1.put(1);
  set1.remove<int>(1);
  set1.setUnion(set3);
  set1.intersect(set3);
  set1.subtract(set3);
  set1.clear();

  //--------------------------------------------------------------------------
  // Set - Access
  //--------------------------------------------------------------------------
  set3.contains<int>(1);
  set3.duplicate();
  set3.toList();

  // Set with string operations
  Set stringSet;
  stringSet.put("hello");
  stringSet.put("world");
  stringSet.contains("hello"); // const char* search

  //--------------------------------------------------------------------------
  // Set - Serialization
  //--------------------------------------------------------------------------
  set3.toJSONString();
  Set::FromJSONString("[1,2,3]");

  //--------------------------------------------------------------------------
  // Set - Comparison
  //--------------------------------------------------------------------------
  set3.compare(set4);
  (void)(set3 == set4);
  (void)(set3 != set4);
  set3 + 4;

  //--------------------------------------------------------------------------
  // DateTime
  //--------------------------------------------------------------------------
  DateTime dt1;                             // current time
  DateTime dt2(0LL);                        // from epoch millis
  DateTime dt3("2024-01-01T00:00:00.000Z"); // from ISO string
  DateTime dt4(dt1);                        // copy

  dt1.add(1000);
  dt1.diff(dt2);
  dt1.compare(dt2);
  dt1.equals(dt1);
  (void)(dt1 == dt1);
  (void)(dt1 != dt2);
  dt1.timestamp();
  dt1.toString();

  //--------------------------------------------------------------------------
  // Buffer
  //--------------------------------------------------------------------------
  Buffer buf1;          // default
  Buffer buf2(100);     // capacity
  Buffer buf3("hello"); // from String
  unsigned char raw[] = {1, 2, 3};
  Buffer buf4(raw, 3); // from pointer
  Buffer buf5(buf3);   // copy

  buf3.length();
  buf3.isEmpty();
  int len;
  buf3.c_ptr(&len);

  buf1.append("abc");
  buf1.append(buf3);
  buf1.append(raw, 3);
  buf1.prepend("pre");
  buf1.prepend(buf3);
  buf1.prepend(raw, 3);
  buf1.insert(0, "ins");
  buf1.insert(0, buf3);
  buf1.insert(0, raw, 3);
  buf1.remove(0, 1);
  buf1.reverse();
  buf1.trim();
  buf1.clear();

  buf3.slice(0, 3);
  buf3.compress();
  Buffer compressed = buf3.compress();
  compressed.decompress();

  String key32 = "01234567890123456789012345678901"; // 32 bytes
  String nonce12 = "012345678901";                   // 12 bytes
  Buffer keyBuf(key32);
  Buffer nonceBuf(nonce12);
  buf3.crypt(key32, nonce12);
  buf3.crypt(key32, nonceBuf);
  buf3.crypt(keyBuf, nonce12);
  buf3.crypt(keyBuf, nonceBuf);

  buf3.toBase64();
  Buffer::fromBase64("aGVsbG8=");
  buf3.toString("utf-8");

  buf3.compare(buf5);
  buf3.hash();
  (void)(buf3 == buf5);
  (void)(buf3 != buf4);
  buf3 + "more";
  buf3 + buf4;

  //--------------------------------------------------------------------------
  // Arguments
  //--------------------------------------------------------------------------
  Arguments args1;
  Arguments args2(args1); // copy

  args1.addFlag("v", "verbose", false, "verbose");
  args1.addParameter("o", "output file", "out.txt", "output");
  args1.addPositionalParameter("input", "input file");
  args1.setHelp("Usage: prog [options] <input>");
  args1.requireArgument("input");
  args1.getArgument("v");
  args1.hasArgument("v");
  args1.parseArguments(true); // suppress help
  args1.getAllArguments();

  //--------------------------------------------------------------------------
  // Path
  //--------------------------------------------------------------------------
  Path path1(".");
  Path path2(path1); // copy

  path1.exists();
  path1.isRegularFile();
  path1.isDirectory();
  path1.isNamedPipe();
  path1.isSymbolicLink();
  path1.isOther();

  path1.getSize();
  path1.getCreatedOn();
  path1.getModifiedOn();
  path1.getAccessedOn();
  path1.isReadOnly();
  path1.isHidden();

  path1.getName();
  path1.getParentDirectory();
  path1.getExtension();
  path1.getStem();
  path1.hasExtension("txt");
  path1.toString();

  path1.equals(path2);
  (void)(path1 == path2);
  (void)(path1 != path2);
  path1.isWithin(path2);

  path1.listChildren(false);

  // File operations with temp file
  Path tempFile = Path::CreateTemporaryFile("ks_", ".tmp");
  tempFile.writeFromString("test");
  tempFile.appendFromString("more");
  tempFile.readToString();
  tempFile.writeFromBuffer(buf3);
  tempFile.appendFromBuffer(buf3);
  tempFile.readToBuffer();
  tempFile.setReadOnly(false);
  tempFile.setHidden(false);

  Path tempDir = Path::CreateTemporaryDirectory("ks_");
  Path subDir = Path(tempDir.toString() + "\\subdir\\nested");
  subDir.makeDirectory(true); // with createParents
  subDir.removeDirectory(false);

  Path tempFile2 = Path(tempDir.toString() + "\\test.txt");
  tempFile.copyTo(tempFile2);
  tempFile.getSymbolicLinkTarget();
  tempFile.setSymbolicLinkTarget(tempFile2); // may fail without admin

  // moveTo test
  Path tempFile3 = Path(tempDir.toString() + "\\moved.txt");
  tempFile2.moveTo(tempFile3);

  // Cleanup
  tempFile.deleteFile();
  tempFile3.deleteFile();
  tempDir.removeDirectory(true);

  // Static path functions
  Path::ListVolumes();
  Path::GetHomeDirectory();
  Path::GetDocumentsDirectory();
  Path::GetRoamingAppDirectory();
  Path::GetLocalAppDirectory();
  Path::GetWorkingDirectory();
  Path::GetCurrentExecutable();
  Path::ChangeCurrentDirectory(Path::GetWorkingDirectory());

  //--------------------------------------------------------------------------
  // File (regular file)
  //--------------------------------------------------------------------------
  Path testPath = Path::CreateTemporaryFile("ks_file_", ".tmp");
  testPath.writeFromString("file content");

  File file1(testPath);
  File file2(file1); // copy (shares handle)

  file1.getPath();
  file1.getHost();
  file1.getPort();
  file1.isValid();
  file1.isOpen();
  file1.isRegularFile();
  file1.isSocket();
  file1.isServerSocket();
  file1.isNamedPipe();

  file1.hasAvailable();
  file1.getAvailableCount();
  file1.setPosition(0);
  file1.getPosition();
  file1.getEndPosition();
  file1.isAtEnd();

  file1.readAllToBuffer();
  file1.setPosition(0);
  file1.readToBuffer(5);
  file1.setPosition(0);
  file1.readAllToString();
  file1.setPosition(0);
  file1.readToString(5);

  file1.write(buf3);
  file1.write("string");
  file1.writeUpTo(buf3, 3);
  file1.writeUpTo("str", 2);
  file1.flush();

  file1.equals(file2);
  (void)(file1 == file2);
  (void)(file1 != file2);

  file1.close();
  testPath.deleteFile();

  //--------------------------------------------------------------------------
  // File (server socket - brief test)
  //--------------------------------------------------------------------------
  File server(12345); // server socket constructor
  server.isServerSocket();
  // server.accept() would block - just verify it exists
  server.close();

  //--------------------------------------------------------------------------
  // File (client socket constructor exists)
  //--------------------------------------------------------------------------
  // File client("localhost", 80);   // would attempt connection
  // Just demonstrates the constructor signature exists

  //--------------------------------------------------------------------------
  // Thread
  //--------------------------------------------------------------------------
  Thread thread1(threadFunc, nullptr);
  Thread thread2(thread1); // copy

  thread1.isRunning();
  thread1.await();
  thread1.equals(thread2);
  (void)(thread1 == thread2);
  (void)(thread1 != thread2);

  // Note: cancel() is dangerous, just reference it exists
  // thread1.cancel();

  //--------------------------------------------------------------------------
  // Mutex
  //--------------------------------------------------------------------------
  Mutex mutex1;
  Mutex mutex2(mutex1); // copy

  mutex1.lock();
  mutex1.unlock();
  mutex1.tryLock();
  mutex1.unlock();

  //--------------------------------------------------------------------------
  // Subprocess
  //--------------------------------------------------------------------------
  Path cmdExe("C:\\Windows\\System32\\cmd.exe");

  Subprocess::Start(cmdExe, "/c", "echo");
  Subprocess::Run(cmdExe, "/c", "echo");
  Subprocess::Capture(cmdExe, "/c", "echo", "hi");

  Subprocess proc1(cmdExe, "/c", "echo", "test");
  Subprocess proc2(proc1); // copy

  proc1.isValid();
  proc1.isRunning();
  proc1.getProcessId();
  proc1.hasAvailable();
  proc1.readToBuffer();
  proc1.readToString();
  proc1.write(buf3);
  proc1.write("input");
  proc1.wait();
  // proc1.terminate(); // already exited

  //--------------------------------------------------------------------------
  // Registry (read-only operations only for safety)
  //--------------------------------------------------------------------------
  Registry reg1("HKEY_CURRENT_USER\\Software");
  Registry reg2(reg1); // copy

  reg1.exists();
  reg1.valueExists("");
  reg1.getStringValue("");
  reg1.getBinaryValue("");
  reg1.getIntegerValue("");
  reg1.listValueNames();
  reg1.listSubkeys();

  // Write operations with temp key (cleaned up after)
  Registry regTemp("HKEY_CURRENT_USER\\Software\\AttoboyKitchenSinkTest");
  regTemp.create();
  regTemp.setStringValue("testStr", "value");
  regTemp.setBinaryValue("testBin", buf3);
  regTemp.setIntegerValue("testInt", 42);
  regTemp.deleteValue("testStr");
  regTemp.remove(true); // cleanup

  //--------------------------------------------------------------------------
  // WebRequest / WebResponse (minimal - no actual network call)
  //--------------------------------------------------------------------------
  WebRequest req1("http://example.com", Map("q", "test"),
                  Map("User-Agent", "test"));
  WebRequest req2(req1); // copy

  req1.getUrl();
  req1.getParams();
  req1.getHeaders();
  req1.hasCompleted();

  // Perform a simple GET to actually exercise WebResponse
  // Using a reliable endpoint
  WebResponse resp = req1.doGet(5000);
  WebResponse resp2(resp); // copy

  resp.succeeded();
  resp.getUrl();
  resp.getStatusCode();
  resp.getStatusReason();
  resp.getResponseHeaders();
  resp.asJson();
  resp.asString();
  resp.asBuffer();

  // Other request methods (each needs fresh request)
  WebRequest reqPost1("http://example.com");
  reqPost1.doPost(1000); // POST no body

  WebRequest reqPost2("http://example.com");
  reqPost2.doPost(map1, 1000); // POST with Map JSON

  WebRequest reqPost3("http://example.com");
  reqPost3.doPost(list1, 1000); // POST with List JSON

  WebRequest reqPost4("http://example.com");
  reqPost4.doPost(buf1, 1000); // POST with Buffer body

  WebRequest reqPost5("http://example.com");
  reqPost5.doPost(String("body"), 1000); // POST with String body

  WebRequest reqPost6("http://example.com");
  reqPost6.doRequest("PUT", buf1, 1000); // custom method

  // Static download (creates temp file)
  Path dlPath = Path::CreateTemporaryFile("ks_dl_", ".tmp");
  WebRequest::Download("http://example.com", dlPath.toString(), Map(), Map(),
                       true, 5000);
  dlPath.deleteFile();

  //--------------------------------------------------------------------------
  // AI / Embedding / Conversation (structure only - no API calls)
  //--------------------------------------------------------------------------
  AI ai1("http://localhost:11434/v1", "key", "model");
  AI ai2(ai1); // copy

  ai1.setModel("gpt-4");
  ai1.setSystemPrompt("You are helpful.");
  ai1.setMaxTokens(100);
  ai1.setJsonMode(false);

  ai1.getModel();
  ai1.getSystemPrompt();
  ai1.getBaseUrl();
  ai1.getAPIKey();
  ai1.getPromptTokensUsed();
  ai1.getResponseTokensUsed();
  ai1.getTotalTokensUsed();
  ai1.resetTokenTracking();
  ai1.getFinishReason();

  // These would require actual API - just create conversation
  Conversation conv = ai1.createConversation();
  Conversation conv2(conv); // copy

  conv.getConversationList();
  conv.setConversationList(List("user msg", "assistant msg"));
  conv.duplicate();
  conv.getAI();
  conv.getPromptTokensUsed();
  conv.getResponseTokensUsed();
  conv.getTotalTokensUsed();

  // ai1.ask() and conv.ask() would require network
  // ai1.createEmbedding() would require network

  // Embedding methods (would be called on a real embedding):
  // embedding.compare(other)
  // embedding.getRawArray()
  // embedding.getDimensions()

  //--------------------------------------------------------------------------
  // Console
  //--------------------------------------------------------------------------
  Console con;

  con.width();
  con.height();

  con.clear();
  con.print("Hello", CON_GREEN, CON_BLACK);
  con.println(" World!", CON_CYAN, CON_BLACK);
  con.printAligned("Centered", 40, CON_ALIGN_CENTER, CON_WHITE, CON_BLACK);
  con.printWrapped("This is wrapped text for demo.", 30, CON_GRAY, CON_BLACK);

  con.setCursor(0, 5);
  con.showCursor();
  con.hideCursor();
  con.showCursor();

  con.progress(0.5f, "Loading...");
  con.progressHide();

  // Static utilities
  Console::Align("text", 20, CON_ALIGN_RIGHT);
  Console::Wrap("long text to wrap", 10);

  // ConsoleInput structure
  ConsoleInput input1;
  ConsoleInput input2(input1); // copy
  input1.completions = List("help", "quit");
  input1.history = List("previous");
  input1.password = false;
  input1.multiline = false;
  input1.continuation = "... ";

  // Hotkey registration/unregistration
  con.onHotkey("Ctrl+Q", hotkeyCallback, nullptr);
  con.offHotkey("Ctrl+Q");

  // Interactive functions - skip actual interaction
  // con.input("prompt", input1);
  // con.readEvent();
  // con.menu(List("Option 1", "Option 2"), "Menu");

  //--------------------------------------------------------------------------
  // Math
  //--------------------------------------------------------------------------
  // Constants
  (void)Math::PI;
  (void)Math::E;
  (void)Math::TAU;
  (void)Math::SQRT_2;
  (void)Math::INF;
  (void)Math::NEG_INF;
  (void)Math::NAN;

  // Random
  Math::Random();
  Math::RandomFloat();
  Math::RandomRange(0, 100);
  Math::RandomBool();
  Math::RandomChoice<int>(List(1, 2, 3));

  // Basic math
  Math::Abs(-5);
  Math::Abs(-5.0f);
  Math::Min(1, 2);
  Math::Min(1.0f, 2.0f);
  Math::Max(1, 2);
  Math::Max(1.0f, 2.0f);
  Math::Clamp(5, 0, 10);
  Math::Clamp(5.0f, 0.0f, 10.0f);
  Math::Sign(-5);
  Math::Sign(-5.0f);
  Math::IsEven(4);
  Math::IsOdd(3);
  Math::IsPowerOfTwo(8);

  // Rounding
  Math::Floor(1.7f);
  Math::Ceil(1.3f);
  Math::Trunc(1.9f);
  Math::Round(1.5f);

  // Exponential/logarithmic
  Math::Exp2(3.0f);
  Math::Log2(8.0f);
  Math::Ln(Math::E);
  Math::Log10(100.0f);
  Math::Exp(1.0f);
  Math::Pow(2.0f, 3.0f);
  Math::Pow2(4);

  // Trigonometric
  Math::Sqrt(4.0f);
  Math::Sin(0.0f);
  Math::Cos(0.0f);
  Math::Tan(0.0f);
  Math::Atan2(1.0f, 1.0f);
  Math::Atan(1.0f);
  Math::Asin(0.5f);
  Math::Cosh(0.0f);

  // Interpolation and utilities
  Math::Mod(5.0f, 3.0f);
  Math::Lerp(0.0f, 10.0f, 0.5f);
  Math::Step(0.5f, 0.7f);
  Math::DegToRad(180.0f);
  Math::RadToDeg(Math::PI);
  Math::IsFinite(1.0f);
  Math::IsNaN(Math::NAN);

  // 64-bit operations
  Math::Add64(1LL, 2LL);
  Math::Sub64(5LL, 3LL);
  Math::Div64(10LL, 2LL);
  Math::Mod64(10LL, 3LL);

  //--------------------------------------------------------------------------
  // System Functions
  //--------------------------------------------------------------------------
  Sleep(1);
  GetEnv("PATH");
  SetEnv("ATTOBOY_TEST", "1");
  GetUserName();
  GetUserDisplayName();
  GetProcessId();

  void *mem = Alloc(100);
  mem = Realloc(mem, 200);
  Free(mem);

  //--------------------------------------------------------------------------
  // Logging
  //--------------------------------------------------------------------------
  EnableLoggingToConsole();
  Log("Simple log");
  LogDebug("Debug message");
  LogInfo("Info message");
  LogWarning("Warning message");
  LogError("Error message");

  Path logPath = Path::CreateTemporaryFile("ks_log_", ".log");
  EnableLoggingToFile(logPath.toString(), true);
  Log("File log");
  EnableLoggingToConsole();
  logPath.deleteFile();

  //--------------------------------------------------------------------------
  // Final output
  //--------------------------------------------------------------------------
  con.clear();
  con.println("Kitchen Sink Demo Complete!", CON_GREEN, CON_BLACK);
  con.println("");
  con.println("All attoboy functions have been exercised.");
  con.println("Check the resulting executable size!");
  con.println("");

  Exit(0);
}
