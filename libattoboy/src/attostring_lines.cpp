#include "attolist_internal.h"
#include "attostring_internal.h"


namespace attoboy {

List String::lines() const {
  List result;

  if (!impl) {
    return result;
  }

  ReadLockGuard guard(&impl->lock);

  if (impl->len == 0) {
    return result;
  }

  const wchar_t *data = impl->data;
  int start = 0;

  for (int i = 0; i < impl->len; i++) {
    if (data[i] == L'\r' && i + 1 < impl->len && data[i + 1] == L'\n') {
      LPWSTR line = AllocString(i - start);
      if (line) {
        MyWcsNCpy(line, data + start, i - start);
        line[i - start] = L'\0';
        result.append(String(line));
        FreeString(line);
      }
      i++;
      start = i + 1;
    } else if (data[i] == L'\n') {
      LPWSTR line = AllocString(i - start);
      if (line) {
        MyWcsNCpy(line, data + start, i - start);
        line[i - start] = L'\0';
        result.append(String(line));
        FreeString(line);
      }
      start = i + 1;
    }
  }

  if (start < impl->len) {
    LPWSTR line = AllocString(impl->len - start);
    if (line) {
      MyWcsNCpy(line, data + start, impl->len - start);
      line[impl->len - start] = L'\0';
      result.append(String(line));
      FreeString(line);
    }
  } else if (start == impl->len && impl->len > 0 &&
             (data[impl->len - 1] == L'\n' ||
              (impl->len > 1 && data[impl->len - 2] == L'\r' &&
               data[impl->len - 1] == L'\n'))) {
    result.append(String());
  }

  return result;
}

} // namespace attoboy
