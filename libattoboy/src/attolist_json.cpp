#include "attolist_internal.h"
#include "attomap_internal.h"
#include "attostring_internal.h"

namespace attoboy {

String List::toJSONString() const { return String(*this); }

void SkipWhitespace(const String &json, int &pos) {
  int len = json.length();
  while (pos < len) {
    String ch = json.at(pos);
    if (ch.equals(ATTO_TEXT(" ")) || ch.equals(ATTO_TEXT("\t")) ||
        ch.equals(ATTO_TEXT("\n")) || ch.equals(ATTO_TEXT("\r"))) {
      pos++;
    } else {
      break;
    }
  }
}

String ParseJsonString(const String &json, int &pos) {
  if (pos >= json.length() || !json.at(pos).equals(ATTO_TEXT("\""))) {
    return String();
  }

  pos++;
  String result;
  int len = json.length();

  while (pos < len) {
    String ch = json.at(pos);

    if (ch.equals(ATTO_TEXT("\\"))) {
      if (pos + 1 < len) {
        String nextCh = json.at(pos + 1);
        if (nextCh.equals(ATTO_TEXT("\""))) {
          result = result.append(ATTO_TEXT("\""));
          pos += 2;
        } else if (nextCh.equals(ATTO_TEXT("\\"))) {
          result = result.append(ATTO_TEXT("\\"));
          pos += 2;
        } else if (nextCh.equals(ATTO_TEXT("/"))) {
          result = result.append(ATTO_TEXT("/"));
          pos += 2;
        } else if (nextCh.equals(ATTO_TEXT("b"))) {
          result = result.append(ATTO_TEXT("\b"));
          pos += 2;
        } else if (nextCh.equals(ATTO_TEXT("f"))) {
          result = result.append(ATTO_TEXT("\f"));
          pos += 2;
        } else if (nextCh.equals(ATTO_TEXT("n"))) {
          result = result.append(ATTO_TEXT("\n"));
          pos += 2;
        } else if (nextCh.equals(ATTO_TEXT("r"))) {
          result = result.append(ATTO_TEXT("\r"));
          pos += 2;
        } else if (nextCh.equals(ATTO_TEXT("t"))) {
          result = result.append(ATTO_TEXT("\t"));
          pos += 2;
        } else if (nextCh.equals(ATTO_TEXT("u"))) {
          if (pos + 5 < len) {
            String hexStr = json.substring(pos + 2, pos + 6);
            int codePoint = 0;
            for (int i = 0; i < 4; i++) {
              String hexCh = hexStr.at(i);
              int digit = 0;
              if (hexCh.c_str()[0] >= '0' && hexCh.c_str()[0] <= '9') {
                digit = hexCh.c_str()[0] - '0';
              } else if (hexCh.c_str()[0] >= 'a' && hexCh.c_str()[0] <= 'f') {
                digit = hexCh.c_str()[0] - 'a' + 10;
              } else if (hexCh.c_str()[0] >= 'A' && hexCh.c_str()[0] <= 'F') {
                digit = hexCh.c_str()[0] - 'A' + 10;
              }
              codePoint = codePoint * 16 + digit;
            }
            ATTO_WCHAR buf[2] = {(ATTO_WCHAR)codePoint, 0};
            result = result.append(buf);
            pos += 6;
          } else {
            pos += 2;
          }
        } else {
          pos++;
        }
      } else {
        pos++;
      }
    } else if (ch.equals(ATTO_TEXT("\""))) {
      pos++;
      break;
    } else {
      result = result.append(ch);
      pos++;
    }
  }

  return result;
}

static String ParseJsonNumber(const String &json, int &pos) {
  int len = json.length();
  String result;

  while (pos < len) {
    String ch = json.at(pos);
    ATTO_WCHAR c = ch.c_str()[0];
    if ((c >= '0' && c <= '9') || c == '-' || c == '+' || c == '.' ||
        c == 'e' || c == 'E') {
      result = result.append(ch);
      pos++;
    } else {
      break;
    }
  }

  return result;
}

List ParseJsonArray(const String &json, int &pos);
Map ParseJsonObject(const String &json, int &pos);

void ParseJsonValue(const String &json, int &pos, List &list) {
  SkipWhitespace(json, pos);

  if (pos >= json.length()) {
    list.append(0);
    return;
  }

  String ch = json.at(pos);

  if (ch.equals(ATTO_TEXT("\""))) {
    String str = ParseJsonString(json, pos);
    list.append(str);
  } else if (ch.equals(ATTO_TEXT("["))) {
    List arr = ParseJsonArray(json, pos);
    list.append(arr);
  } else if (ch.equals(ATTO_TEXT("{"))) {
    Map obj = ParseJsonObject(json, pos);
    list.append(obj);
  } else if (ch.equals(ATTO_TEXT("t"))) {
    if (pos + 4 <= json.length() &&
        json.substring(pos, pos + 4).equals(ATTO_TEXT("true"))) {
      list.append(true);
      pos += 4;
    } else {
      list.append(0);
    }
  } else if (ch.equals(ATTO_TEXT("f"))) {
    if (pos + 5 <= json.length() &&
        json.substring(pos, pos + 5).equals(ATTO_TEXT("false"))) {
      list.append(false);
      pos += 5;
    } else {
      list.append(0);
    }
  } else if (ch.equals(ATTO_TEXT("n"))) {
    if (pos + 4 <= json.length() &&
        json.substring(pos, pos + 4).equals(ATTO_TEXT("null"))) {
      list.append(0);
      pos += 4;
    } else {
      list.append(0);
    }
  } else {
    String numStr = ParseJsonNumber(json, pos);
    if (numStr.contains(ATTO_TEXT(".")) || numStr.contains(ATTO_TEXT("e")) ||
        numStr.contains(ATTO_TEXT("E"))) {
      list.append(numStr.toFloat());
    } else {
      list.append(numStr.toInteger());
    }
  }
}

List ParseJsonArray(const String &json, int &pos) {
  List result;

  if (pos >= json.length() || !json.at(pos).equals(ATTO_TEXT("["))) {
    return result;
  }

  pos++;
  SkipWhitespace(json, pos);

  if (pos < json.length() && json.at(pos).equals(ATTO_TEXT("]"))) {
    pos++;
    return result;
  }

  while (pos < json.length()) {
    ParseJsonValue(json, pos, result);
    SkipWhitespace(json, pos);

    if (pos >= json.length()) {
      break;
    }

    String ch = json.at(pos);
    if (ch.equals(ATTO_TEXT(","))) {
      pos++;
      SkipWhitespace(json, pos);
    } else if (ch.equals(ATTO_TEXT("]"))) {
      pos++;
      break;
    } else {
      break;
    }
  }

  return result;
}

Map ParseJsonObject(const String &json, int &pos) {
  Map result;

  if (pos >= json.length() || !json.at(pos).equals(ATTO_TEXT("{"))) {
    return result;
  }

  pos++;
  SkipWhitespace(json, pos);

  if (pos < json.length() && json.at(pos).equals(ATTO_TEXT("}"))) {
    pos++;
    return result;
  }

  while (pos < json.length()) {
    SkipWhitespace(json, pos);

    String key = ParseJsonString(json, pos);
    SkipWhitespace(json, pos);

    if (pos >= json.length() || !json.at(pos).equals(ATTO_TEXT(":"))) {
      break;
    }

    pos++;
    SkipWhitespace(json, pos);

    List tempList;
    ParseJsonValue(json, pos, tempList);

    if (tempList.length() > 0) {
      ValueType type = tempList.typeAt(0);
      switch (type) {
      case TYPE_BOOL:
        result.put(key, tempList.at<bool>(0));
        break;
      case TYPE_INT:
        result.put(key, tempList.at<int>(0));
        break;
      case TYPE_FLOAT:
        result.put(key, tempList.at<float>(0));
        break;
      case TYPE_STRING:
        result.put(key, tempList.at<String>(0));
        break;
      case TYPE_LIST:
        result.put(key, tempList.at<List>(0));
        break;
      case TYPE_MAP:
        result.put(key, tempList.at<Map>(0));
        break;
      default:
        break;
      }
    }

    SkipWhitespace(json, pos);

    if (pos >= json.length()) {
      break;
    }

    String ch = json.at(pos);
    if (ch.equals(ATTO_TEXT(","))) {
      pos++;
    } else if (ch.equals(ATTO_TEXT("}"))) {
      pos++;
      break;
    } else {
      break;
    }
  }

  return result;
}

List List::FromJSONString(const String &json) {
  int pos = 0;
  return ParseJsonArray(json, pos);
}

} // namespace attoboy
