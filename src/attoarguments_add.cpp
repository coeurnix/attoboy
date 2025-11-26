#include "attoarguments_internal.h"

namespace attoboy {

Arguments &Arguments::addFlag(const String &name, const String &description,
                               bool defaultValue, const String &longName) {
  if (!impl)
    return *this;

  WriteLockGuard guard(&impl->lock);

  for (int i = 0; i < impl->argDefs->length(); i++) {
    Map existingDef = impl->argDefs->at<Map>(i);
    String existingName = existingDef.get<String, String>("name");
    if (existingName == name) {
      impl->argDefs->remove(i);
      break;
    }
  }

  Map argDef;
  argDef.put("name", name);
  argDef.put("description", description);
  argDef.put("longName", longName);
  argDef.put("defaultValue", String(defaultValue));
  argDef.put("type", (int)ARG_FLAG);
  argDef.put("required", false);

  impl->argDefs->append(argDef);

  return *this;
}

Arguments &Arguments::addParameter(const String &name,
                                    const String &description,
                                    const String &defaultValue,
                                    const String &longName) {
  if (!impl)
    return *this;

  WriteLockGuard guard(&impl->lock);

  for (int i = 0; i < impl->argDefs->length(); i++) {
    Map existingDef = impl->argDefs->at<Map>(i);
    String existingName = existingDef.get<String, String>("name");
    if (existingName == name) {
      impl->argDefs->remove(i);
      break;
    }
  }

  Map argDef;
  argDef.put("name", name);
  argDef.put("description", description);
  argDef.put("longName", longName);
  argDef.put("defaultValue", defaultValue);
  argDef.put("type", (int)ARG_PARAMETER);
  argDef.put("required", false);

  impl->argDefs->append(argDef);

  return *this;
}

Arguments &Arguments::addPositionalParameter(const String &name,
                                              const String &description) {
  if (!impl)
    return *this;

  WriteLockGuard guard(&impl->lock);

  for (int i = 0; i < impl->argDefs->length(); i++) {
    Map existingDef = impl->argDefs->at<Map>(i);
    String existingName = existingDef.get<String, String>("name");
    if (existingName == name) {
      impl->argDefs->remove(i);
      break;
    }
  }

  Map argDef;
  argDef.put("name", name);
  argDef.put("description", description);
  argDef.put("longName", String());
  argDef.put("defaultValue", String());
  argDef.put("type", (int)ARG_POSITIONAL);
  argDef.put("required", false);

  impl->argDefs->append(argDef);

  return *this;
}

Arguments &Arguments::setHelp(const String &helpText) {
  if (!impl)
    return *this;

  WriteLockGuard guard(&impl->lock);
  *impl->helpText = helpText;

  return *this;
}

Arguments &Arguments::requireArgument(const String &name) {
  if (!impl)
    return *this;

  WriteLockGuard guard(&impl->lock);

  for (int i = 0; i < impl->argDefs->length(); i++) {
    Map argDef = impl->argDefs->at<Map>(i);
    String argName = argDef.get<String, String>("name");
    if (argName == name) {
      argDef.put("required", true);
      impl->argDefs->set(i, argDef);
      break;
    }
  }

  return *this;
}

} // namespace attoboy
