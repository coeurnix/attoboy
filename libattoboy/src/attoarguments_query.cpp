#include "attoarguments_internal.h"

namespace attoboy {

String Arguments::getArgument(const String &name) const {
  if (!impl)
    return String();

  ReadLockGuard guard(&impl->lock);

  for (int i = 0; i < impl->argDefs->length(); i++) {
    Map argDef = impl->argDefs->at<Map>(i);
    String argName = argDef.get<String, String>("name");
    if (argName == name) {
      return argDef.get<String, String>("defaultValue");
    }
  }

  return String();
}

bool Arguments::hasArgument(const String &name) const {
  if (!impl)
    return false;

  ReadLockGuard guard(&impl->lock);

  for (int i = 0; i < impl->argDefs->length(); i++) {
    Map argDef = impl->argDefs->at<Map>(i);
    String argName = argDef.get<String, String>("name");
    if (argName == name) {
      String defaultValue =
          argDef.get<String, String>("defaultValue");
      return !defaultValue.isEmpty();
    }
  }

  return false;
}

} // namespace attoboy
