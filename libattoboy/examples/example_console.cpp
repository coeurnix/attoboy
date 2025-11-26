//==============================================================================
// attoboy TUI Feature Showcase
// An interactive demonstration of all Console/TUI capabilities
//==============================================================================

#include "attoboy/attoboy.h"

using namespace attoboy;

//------------------------------------------------------------------------------
// Forward Declarations
//------------------------------------------------------------------------------

void showWelcomeBanner(Console &con);
void demoColorPalette(Console &con);
void demoTextAlignment(Console &con);
void demoWordWrapping(Console &con);
void demoProgressBars(Console &con);
void demoInteractiveInput(Console &con);
void demoKeyReader(Console &con);
void demoAdvancedMenu(Console &con);
void demoMatrixRain(Console &con);
void showGoodbye(Console &con);

// Hotkey state (uses pointer to avoid global with destructor)
struct HotkeyState {
  bool triggered;
  String message;
};

void hotkeyCallback(void *arg) {
  HotkeyState *state = static_cast<HotkeyState *>(arg);
  state->triggered = true;
  state->message = "Hotkey triggered!";
}

//------------------------------------------------------------------------------
// Utility Functions
//------------------------------------------------------------------------------

void waitForKey(Console &con,
                const String &message = "Press any key to continue...") {
  con.println();
  con.print(message, CON_DARK_GRAY);
  con.readKey();
}

void drawHorizontalLine(Console &con, int width,
                        ConsoleColor color = CON_DARK_GRAY) {
  con.println(String("\xE2\x94\x80").repeat(width), color); // ─ character
}

void drawBoxTop(Console &con, int width, ConsoleColor color = CON_CYAN) {
  String line = String("\xE2\x95\x94");                   // ╔
  line = line + String("\xE2\x95\x90").repeat(width - 2); // ═
  line = line + String("\xE2\x95\x97");                   // ╗
  con.println(line, color);
}

void drawBoxBottom(Console &con, int width, ConsoleColor color = CON_CYAN) {
  String line = String("\xE2\x95\x9A");                   // ╚
  line = line + String("\xE2\x95\x90").repeat(width - 2); // ═
  line = line + String("\xE2\x95\x9D");                   // ╝
  con.println(line, color);
}

void drawBoxLine(Console &con, const String &text, int width,
                 ConsoleColor borderColor = CON_CYAN,
                 ConsoleColor textColor = CON_WHITE) {
  String padded = Console::Align(text, width - 4, CON_ALIGN_CENTER);
  con.print(String("\xE2\x95\x91 "), borderColor); // ║
  con.print(padded, textColor);
  con.println(String(" \xE2\x95\x91"), borderColor); // ║
}

//------------------------------------------------------------------------------
// Welcome Banner with Animated Title
//------------------------------------------------------------------------------

void showWelcomeBanner(Console &con) {
  con.clear();
  con.hideCursor();

  int w = con.width();
  int h = con.height();
  int boxWidth = 60;
  if (boxWidth > w - 4)
    boxWidth = w - 4;

  // Center vertically
  int startY = (h - 12) / 2;
  if (startY < 0)
    startY = 0;

  con.setCursor(0, startY);

  // Animate the banner appearance
  List bannerLines("", "   ___  ______________  ___  ____  __  __",
                   "  / _ |/_  __/_  __/ / / / |/ /  |/  / / /",
                   " / __ | / /   / / / /_/ /|   / /|_/ / /_/ ",
                   "/_/ |_|/_/   /_/  \\____/_/|_/_/  /_/\\__, / ",
                   "                                   /____/  ", "",
                   "       T U I   D E M O N S T R A T I O N");

  // Reveal animation
  for (int i = 0; i < bannerLines.length(); i++) {
    String line = bannerLines.at<String>(i);
    String centered = Console::Align(line, w, CON_ALIGN_CENTER);

    // Color gradient effect
    ConsoleColor color = CON_CYAN;
    if (i >= 1 && i <= 5) {
      int colorIdx = (i - 1) % 3;
      ConsoleColor colors[] = {CON_CYAN, CON_BLUE, CON_DARK_CYAN};
      color = colors[colorIdx];
    } else if (i == 7) {
      color = CON_YELLOW;
    }

    con.println(centered, color);
    Sleep(80);
  }

  con.println();
  con.printAligned("Showcasing all Console/TUI features", w, CON_ALIGN_CENTER,
                   CON_DARK_GRAY);
  con.println();

  // Animated loading dots
  String loadingBase = "Initializing";
  for (int i = 0; i < 3; i++) {
    String dots = String(".").repeat(i + 1);
    String loading = Console::Align(loadingBase + dots, w, CON_ALIGN_CENTER);
    con.setCursor(0, startY + 11);
    con.print(loading, CON_GREEN);
    Sleep(300);
  }

  Sleep(500);
  con.showCursor();
}

