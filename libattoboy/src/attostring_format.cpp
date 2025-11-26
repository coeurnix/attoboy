#include "attolist_internal.h"
#include "attomap_internal.h"
#include "attostring_internal.h"


namespace attoboy {

static String IntToString(int value) {
  bool isNegative = value < 0;
  if (isNegative)
    value = -value;

  ATTO_WCHAR buffer[12];
  int pos = 0;

  if (value == 0) {
    buffer[pos++] = '0';
  } else {
    while (value > 0) {
      buffer[pos++] = '0' + (value % 10);
      value /= 10;
    }
  }

  if (isNegative) {
    buffer[pos++] = '-';
  }

  for (int i = 0; i < pos / 2; i++) {
    ATTO_WCHAR temp = buffer[i];
    buffer[i] = buffer[pos - 1 - i];
    buffer[pos - 1 - i] = temp;
  }

  buffer[pos] = '\0';
  return String(buffer);
}

String String::format(const List &list) const {
  if (!impl) {
    return String();
  }

  ReadLockGuard guard(&impl->lock);

  String result;
  const ATTO_WCHAR *data = impl->data;
  int len = impl->len;

  for (int i = 0; i < len; i++) {
    if (data[i] == '{') {
      int start = i + 1;
      int end = start;

      while (end < len && data[end] >= '0' && data[end] <= '9') {
        end++;
      }

      if (end > start && end < len && data[end] == '}') {
        int index = 0;
        for (int j = start; j < end; j++) {
          index = index * 10 + (data[j] - '0');
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
          case TYPE_FLOAT:
            replacement = String(list.at<float>(index));
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

          result = result.append(replacement);
        } else {
          result = result.append("{");
          ATTO_LPSTR partial = AllocString(end - start);
          if (partial) {
            MyStrNCpy(partial, data + start, end - start);
            partial[end - start] = '\0';
            result = result.append(String(partial));
            FreeString(partial);
          }
          result = result.append("}");
        }

        i = end;
        continue;
      }
    }

    ATTO_WCHAR ch[2] = {data[i], '\0'};
    result = result.append(String(ch));
  }

  return result;
}

String String::format(const Map &map) const {
  if (!impl) {
    return String();
  }

  ReadLockGuard guard1(&impl->lock);

  String result;
  const ATTO_WCHAR *data = impl->data;
  int len = impl->len;

  for (int i = 0; i < len; i++) {
    if (data[i] == '{') {
      int start = i + 1;
      int end = start;

      while (end < len && data[end] != '}') {
        end++;
      }

      if (end > start && end < len && data[end] == '}') {
        ATTO_LPSTR keyBuffer = AllocString(end - start);
        if (keyBuffer) {
          MyStrNCpy(keyBuffer, data + start, end - start);
          keyBuffer[end - start] = '\0';
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
            case TYPE_FLOAT:
              replacement = String(map.get<String, float>(key));
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

            result = result.append(replacement);
          } else {
            result = result.append("{");
            result = result.append(key);
            result = result.append("}");
          }
        }

        i = end;
        continue;
      }
    }

    ATTO_WCHAR ch[2] = {data[i], '\0'};
    result = result.append(String(ch));
  }

  return result;
}

} // namespace attoboy
