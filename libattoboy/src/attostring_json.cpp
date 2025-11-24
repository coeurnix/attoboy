#include "attolist_internal.h"
#include "attomap_internal.h"
#include "attostring_internal.h"


namespace attoboy {

static String EscapeJsonString(const String &str) {
  String result;
  int len = str.length();

  for (int i = 0; i < len; i++) {
    ATTO_WCHAR ch = str.c_str()[i];

    switch (ch) {
    case ATTO_TEXT('"'):
      result = result.append(ATTO_TEXT("\\\""));
      break;
    case ATTO_TEXT('\\'):
      result = result.append(ATTO_TEXT("\\\\"));
      break;
    case ATTO_TEXT('\b'):
      result = result.append(ATTO_TEXT("\\b"));
      break;
    case ATTO_TEXT('\f'):
      result = result.append(ATTO_TEXT("\\f"));
      break;
    case ATTO_TEXT('\n'):
      result = result.append(ATTO_TEXT("\\n"));
      break;
    case ATTO_TEXT('\r'):
      result = result.append(ATTO_TEXT("\\r"));
      break;
    case ATTO_TEXT('\t'):
      result = result.append(ATTO_TEXT("\\t"));
      break;
    default:
      if (ch < 32) {
        ATTO_WCHAR buf[7];
        wsprintfA(buf, "\\u%04x", (int)ch);
        result = result.append(buf);
      } else {
        ATTO_WCHAR temp[2] = {ch, ATTO_TEXT('\0')};
        result = result.append(temp);
      }
      break;
    }
  }

  return result;
}

static String ValueToJson(ValueType type, const List &list, int index);

static String ListToJson(const List &list) {
  String result(ATTO_TEXT("["));
  int len = list.length();

  for (int i = 0; i < len; i++) {
    ValueType type = list.typeAt(i);
    result = result.append(ValueToJson(type, list, i));

    if (i < len - 1) {
      result = result.append(ATTO_TEXT(","));
    }
  }

  result = result.append(ATTO_TEXT("]"));
  return result;
}

static String MapToJson(const Map &map) {
  String result(ATTO_TEXT("{"));
  List keys = map.keys();
  List values = map.values();
  int len = keys.length();

  for (int i = 0; i < len; i++) {
    ValueType keyType = keys.typeAt(i);

    String keyStr;
    switch (keyType) {
    case TYPE_BOOL:
      keyStr = String(keys.at<bool>(i));
      break;
    case TYPE_INT:
      keyStr = String(keys.at<int>(i));
      break;
    case TYPE_FLOAT:
      keyStr = String(keys.at<float>(i));
      break;
    case TYPE_STRING:
      keyStr = keys.at<String>(i);
      break;
    default:
      keyStr = String(ATTO_TEXT("null"));
      break;
    }

    result = result.append(ATTO_TEXT("\""));
    result = result.append(EscapeJsonString(keyStr));
    result = result.append(ATTO_TEXT("\":"));

    ValueType valueType = values.typeAt(i);
    result = result.append(ValueToJson(valueType, values, i));

    if (i < len - 1) {
      result = result.append(ATTO_TEXT(","));
    }
  }

  result = result.append(ATTO_TEXT("}"));
  return result;
}

static String ValueToJson(ValueType type, const List &list, int index) {
  switch (type) {
  case TYPE_BOOL:
    return String(list.at<bool>(index) ? ATTO_TEXT("true") : ATTO_TEXT("false"));
  case TYPE_INT:
    return String(list.at<int>(index));
  case TYPE_FLOAT: {
    String num = String(list.at<float>(index));
    if (!num.contains(ATTO_TEXT(".")) && !num.contains(ATTO_TEXT("e")) && !num.contains(ATTO_TEXT("E"))) {
      num = num.append(ATTO_TEXT(".0"));
    }
    return num;
  }
  case TYPE_STRING: {
    String str = list.at<String>(index);
    String result(ATTO_TEXT("\""));
    result = result.append(EscapeJsonString(str));
    result = result.append(ATTO_TEXT("\""));
    return result;
  }
  case TYPE_LIST:
    return ListToJson(list.at<List>(index));
  case TYPE_MAP:
    return MapToJson(list.at<Map>(index));
  case TYPE_NULL:
  default:
    return String(ATTO_TEXT("null"));
  }
}

String::String(const List &list) {
  impl = (StringImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                 sizeof(StringImpl));
  InitializeSRWLock(&impl->lock);

  String json = ListToJson(list);

  if (json.impl) {
    ReadLockGuard guard(&json.impl->lock);
    int len = json.impl->len;
    impl->data = AllocString(len);
    if (impl->data) {
      lstrcpyA(impl->data, json.impl->data);
      impl->len = len;
    } else {
      impl->data = AllocString(0);
      impl->len = 0;
    }
  } else {
    impl->data = AllocString(0);
    impl->len = 0;
  }
}

String::String(const Map &map) {
  impl = (StringImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                 sizeof(StringImpl));
  InitializeSRWLock(&impl->lock);

  String json = MapToJson(map);

  if (json.impl) {
    ReadLockGuard guard(&json.impl->lock);
    int len = json.impl->len;
    impl->data = AllocString(len);
    if (impl->data) {
      lstrcpyA(impl->data, json.impl->data);
      impl->len = len;
    } else {
      impl->data = AllocString(0);
      impl->len = 0;
    }
  } else {
    impl->data = AllocString(0);
    impl->len = 0;
  }
}

String::String(const Set &set) {
  impl = (StringImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                 sizeof(StringImpl));
  InitializeSRWLock(&impl->lock);

  List list = set.toList();
  String json = ListToJson(list);

  if (json.impl) {
    ReadLockGuard guard(&json.impl->lock);
    int len = json.impl->len;
    impl->data = AllocString(len);
    if (impl->data) {
      lstrcpyA(impl->data, json.impl->data);
      impl->len = len;
    } else {
      impl->data = AllocString(0);
      impl->len = 0;
    }
  } else {
    impl->data = AllocString(0);
    impl->len = 0;
  }
}

} // namespace attoboy