//------------------------------------------------------------------------------
// Color Palette Demo
//------------------------------------------------------------------------------

void demoColorPalette(Console &con) {
  con.clear();
  con.hideCursor();

  int w = con.width();

  con.println();
  con.printAligned("\xE2\x96\x88 COLOR PALETTE SHOWCASE \xE2\x96\x88", w,
                   CON_ALIGN_CENTER, CON_YELLOW);
  con.println();
  drawHorizontalLine(con, w, CON_DARK_YELLOW);
  con.println();

  // Color names
  List colorNames("BLACK", "DARK_BLUE", "DARK_GREEN", "DARK_CYAN", "DARK_RED",
                  "DARK_MAGENTA", "DARK_YELLOW", "GRAY", "DARK_GRAY", "BLUE",
                  "GREEN", "CYAN", "RED", "MAGENTA", "YELLOW", "WHITE");

  // Display all foreground colors
  con.println("  FOREGROUND COLORS:", CON_WHITE);
  con.println();

  for (int i = 0; i < 16; i++) {
    ConsoleColor fg = static_cast<ConsoleColor>(i);
    ConsoleColor bg = (i == 0) ? CON_GRAY : CON_BLACK; // Show black on gray

    String name = colorNames.at<String>(i);
    String display =
        String("  ") + Console::Align(name, 14, CON_ALIGN_LEFT) + " ";
    con.print(display, fg, bg);
    con.print("\xE2\x96\x88\xE2\x96\x88\xE2\x96\x88\xE2\x96\x88", fg,
              bg); // ████

    if ((i + 1) % 4 == 0) {
      con.println();
    }
  }

  con.println();
  con.println();

  // Display background colors
  con.println("  BACKGROUND COLORS:", CON_WHITE);
  con.println();

  for (int i = 0; i < 16; i++) {
    ConsoleColor bg = static_cast<ConsoleColor>(i);
    ConsoleColor fg = (i < 8) ? CON_WHITE : CON_BLACK;

    String name = colorNames.at<String>(i);
    String display =
        String(" ") + Console::Align(name, 12, CON_ALIGN_CENTER) + " ";
    con.print(display, fg, bg);

    if ((i + 1) % 4 == 0) {
      con.println();
    }
  }

  con.println();
  con.println();

  // Rainbow gradient demo
  con.println("  GRADIENT EFFECT:", CON_WHITE);
  con.println();

  ConsoleColor gradient[] = {CON_DARK_BLUE, CON_BLUE,        CON_CYAN,
                             CON_GREEN,     CON_YELLOW,      CON_RED,
                             CON_MAGENTA,   CON_DARK_MAGENTA};

  con.print("    ");
  for (int row = 0; row < 3; row++) {
    if (row > 0)
      con.print("    ");
    for (int i = 0; i < 8; i++) {
      con.print("\xE2\x96\x88\xE2\x96\x88\xE2\x96\x88\xE2\x96\x88",
                gradient[i]);
    }
    con.println();
  }

  con.showCursor();
  waitForKey(con);
}

