#include "attoconsole_internal.h"

namespace attoboy {

static DWORD WINAPI HotkeyThreadProc(LPVOID lpParam) {
  ConsoleImpl *impl = (ConsoleImpl *)lpParam;

  while (impl->hotkeyThreadRunning) {
    if (!impl->hotkeyCallbacks || impl->hotkeyCallbacks->isEmpty()) {
      Sleep(100);
      continue;
    }

    for (int i = 0; i < impl->hotkeyCallbacks->length(); i++) {
      HotkeyCallback *hk = (HotkeyCallback *)impl->hotkeyCallbacks->at<int>(i);

      bool ctrlPressed = (GetAsyncKeyState(VK_CONTROL) & 0x8000) &&
                         (hk->modifiers & (MOD_CONTROL));
      bool altPressed =
          (GetAsyncKeyState(VK_MENU) & 0x8000) && (hk->modifiers & (MOD_ALT));
      bool shiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) &&
                          (hk->modifiers & (MOD_SHIFT));

      bool modifiersMatch = true;
      if (hk->modifiers & MOD_CONTROL) {
        if (!(GetAsyncKeyState(VK_CONTROL) & 0x8000))
          modifiersMatch = false;
      } else {
        if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
          modifiersMatch = false;
      }

      if (hk->modifiers & MOD_ALT) {
        if (!(GetAsyncKeyState(VK_MENU) & 0x8000))
          modifiersMatch = false;
      } else {
        if (GetAsyncKeyState(VK_MENU) & 0x8000)
          modifiersMatch = false;
      }

      if (hk->modifiers & MOD_SHIFT) {
        if (!(GetAsyncKeyState(VK_SHIFT) & 0x8000))
          modifiersMatch = false;
      } else {
        if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
          modifiersMatch = false;
      }

      if (modifiersMatch && (GetAsyncKeyState(hk->vk) & 0x8000)) {
        if (hk->callback) {
          hk->callback(hk->arg);
        }
        Sleep(200);
      }
    }

    Sleep(50);
  }

  return 0;
}

Console &Console::onHotkey(const String &key, void (*callback)(void *),
                           void *arg) {
  AcquireSRWLockExclusive(&impl->lock);

  if (!impl->hotkeyCallbacks) {
    impl->hotkeyCallbacks = new List();
  }

  HotkeyCallback *hk = (HotkeyCallback *)HeapAlloc(
      GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(HotkeyCallback));
  hk->callback = callback;
  hk->arg = arg;
  hk->modifiers = 0;
  hk->vk = 0;
  hk->id = impl->hotkeyCallbacks->length();

  List parts = key.split("+");
  for (int i = 0; i < parts.length(); i++) {
    String part = parts.at<String>(i).trim().lower();

    if (part == "ctrl" || part == "control") {
      hk->modifiers |= MOD_CONTROL;
    } else if (part == "alt") {
      hk->modifiers |= MOD_ALT;
    } else if (part == "shift") {
      hk->modifiers |= MOD_SHIFT;
    } else if (part.length() == 1) {
      char c = part.c_str()[0];
      if (c >= 'a' && c <= 'z') {
        hk->vk = (WORD)(c - 'a' + 'A');
      } else {
        hk->vk = (WORD)c;
      }
    } else if (part.startsWith("f") && part.length() > 1) {
      int fnum = part.substring(1).toInteger();
      if (fnum >= 1 && fnum <= 24) {
        hk->vk = VK_F1 + fnum - 1;
      }
    }
  }

  impl->hotkeyCallbacks->append((int)(size_t)hk);

  if (!impl->hotkeyThreadRunning) {
    impl->hotkeyThreadRunning = true;
    impl->hotkeyThread =
        CreateThread(nullptr, 0, HotkeyThreadProc, impl, 0, nullptr);
  }

  ReleaseSRWLockExclusive(&impl->lock);
  return *this;
}

Console &Console::offHotkey(const String &key) {
  AcquireSRWLockExclusive(&impl->lock);

  if (!impl->hotkeyCallbacks) {
    ReleaseSRWLockExclusive(&impl->lock);
    return *this;
  }

  for (int i = impl->hotkeyCallbacks->length() - 1; i >= 0; i--) {
    HotkeyCallback *hk = (HotkeyCallback *)impl->hotkeyCallbacks->at<int>(i);
    HeapFree(GetProcessHeap(), 0, hk);
    impl->hotkeyCallbacks->remove(i);
  }

  ReleaseSRWLockExclusive(&impl->lock);
  return *this;
}

} // namespace attoboy
