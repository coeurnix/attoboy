#include "attostr_internal.h"
#include "attolist_internal.h"
#include "attomap_internal.h"

namespace attoboy {

static String EscapeJsonString(const String &str) {
  String result;
  int len = str.length();

  for (int i = 0; i < len; i++) {
    wchar_t ch = str.c_str()[i];

    switch (ch) {
    case L'"':
      result.append(L"\\\"");
      break;
    case L'\\':
      result.append(L"\\\\");
      break;
    case L'\b':
      result.append(L"\\b");
      break;
    case L'\f':
      result.append(L"\\f");
      break;
    case L'\n':
      result.append(L"\\n");
      break;
    case L'\r':
      result.append(L"\\r");
      break;
    case L'\t':
      result.append(L"\\t");
      break;
    default:
      if (ch < 32) {
        wchar_t buf[7];
        wsprintfW(buf, L"\\u%04x", (int)ch);
        result.append(buf);
      } else {
        wchar_t temp[2] = {ch, L'\0'};
        result.append(temp);
      }
      break;
    }
  }

  return result;
}

static String ValueToJson(ValueType type, const List &list, int index);

static String ListToJson(const List &list) {
  String result(L"[");
  int len = list.length();

  for (int i = 0; i < len; i++) {
    ValueType type = list.typeAt(i);
    result.append(ValueToJson(type, list, i));

    if (i < len - 1) {
      result.append(L",");
    }
  }

  result.append(L"]");
  return result;
}

static String MapToJson(const Map &map) {
  String result(L"{");
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
    case TYPE_LONG_LONG:
      keyStr = String(keys.at<long long>(i));
      break;
    case TYPE_DOUBLE:
      keyStr = String(keys.at<double>(i));
      break;
    case TYPE_STRING:
      keyStr = keys.at<String>(i);
      break;
    default:
      keyStr = String("null");
      break;
    }

    result.append(L"\"");
    result.append(EscapeJsonString(keyStr));
    result.append(L"\":");

    ValueType valueType = values.typeAt(i);
    result.append(ValueToJson(valueType, values, i));

    if (i < len - 1) {
      result.append(L",");
    }
  }

  result.append(L"}");
  return result;
}

static String ValueToJson(ValueType type, const List &list, int index) {
  switch (type) {
  case TYPE_BOOL:
    return String(list.at<bool>(index) ? "true" : "false");
  case TYPE_INT:
    return String(list.at<int>(index));
  case TYPE_LONG_LONG:
    return String(list.at<long long>(index));
  case TYPE_DOUBLE: {
    String num = String(list.at<double>(index));
    if (!num.contains(L".") && !num.contains(L"e") && !num.contains(L"E")) {
      num.append(L".0");
    }
    return num;
  }
  case TYPE_STRING: {
    String str = list.at<String>(index);
    String result(L"\"");
    result.append(EscapeJsonString(str));
    result.append(L"\"");
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
      lstrcpyW(impl->data, json.impl->data);
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
      lstrcpyW(impl->data, json.impl->data);
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
      lstrcpyW(impl->data, json.impl->data);
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