//------------------------------------------------------------------------------
// Text Alignment Demo
//------------------------------------------------------------------------------

void demoTextAlignment(Console &con) {
  con.clear();
  con.hideCursor();

  int w = con.width();
  int boxWidth = 60;
  if (boxWidth > w - 4)
    boxWidth = w - 4;

  con.println();
  con.printAligned("\xE2\x96\xB6 TEXT ALIGNMENT DEMO \xE2\x97\x80", w,
                   CON_ALIGN_CENTER, CON_YELLOW);
  con.println();
  drawHorizontalLine(con, w, CON_DARK_YELLOW);
  con.println();

  String sampleText = "The quick brown fox jumps over the lazy dog.";

  // Left alignment
  con.println("  LEFT ALIGNED:", CON_GREEN);
  con.print("  \xE2\x94\x8C");
  con.print(String("\xE2\x94\x80").repeat(boxWidth - 2), CON_DARK_GRAY);
  con.println("\xE2\x94\x90", CON_DARK_GRAY);

  con.print("  \xE2\x94\x82", CON_DARK_GRAY);
  con.printAligned(sampleText, boxWidth - 2, CON_ALIGN_LEFT, CON_WHITE);
  con.println("\xE2\x94\x82", CON_DARK_GRAY);

  con.print("  \xE2\x94\x94");
  con.print(String("\xE2\x94\x80").repeat(boxWidth - 2), CON_DARK_GRAY);
  con.println("\xE2\x94\x98", CON_DARK_GRAY);
  con.println();

  // Center alignment
  con.println("  CENTER ALIGNED:", CON_CYAN);
  con.print("  \xE2\x94\x8C");
  con.print(String("\xE2\x94\x80").repeat(boxWidth - 2), CON_DARK_GRAY);
  con.println("\xE2\x94\x90", CON_DARK_GRAY);

  con.print("  \xE2\x94\x82", CON_DARK_GRAY);
  con.printAligned(sampleText, boxWidth - 2, CON_ALIGN_CENTER, CON_WHITE);
  con.println("\xE2\x94\x82", CON_DARK_GRAY);

  con.print("  \xE2\x94\x94");
  con.print(String("\xE2\x94\x80").repeat(boxWidth - 2), CON_DARK_GRAY);
  con.println("\xE2\x94\x98", CON_DARK_GRAY);
  con.println();

  // Right alignment
  con.println("  RIGHT ALIGNED:", CON_MAGENTA);
  con.print("  \xE2\x94\x8C");
  con.print(String("\xE2\x94\x80").repeat(boxWidth - 2), CON_DARK_GRAY);
  con.println("\xE2\x94\x90", CON_DARK_GRAY);

  con.print("  \xE2\x94\x82", CON_DARK_GRAY);
  con.printAligned(sampleText, boxWidth - 2, CON_ALIGN_RIGHT, CON_WHITE);
  con.println("\xE2\x94\x82", CON_DARK_GRAY);

  con.print("  \xE2\x94\x94");
  con.print(String("\xE2\x94\x80").repeat(boxWidth - 2), CON_DARK_GRAY);
  con.println("\xE2\x94\x98", CON_DARK_GRAY);
  con.println();

  // Justified alignment
  String justifyText = "This text demonstrates justified alignment which "
                       "stretches words evenly.";
  con.println("  JUSTIFIED:", CON_YELLOW);
  con.print("  \xE2\x94\x8C");
  con.print(String("\xE2\x94\x80").repeat(boxWidth - 2), CON_DARK_GRAY);
  con.println("\xE2\x94\x90", CON_DARK_GRAY);

  con.print("  \xE2\x94\x82", CON_DARK_GRAY);
  con.printAligned(justifyText, boxWidth - 2, CON_ALIGN_JUSTIFY, CON_WHITE);
  con.println("\xE2\x94\x82", CON_DARK_GRAY);

  con.print("  \xE2\x94\x94");
  con.print(String("\xE2\x94\x80").repeat(boxWidth - 2), CON_DARK_GRAY);
  con.println("\xE2\x94\x98", CON_DARK_GRAY);

  con.showCursor();
  waitForKey(con);
}

