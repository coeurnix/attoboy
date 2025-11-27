//==============================================================================
// Attoboy AI Chat Demo
// A simple GUI chatbot demonstrating attoboy's AI integration
//==============================================================================

#include "attoboy/attoboy.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Edit control messages (from commctrl.h)
#define ECM_FIRST 0x1500
#define EM_SETCUEBANNER (ECM_FIRST + 1)

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

// Placement new for RAII object construction (no CRT dependency)
inline void *operator new(decltype(sizeof(0)), void *ptr) noexcept {
  return ptr;
}

using namespace attoboy;

//------------------------------------------------------------------------------
// Control Identifiers
//------------------------------------------------------------------------------

namespace CtrlId {
enum {
  BaseUrlLabel = 100,
  BaseUrlEdit,
  ApiKeyLabel,
  ApiKeyEdit,
  ModelLabel,
  ModelEdit,
  ChatDisplay,
  MessageInput,
  SendButton,
  ClearButton
};
}

//------------------------------------------------------------------------------
// Layout Constants
//------------------------------------------------------------------------------

namespace Layout {
const int Margin = 12;
const int LabelWidth = 60;
const int ControlHeight = 24;
const int ButtonWidth = 70;
const int SmallGap = 6;
} // namespace Layout

//------------------------------------------------------------------------------
// Application State
//------------------------------------------------------------------------------

struct ChatApp {
  // Window handles
  HWND hMain;
  HWND hBaseUrlLabel;
  HWND hBaseUrlEdit;
  HWND hApiKeyLabel;
  HWND hApiKeyEdit;
  HWND hModelLabel;
  HWND hModelEdit;
  HWND hChatDisplay;
  HWND hMessageInput;
  HWND hSendButton;
  HWND hClearButton;

  // Resources
  HFONT hFont;
  HFONT hChatFont;
  HBRUSH hChatBgBrush;

  // AI state (heap-allocated, lazily constructed)
  AI *ai;
  Conversation *conv;

  // UI state
  bool isSending;

  // Subclass data
  WNDPROC origInputProc;
};

//------------------------------------------------------------------------------
// Forward Declarations
//------------------------------------------------------------------------------

static LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK InputSubclassProc(HWND, UINT, WPARAM, LPARAM);
static void CreateControls(ChatApp *app);
static void LayoutControls(ChatApp *app);
static void HandleSend(ChatApp *app);
static void HandleClear(ChatApp *app);
static String GetControlText(HWND hwnd);
static void SetControlText(HWND hwnd, const String &text);
static void AppendChatText(ChatApp *app, const String &text);
static void ScrollChatToBottom(ChatApp *app);
static void SetSendingState(ChatApp *app, bool sending);
static void CleanupAI(ChatApp *app);

//------------------------------------------------------------------------------
// Entry Point
//------------------------------------------------------------------------------

