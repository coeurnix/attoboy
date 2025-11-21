#include "attoboy/attoboy.h"

using namespace attoboy;

void atto_main() {

  Log(ATTO_TEXT("=== Attoboy String Class Demo ===\n"));

  // Creating strings from various types
  Log(ATTO_TEXT("1. Creating strings:"));
  String greeting(ATTO_TEXT("Hello"));
  String world(ATTO_TEXT("World"));
  String answer(42);
  String pi(3.14159f);
  String ready(true);

  Log(ATTO_TEXT("  Text: "), greeting);
  Log(ATTO_TEXT("  Wide: "), world);
  Log(ATTO_TEXT("  Number: "), answer);
  Log(ATTO_TEXT("  Float: "), pi);
  Log(ATTO_TEXT("  Boolean: "), ready, ATTO_TEXT("\n"));

  // Basic string operations
  Log(ATTO_TEXT("2. Concatenation and chaining:"));
  String message = greeting.duplicate();
  message.append(ATTO_TEXT(" ")).append(world).append(ATTO_TEXT("!"));
  Log(ATTO_TEXT("  "), message);

  message.upper().replace(ATTO_TEXT("WORLD"), ATTO_TEXT("ATTOBOY"));
  Log(ATTO_TEXT("  Uppercase + replace: "), message, ATTO_TEXT("\n"));

  // String analysis
  Log(ATTO_TEXT("3. String analysis:"));
  String text(ATTO_TEXT("The quick brown fox jumps over the lazy dog"));
  Log(ATTO_TEXT("  Text: "), text);
  Log(ATTO_TEXT("  Length: "), text.length());
  Log(ATTO_TEXT("  Contains 'fox': "), text.contains(ATTO_TEXT("fox")));
  Log(ATTO_TEXT("  Starts with 'The': "), text.startsWith(ATTO_TEXT("The")));
  Log(ATTO_TEXT("  Ends with 'dog': "), text.endsWith(ATTO_TEXT("dog")));
  Log(ATTO_TEXT("  Count 'o': "), text.count(ATTO_TEXT("o")), ATTO_TEXT("\n"));

  // Extracting substrings
  Log(ATTO_TEXT("4. Substring operations:"));
  String sample(ATTO_TEXT("Programming"));
  Log(ATTO_TEXT("  Original: "), sample);
  Log(ATTO_TEXT("  First 4 chars: "), sample.substring(0, 4));
  Log(ATTO_TEXT("  Last 4 chars: "), sample.substring(-4, -1));
  Log(ATTO_TEXT("  Character at index 0: "), sample.at(0));
  Log(ATTO_TEXT("  Character at index -1: "), sample.at(-1), ATTO_TEXT("\n"));

  // Transformations
  Log(ATTO_TEXT("5. String transformations:"));
  String transform(ATTO_TEXT("Hello World"));
  Log(ATTO_TEXT("  Original: "), transform);

  String lower = transform.duplicate();
  lower.lower();
  Log(ATTO_TEXT("  Lowercase: "), lower);

  String upper = transform.duplicate();
  upper.upper();
  Log(ATTO_TEXT("  Uppercase: "), upper);

  String reversed = transform.duplicate();
  reversed.reverse();
  Log(ATTO_TEXT("  Reversed: "), reversed, ATTO_TEXT("\n"));

  // Practical example: formatting user input
  Log(ATTO_TEXT("6. Practical example - cleaning input:"));
  String userInput(ATTO_TEXT("  john.doe@example.com  "));
  Log(ATTO_TEXT("  Raw input: '"), userInput, ATTO_TEXT("'"));
  userInput.trim().lower();
  Log(ATTO_TEXT("  Cleaned: '"), userInput, ATTO_TEXT("'"));

  if (userInput.contains(ATTO_TEXT("@")) && userInput.contains(ATTO_TEXT("."))) {
    Log(ATTO_TEXT("  Valid email format!"));
  } else {
    Log(ATTO_TEXT("  Invalid email format!"));
  }
  Log(ATTO_TEXT(""));

  // Number parsing
  Log(ATTO_TEXT("7. Number parsing:"));
  String num1(ATTO_TEXT("12345"));
  String num2(ATTO_TEXT("3.14159"));
  String num3(ATTO_TEXT("not a number"));

  Log(ATTO_TEXT("  '"), num1, ATTO_TEXT("' is number: "), num1.isNumber(), ATTO_TEXT(" -> "), num1.toInteger());
  Log(ATTO_TEXT("  '"), num2, ATTO_TEXT("' is number: "), num2.isNumber(), ATTO_TEXT(" -> "), num2.toFloat());
  Log(ATTO_TEXT("  '"), num3, ATTO_TEXT("' is number: "), num3.isNumber(), ATTO_TEXT("\n"));

  // String manipulation
  Log(ATTO_TEXT("8. Advanced manipulation:"));
  String builder(ATTO_TEXT("X"));
  builder.repeat(3);
  Log(ATTO_TEXT("  Repeat 'X' 3 times: "), builder);

  builder.prepend(ATTO_TEXT("[")).append(ATTO_TEXT("]"));
  Log(ATTO_TEXT("  Add brackets: "), builder);

  builder.insert(4, ATTO_TEXT("-"));
  Log(ATTO_TEXT("  Insert '-' at index 4: "), builder, ATTO_TEXT("\n"));

  // Method chaining
  Log(ATTO_TEXT("9. Method chaining:"));
  String chain(ATTO_TEXT("  hello attoboy  "));
  Log(ATTO_TEXT("  Original: '"), chain, ATTO_TEXT("'"));
  chain.trim().upper().replace(ATTO_TEXT(" "), ATTO_TEXT("_")).append(ATTO_TEXT("!"));
  Log(ATTO_TEXT("  After trim().upper().replace().append(): '"), chain, ATTO_TEXT("'"));

  Log(ATTO_TEXT("\n=== Demo Complete ==="));
  Exit(0);
}
