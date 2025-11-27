#include "test_framework.h"

void atto_main() {
  EnableLoggingToFile("test_string_comprehensive.log", true);
  Log("=== Comprehensive String Class Tests ===");

  // ========== CONSTRUCTORS ==========

  // Empty constructor
  {
    String s;
    REGISTER_TESTED(String_constructor_empty);
    ASSERT_TRUE(s.isEmpty());
    ASSERT_EQ(s.length(), 0);
    Log("String() [empty]: passed");
  }

  // C-string constructor
  {
    String s("hello");
    REGISTER_TESTED(String_constructor_cstr);
    ASSERT_FALSE(s.isEmpty());
    ASSERT_EQ(s.length(), 5);
    ASSERT_EQ(s, String("hello"));
    Log("String(const char*): passed");
  }

  // Bool constructor
  {
    String t(true);
    String f(false);
    REGISTER_TESTED(String_constructor_bool);
    ASSERT_EQ(t, String("true"));
    ASSERT_EQ(f, String("false"));
    Log("String(bool): passed");
  }

  // Char constructor
  {
    String s('a');
    REGISTER_TESTED(String_constructor_char);
    ASSERT_EQ(s, String("a"));
    Log("String(char): passed");
  }

  // Int constructor
  {
    String s(42);
    REGISTER_TESTED(String_constructor_int);
    ASSERT_EQ(s, String("42"));
    Log("String(int): passed");
  }

  // Long long constructor
  {
    String s(123456789LL);
    REGISTER_TESTED(String_constructor_longlong);
    ASSERT_EQ(s, String("123456789"));
    Log("String(long long): passed");
  }

  // Float constructor
  {
    String s(3.14f);
    REGISTER_TESTED(String_constructor_float);
    ASSERT_TRUE(s.contains(String("3.14")));
    Log("String(float): passed");
  }

  // List constructor
  {
    List list;
    list.append(1).append(2).append("three");
    String s(list);
    REGISTER_TESTED(String_constructor_list);
    ASSERT_TRUE(s.contains(String("[")));
    ASSERT_TRUE(s.contains(String("]")));
    Log("String(List): passed");
  }

  // Map constructor
  {
    Map map;
    map.put("key", "value");
    String s(map);
    REGISTER_TESTED(String_constructor_map);
    ASSERT_TRUE(s.contains(String("{")));
    ASSERT_TRUE(s.contains(String("}")));
    Log("String(Map): passed");
  }

  // Set constructor
  {
    Set set;
    set.put(1).put(2).put(3);
    String s(set);
    REGISTER_TESTED(String_constructor_set);
    ASSERT_TRUE(s.contains(String("[")));
    ASSERT_TRUE(s.contains(String("]")));
    Log("String(Set): passed");
  }

  // Variadic constructor
  {
    String s("Hello", " ", "World");
    REGISTER_TESTED(String_constructor_variadic);
    ASSERT_EQ(s, String("Hello World"));
    Log("String(variadic): passed");
  }

  // Copy constructor
  {
    String orig("original");
    String copy(orig);
    REGISTER_TESTED(String_constructor_copy);
    ASSERT_EQ(copy, orig);
    Log("String(const String&): passed");
  }

  // Destructor (implicitly tested by all the above)
  {
    REGISTER_TESTED(String_destructor);
    Log("~String(): passed (implicit)");
  }

  // Assignment operator
  {
    String s;
    s = String("assigned");
    REGISTER_TESTED(String_operator_assign);
    ASSERT_EQ(s, String("assigned"));
    Log("operator=: passed");
  }

  // ========== BASIC PROPERTIES ==========

  // length()
  {
    String s("12345");
    REGISTER_TESTED(String_length);
    ASSERT_EQ(s.length(), 5);
    Log("length(): passed");
  }

  // isEmpty()
  {
    String empty;
    String notEmpty("x");
    REGISTER_TESTED(String_isEmpty);
    ASSERT_TRUE(empty.isEmpty());
    ASSERT_FALSE(notEmpty.isEmpty());
    Log("isEmpty(): passed");
  }

  // c_str()
  {
    String s("test");
    REGISTER_TESTED(String_c_str);
    const char *cstr = s.c_str();
    ASSERT(cstr != nullptr);
    Log("c_str(): passed");
  }

  // ========== ACCESS & SLICING ==========

  // at()
  {
    String s("hello");
    REGISTER_TESTED(String_at);
    ASSERT_EQ(s.at(0), String("h"));
    ASSERT_EQ(s.at(4), String("o"));
    ASSERT_EQ(s.at(-1), String("o")); // Negative index
    ASSERT_TRUE(s.at(10).isEmpty());  // Out of bounds
    Log("at(): passed");
  }

  // substring()
  {
    String s("hello world");
    REGISTER_TESTED(String_substring);
    ASSERT_EQ(s.substring(0, 5), String("hello"));
    ASSERT_EQ(s.substring(6), String("world"));     // To end
    ASSERT_EQ(s.substring(6, -1), String("world")); // Explicit -1
    Log("substring(): passed");
  }

  // duplicate()
  {
    String s("original");
    String dup = s.duplicate();
    REGISTER_TESTED(String_duplicate);
    ASSERT_EQ(dup, s);
    Log("duplicate(): passed");
  }

  // ========== SEARCH & QUERY ==========

  // contains()
  {
    String s("hello world");
    REGISTER_TESTED(String_contains);
    ASSERT_TRUE(s.contains(String("world")));
    ASSERT_FALSE(s.contains(String("xyz")));
    Log("contains(): passed");
  }

  // startsWith()
  {
    String s("hello world");
    REGISTER_TESTED(String_startsWith);
    ASSERT_TRUE(s.startsWith(String("hello")));
    ASSERT_FALSE(s.startsWith(String("world")));
    Log("startsWith(): passed");
  }

  // endsWith()
  {
    String s("hello world");
    REGISTER_TESTED(String_endsWith);
    ASSERT_TRUE(s.endsWith(String("world")));
    ASSERT_FALSE(s.endsWith(String("hello")));
    Log("endsWith(): passed");
  }

  // count()
  {
    String s("aaabaaacaaa");
    REGISTER_TESTED(String_count);
    ASSERT_EQ(s.count(String("aaa")), 3); // Non-overlapping
    ASSERT_EQ(s.count(String("b")), 1);
    Log("count(): passed");
  }

  // getPositionOf()
  {
    String s("hello world");
    REGISTER_TESTED(String_getPositionOf);
    ASSERT_EQ(s.getPositionOf(String("world")), 6);
    ASSERT_EQ(s.getPositionOf(String("hello")), 0);
    ASSERT_EQ(s.getPositionOf(String("xyz")), -1); // Not found
    ASSERT_EQ(s.getPositionOf(String("")), 0);     // Empty string at 0
    ASSERT_EQ(s.getPositionOf(String(" ")), 5);    // Space between
    // UTF-8 test
    String utf8("héllo wörld");
    ASSERT_EQ(utf8.getPositionOf(String("wörld")),
              6); // h,e,l,l,o,  -> é is 1 char
    // "héllo " is h,é,l,l,o,space -> 6 chars
    Log("getPositionOf(): passed");
  }

  // ========== CONVERSIONS ==========

  // isNumber()
  {
    String valid("123");
    String validFloat("3.14");
    String invalid("abc");
    REGISTER_TESTED(String_isNumber);
    ASSERT_TRUE(valid.isNumber());
    ASSERT_TRUE(validFloat.isNumber());
    ASSERT_FALSE(invalid.isNumber());
    Log("isNumber(): passed");
  }

  // toInteger()
  {
    String s("42");
    REGISTER_TESTED(String_toInteger);
    ASSERT_EQ(s.toInteger(), 42);
    Log("toInteger(): passed");
  }

  // toFloat()
  {
    String s("3.14");
    REGISTER_TESTED(String_toFloat);
    float f = s.toFloat();
    ASSERT(f > 3.13f && f < 3.15f);
    Log("toFloat(): passed");
  }

  // toBool()
  {
    String t("true");
    String f("false");
    String one("1");
    REGISTER_TESTED(String_toBool);
    ASSERT_TRUE(t.toBool());
    ASSERT_FALSE(f.toBool());
    ASSERT_TRUE(one.toBool());
    Log("toBool(): passed");
  }

  // ========== MANIPULATION ==========

  // append()
  {
    String s("hello");
    String result = s.append(String(" world"));
    REGISTER_TESTED(String_append);
    ASSERT_EQ(result, String("hello world"));
    Log("append(): passed");
  }

  // prepend()
  {
    String s("world");
    String result = s.prepend(String("hello "));
    REGISTER_TESTED(String_prepend);
    ASSERT_EQ(result, String("hello world"));
    Log("prepend(): passed");
  }

  // insert()
  {
    String s("helloworld");
    String result = s.insert(5, String(" "));
    REGISTER_TESTED(String_insert);
    ASSERT_EQ(result, String("hello world"));
    Log("insert(): passed");
  }

  // remove()
  {
    String s("hello world");
    String result = s.remove(5, 11);
    REGISTER_TESTED(String_remove);
    ASSERT_EQ(result, String("hello"));
    Log("remove(): passed");
  }

  // replace()
  {
    String s("hello world");
    String result = s.replace(String("world"), String("there"));
    REGISTER_TESTED(String_replace);
    ASSERT_EQ(result, String("hello there"));
    Log("replace(): passed");
  }

  // trim()
  {
    String s("  hello  ");
    String result = s.trim();
    REGISTER_TESTED(String_trim);
    ASSERT_EQ(result, String("hello"));
    Log("trim(): passed");
  }

  // upper()
  {
    String s("hello");
    String result = s.upper();
    REGISTER_TESTED(String_upper);
    ASSERT_EQ(result, String("HELLO"));
    Log("upper(): passed");
  }

  // lower()
  {
    String s("HELLO");
    String result = s.lower();
    REGISTER_TESTED(String_lower);
    ASSERT_EQ(result, String("hello"));
    Log("lower(): passed");
  }

  // reverse()
  {
    String s("hello");
    String result = s.reverse();
    REGISTER_TESTED(String_reverse);
    ASSERT_EQ(result, String("olleh"));
    Log("reverse(): passed");
  }

  // repeat()
  {
    String s("ab");
    String result = s.repeat(3);
    REGISTER_TESTED(String_repeat);
    ASSERT_EQ(result, String("ababab"));
    Log("repeat(): passed");
  }

  // ========== COMPARISON ==========

  // equals()
  {
    String a("test");
    String b("test");
    String c("other");
    REGISTER_TESTED(String_equals);
    ASSERT_TRUE(a.equals(b));
    ASSERT_FALSE(a.equals(c));
    Log("equals(): passed");
  }

  // compare()
  {
    String a("apple");
    String b("banana");
    String c("apple");
    REGISTER_TESTED(String_compare);
    ASSERT(a.compare(b) < 0);   // apple < banana
    ASSERT(b.compare(a) > 0);   // banana > apple
    ASSERT_EQ(a.compare(c), 0); // apple == apple
    Log("compare(): passed");
  }

  // operator==
  {
    String a("test");
    String b("test");
    REGISTER_TESTED(String_operator_eq);
    ASSERT_TRUE(a == b);
    Log("operator==: passed");
  }

  // operator!=
  {
    String a("test");
    String b("other");
    REGISTER_TESTED(String_operator_ne);
    ASSERT_TRUE(a != b);
    Log("operator!=: passed");
  }

  // operator+
  {
    String a("hello");
    String b(" world");
    String result = a + b;
    REGISTER_TESTED(String_operator_plus);
    ASSERT_EQ(result, String("hello world"));
    Log("operator+: passed");
  }

  // ========== HASHING ==========

  // hash()
  {
    String s("test");
    int h = s.hash();
    REGISTER_TESTED(String_hash);
    ASSERT_NE(h, 0); // Should produce some hash
    Log("hash(): passed");
  }

  // ========== ADVANCED OPERATIONS ==========

  // lines()
  {
    String s("line1\nline2\nline3");
    List result = s.lines();
    REGISTER_TESTED(String_lines);
    ASSERT_EQ(result.length(), 3);
    ASSERT_EQ(result.at<String>(0), String("line1"));
    Log("lines(): passed");
  }

  // join()
  {
    List list;
    list.append("a").append("b").append("c");
    String sep(",");
    String result = sep.join(list);
    REGISTER_TESTED(String_join);
    ASSERT_EQ(result, String("a,b,c"));
    Log("join(): passed");
  }

  // split(separator)
  {
    String s("a,b,c");
    List result = s.split(String(","), 10);
    REGISTER_TESTED(String_split_separator);
    ASSERT_EQ(result.length(), 3);
    ASSERT_EQ(result.at<String>(0), String("a"));
    ASSERT_EQ(result.at<String>(1), String("b"));
    ASSERT_EQ(result.at<String>(2), String("c"));
    Log("split(separator): passed");
  }

  // split() [whitespace]
  {
    String s("one  two   three");
    List result = s.split();
    REGISTER_TESTED(String_split);
    ASSERT_EQ(result.length(), 3);
    ASSERT_EQ(result.at<String>(0), String("one"));
    ASSERT_EQ(result.at<String>(1), String("two"));
    ASSERT_EQ(result.at<String>(2), String("three"));
    Log("split() [whitespace]: passed");
  }

  // format(list)
  {
    String template_str("Hello {0}, you are {1} years old");
    List args;
    args.append("Alice").append(30);
    String result = template_str.format(args);
    REGISTER_TESTED(String_format_list);
    ASSERT_TRUE(result.contains(String("Alice")));
    ASSERT_TRUE(result.contains(String("30")));
    Log("format(list): passed");
  }

  // format(map)
  {
    String template_str("Hello {name}, you are {age} years old");
    Map args;
    args.put("name", "Bob");
    args.put("age", 25);
    String result = template_str.format(args);
    REGISTER_TESTED(String_format_map);
    ASSERT_TRUE(result.contains(String("Bob")));
    ASSERT_TRUE(result.contains(String("25")));
    Log("format(map): passed");
  }

  Log("=== All String Tests Passed ===");
  TestFramework::DisplayCoverage();
  TestFramework::WriteCoverageData("test_string_comprehensive");
  Exit(0);
}