//------------------------------------------------------------------------------
// Word Wrapping Demo
//------------------------------------------------------------------------------

void demoWordWrapping(Console &con) {
  con.clear();
  con.hideCursor();

  int w = con.width();

  con.println();
  con.printAligned("\xE2\x9C\x8D WORD WRAPPING DEMO \xE2\x9C\x8D", w,
                   CON_ALIGN_CENTER, CON_YELLOW);
  con.println();
  drawHorizontalLine(con, w, CON_DARK_YELLOW);
  con.println();

  String longText =
      "The attoboy library provides elegant, batteries-included C++ utilities "
      "for building tiny Windows executables. It handles UTF-8 strings, "
      "collections, filesystem operations, networking, threading, and "
      "this beautiful TUI system you're experiencing right now. "
      "All without the STL, CRT, or libc!";

  // Show wrapping at different widths
  int widths[] = {70, 50, 35};
  ConsoleColor colors[] = {CON_GREEN, CON_CYAN, CON_MAGENTA};

  for (int i = 0; i < 3; i++) {
    int wrapWidth = widths[i];
    if (wrapWidth > w - 4)
      wrapWidth = w - 4;

    con.print(String("  Width = "), CON_DARK_GRAY);
    con.print(String(wrapWidth), CON_WHITE);
    con.println(" characters:", CON_DARK_GRAY);

    // Draw box
    con.print("  \xE2\x94\x8C");
    con.print(String("\xE2\x94\x80").repeat(wrapWidth), CON_DARK_GRAY);
    con.println("\xE2\x94\x90", CON_DARK_GRAY);

    // Wrap and display
    String wrapped = Console::Wrap(longText, wrapWidth);
    List lines = wrapped.lines();

    for (int j = 0; j < lines.length(); j++) {
      String line = lines.at<String>(j);
      con.print("  \xE2\x94\x82", CON_DARK_GRAY);
      con.printAligned(line, wrapWidth, CON_ALIGN_LEFT, colors[i]);
      con.println("\xE2\x94\x82", CON_DARK_GRAY);
    }

    con.print("  \xE2\x94\x94");
    con.print(String("\xE2\x94\x80").repeat(wrapWidth), CON_DARK_GRAY);
    con.println("\xE2\x94\x98", CON_DARK_GRAY);
    con.println();
  }

  con.showCursor();
  waitForKey(con);
}

//------------------------------------------------------------------------------
// Progress Bar Demo
//------------------------------------------------------------------------------

void demoProgressBars(Console &con) {
  con.clear();
  con.hideCursor();

  int w = con.width();

  con.println();
  con.printAligned("\xE2\x8F\xB3 PROGRESS BAR DEMO \xE2\x8F\xB3", w,
                   CON_ALIGN_CENTER, CON_YELLOW);
  con.println();
  drawHorizontalLine(con, w, CON_DARK_YELLOW);
  con.println();

  // Determinate progress bar
  con.println("  Determinate Progress:", CON_GREEN);
  con.println();

  for (int i = 0; i <= 100; i += 2) {
    float pct = static_cast<float>(i) / 100.0f;
    String label = String("Downloading: ") + String(i) + "%";
    con.progress(pct, label);
    Sleep(30);
  }
  con.progressHide();
  con.println("  \xE2\x9C\x93 Download complete!", CON_GREEN);
  con.println();

  Sleep(500);

  // Multi-stage progress
  con.println("  Multi-Stage Progress:", CON_CYAN);
  con.println();

  List stages("Compiling...", "Linking...", "Optimizing...", "Packaging...");

  for (int stage = 0; stage < stages.length(); stage++) {
    String stageLabel = stages.at<String>(stage);
    for (int i = 0; i <= 25; i++) {
      float totalPct =
          (static_cast<float>(stage) * 25.0f + static_cast<float>(i)) / 100.0f;
      con.progress(totalPct, stageLabel);
      Sleep(20);
    }
  }
  con.progressHide();
  con.println("  \xE2\x9C\x93 Build successful!", CON_CYAN);
  con.println();

  Sleep(500);

  // Indeterminate spinner
  con.println("  Indeterminate Spinner:", CON_MAGENTA);
  con.println();

  for (int i = 0; i < 60; i++) {
    con.progress(-1.0f, "Searching for updates...");
    Sleep(50);
  }
  con.progressHide();
  con.println("  \xE2\x9C\x93 No updates available.", CON_MAGENTA);

  con.showCursor();
  waitForKey(con);
}

