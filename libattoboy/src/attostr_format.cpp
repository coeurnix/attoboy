#include "attostr_internal.h"
#include "attolist_internal.h"
#include "attomap_internal.h"

namespace attoboy {

static String IntToString(int value) {
  bool isNegative = value < 0;
  if (isNegative)
    value = -value;

  wchar_t buffer[12];
  int pos = 0;

  if (value == 0) {
    buffer[pos++] = L'0';
  } else {
    while (value > 0) {
      buffer[pos++] = L'0' + (value % 10);
      value /= 10;
    }
  }

  if (isNegative) {
    buffer[pos++] = L'-';
  }

  for (int i = 0; i < pos / 2; i++) {
    wchar_t temp = buffer[i];
    buffer[i] = buffer[pos - 1 - i];
    buffer[pos - 1 - i] = temp;
  }

  buffer[pos] = L'\0';
  return String(buffer);
}

String String::format(const List &list) const {
  if (!impl) {
    return String();
  }

  ReadLockGuard guard(&impl->lock);

  String result;
  const wchar_t *data = impl->data;
  int len = impl->len;

  for (int i = 0; i < len; i++) {
    if (data[i] == L'{') {
      int start = i + 1;
      int end = start;

      while (end < len && data[end] >= L'0' && data[end] <= L'9') {
        end++;
      }

      if (end > start && end < len && data[end] == L'}') {
        int index = 0;
        for (int j = start; j < end; j++) {
          index = index * 10 + (data[j] - L'0');
        }

        if (index >= 0 && index < list.length()) {
          ValueType type = list.typeAt(index);

          String replacement;
          switch (type) {
          case TYPE_BOOL:
            replacement = String(list.at<bool>(index));
            break;
          case TYPE_INT:
            replacement = String(list.at<int>(index));
            break;
          case TYPE_LONG_LONG:
            replacement = String(list.at<long long>(index));
            break;
          case TYPE_DOUBLE:
            replacement = String(list.at<double>(index));
            break;
          case TYPE_STRING:
            replacement = list.at<String>(index);
            break;
          case TYPE_LIST:
            replacement = String(list.at<List>(index));
            break;
          case TYPE_MAP:
            replacement = String(list.at<Map>(index));
            break;
          case TYPE_NULL:
            replacement = String("null");
            break;
          default:
            replacement = String();
            break;
          }

          result.append(replacement);
        } else {
          result.append(L"{");
          LPWSTR partial = AllocString(end - start);
          if (partial) {
            MyWcsNCpy(partial, data + start, end - start);
            partial[end - start] = L'\0';
            result.append(String(partial));
            FreeString(partial);
          }
          result.append(L"}");
        }

        i = end;
        continue;
      }
    }

    wchar_t ch[2] = {data[i], L'\0'};
    result.append(String(ch));
  }

  return result;
}

String String::format(const Map &map) const {
  if (!impl) {
    return String();
  }

  ReadLockGuard guard1(&impl->lock);

  String result;
  const wchar_t *data = impl->data;
  int len = impl->len;

  for (int i = 0; i < len; i++) {
    if (data[i] == L'{') {
      int start = i + 1;
      int end = start;

      while (end < len && data[end] != L'}') {
        end++;
      }

      if (end > start && end < len && data[end] == L'}') {
        LPWSTR keyBuffer = AllocString(end - start);
        if (keyBuffer) {
          MyWcsNCpy(keyBuffer, data + start, end - start);
          keyBuffer[end - start] = L'\0';
          String key(keyBuffer);
          FreeString(keyBuffer);

          ValueType type = map.typeAt<String>(key);
          if (type != TYPE_INVALID) {
            String replacement;
            switch (type) {
            case TYPE_BOOL:
              replacement = String(map.get<String, bool>(key));
              break;
            case TYPE_INT:
              replacement = String(map.get<String, int>(key));
              break;
            case TYPE_LONG_LONG:
              replacement = String(map.get<String, long long>(key));
              break;
            case TYPE_DOUBLE:
              replacement = String(map.get<String, double>(key));
              break;
            case TYPE_STRING:
              replacement = map.get<String, String>(key);
              break;
            case TYPE_LIST:
              replacement = String(map.get<String, List>(key));
              break;
            case TYPE_MAP:
              replacement = String(map.get<String, Map>(key));
              break;
            case TYPE_NULL:
              replacement = String("null");
              break;
            default:
              replacement = String();
              break;
            }

            result.append(replacement);
          } else {
            result.append(L"{");
            result.append(key);
            result.append(L"}");
          }
        }

        i = end;
        continue;
      }
    }

    wchar_t ch[2] = {data[i], L'\0'};
    result.append(String(ch));
  }

  return result;
}

} // namespace attoboy