extern "C" void atto_main() {

  // Register the main window class
  WNDCLASSEXA wc;
  wc.cbSize = sizeof(wc);
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = MainWndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = sizeof(ChatApp *);
  wc.hInstance = GetModuleHandleA(nullptr);
  wc.hIcon = LoadIconA(nullptr, IDI_APPLICATION);
  wc.hCursor = LoadCursorA(nullptr, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
  wc.lpszMenuName = nullptr;
  wc.lpszClassName = "AttoboyChat";
  wc.hIconSm = LoadIconA(nullptr, IDI_APPLICATION);

  if (!RegisterClassExA(&wc)) {
    MessageBoxA(nullptr, "Failed to register window class", "Error",
                MB_ICONERROR);
    Exit(1);
  }

  // Create the main window
  HWND hwnd = CreateWindowExA(0, "AttoboyChat", "Attoboy AI Chat",
                              WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                              CW_USEDEFAULT, CW_USEDEFAULT, 750, 550, nullptr,
                              nullptr, GetModuleHandleA(nullptr), nullptr);

  if (!hwnd) {
    MessageBoxA(nullptr, "Failed to create main window", "Error", MB_ICONERROR);
    Exit(1);
  }

  ShowWindow(hwnd, SW_SHOW);
  UpdateWindow(hwnd);

  // Message loop
  MSG msg;
  while (GetMessageA(&msg, nullptr, 0, 0) > 0) {
    TranslateMessage(&msg);
    DispatchMessageA(&msg);
  }
}

//------------------------------------------------------------------------------
// Window Procedure
//------------------------------------------------------------------------------

static LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam) {
  ChatApp *app = (ChatApp *)GetWindowLongPtrA(hwnd, 0);

  switch (msg) {
  case WM_CREATE: {
    // Allocate application state
    void *mem = Alloc(sizeof(ChatApp));
    if (!mem)
      return -1;

    // Zero-initialize the memory
    unsigned char *p = (unsigned char *)mem;
    for (int i = 0; i < (int)sizeof(ChatApp); ++i) {
      p[i] = 0;
    }

    app = (ChatApp *)mem;
    app->hMain = hwnd;
    app->ai = nullptr;
    app->conv = nullptr;
    app->isSending = false;

    SetWindowLongPtrA(hwnd, 0, (LONG_PTR)app);

    CreateControls(app);

    // Set focus to message input
    SetFocus(app->hMessageInput);
    return 0;
  }

  case WM_SIZE: {
    if (app && wParam != SIZE_MINIMIZED) {
      LayoutControls(app);
    }
    return 0;
  }

  case WM_GETMINMAXINFO: {
    MINMAXINFO *mmi = (MINMAXINFO *)lParam;
    mmi->ptMinTrackSize.x = 550;
    mmi->ptMinTrackSize.y = 350;
    return 0;
  }

  case WM_CTLCOLORSTATIC:
  case WM_CTLCOLOREDIT: {
    if (app && (HWND)lParam == app->hChatDisplay) {
      HDC hdc = (HDC)wParam;
      SetBkColor(hdc, RGB(250, 250, 250));
      SetTextColor(hdc, RGB(30, 30, 30));
      if (!app->hChatBgBrush) {
        app->hChatBgBrush = CreateSolidBrush(RGB(250, 250, 250));
      }
      return (LRESULT)app->hChatBgBrush;
    }
    break;
  }

  case WM_COMMAND: {
    if (!app)
      return 0;

    int id = LOWORD(wParam);
    int notify = HIWORD(wParam);

    if (id == CtrlId::SendButton && notify == BN_CLICKED) {
      HandleSend(app);
    } else if (id == CtrlId::ClearButton && notify == BN_CLICKED) {
      HandleClear(app);
    }
    return 0;
  }

  case WM_SETFOCUS: {
    if (app && app->hMessageInput) {
      SetFocus(app->hMessageInput);
    }
    return 0;
  }

  case WM_DESTROY: {
    if (app) {
      // Clean up AI resources
      CleanupAI(app);

      // Clean up GDI resources
      if (app->hFont)
        DeleteObject(app->hFont);
      if (app->hChatFont)
        DeleteObject(app->hChatFont);
      if (app->hChatBgBrush)
        DeleteObject(app->hChatBgBrush);

      Free(app);
    }
    PostQuitMessage(0);
    Exit(0);
  }
  }

  return DefWindowProcA(hwnd, msg, wParam, lParam);
}

//------------------------------------------------------------------------------
// Input Subclass Procedure (for Enter key handling)
//------------------------------------------------------------------------------

static LRESULT CALLBACK InputSubclassProc(HWND hwnd, UINT msg, WPARAM wParam,
                                          LPARAM lParam) {
  ChatApp *app = (ChatApp *)GetWindowLongPtrA(hwnd, GWLP_USERDATA);

  if (msg == WM_KEYDOWN && wParam == VK_RETURN) {
    if (app && !app->isSending) {
      HandleSend(app);
    }
    return 0;
  }

  if (app && app->origInputProc) {
    return CallWindowProcA(app->origInputProc, hwnd, msg, wParam, lParam);
  }
  return DefWindowProcA(hwnd, msg, wParam, lParam);
}

//------------------------------------------------------------------------------
// Control Creation
//------------------------------------------------------------------------------

