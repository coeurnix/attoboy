#include "attolist_internal.h"
#include "attomap_internal.h"
#include "attoset_internal.h"
#include "attostring_internal.h"

namespace attoboy {

static String EscapeCsvField(const String &field) {
  bool needsQuotes = false;

  if (field.contains(",") || field.contains("\"") ||
      field.contains("\n") || field.contains("\r")) {
    needsQuotes = true;
  }

  if (!needsQuotes) {
    return field;
  }

  String result("\"");
  result = result.append(field.replace("\"", "\"\""));
  result = result.append("\"");
  return result;
}

static String ValueToCsvString(ValueType type, const List &list, int index) {
  switch (type) {
  case TYPE_BOOL:
    return String(list.at<bool>(index));
  case TYPE_INT:
    return String(list.at<int>(index));
  case TYPE_FLOAT:
    return String(list.at<float>(index));
  case TYPE_STRING:
    return EscapeCsvField(list.at<String>(index));
  case TYPE_LIST:
    return EscapeCsvField(String(list.at<List>(index)));
  case TYPE_MAP:
    return EscapeCsvField(String(list.at<Map>(index)));
  case TYPE_SET:
    return EscapeCsvField(String(list.at<Set>(index)));
  case TYPE_NULL:
  default:
    return String("");
  }
}

String List::toCSVString() const {
  if (!impl) {
    return String();
  }

  ReadLockGuard guard(&impl->lock);

  String result;
  int rowCount = impl->size;

  for (int i = 0; i < rowCount; i++) {
    if (impl->items[i].type != TYPE_LIST) {
      continue;
    }

    List *row = impl->items[i].listVal;
    if (!row || !row->impl) {
      continue;
    }

    ReadLockGuard rowGuard(&row->impl->lock);
    int colCount = row->impl->size;

    for (int j = 0; j < colCount; j++) {
      ValueType type = row->typeAt(j);
      result = result.append(ValueToCsvString(type, *row, j));

      if (j < colCount - 1) {
        result = result.append(",");
      }
    }

    if (i < rowCount - 1) {
      result = result.append("\r\n");
    }
  }

  return result;
}

static List ParseCsvLine(const String &line) {
  List row;

  if (line.isEmpty()) {
    return row;
  }

  int len = line.length();
  String currentField;
  bool inQuotes = false;
  int i = 0;

  while (i < len) {
    String ch = line.at(i);

    if (inQuotes) {
      if (ch.equals("\"")) {
        if (i + 1 < len && line.at(i + 1).equals("\"")) {
          currentField = currentField.append("\"");
          i += 2;
          continue;
        } else {
          inQuotes = false;
          i++;
          continue;
        }
      } else {
        currentField = currentField.append(ch);
        i++;
      }
    } else {
      if (ch.equals("\"")) {
        inQuotes = true;
        i++;
      } else if (ch.equals(",")) {
        row.append(currentField);
        currentField = String();
        i++;
      } else {
        currentField = currentField.append(ch);
        i++;
      }
    }
  }

  row.append(currentField);
  return row;
}

List List::FromCSVString(const String &csv) {
  List result;

  if (csv.isEmpty()) {
    return result;
  }

  List lines = csv.lines();
  int lineCount = lines.length();

  for (int i = 0; i < lineCount; i++) {
    String line = lines.at<String>(i);

    if (line.isEmpty()) {
      continue;
    }

    List row = ParseCsvLine(line);
    result.append(row);
  }

  return result;
}

} // namespace attoboy
