#include "attoconsole_internal.h"

namespace attoboy {

int UTF8CharWidth(const char *str) {
  if (!str || !*str)
    return 0;
  unsigned char c = (unsigned char)*str;
  if (c < 0x80)
    return 1;
  if (c < 0xE0)
    return 2;
  if (c < 0xF0)
    return 3;
  return 4;
}

int UTF8StringWidth(const char *str) {
  int width = 0;
  while (*str) {
    int charWidth = UTF8CharWidth(str);
    unsigned char c0 = (unsigned char)str[0];
    if (charWidth == 4 && c0 == 0xF0) {
      width += 2;
    } else if (charWidth == 3 && c0 >= 0xE0) {
      unsigned char c1 = (charWidth > 1) ? (unsigned char)str[1] : 0;
      unsigned char c2 = (charWidth > 2) ? (unsigned char)str[2] : 0;
      if ((c0 == 0xE2 && c1 >= 0x80) || (c0 == 0xE3 && c1 == 0x80)) {
        width += 2;
      } else {
        width++;
      }
    } else {
      width++;
    }
    str += charWidth;
  }
  return width;
}

List SplitIntoWords(const String &text) {
  List words;
  String current = "";
  const char *str = text.c_str();

  while (*str) {
    if (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r') {
      if (!current.isEmpty()) {
        words.append(current);
        current = "";
      }
      if (*str == '\n') {
        words.append("\n");
      }
      str++;
    } else {
      int charWidth = UTF8CharWidth(str);
      current = current + String(str, charWidth);
      str += charWidth;
    }
  }

  if (!current.isEmpty()) {
    words.append(current);
  }

  return words;
}

String Console::Align(const String &text, int width, ConsoleAlign align) {
  if (width <= 0 || text.isEmpty())
    return text;

  List lines = text.lines();
  String result = "";

  for (int i = 0; i < lines.length(); i++) {
    String line = lines.at<String>(i);
    int len = UTF8StringWidth(line.c_str());

    if (len >= width) {
      result = result + line;
    } else {
      int padding = width - len;

      if (align == CON_ALIGN_LEFT) {
        result = result + line + String(" ").repeat(padding);
      } else if (align == CON_ALIGN_RIGHT) {
        result = result + String(" ").repeat(padding) + line;
      } else if (align == CON_ALIGN_CENTER) {
        int leftPad = padding / 2;
        int rightPad = padding - leftPad;
        result = result + String(" ").repeat(leftPad) + line +
                 String(" ").repeat(rightPad);
      } else if (align == CON_ALIGN_JUSTIFY) {
        List words = SplitIntoWords(line);
        int wordCount = 0;
        int totalWordLen = 0;
        for (int j = 0; j < words.length(); j++) {
          String word = words.at<String>(j);
          if (word != " " && word != "\n") {
            wordCount++;
            totalWordLen += UTF8StringWidth(word.c_str());
          }
        }
        if (wordCount <= 1) {
          result = result + line;
        } else {
          int gaps = wordCount - 1;
          int totalSpaces = width - totalWordLen;
          int spacePerGap = totalSpaces / gaps;
          int extraSpaces = totalSpaces % gaps;

          int gapIndex = 0;
          for (int j = 0; j < words.length(); j++) {
            String word = words.at<String>(j);
            if (word != " " && word != "\n") {
              result = result + word;
              if (gapIndex < gaps) {
                int spaceCount = spacePerGap;
                if (gapIndex < extraSpaces)
                  spaceCount++;
                result = result + String(" ").repeat(spaceCount);
                gapIndex++;
              }
            }
          }
        }
      }
    }

    if (i < lines.length() - 1) {
      result = result + "\n";
    }
  }

  return result;
}

String Console::Wrap(const String &text, int width) {
  if (width <= 0)
    return text;

  List lines = text.lines();
  String result = "";

  for (int lineIdx = 0; lineIdx < lines.length(); lineIdx++) {
    String line = lines.at<String>(lineIdx);
    List words = SplitIntoWords(line);

    String currentLine = "";
    int currentWidth = 0;

    for (int i = 0; i < words.length(); i++) {
      String word = words.at<String>(i);

      if (word == "\n") {
        result = result + currentLine + "\n";
        currentLine = "";
        currentWidth = 0;
        continue;
      }

      int wordLen = UTF8StringWidth(word.c_str());

      if (currentWidth == 0) {
        if (wordLen > width) {
          result = result + word + "\n";
        } else {
          currentLine = word;
          currentWidth = wordLen;
        }
      } else if (currentWidth + 1 + wordLen <= width) {
        currentLine = currentLine + " " + word;
        currentWidth += 1 + wordLen;
      } else {
        result = result + currentLine + "\n";
        if (wordLen > width) {
          result = result + word + "\n";
          currentLine = "";
          currentWidth = 0;
        } else {
          currentLine = word;
          currentWidth = wordLen;
        }
      }
    }

    if (!currentLine.isEmpty()) {
      result = result + currentLine;
    }

    if (lineIdx < lines.length() - 1) {
      result = result + "\n";
    }
  }

  return result;
}

} // namespace attoboy
