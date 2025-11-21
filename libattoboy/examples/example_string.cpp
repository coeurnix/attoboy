#include "attoboy/attoboy.h"

using namespace attoboy;

void atto_main() {

  Log("=== Attoboy String Class Demo ===\n");

  // Creating strings from various types
  Log("1. Creating strings:");
  String greeting("Hello");
  String world(L"World");
  String answer(42);
  String pi(3.14159f);
  String ready(true);

  Log("  Text: ", greeting);
  Log("  Wide: ", world);
  Log("  Number: ", answer);
  Log("  Float: ", pi);
  Log("  Boolean: ", ready, "\n");

  // Basic string operations
  Log("2. Concatenation and chaining:");
  String message = greeting.duplicate();
  message.append(" ").append(world).append("!");
  Log("  ", message);

  message.upper().replace("WORLD", "ATTOBOY");
  Log("  Uppercase + replace: ", message, "\n");

  // String analysis
  Log("3. String analysis:");
  String text("The quick brown fox jumps over the lazy dog");
  Log("  Text: ", text);
  Log("  Length: ", text.length());
  Log("  Contains 'fox': ", text.contains("fox"));
  Log("  Starts with 'The': ", text.startsWith("The"));
  Log("  Ends with 'dog': ", text.endsWith("dog"));
  Log("  Count 'o': ", text.count("o"), "\n");

  // Extracting substrings
  Log("4. Substring operations:");
  String sample("Programming");
  Log("  Original: ", sample);
  Log("  First 4 chars: ", sample.substring(0, 4));
  Log("  Last 4 chars: ", sample.substring(-4, -1));
  Log("  Character at index 0: ", sample.at(0));
  Log("  Character at index -1: ", sample.at(-1), "\n");

  // Transformations
  Log("5. String transformations:");
  String transform("Hello World");
  Log("  Original: ", transform);

  String lower = transform.duplicate();
  lower.lower();
  Log("  Lowercase: ", lower);

  String upper = transform.duplicate();
  upper.upper();
  Log("  Uppercase: ", upper);

  String reversed = transform.duplicate();
  reversed.reverse();
  Log("  Reversed: ", reversed, "\n");

  // Practical example: formatting user input
  Log("6. Practical example - cleaning input:");
  String userInput("  john.doe@example.com  ");
  Log("  Raw input: '", userInput, "'");
  userInput.trim().lower();
  Log("  Cleaned: '", userInput, "'");

  if (userInput.contains("@") && userInput.contains(".")) {
    Log("  Valid email format!");
  } else {
    Log("  Invalid email format!");
  }
  Log("");

  // Number parsing
  Log("7. Number parsing:");
  String num1("12345");
  String num2("3.14159");
  String num3("not a number");

  Log("  '", num1, "' is number: ", num1.isNumber(), " -> ", num1.toInteger());
  Log("  '", num2, "' is number: ", num2.isNumber(), " -> ", num2.toFloat());
  Log("  '", num3, "' is number: ", num3.isNumber(), "\n");

  // String manipulation
  Log("8. Advanced manipulation:");
  String builder("X");
  builder.repeat(3);
  Log("  Repeat 'X' 3 times: ", builder);

  builder.prepend("[").append("]");
  Log("  Add brackets: ", builder);

  builder.insert(4, "-");
  Log("  Insert '-' at index 4: ", builder, "\n");

  // Method chaining
  Log("9. Method chaining:");
  String chain("  hello attoboy  ");
  Log("  Original: '", chain, "'");
  chain.trim().upper().replace(" ", "_").append("!");
  Log("  After trim().upper().replace().append(): '", chain, "'");

  Log("\n=== Demo Complete ===");
  Exit(0);
}