//------------------------------------------------------------------------------
// Interactive Input Demo
//------------------------------------------------------------------------------

void demoInteractiveInput(Console &con) {
  con.clear();

  int w = con.width();

  con.println();
  con.printAligned("\xE2\x8C\xA8 INTERACTIVE INPUT DEMO \xE2\x8C\xA8", w,
                   CON_ALIGN_CENTER, CON_YELLOW);
  con.println();
  drawHorizontalLine(con, w, CON_DARK_YELLOW);
  con.println();

  // Simple input
  con.println("  1. SIMPLE INPUT", CON_GREEN);
  con.println("     (Just type and press Enter)", CON_DARK_GRAY);
  con.println();

  String name = con.input("     Your name: ");
  if (!name.isEmpty()) {
    con.print("     Hello, ", CON_WHITE);
    con.print(name, CON_CYAN);
    con.println("!", CON_WHITE);
  }
  con.println();

  // Password input
  con.println("  2. PASSWORD INPUT (masked with ***)", CON_MAGENTA);
  con.println();

  ConsoleInput pwdOptions;
  pwdOptions.password = true;

  String pwd = con.input("     Enter secret: ", &pwdOptions);
  con.print("     You entered ", CON_WHITE);
  con.print(String(pwd.length()), CON_YELLOW);
  con.println(" characters (masked).", CON_WHITE);
  con.println();

  // Tab completion
  con.println("  3. TAB COMPLETION", CON_CYAN);
  con.println("     (Type partial command, press Tab)", CON_DARK_GRAY);
  con.println();

  List commands("help", "history", "exit", "clear", "color", "compile",
                "connect");
  ConsoleInput compOptions;
  compOptions.completions = &commands;

  String cmd = con.input("     Command: ", &compOptions);
  if (!cmd.isEmpty()) {
    con.print("     Selected: ", CON_WHITE);
    con.println(cmd, CON_GREEN);
  }
  con.println();

  // History navigation
  con.println("  4. COMMAND HISTORY", CON_YELLOW);
  con.println("     (Use Up/Down arrows to navigate)", CON_DARK_GRAY);
  con.println();

  List history("ls -la", "cd /home", "make clean", "git status");
  ConsoleInput histOptions;
  histOptions.history = &history;

  con.print("     Previous commands: ", CON_DARK_GRAY);
  con.println(String(", ").join(history), CON_DARK_GRAY);

  String histCmd = con.input("     $ ", &histOptions);
  if (!histCmd.isEmpty()) {
    con.print("     Executed: ", CON_WHITE);
    con.println(histCmd, CON_GREEN);
  }
  con.println();

  // Multiline input
  con.println("  5. MULTILINE INPUT", CON_BLUE);
  con.println("     (Shift+Enter or Ctrl+Enter for new line)", CON_DARK_GRAY);
  con.println();

  ConsoleInput multiOptions;
  multiOptions.multiline = true;
  multiOptions.continuation = "  ... ";

  String multiText = con.input("     Message: ", &multiOptions);
  if (!multiText.isEmpty()) {
    List textLines = multiText.lines();
    con.print("     You entered ", CON_WHITE);
    con.print(String(textLines.length()), CON_YELLOW);
    con.println(" line(s).", CON_WHITE);
  }

  waitForKey(con);
}

