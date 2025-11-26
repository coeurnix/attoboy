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

  const ATTO_WCHAR *data = impl->data;
  int start = 0;

  for (int i = 0; i < impl->len; i++) {
    if (data[i] == '\r' && i + 1 < impl->len && data[i + 1] == '\n') {
      ATTO_LPSTR line = AllocString(i - start);
      if (line) {
        MyStrNCpy(line, data + start, i - start);
        line[i - start] = '\0';
        result.append(String(line));
        FreeString(line);
      }
      i++;
      start = i + 1;
    } else if (data[i] == '\n') {
      ATTO_LPSTR line = AllocString(i - start);
      if (line) {
        MyStrNCpy(line, data + start, i - start);
        line[i - start] = '\0';
        result.append(String(line));
        FreeString(line);
      }
      start = i + 1;
    }
  }

  if (start < impl->len) {
    ATTO_LPSTR line = AllocString(impl->len - start);
    if (line) {
      MyStrNCpy(line, data + start, impl->len - start);
      line[impl->len - start] = '\0';
      result.append(String(line));
      FreeString(line);
    }
  } else if (start == impl->len && impl->len > 0 &&
             (data[impl->len - 1] == '\n' ||
              (impl->len > 1 && data[impl->len - 2] == '\r' &&
               data[impl->len - 1] == '\n'))) {
    result.append(String());
  }

  return result;
}

} // namespace attoboy
