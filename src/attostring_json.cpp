#include "attolist_internal.h"
#include "attomap_internal.h"
#include "attostring_internal.h"


namespace attoboy {

static String EscapeJsonString(const String &str) {
  String result;
  int len = str.byteLength();

  for (int i = 0; i < len; i++) {
    ATTO_WCHAR ch = str.c_str()[i];

    switch (ch) {
    case '"':
      result = result.append("\\\"");
      break;
    case '\\':
      result = result.append("\\\\");
      break;
    case '\b':
      result = result.append("\\b");
      break;
    case '\f':
      result = result.append("\\f");
      break;
    case '\n':
      result = result.append("\\n");
      break;
    case '\r':
      result = result.append("\\r");
      break;
    case '\t':
      result = result.append("\\t");
      break;
    default:
      if (ch < 32) {
        ATTO_WCHAR buf[7];
        wsprintfA(buf, "\\u%04x", (int)ch);
        result = result.append(buf);
      } else {
        ATTO_WCHAR temp[2] = {ch, '\0'};
        result = result.append(temp);
      }
      break;
    }
  }

  return result;
}

static String ValueToJson(ValueType type, const List &list, int index);

static String ListToJson(const List &list) {
  String result("[");
  int len = list.length();

  for (int i = 0; i < len; i++) {
    ValueType type = list.typeAt(i);
    result = result.append(ValueToJson(type, list, i));

    if (i < len - 1) {
      result = result.append(",");
    }
  }

  result = result.append("]");
  return result;
}

static String MapToJson(const Map &map) {
  String result("{");
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
      keyStr = String("null");
      break;
    }

    result = result.append("\"");
    result = result.append(EscapeJsonString(keyStr));
    result = result.append("\":");

    ValueType valueType = values.typeAt(i);
    result = result.append(ValueToJson(valueType, values, i));

    if (i < len - 1) {
      result = result.append(",");
    }
  }

  result = result.append("}");
  return result;
}

static String ValueToJson(ValueType type, const List &list, int index) {
  switch (type) {
  case TYPE_BOOL:
    return String(list.at<bool>(index) ? "true" : "false");
  case TYPE_INT:
    return String(list.at<int>(index));
  case TYPE_FLOAT: {
    String num = String(list.at<float>(index));
    if (!num.contains(".") && !num.contains("e") && !num.contains("E")) {
      num = num.append(".0");
    }
    return num;
  }
  case TYPE_STRING: {
    String str = list.at<String>(index);
    String result("\"");
    result = result.append(EscapeJsonString(str));
    result = result.append("\"");
    return result;
  }
  case TYPE_LIST:
    return ListToJson(list.at<List>(index));
  case TYPE_MAP:
    return MapToJson(list.at<Map>(index));
  case TYPE_NULL:
  default:
    return String("null");
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
      memcpy(impl->data, json.impl->data, len);
      impl->data[len] = '\0';
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
      memcpy(impl->data, json.impl->data, len);
      impl->data[len] = '\0';
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
      memcpy(impl->data, json.impl->data, len);
      impl->data[len] = '\0';
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