//------------------------------------------------------------------------------
// Key Reader Demo
//------------------------------------------------------------------------------

void demoKeyReader(Console &con) {
  con.clear();
  con.hideCursor();

  int w = con.width();

  con.println();
  con.printAligned("\xE2\x9A\xA1 KEY READER DEMO \xE2\x9A\xA1", w,
                   CON_ALIGN_CENTER, CON_YELLOW);
  con.println();
  drawHorizontalLine(con, w, CON_DARK_YELLOW);
  con.println();

  con.println("  Press any key to see its name.", CON_WHITE);
  con.println("  Press ESC to return to menu.", CON_DARK_GRAY);
  con.println();

  // Key display box
  int boxY = 8;

  while (true) {
    con.setCursor(0, boxY);

    // Clear the display area
    for (int i = 0; i < 5; i++) {
      con.println(String(" ").repeat(w));
    }

    con.setCursor(0, boxY);

    // Draw key display box
    int boxWidth = 40;
    int boxX = (w - boxWidth) / 2;

    con.setCursor(boxX, boxY);
    con.print("\xE2\x95\x94" + String("\xE2\x95\x90").repeat(boxWidth - 2) +
                  "\xE2\x95\x97",
              CON_CYAN);

    con.setCursor(boxX, boxY + 1);
    con.print("\xE2\x95\x91", CON_CYAN);
    con.printAligned("Waiting for key...", boxWidth - 2, CON_ALIGN_CENTER,
                     CON_DARK_GRAY);
    con.print("\xE2\x95\x91", CON_CYAN);

    con.setCursor(boxX, boxY + 2);
    con.print("\xE2\x95\x9A" + String("\xE2\x95\x90").repeat(boxWidth - 2) +
                  "\xE2\x95\x9D",
              CON_CYAN);

    con.showCursor();
    String key = con.readKey();
    con.hideCursor();

    if (key == "Escape") {
      break;
    }

    // Display the key
    con.setCursor(boxX, boxY + 1);
    con.print("\xE2\x95\x91", CON_CYAN);

    String display = String("Key: ") + key;
    con.printAligned(display, boxWidth - 2, CON_ALIGN_CENTER, CON_GREEN);
    con.print("\xE2\x95\x91", CON_CYAN);

    // Show key info below
    con.setCursor(0, boxY + 4);

    if (key.startsWith("Ctrl+")) {
      con.printAligned("(Control modifier detected)", w, CON_ALIGN_CENTER,
                       CON_MAGENTA);
    } else if (key.startsWith("Alt+")) {
      con.printAligned("(Alt modifier detected)", w, CON_ALIGN_CENTER,
                       CON_YELLOW);
    } else if (key.startsWith("F")) {
      con.printAligned("(Function key detected)", w, CON_ALIGN_CENTER,
                       CON_CYAN);
    } else if (key == "Space" || key == "Enter" || key == "Tab") {
      con.printAligned("(Whitespace key)", w, CON_ALIGN_CENTER, CON_DARK_GRAY);
    }

    Sleep(100);
  }

  con.showCursor();
}

//------------------------------------------------------------------------------
// Advanced Menu Demo
//------------------------------------------------------------------------------

