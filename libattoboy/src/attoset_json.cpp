#include "attoset_internal.h"
#include "attolist_internal.h"
#include "attostring_internal.h"

namespace attoboy {

String Set::toJSONString() const { return String(*this); }

Set Set::FromJSONString(const String &json) {
  List list = List::FromJSONString(json);
  Set result;

  int len = list.length();

  for (int i = 0; i < len; i++) {
    ValueType type = list.typeAt(i);
    switch (type) {
    case TYPE_BOOL:
      result.put(list.at<bool>(i));
      break;
    case TYPE_INT:
      result.put(list.at<int>(i));
      break;
    case TYPE_FLOAT:
      result.put(list.at<float>(i));
      break;
    case TYPE_STRING:
      result.put(list.at<String>(i));
      break;
    case TYPE_LIST:
      result.put(list.at<List>(i));
      break;
    case TYPE_MAP:
      result.put(list.at<Map>(i));
      break;
    case TYPE_SET:
      result.put(list.at<Set>(i));
      break;
    default:
      break;
    }
  }

  return result;
}

} // namespace attoboy
