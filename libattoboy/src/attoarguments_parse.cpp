#include "attoarguments_internal.h"

namespace attoboy {

static bool IsBoolValue(const String &value, bool *outValue) {
  String lower = value.lower();
  if (lower == ATTO_TEXT("true") || lower == ATTO_TEXT("1") ||
      lower == ATTO_TEXT("on") || lower == ATTO_TEXT("yes")) {
    *outValue = true;
    return true;
  }
  if (lower == ATTO_TEXT("false") || lower == ATTO_TEXT("0") ||
      lower == ATTO_TEXT("off") || lower == ATTO_TEXT("no")) {
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
    String name = argDef.get<String, String>(ATTO_TEXT("name"));
    String defaultValue = argDef.get<String, String>(ATTO_TEXT("defaultValue"));
    int type = argDef.get<String, int>(ATTO_TEXT("type"));

    if (type == ARG_FLAG) {
      results.put(name, defaultValue);
    } else if (type == ARG_PARAMETER && !defaultValue.isEmpty()) {
      results.put(name, defaultValue);
    }
  }

  for (int i = 1; i < impl->cmdLineArgs->length(); i++) {
    String arg = impl->cmdLineArgs->at<String>(i);

    if (arg == ATTO_TEXT("-h") || arg == ATTO_TEXT("--help")) {
      helpRequested = true;
      continue;
    }

    if (arg.startsWith(ATTO_TEXT("--"))) {
      String argBody = arg.substring(2);
      int eqPos = -1;
      for (int j = 0; j < argBody.length(); j++) {
        if (argBody.at(j) == ATTO_TEXT("=")) {
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
        String defName = argDef.get<String, String>(ATTO_TEXT("name"));
        String defLongName = argDef.get<String, String>(ATTO_TEXT("longName"));
        int type = argDef.get<String, int>(ATTO_TEXT("type"));

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
              if (!nextArg.startsWith(ATTO_TEXT("-"))) {
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

      if (!found && argName == ATTO_TEXT("help")) {
        helpRequested = true;
      }

    } else if (arg.startsWith(ATTO_TEXT("-"))) {
      String argBody = arg.substring(1);
      int eqPos = -1;
      for (int j = 0; j < argBody.length(); j++) {
        if (argBody.at(j) == ATTO_TEXT("=")) {
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
        String defName = argDef.get<String, String>(ATTO_TEXT("name"));
        int type = argDef.get<String, int>(ATTO_TEXT("type"));

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
              if (!nextArg.startsWith(ATTO_TEXT("-"))) {
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

      if (!found && argName == ATTO_TEXT("h")) {
        helpRequested = true;
      }

    } else {
      positionalArgs.append(arg);
    }
  }

  int positionalIndex = 0;
  for (int i = 0; i < impl->argDefs->length(); i++) {
    Map argDef = impl->argDefs->at<Map>(i);
    int type = argDef.get<String, int>(ATTO_TEXT("type"));

    if (type == ARG_POSITIONAL) {
      String name = argDef.get<String, String>(ATTO_TEXT("name"));
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
    bool required = argDef.get<String, bool>(ATTO_TEXT("required"));
    if (required) {
      String name = argDef.get<String, String>(ATTO_TEXT("name"));
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
        const ATTO_CHAR *str = impl->helpText->c_str();
#ifdef UNICODE
        int len = lstrlenW(str);
        WriteFile(hStdOut, str, len * sizeof(wchar_t), &written, nullptr);
        WriteFile(hStdOut, L"\r\n\r\n", 4 * sizeof(wchar_t), &written, nullptr);
#else
        int len = lstrlenA(str);
        WriteFile(hStdOut, str, len, &written, nullptr);
        WriteFile(hStdOut, "\r\n\r\n", 4, &written, nullptr);
#endif
      }

      if (impl->argDefs->length() > 0) {
#ifdef UNICODE
        WriteFile(hStdOut, L"Options:\r\n", 10 * sizeof(wchar_t), &written,
                  nullptr);
#else
        WriteFile(hStdOut, "Options:\r\n", 10, &written, nullptr);
#endif

        for (int i = 0; i < impl->argDefs->length(); i++) {
          Map argDef = impl->argDefs->at<Map>(i);
          String name = argDef.get<String, String>(ATTO_TEXT("name"));
          String longName = argDef.get<String, String>(ATTO_TEXT("longName"));
          String description =
              argDef.get<String, String>(ATTO_TEXT("description"));
          int type = argDef.get<String, int>(ATTO_TEXT("type"));

          String line;
          if (type == ARG_POSITIONAL) {
            line = String(ATTO_TEXT("  "), name);
          } else {
            line = String(ATTO_TEXT("  -"), name);
            if (!longName.isEmpty()) {
              line = String(line, ATTO_TEXT(", --"), longName);
            }
          }

          if (!description.isEmpty()) {
            while (line.length() < 25) {
              line = String(line, ATTO_TEXT(" "));
            }
            line = String(line, ATTO_TEXT("  "), description);
          }

          line = String(line, ATTO_TEXT("\r\n"));
          const ATTO_CHAR *lineStr = line.c_str();
#ifdef UNICODE
          WriteFile(hStdOut, lineStr, lstrlenW(lineStr) * sizeof(wchar_t),
                    &written, nullptr);
#else
          WriteFile(hStdOut, lineStr, lstrlenA(lineStr), &written, nullptr);
#endif
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