void demoAdvancedMenu(Console &con) {
  con.clear();
  con.hideCursor();

  int w = con.width();

  con.println();
  con.printAligned("\xE2\x98\xB0 INTERACTIVE MENU DEMO \xE2\x98\xB0", w,
                   CON_ALIGN_CENTER, CON_YELLOW);
  con.println();
  drawHorizontalLine(con, w, CON_DARK_YELLOW);
  con.println();

  con.println("  Features:", CON_WHITE);
  con.println("    \xE2\x80\xA2 Arrow key navigation", CON_DARK_GRAY);
  con.println("    \xE2\x80\xA2 Number keys (1-9) for quick select",
              CON_DARK_GRAY);
  con.println("    \xE2\x80\xA2 First letter jump", CON_DARK_GRAY);
  con.println("    \xE2\x80\xA2 Mouse hover and click", CON_DARK_GRAY);
  con.println("    \xE2\x80\xA2 Enter to confirm, Escape to cancel",
              CON_DARK_GRAY);
  con.println();

  List fileOptions("\xF0\x9F\x93\x84 New File", "\xF0\x9F\x93\x82 Open Folder",
                   "\xF0\x9F\x92\xBE Save As...",
                   "\xF0\x9F\x93\x8B Copy to Clipboard",
                   "\xE2\x9C\x82 Cut Selection", "\xF0\x9F\x93\x8C Paste",
                   "\xE2\x86\xA9 Undo", "\xE2\x86\xAA Redo",
                   "\xF0\x9F\x94\x8D Find & Replace", "\xE2\x9A\x99 Settings");

  int choice = con.menu(fileOptions, "File Menu");

  con.println();
  if (choice >= 0) {
    con.print("  You selected: ", CON_WHITE);
    con.println(fileOptions.at<String>(choice), CON_GREEN);
  } else {
    con.println("  Menu cancelled.", CON_DARK_GRAY);
  }

  con.println();

  // Nested menu example
  con.println("  Now try a nested menu experience:", CON_CYAN);
  con.println();

  List categoryMenu("Programming", "Graphics", "Audio", "Back");
  int category = con.menu(categoryMenu, "Choose Category");

  if (category >= 0 && category < 3) {
    List subItems;
    String title;

    if (category == 0) {
      subItems.append("C++").append("Rust").append("Python").append(
          "JavaScript");
      title = "Programming Languages";
    } else if (category == 1) {
      subItems.append("OpenGL").append("Vulkan").append("DirectX").append(
          "Metal");
      title = "Graphics APIs";
    } else {
      subItems.append("WASAPI")
          .append("CoreAudio")
          .append("ALSA")
          .append("PulseAudio");
      title = "Audio Systems";
    }

    int subChoice = con.menu(subItems, title);
    if (subChoice >= 0) {
      con.println();
      con.print("  Final selection: ", CON_WHITE);
      con.println(subItems.at<String>(subChoice), CON_YELLOW);
    }
  }

  con.showCursor();
  waitForKey(con);
}

//------------------------------------------------------------------------------
// Matrix Rain Effect (Fun Demo)
//------------------------------------------------------------------------------

void demoMatrixRain(Console &con) {
  con.clear();
  con.hideCursor();

  int w = con.width();
  int h = con.height();

  // Initialize drops
  List drops;
  List speeds;
  List lengths;

  for (int i = 0; i < w; i++) {
    drops.append(Math::randomRange(-h, 0));
    speeds.append(Math::randomRange(1, 3));
    lengths.append(Math::randomRange(4, 12));
  }

  // Character set for the rain
  String chars = "0123456789ABCDEF@#$%&*+=<>?";

  int frames = 150;
  for (int frame = 0; frame < frames; frame++) {
    // Update and draw each column
    for (int x = 0; x < w && x < drops.length(); x++) {
      int y = drops.at<int>(x);
      int speed = speeds.at<int>(x);
      int len = lengths.at<int>(x);

      // Draw the drop
      for (int dy = 0; dy < len; dy++) {
        int drawY = y - dy;
        if (drawY >= 0 && drawY < h) {
          con.setCursor(x, drawY);

          // Head is bright, tail fades
          ConsoleColor color;
          if (dy == 0) {
            color = CON_WHITE;
          } else if (dy < 3) {
            color = CON_GREEN;
          } else {
            color = CON_DARK_GREEN;
          }

          int charIdx = Math::randomRange(0, chars.length());
          con.print(chars.at(charIdx), color);
        }
      }

      // Erase tail
      int tailY = y - len;
      if (tailY >= 0 && tailY < h) {
        con.setCursor(x, tailY);
        con.print(" ");
      }

      // Update position
      int newY = y + speed;
      if (newY - len > h) {
        newY = Math::randomRange(-10, 0);
        speeds.set(x, Math::randomRange(1, 3));
        lengths.set(x, Math::randomRange(4, 12));
      }
      drops.set(x, newY);
    }

    Sleep(40);

    // Check for escape key (non-blocking would be ideal, but we'll use a
    // shorter duration)
  }

  // Show message
  con.setCursor(0, h / 2 - 1);
  con.printAligned("THE MATRIX HAS YOU...", w, CON_ALIGN_CENTER, CON_GREEN);
  con.setCursor(0, h / 2 + 1);
  con.printAligned("Press any key to wake up", w, CON_ALIGN_CENTER,
                   CON_DARK_GREEN);

  con.readKey();
  con.showCursor();
}

