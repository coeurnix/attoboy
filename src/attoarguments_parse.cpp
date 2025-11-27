#include "attoarguments_internal.h"
#include "attostring_internal.h"
#include <windows.h>

namespace attoboy {

static void PrintString(const String &s) {
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD written;
  WriteConsoleA(hOut, s.c_str(), s.byteLength(), &written, nullptr);
}

static bool IsBoolValue(const String &value, bool *outValue) {
  String lower = value.lower();
  if (lower == "true" || lower == "1" ||
      lower == "on" || lower == "yes") {
    *outValue = true;
    return true;
  }
  if (lower == "false" || lower == "0" ||
      lower == "off" || lower == "no") {
    *outValue = false;
    return true;
  }
  return false;
}

Map Arguments::parseArguments(bool suppressHelp) {
  if (!impl)
    return Map();

  WriteLockGuard guard(&impl->lock);

  Map results;
  List positionalArgs;
  bool helpRequested = false;

  for (int i = 0; i < impl->argDefs->length(); i++) {
    Map argDef = impl->argDefs->at<Map>(i);
    String name = argDef.get<String, String>("name");
    String defaultValue = argDef.get<String, String>("defaultValue");
    int type = argDef.get<String, int>("type");

    if (type == ARG_FLAG) {
      results.put(name, defaultValue);
    } else if (type == ARG_PARAMETER && !defaultValue.isEmpty()) {
      results.put(name, defaultValue);
    }
  }

  for (int i = 1; i < impl->cmdLineArgs->length(); i++) {
    String arg = impl->cmdLineArgs->at<String>(i);

    if (arg == "-h" || arg == "--help") {
      helpRequested = true;
      continue;
    }

    if (arg.startsWith("--")) {
      String argBody = arg.substring(2);
      int eqPos = -1;
      for (int j = 0; j < argBody.length(); j++) {
        if (argBody.at(j) == "=") {
          eqPos = j;
          break;
        }
      }

      String argName;
      String argValue;

      if (eqPos >= 0) {
        argName = argBody.substring(0, eqPos);
        argValue = argBody.substring(eqPos + 1);
      } else {
        argName = argBody;
      }

      bool found = false;
      for (int j = 0; j < impl->argDefs->length(); j++) {
        Map argDef = impl->argDefs->at<Map>(j);
        String defName = argDef.get<String, String>("name");
        String defLongName = argDef.get<String, String>("longName");
        int type = argDef.get<String, int>("type");

        if (defLongName == argName) {
          if (type == ARG_FLAG) {
            if (eqPos >= 0) {
              bool boolVal;
              if (IsBoolValue(argValue, &boolVal)) {
                results.put(defName, String(boolVal));
              }
            } else {
              results.put(defName, String(true));
            }
          } else if (type == ARG_PARAMETER) {
            String paramValue;
            bool hasValue = false;

            if (eqPos >= 0) {
              paramValue = argValue;
              hasValue = true;
            } else if (i + 1 < impl->cmdLineArgs->length()) {
              String nextArg = impl->cmdLineArgs->at<String>(i + 1);
              if (!nextArg.startsWith("-")) {
                paramValue = nextArg;
                hasValue = true;
                i++;
              }
            }

            if (hasValue) {
              results.put(defName, paramValue);
              found = true;
              break;
            }
          }
          if (found)
            break;
        }
      }

      if (!found && argName == "help") {
        helpRequested = true;
      }

    } else if (arg.startsWith("-")) {
      String argBody = arg.substring(1);
      int eqPos = -1;
      for (int j = 0; j < argBody.length(); j++) {
        if (argBody.at(j) == "=") {
          eqPos = j;
          break;
        }
      }

      String argName;
      String argValue;

      if (eqPos >= 0) {
        argName = argBody.substring(0, eqPos);
        argValue = argBody.substring(eqPos + 1);
      } else {
        argName = argBody;
      }

      bool found = false;
      for (int j = 0; j < impl->argDefs->length(); j++) {
        Map argDef = impl->argDefs->at<Map>(j);
        String defName = argDef.get<String, String>("name");
        int type = argDef.get<String, int>("type");

        if (defName == argName) {
          if (type == ARG_FLAG) {
            if (eqPos >= 0) {
              bool boolVal;
              if (IsBoolValue(argValue, &boolVal)) {
                results.put(defName, String(boolVal));
              }
            } else {
              results.put(defName, String(true));
            }
          } else if (type == ARG_PARAMETER) {
            String paramValue;
            bool hasValue = false;

            if (eqPos >= 0) {
              paramValue = argValue;
              hasValue = true;
            } else if (i + 1 < impl->cmdLineArgs->length()) {
              String nextArg = impl->cmdLineArgs->at<String>(i + 1);
              if (!nextArg.startsWith("-")) {
                paramValue = nextArg;
                hasValue = true;
                i++;
              }
            }

            if (hasValue) {
              results.put(defName, paramValue);
              found = true;
              break;
            }
          }
          if (found)
            break;
        }
      }

      if (!found && argName == "h") {
        helpRequested = true;
      }

    } else {
      positionalArgs.append(arg);
    }
  }

  int positionalIndex = 0;
  for (int i = 0; i < impl->argDefs->length(); i++) {
    Map argDef = impl->argDefs->at<Map>(i);
    int type = argDef.get<String, int>("type");

    if (type == ARG_POSITIONAL) {
      String name = argDef.get<String, String>("name");
      if (positionalIndex < positionalArgs.length()) {
        String value = positionalArgs.at<String>(positionalIndex);
        results.put(name, value);
        positionalIndex++;
      }
    }
  }

  bool missingRequired = false;
  for (int i = 0; i < impl->argDefs->length(); i++) {
    Map argDef = impl->argDefs->at<Map>(i);
    bool required = argDef.get<String, bool>("required");
    if (required) {
      String name = argDef.get<String, String>("name");
      if (!results.hasKey(name) ||
          results.get<String, String>(name).isEmpty()) {
        missingRequired = true;
        break;
      }
    }
  }

  if ((helpRequested || missingRequired) && !suppressHelp) {
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut != INVALID_HANDLE_VALUE) {
      DWORD written;

      if (!impl->helpText->isEmpty()) {
        PrintString(*(impl->helpText));
        PrintString(String("\r\n\r\n"));
      }

      if (impl->argDefs->length() > 0) {
        PrintString(String("Options:\r\n"));

        for (int i = 0; i < impl->argDefs->length(); i++) {
          Map argDef = impl->argDefs->at<Map>(i);
          String name = argDef.get<String, String>("name");
          String longName = argDef.get<String, String>("longName");
          String description =
              argDef.get<String, String>("description");
          int type = argDef.get<String, int>("type");

          String line;
          if (type == ARG_POSITIONAL) {
            line = String("  ") + name;
          } else {
            line = String("  -") + name;
            if (!longName.isEmpty()) {
              line = line + ", --" + longName;
            }
          }

          if (!description.isEmpty()) {
            while (line.length() < 25) {
              line = line + " ";
            }
            line = line + "  " + description;
          }

          line = line + "\r\n";
          PrintString(line);
        }
      }
    }
  }

  if (helpRequested || missingRequired) {
    return Map();
  }

  return results;
}

} // namespace attoboy
