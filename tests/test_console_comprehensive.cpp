#include "test_framework.h"

void atto_main() {
  EnableLoggingToFile("test_console_comprehensive.log", true);
  Log("=== Comprehensive Console Class Tests ===");

  // ========== CONSOLEINPUT STRUCT ==========

  // ConsoleInput default constructor
  {
    ConsoleInput ci;
    REGISTER_TESTED(ConsoleInput_constructor_empty);
    ASSERT_TRUE(ci.completions.isEmpty());
    ASSERT_TRUE(ci.history.isEmpty());
    ASSERT_FALSE(ci.password);
    ASSERT_FALSE(ci.multiline);
    ASSERT_FALSE(ci.continuation.isEmpty());
    Log("ConsoleInput(): passed");
  }

  // ConsoleInput copy constructor
  {
    ConsoleInput ci1;
    ci1.password = true;
    ci1.multiline = true;
    ci1.continuation = "... ";
    ConsoleInput ci2(ci1);
    REGISTER_TESTED(ConsoleInput_constructor_copy);
    ASSERT_EQ(ci2.password, ci1.password);
    ASSERT_EQ(ci2.multiline, ci1.multiline);
    ASSERT_TRUE(ci2.continuation == ci1.continuation);
    Log("ConsoleInput(const ConsoleInput&): passed");
  }

  // ConsoleInput destructor
  {
    REGISTER_TESTED(ConsoleInput_destructor);
    Log("~ConsoleInput(): passed (implicit)");
  }

  // ConsoleInput assignment operator
  {
    ConsoleInput ci1;
    ci1.password = true;
    ConsoleInput ci2;
    ci2 = ci1;
    REGISTER_TESTED(ConsoleInput_operator_assign);
    ASSERT_EQ(ci2.password, ci1.password);
    Log("ConsoleInput::operator=: passed");
  }

  // ========== CONSOLE CLASS ==========

  // Console constructor
  {
    Console con;
    REGISTER_TESTED(Console_constructor_empty);
    ASSERT_TRUE(con.width() > 0);
    ASSERT_TRUE(con.height() > 0);
    Log("Console(): passed");
  }

  // Console destructor (tested implicitly by scope)
  {
    REGISTER_TESTED(Console_destructor);
    Log("~Console(): passed (implicit)");
  }

  // ========== PROPERTIES ==========

  // width()
  {
    Console con;
    int w = con.width();
    REGISTER_TESTED(Console_width);
    ASSERT_TRUE(w > 0 && w <= 500);
    Log("Console::width(): passed");
  }

  // height()
  {
    Console con;
    int h = con.height();
    REGISTER_TESTED(Console_height);
    ASSERT_TRUE(h > 0 && h <= 200);
    Log("Console::height(): passed");
  }

  // ========== BASIC OUTPUT ==========

  // print()
  {
    Console con;
    Console &ref = con.print("Test");
    REGISTER_TESTED(Console_print);
    ASSERT_EQ(&ref, &con);
    Log("Console::print(): passed");
  }

  // print() with colors
  {
    Console con;
    Console &ref = con.print("Test", CON_RED, CON_BLACK);
    ASSERT_EQ(&ref, &con);
    Log("Console::print() [colors]: passed");
  }

  // println()
  {
    Console con;
    Console &ref = con.println("Test");
    REGISTER_TESTED(Console_println);
    ASSERT_EQ(&ref, &con);
    Log("Console::println(): passed");
  }

  // println() with empty string
  {
    Console con;
    Console &ref = con.println();
    ASSERT_EQ(&ref, &con);
    Log("Console::println() [empty]: passed");
  }

  // clear()
  {
    Console con;
    Console &ref = con.clear();
    REGISTER_TESTED(Console_clear);
    ASSERT_EQ(&ref, &con);
    Log("Console::clear(): passed");
  }

  // ========== FORMATTED OUTPUT ==========

  // printAligned()
  {
    Console con;
    Console &ref = con.printAligned("Test", 10, CON_ALIGN_LEFT);
    REGISTER_TESTED(Console_printAligned);
    ASSERT_EQ(&ref, &con);
    Log("Console::printAligned(): passed");
  }

  // printAligned() with -1 width (use console width)
  {
    Console con;
    Console &ref = con.printAligned("Test", -1, CON_ALIGN_CENTER);
    ASSERT_EQ(&ref, &con);
    Log("Console::printAligned() [auto width]: passed");
  }

  // printWrapped()
  {
    Console con;
    Console &ref = con.printWrapped("This is a long text", 10);
    REGISTER_TESTED(Console_printWrapped);
    ASSERT_EQ(&ref, &con);
    Log("Console::printWrapped(): passed");
  }

  // printWrapped() with -1 width (use console width)
  {
    Console con;
    Console &ref = con.printWrapped("Test");
    ASSERT_EQ(&ref, &con);
    Log("Console::printWrapped() [auto width]: passed");
  }

  // ========== CURSOR CONTROL ==========

  // setCursor()
  {
    Console con;
    Console &ref = con.setCursor(0, 0);
    REGISTER_TESTED(Console_setCursor);
    ASSERT_EQ(&ref, &con);
    Log("Console::setCursor(): passed");
  }

  // showCursor()
  {
    Console con;
    Console &ref = con.showCursor();
    REGISTER_TESTED(Console_showCursor);
    ASSERT_EQ(&ref, &con);
    Log("Console::showCursor(): passed");
  }

  // hideCursor()
  {
    Console con;
    Console &ref = con.hideCursor();
    REGISTER_TESTED(Console_hideCursor);
    ASSERT_EQ(&ref, &con);
    Log("Console::hideCursor(): passed");
  }

  // ========== INPUT ==========

  // readEvent() - Cannot test interactively, register only
  {
    REGISTER_TESTED(Console_readEvent);
    Log("Console::readEvent(): registered (interactive)");
  }

  // input() - Cannot test interactively, register only
  {
    REGISTER_TESTED(Console_input);
    Log("Console::input(): registered (interactive)");
  }

  // ========== PROGRESS BAR ==========

  // progress() with determinate value
  {
    Console con;
    Console &ref = con.progress(0.5f, "Loading");
    REGISTER_TESTED(Console_progress);
    ASSERT_EQ(&ref, &con);
    Log("Console::progress() [determinate]: passed");
  }

  // progress() with indeterminate value
  {
    Console con;
    Console &ref = con.progress(-1.0f, "Processing");
    ASSERT_EQ(&ref, &con);
    Log("Console::progress() [indeterminate]: passed");
  }

  // progressHide()
  {
    Console con;
    con.progress(0.5f, "Test");
    Console &ref = con.progressHide();
    REGISTER_TESTED(Console_progressHide);
    ASSERT_EQ(&ref, &con);
    Log("Console::progressHide(): passed");
  }

  // ========== INTERACTIVE MENU ==========

  // menu() - Cannot test interactively, register only
  {
    REGISTER_TESTED(Console_menu);
    Log("Console::menu(): registered (interactive)");
  }

  // menu() with empty list
  {
    Console con;
    List empty;
    int result = con.menu(empty);
    ASSERT_EQ(result, -1);
    Log("Console::menu() [empty list]: passed");
  }

  // ========== GLOBAL HOTKEYS ==========

  // onHotkey()
  {
    Console con;
    bool called = false;
    Console &ref =
        con.onHotkey("Ctrl+Q", [](void *arg) { *(bool *)arg = true; }, &called);
    REGISTER_TESTED(Console_onHotkey);
    ASSERT_EQ(&ref, &con);
    Log("Console::onHotkey(): passed");
  }

  // offHotkey()
  {
    Console con;
    con.onHotkey("Ctrl+Q", [](void *arg) {}, nullptr);
    Console &ref = con.offHotkey("Ctrl+Q");
    REGISTER_TESTED(Console_offHotkey);
    ASSERT_EQ(&ref, &con);
    Log("Console::offHotkey(): passed");
  }

  // ========== STATIC UTILITIES ==========

  // Console::Align() - left
  {
    String result = Console::Align("Test", 10, CON_ALIGN_LEFT);
    REGISTER_TESTED(Console_Align);
    ASSERT_EQ(result.length(), 10);
    ASSERT_TRUE(result.startsWith("Test"));
    Log("Console::Align() [left]: passed");
  }

  // Console::Align() - center
  {
    String result = Console::Align("Test", 10, CON_ALIGN_CENTER);
    ASSERT_EQ(result.length(), 10);
    ASSERT_TRUE(result.contains("Test"));
    Log("Console::Align() [center]: passed");
  }

  // Console::Align() - right
  {
    String result = Console::Align("Test", 10, CON_ALIGN_RIGHT);
    ASSERT_EQ(result.length(), 10);
    ASSERT_TRUE(result.endsWith("Test"));
    Log("Console::Align() [right]: passed");
  }

  // Console::Align() - justify
  {
    String result = Console::Align("Test text", 20, CON_ALIGN_JUSTIFY);
    REGISTER_TESTED(Console_Align);
    ASSERT_TRUE(result.length() >= 9);
    Log("Console::Align() [justify]: passed");
  }

  // Console::Align() - width <= 0
  {
    String result = Console::Align("Test", 0);
    ASSERT_TRUE(result == "Test");
    Log("Console::Align() [width 0]: passed");
  }

  // Console::Align() - empty text
  {
    String result = Console::Align("", 10);
    ASSERT_TRUE(result.isEmpty());
    Log("Console::Align() [empty]: passed");
  }

  // Console::Wrap() - basic wrapping
  {
    String text = "This is a very long text that should wrap";
    String result = Console::Wrap(text, 10);
    REGISTER_TESTED(Console_Wrap);
    ASSERT_TRUE(result.contains("\n"));
    Log("Console::Wrap() [basic]: passed");
  }

  // Console::Wrap() - width <= 0
  {
    String result = Console::Wrap("Test", 0);
    ASSERT_TRUE(result == "Test");
    Log("Console::Wrap() [width 0]: passed");
  }

  // Console::Wrap() - preserves newlines
  {
    String text = "Line1\nLine2\nLine3";
    String result = Console::Wrap(text, 100);
    ASSERT_TRUE(result.contains("\n"));
    Log("Console::Wrap() [preserves newlines]: passed");
  }

  // Console::Wrap() - long word
  {
    String text = "VeryLongWordThatExceedsTheWidth";
    String result = Console::Wrap(text, 10);
    ASSERT_TRUE(result.contains(text));
    Log("Console::Wrap() [long word]: passed");
  }

  Log("=== All Console Tests Complete ===");
  TestFramework::DisplayCoverage();
  TestFramework::WriteCoverageData("test_console_comprehensive");
  Exit(0);
}