//------------------------------------------------------------------------------
// Goodbye Screen
//------------------------------------------------------------------------------

void showGoodbye(Console &con) {
  con.clear();
  con.hideCursor();

  int w = con.width();
  int h = con.height();

  // Fade out message
  List farewell(
      "", " \xE2\x9C\xA8  Thanks for exploring attoboy!  \xE2\x9C\xA8 ", "",
      "A batteries-included C++ library", "for tiny Windows executables", "",
      "github.com/coeurnix/attoboy");

  int startY = (h - farewell.length()) / 2;

  for (int i = 0; i < farewell.length(); i++) {
    con.setCursor(0, startY + i);

    ConsoleColor color = CON_WHITE;
    if (i == 1)
      color = CON_CYAN;
    else if (i == 6)
      color = CON_DARK_GRAY;
    else if (i != 0 && i != 2 && i != 5)
      color = CON_GRAY;

    con.printAligned(farewell.at<String>(i), w, CON_ALIGN_CENTER, color);
    Sleep(150);
  }

  Sleep(1500);
  con.showCursor();
}

//------------------------------------------------------------------------------
// Main Entry Point
//------------------------------------------------------------------------------

extern "C" void atto_main() {
  Console con;

  // Show animated welcome
  showWelcomeBanner(con);
  Sleep(800);

  // Main menu loop
  List mainMenu(
      "\xF0\x9F\x8E\xA8  Color Palette", "\xF0\x9F\x93\x90  Text Alignment",
      "\xF0\x9F\x93\x9D  Word Wrapping", "\xE2\x8F\xB3  Progress Bars",
      "\xE2\x8C\xA8  Interactive Input", "\xF0\x9F\x94\xA4  Key Reader",
      "\xF0\x9F\x93\x8B  Menu Navigation", "\xF0\x9F\x92\xBB  Matrix Rain",
      "\xF0\x9F\x9A\xAA  Exit");

  bool running = true;
  while (running) {
    con.clear();

    int w = con.width();
    con.println();
    con.printAligned("ATTOBOY TUI SHOWCASE", w, CON_ALIGN_CENTER, CON_CYAN);
    con.printAligned(String("Console: ") + String(w) + "x" +
                         String(con.height()),
                     w, CON_ALIGN_CENTER, CON_DARK_GRAY);
    con.println();

    int choice = con.menu(mainMenu, "Select a Demo");

    switch (choice) {
    case 0:
      demoColorPalette(con);
      break;
    case 1:
      demoTextAlignment(con);
      break;
    case 2:
      demoWordWrapping(con);
      break;
    case 3:
      demoProgressBars(con);
      break;
    case 4:
      demoInteractiveInput(con);
      break;
    case 5:
      demoKeyReader(con);
      break;
    case 6:
      demoAdvancedMenu(con);
      break;
    case 7:
      demoMatrixRain(con);
      break;
    case 8:
    case -1:
      running = false;
      break;
    }
  }

  showGoodbye(con);

  Exit(0);
}