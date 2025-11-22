#include "attoarguments_internal.h"

namespace attoboy {

Arguments &Arguments::addFlag(const String &name, const String &description,
                               bool defaultValue, const String &longName) {
  if (!impl)
    return *this;

  WriteLockGuard guard(&impl->lock);

  for (int i = 0; i < impl->argDefs->length(); i++) {
    Map existingDef = impl->argDefs->at<Map>(i);
    String existingName = existingDef.get<String, String>(ATTO_TEXT("name"));
    if (existingName == name) {
      impl->argDefs->remove(i);
      break;
    }
  }

  Map argDef;
  argDef.put(ATTO_TEXT("name"), name);
  argDef.put(ATTO_TEXT("description"), description);
  argDef.put(ATTO_TEXT("longName"), longName);
  argDef.put(ATTO_TEXT("defaultValue"), String(defaultValue));
  argDef.put(ATTO_TEXT("type"), (int)ARG_FLAG);
  argDef.put(ATTO_TEXT("required"), false);

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
    String existingName = existingDef.get<String, String>(ATTO_TEXT("name"));
    if (existingName == name) {
      impl->argDefs->remove(i);
      break;
    }
  }

  Map argDef;
  argDef.put(ATTO_TEXT("name"), name);
  argDef.put(ATTO_TEXT("description"), description);
  argDef.put(ATTO_TEXT("longName"), longName);
  argDef.put(ATTO_TEXT("defaultValue"), defaultValue);
  argDef.put(ATTO_TEXT("type"), (int)ARG_PARAMETER);
  argDef.put(ATTO_TEXT("required"), false);

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
    String existingName = existingDef.get<String, String>(ATTO_TEXT("name"));
    if (existingName == name) {
      impl->argDefs->remove(i);
      break;
    }
  }

  Map argDef;
  argDef.put(ATTO_TEXT("name"), name);
  argDef.put(ATTO_TEXT("description"), description);
  argDef.put(ATTO_TEXT("longName"), String());
  argDef.put(ATTO_TEXT("defaultValue"), String());
  argDef.put(ATTO_TEXT("type"), (int)ARG_POSITIONAL);
  argDef.put(ATTO_TEXT("required"), false);

  impl->argDefs->append(argDef);

  return *this;
}

Arguments &Arguments::setHelp(const String &help) {
  if (!impl)
    return *this;

  WriteLockGuard guard(&impl->lock);
  *impl->helpText = help;

  return *this;
}

Arguments &Arguments::requireArgument(const String &name) {
  if (!impl)
    return *this;

  WriteLockGuard guard(&impl->lock);

  for (int i = 0; i < impl->argDefs->length(); i++) {
    Map argDef = impl->argDefs->at<Map>(i);
    String argName = argDef.get<String, String>(ATTO_TEXT("name"));
    if (argName == name) {
      argDef.put(ATTO_TEXT("required"), true);
      impl->argDefs->set(i, argDef);
      break;
    }
  }

  return *this;
}

} // namespace attoboy