static void CreateControls(ChatApp *app) {
  HINSTANCE hInst = GetModuleHandleA(nullptr);

  // Create fonts
  app->hFont =
      CreateFontA(-14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                  DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");

  app->hChatFont =
      CreateFontA(-13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                  FIXED_PITCH | FF_MODERN, "Consolas");

  // Row 1: Base URL
  app->hBaseUrlLabel = CreateWindowExA(
      0, "STATIC", "Base URL:", WS_CHILD | WS_VISIBLE | SS_RIGHT, 0, 0, 0, 0,
      app->hMain, (HMENU)(INT_PTR)CtrlId::BaseUrlLabel, hInst, nullptr);

  app->hBaseUrlEdit = CreateWindowExA(
      WS_EX_CLIENTEDGE, "EDIT", "https://api.openai.com/v1/",
      WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL, 0, 0, 0, 0,
      app->hMain, (HMENU)(INT_PTR)CtrlId::BaseUrlEdit, hInst, nullptr);

  // Row 1: API Key
  app->hApiKeyLabel = CreateWindowExA(
      0, "STATIC", "API Key:", WS_CHILD | WS_VISIBLE | SS_RIGHT, 0, 0, 0, 0,
      app->hMain, (HMENU)(INT_PTR)CtrlId::ApiKeyLabel, hInst, nullptr);

  app->hApiKeyEdit = CreateWindowExA(
      WS_EX_CLIENTEDGE, "EDIT", "",
      WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL | ES_PASSWORD, 0, 0,
      0, 0, app->hMain, (HMENU)(INT_PTR)CtrlId::ApiKeyEdit, hInst, nullptr);

  // Row 1: Model
  app->hModelLabel = CreateWindowExA(
      0, "STATIC", "Model:", WS_CHILD | WS_VISIBLE | SS_RIGHT, 0, 0, 0, 0,
      app->hMain, (HMENU)(INT_PTR)CtrlId::ModelLabel, hInst, nullptr);

  app->hModelEdit = CreateWindowExA(
      WS_EX_CLIENTEDGE, "EDIT", "gpt-5-mini-2025-08-07",
      WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL, 0, 0, 0, 0,
      app->hMain, (HMENU)(INT_PTR)CtrlId::ModelEdit, hInst, nullptr);

  // Chat display (multiline, read-only)
  app->hChatDisplay = CreateWindowExA(
      WS_EX_CLIENTEDGE, "EDIT", "",
      WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_TABSTOP | ES_MULTILINE |
          ES_READONLY | ES_AUTOVSCROLL | ES_WANTRETURN,
      0, 0, 0, 0, app->hMain, (HMENU)(INT_PTR)CtrlId::ChatDisplay, hInst,
      nullptr);

  // Message input
  app->hMessageInput = CreateWindowExA(
      WS_EX_CLIENTEDGE, "EDIT", "",
      WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL, 0, 0, 0, 0,
      app->hMain, (HMENU)(INT_PTR)CtrlId::MessageInput, hInst, nullptr);

  // Send button
  app->hSendButton = CreateWindowExA(
      0, "BUTTON", "Send",
      WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_DEFPUSHBUTTON, 0,
      0, 0, 0, app->hMain, (HMENU)(INT_PTR)CtrlId::SendButton, hInst, nullptr);

  // Clear button
  app->hClearButton = CreateWindowExA(
      0, "BUTTON", "Clear", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
      0, 0, 0, 0, app->hMain, (HMENU)(INT_PTR)CtrlId::ClearButton, hInst,
      nullptr);

  // Apply fonts
  HWND controls[] = {app->hBaseUrlLabel, app->hBaseUrlEdit, app->hApiKeyLabel,
                     app->hApiKeyEdit,   app->hModelLabel,  app->hModelEdit,
                     app->hMessageInput, app->hSendButton,  app->hClearButton};
  for (int i = 0; i < 9; ++i) {
    SendMessageA(controls[i], WM_SETFONT, (WPARAM)app->hFont, TRUE);
  }
  SendMessageA(app->hChatDisplay, WM_SETFONT, (WPARAM)app->hChatFont, TRUE);

  // Set up placeholder/cue text
  SendMessageA(app->hMessageInput, EM_SETCUEBANNER, TRUE,
               (LPARAM) "Type your message here...");
  SendMessageA(app->hApiKeyEdit, EM_SETCUEBANNER, TRUE,
               (LPARAM) "Enter your API key");

  // Subclass the message input for Enter key handling
  SetWindowLongPtrA(app->hMessageInput, GWLP_USERDATA, (LONG_PTR)app);
  app->origInputProc = (WNDPROC)SetWindowLongPtrA(
      app->hMessageInput, GWLP_WNDPROC, (LONG_PTR)InputSubclassProc);

  // Initial welcome message
  String welcome = "=== Attoboy AI Chat Demo ===\r\n\r\n";
  welcome = welcome + "Enter your Base URL, API Key, and Model above.\r\n";
  welcome =
      welcome + "Then type a message and press Enter or click Send.\r\n\r\n";
  String separator;
  for (int i = 0; i < 60; ++i) {
    separator = separator + "-";
  }
  welcome = welcome + separator + "\r\n\r\n";
  SetControlText(app->hChatDisplay, welcome);
}

//------------------------------------------------------------------------------
// Control Layout
//------------------------------------------------------------------------------

static void LayoutControls(ChatApp *app) {
  RECT rc;
  GetClientRect(app->hMain, &rc);

  int m = Layout::Margin;
  int h = Layout::ControlHeight;
  int gap = Layout::SmallGap;
  int btnW = Layout::ButtonWidth;
  int lblW = Layout::LabelWidth;

  int clientW = rc.right;
  int clientH = rc.bottom;

  // Calculate widths for the top row (three groups)
  // [Label][Edit] [Label][Edit] [Label][Edit]
  int totalLabelW = lblW * 3;
  int totalGaps = gap * 5; // gaps between elements
  int editTotalW = clientW - 2 * m - totalLabelW - totalGaps;

  // Distribute: URL gets 40%, Key gets 35%, Model gets 25%
  int urlEditW = (editTotalW * 40) / 100;
  int keyEditW = (editTotalW * 35) / 100;
  int modelEditW = editTotalW - urlEditW - keyEditW;

  int y = m;
  int x = m;

  // Row 1: URL
  MoveWindow(app->hBaseUrlLabel, x, y + 3, lblW, h, TRUE);
  x += lblW + gap;
  MoveWindow(app->hBaseUrlEdit, x, y, urlEditW, h, TRUE);
  x += urlEditW + gap;

  // Row 1: API Key
  MoveWindow(app->hApiKeyLabel, x, y + 3, lblW, h, TRUE);
  x += lblW + gap;
  MoveWindow(app->hApiKeyEdit, x, y, keyEditW, h, TRUE);
  x += keyEditW + gap;

  // Row 1: Model
  MoveWindow(app->hModelLabel, x, y + 3, lblW, h, TRUE);
  x += lblW + gap;
  MoveWindow(app->hModelEdit, x, y, modelEditW, h, TRUE);

  // Chat display area
  int chatTop = y + h + m;
  int bottomRowY = clientH - m - h;
  int chatHeight = bottomRowY - chatTop - m;

  MoveWindow(app->hChatDisplay, m, chatTop, clientW - 2 * m, chatHeight, TRUE);

  // Bottom row: Input and buttons
  int btnTotalW = btnW * 2 + gap;
  int inputW = clientW - 2 * m - btnTotalW - gap;

  x = m;
  MoveWindow(app->hMessageInput, x, bottomRowY, inputW, h, TRUE);
  x += inputW + gap;
  MoveWindow(app->hSendButton, x, bottomRowY, btnW, h, TRUE);
  x += btnW + gap;
  MoveWindow(app->hClearButton, x, bottomRowY, btnW, h, TRUE);

  InvalidateRect(app->hMain, nullptr, TRUE);
}

//------------------------------------------------------------------------------
// Helper Functions
//------------------------------------------------------------------------------

static String GetControlText(HWND hwnd) {
  int len = GetWindowTextLengthA(hwnd);
  if (len == 0)
    return String();

  char *buf = (char *)Alloc(len + 1);
  if (!buf)
    return String();

  GetWindowTextA(hwnd, buf, len + 1);
  String result(buf, len);
  Free(buf);
  return result;
}

static void SetControlText(HWND hwnd, const String &text) {
  SetWindowTextA(hwnd, text.c_str());
}

static void AppendChatText(ChatApp *app, const String &text) {
  // Move cursor to end
  int len = GetWindowTextLengthA(app->hChatDisplay);
  SendMessageA(app->hChatDisplay, EM_SETSEL, len, len);

  // Insert the text
  SendMessageA(app->hChatDisplay, EM_REPLACESEL, FALSE, (LPARAM)text.c_str());

  ScrollChatToBottom(app);
}

static void ScrollChatToBottom(ChatApp *app) {
  // Scroll to show the caret (which is at the end)
  SendMessageA(app->hChatDisplay, EM_SCROLLCARET, 0, 0);
}

static void SetSendingState(ChatApp *app, bool sending) {
  app->isSending = sending;
  EnableWindow(app->hSendButton, !sending);
  EnableWindow(app->hMessageInput, !sending);
  EnableWindow(app->hClearButton, !sending);
  EnableWindow(app->hBaseUrlEdit, !sending);
  EnableWindow(app->hApiKeyEdit, !sending);
  EnableWindow(app->hModelEdit, !sending);

  SetWindowTextA(app->hSendButton, sending ? "..." : "Send");

  if (!sending) {
    SetFocus(app->hMessageInput);
  }
}

static void CleanupAI(ChatApp *app) {
  if (app->conv) {
    app->conv->~Conversation();
    Free(app->conv);
    app->conv = nullptr;
  }
  if (app->ai) {
    app->ai->~AI();
    Free(app->ai);
    app->ai = nullptr;
  }
}

//------------------------------------------------------------------------------
// Message Handling
//------------------------------------------------------------------------------

static void HandleSend(ChatApp *app) {
  if (app->isSending)
    return;

  // Get input values
  String baseUrl = GetControlText(app->hBaseUrlEdit).trim();
  String apiKey = GetControlText(app->hApiKeyEdit).trim();
  String model = GetControlText(app->hModelEdit).trim();
  String message = GetControlText(app->hMessageInput).trim();

  // Validate
  if (message.isEmpty()) {
    SetFocus(app->hMessageInput);
    return;
  }

  if (baseUrl.isEmpty()) {
    MessageBoxA(app->hMain, "Please enter a Base URL.", "Missing Information",
                MB_OK | MB_ICONWARNING);
    SetFocus(app->hBaseUrlEdit);
    return;
  }

  if (apiKey.isEmpty()) {
    MessageBoxA(app->hMain, "Please enter an API Key.", "Missing Information",
                MB_OK | MB_ICONWARNING);
    SetFocus(app->hApiKeyEdit);
    return;
  }

  if (model.isEmpty()) {
    MessageBoxA(app->hMain, "Please enter a Model name.", "Missing Information",
                MB_OK | MB_ICONWARNING);
    SetFocus(app->hModelEdit);
    return;
  }

  // Clear the input field immediately
  SetControlText(app->hMessageInput, "");

  // Disable UI during send
  SetSendingState(app, true);

  // Display user message
  AppendChatText(app, String("You:\r\n") + message + "\r\n\r\n");

  // Check if we need to (re)create the AI instance
  bool needNewAI = (app->ai == nullptr);
  if (app->ai) {
    // Check if settings changed
    if (!app->ai->getBaseUrl().equals(baseUrl) ||
        !app->ai->getAPIKey().equals(apiKey) ||
        !app->ai->getModel().equals(model)) {
      CleanupAI(app);
      needNewAI = true;
      AppendChatText(app,
                     "[Settings changed - starting new conversation]\r\n\r\n");
    }
  }

  // Create AI instance if needed
  if (needNewAI) {
    void *aiMem = Alloc(sizeof(AI));
    if (aiMem) {
      app->ai = new (aiMem) AI(baseUrl, apiKey, model);
    } else {
      AppendChatText(app,
                     "Assistant:\r\n[Error: Memory allocation failed]\r\n\r\n");
      SetSendingState(app, false);
      return;
    }
  }

  // Create conversation if needed
  if (app->ai && !app->conv) {
    void *convMem = Alloc(sizeof(Conversation));
    if (convMem) {
      app->conv = new (convMem) Conversation(app->ai->createConversation());
    } else {
      AppendChatText(app,
                     "Assistant:\r\n[Error: Memory allocation failed]\r\n\r\n");
      SetSendingState(app, false);
      return;
    }
  }

  // Send the message (this blocks - acceptable for a demo)
  if (app->conv) {
    String response = app->conv->ask(message, 120000); // 2 minute timeout

    if (response.isEmpty()) {
      AppendChatText(app, "Assistant:\r\n[No response received. Check your API "
                          "settings.]\r\n\r\n");
    } else {
      // Format the response with proper line endings for Windows
      String formattedResponse = response.replace("\n", "\r\n");
      AppendChatText(app,
                     String("Assistant:\r\n") + formattedResponse + "\r\n\r\n");
    }
  } else {
    AppendChatText(
        app, "Assistant:\r\n[Error: Could not establish conversation]\r\n\r\n");
  }

  // Add separator
  String separator;
  for (int i = 0; i < 60; ++i) {
    separator = separator + "-";
  }
  AppendChatText(app, separator + "\r\n\r\n");

  // Re-enable UI
  SetSendingState(app, false);
}

static void HandleClear(ChatApp *app) {
  if (app->isSending)
    return;

  int result = MessageBoxA(app->hMain,
                           "Clear the conversation history?\n\nThis will also "
                           "reset the AI conversation context.",
                           "Clear Chat", MB_YESNO | MB_ICONQUESTION);

  if (result == IDYES) {
    // Clear the display
    SetControlText(app->hChatDisplay, "");

    // Reset conversation (keep AI instance)
    if (app->conv) {
      app->conv->~Conversation();
      Free(app->conv);
      app->conv = nullptr;
    }

    // Show welcome message again
    String welcome = "=== Conversation Cleared ===\r\n\r\n";
    welcome = welcome + "Ready for a new conversation.\r\n\r\n";
    String separator;
    for (int i = 0; i < 60; ++i) {
      separator = separator + "-";
    }
    welcome = welcome + separator + "\r\n\r\n";
    AppendChatText(app, welcome);

    SetFocus(app->hMessageInput);
  }
}
