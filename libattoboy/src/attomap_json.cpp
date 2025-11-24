#include "attomap_internal.h"
#include "attolist_internal.h"
#include "attostring_internal.h"

namespace attoboy {

String Map::toJSONString() const { return String(*this); }

extern Map ParseJsonObject(const String &json, int &pos);

Map Map::FromJSONString(const String &json) {
  int pos = 0;
  return ParseJsonObject(json, pos);
}

} // namespace attoboy
