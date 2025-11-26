#include "attolist_internal.h"
#include "attostring_internal.h"


namespace attoboy {

String String::join(const List &list) const {
  String result;

  int len = list.length();
  if (len == 0) {
    return result;
  }

  for (int i = 0; i < len; i++) {
    ValueType type = list.typeAt(i);

    String element;
    switch (type) {
    case TYPE_BOOL:
      element = String(list.at<bool>(i));
      break;
    case TYPE_INT:
      element = String(list.at<int>(i));
      break;
    case TYPE_FLOAT:
      element = String(list.at<float>(i));
      break;
    case TYPE_STRING:
      element = list.at<String>(i);
      break;
    case TYPE_LIST:
      element = String(list.at<List>(i));
      break;
    case TYPE_MAP:
      element = String(list.at<Map>(i));
      break;
    case TYPE_NULL:
      element = String("null");
      break;
    default:
      element = String();
      break;
    }

    result = result.append(element);

    if (i < len - 1) {
      result = result.append(*this);
    }
  }

  return result;
}

} // namespace attoboy
