#include "test_framework.h"

void atto_main() {
    EnableLoggingToFile(ATTO_TEXT("test_string_comprehensive.log"));
    Log(ATTO_TEXT("=== Comprehensive String Class Tests ==="));

    // ========== CONSTRUCTORS ==========

    // Empty constructor
    {
        String s;
        REGISTER_TESTED(String_constructor_empty);
        ASSERT_TRUE(s.isEmpty());
        ASSERT_EQ(s.length(), 0);
        Log(ATTO_TEXT("String() [empty]: passed"));
    }

    // C-string constructor
    {
        String s(ATTO_TEXT("hello"));
        REGISTER_TESTED(String_constructor_cstr);
        ASSERT_FALSE(s.isEmpty());
        ASSERT_EQ(s.length(), 5);
        ASSERT_EQ(s, String(ATTO_TEXT("hello")));
        Log(ATTO_TEXT("String(const ATTO_CHAR*): passed"));
    }

    // Bool constructor
    {
        String t(true);
        String f(false);
        REGISTER_TESTED(String_constructor_bool);
        ASSERT_EQ(t, String(ATTO_TEXT("true")));
        ASSERT_EQ(f, String(ATTO_TEXT("false")));
        Log(ATTO_TEXT("String(bool): passed"));
    }

    // Int constructor
    {
        String s(42);
        REGISTER_TESTED(String_constructor_int);
        ASSERT_EQ(s, String(ATTO_TEXT("42")));
        Log(ATTO_TEXT("String(int): passed"));
    }

    // Long long constructor
    {
        String s(123456789LL);
        REGISTER_TESTED(String_constructor_longlong);
        ASSERT_EQ(s, String(ATTO_TEXT("123456789")));
        Log(ATTO_TEXT("String(long long): passed"));
    }

    // Float constructor
    {
        String s(3.14f);
        REGISTER_TESTED(String_constructor_float);
        ASSERT_TRUE(s.contains(String(ATTO_TEXT("3.14"))));
        Log(ATTO_TEXT("String(float): passed"));
    }

    // List constructor
    {
        List list;
        list.append(1).append(2).append(ATTO_TEXT("three"));
        String s(list);
        REGISTER_TESTED(String_constructor_list);
        ASSERT_TRUE(s.contains(String(ATTO_TEXT("["))));
        ASSERT_TRUE(s.contains(String(ATTO_TEXT("]"))));
        Log(ATTO_TEXT("String(List): passed"));
    }

    // Map constructor
    {
        Map map;
        map.put(ATTO_TEXT("key"), ATTO_TEXT("value"));
        String s(map);
        REGISTER_TESTED(String_constructor_map);
        ASSERT_TRUE(s.contains(String(ATTO_TEXT("{"))));
        ASSERT_TRUE(s.contains(String(ATTO_TEXT("}"))));
        Log(ATTO_TEXT("String(Map): passed"));
    }

    // Set constructor
    {
        Set set;
        set.put(1).put(2).put(3);
        String s(set);
        REGISTER_TESTED(String_constructor_set);
        ASSERT_TRUE(s.contains(String(ATTO_TEXT("["))));
        ASSERT_TRUE(s.contains(String(ATTO_TEXT("]"))));
        Log(ATTO_TEXT("String(Set): passed"));
    }

    // Variadic constructor
    {
        String s(ATTO_TEXT("Hello"), ATTO_TEXT(" "), ATTO_TEXT("World"));
        REGISTER_TESTED(String_constructor_variadic);
        ASSERT_EQ(s, String(ATTO_TEXT("Hello World")));
        Log(ATTO_TEXT("String(variadic): passed"));
    }

    // Copy constructor
    {
        String orig(ATTO_TEXT("original"));
        String copy(orig);
        REGISTER_TESTED(String_constructor_copy);
        ASSERT_EQ(copy, orig);
        Log(ATTO_TEXT("String(const String&): passed"));
    }

    // Destructor (implicitly tested by all the above)
    {
        REGISTER_TESTED(String_destructor);
        Log(ATTO_TEXT("~String(): passed (implicit)"));
    }

    // Assignment operator
    {
        String s;
        s = String(ATTO_TEXT("assigned"));
        REGISTER_TESTED(String_operator_assign);
        ASSERT_EQ(s, String(ATTO_TEXT("assigned")));
        Log(ATTO_TEXT("operator=: passed"));
    }

    // ========== BASIC PROPERTIES ==========

    // length()
    {
        String s(ATTO_TEXT("12345"));
        REGISTER_TESTED(String_length);
        ASSERT_EQ(s.length(), 5);
        Log(ATTO_TEXT("length(): passed"));
    }

    // isEmpty()
    {
        String empty;
        String notEmpty(ATTO_TEXT("x"));
        REGISTER_TESTED(String_isEmpty);
        ASSERT_TRUE(empty.isEmpty());
        ASSERT_FALSE(notEmpty.isEmpty());
        Log(ATTO_TEXT("isEmpty(): passed"));
    }

    // c_str()
    {
        String s(ATTO_TEXT("test"));
        REGISTER_TESTED(String_c_str);
        const ATTO_CHAR* cstr = s.c_str();
        ASSERT(cstr != nullptr);
        Log(ATTO_TEXT("c_str(): passed"));
    }

    // ========== ACCESS & SLICING ==========

    // at()
    {
        String s(ATTO_TEXT("hello"));
        REGISTER_TESTED(String_at);
        ASSERT_EQ(s.at(0), String(ATTO_TEXT("h")));
        ASSERT_EQ(s.at(4), String(ATTO_TEXT("o")));
        ASSERT_EQ(s.at(-1), String(ATTO_TEXT("o"))); // Negative index
        ASSERT_TRUE(s.at(10).isEmpty()); // Out of bounds
        Log(ATTO_TEXT("at(): passed"));
    }

    // substring()
    {
        String s(ATTO_TEXT("hello world"));
        REGISTER_TESTED(String_substring);
        ASSERT_EQ(s.substring(0, 5), String(ATTO_TEXT("hello")));
        ASSERT_EQ(s.substring(6), String(ATTO_TEXT("world"))); // To end
        ASSERT_EQ(s.substring(6, -1), String(ATTO_TEXT("world"))); // Explicit -1
        Log(ATTO_TEXT("substring(): passed"));
    }

    // duplicate()
    {
        String s(ATTO_TEXT("original"));
        String dup = s.duplicate();
        REGISTER_TESTED(String_duplicate);
        ASSERT_EQ(dup, s);
        Log(ATTO_TEXT("duplicate(): passed"));
    }

    // ========== SEARCH & QUERY ==========

    // contains()
    {
        String s(ATTO_TEXT("hello world"));
        REGISTER_TESTED(String_contains);
        ASSERT_TRUE(s.contains(String(ATTO_TEXT("world"))));
        ASSERT_FALSE(s.contains(String(ATTO_TEXT("xyz"))));
        Log(ATTO_TEXT("contains(): passed"));
    }

    // startsWith()
    {
        String s(ATTO_TEXT("hello world"));
        REGISTER_TESTED(String_startsWith);
        ASSERT_TRUE(s.startsWith(String(ATTO_TEXT("hello"))));
        ASSERT_FALSE(s.startsWith(String(ATTO_TEXT("world"))));
        Log(ATTO_TEXT("startsWith(): passed"));
    }

    // endsWith()
    {
        String s(ATTO_TEXT("hello world"));
        REGISTER_TESTED(String_endsWith);
        ASSERT_TRUE(s.endsWith(String(ATTO_TEXT("world"))));
        ASSERT_FALSE(s.endsWith(String(ATTO_TEXT("hello"))));
        Log(ATTO_TEXT("endsWith(): passed"));
    }

    // count()
    {
        String s(ATTO_TEXT("aaabaaacaaa"));
        REGISTER_TESTED(String_count);
        ASSERT_EQ(s.count(String(ATTO_TEXT("aaa"))), 3); // Non-overlapping
        ASSERT_EQ(s.count(String(ATTO_TEXT("b"))), 1);
        Log(ATTO_TEXT("count(): passed"));
    }

    // ========== CONVERSIONS ==========

    // isNumber()
    {
        String valid(ATTO_TEXT("123"));
        String validFloat(ATTO_TEXT("3.14"));
        String invalid(ATTO_TEXT("abc"));
        REGISTER_TESTED(String_isNumber);
        ASSERT_TRUE(valid.isNumber());
        ASSERT_TRUE(validFloat.isNumber());
        ASSERT_FALSE(invalid.isNumber());
        Log(ATTO_TEXT("isNumber(): passed"));
    }

    // toInteger()
    {
        String s(ATTO_TEXT("42"));
        REGISTER_TESTED(String_toInteger);
        ASSERT_EQ(s.toInteger(), 42);
        Log(ATTO_TEXT("toInteger(): passed"));
    }

    // toFloat()
    {
        String s(ATTO_TEXT("3.14"));
        REGISTER_TESTED(String_toFloat);
        float f = s.toFloat();
        ASSERT(f > 3.13f && f < 3.15f);
        Log(ATTO_TEXT("toFloat(): passed"));
    }

    // toBool()
    {
        String t(ATTO_TEXT("true"));
        String f(ATTO_TEXT("false"));
        String one(ATTO_TEXT("1"));
        REGISTER_TESTED(String_toBool);
        ASSERT_TRUE(t.toBool());
        ASSERT_FALSE(f.toBool());
        ASSERT_TRUE(one.toBool());
        Log(ATTO_TEXT("toBool(): passed"));
    }

    // ========== MANIPULATION ==========

    // append()
    {
        String s(ATTO_TEXT("hello"));
        String result = s.append(String(ATTO_TEXT(" world")));
        REGISTER_TESTED(String_append);
        ASSERT_EQ(result, String(ATTO_TEXT("hello world")));
        Log(ATTO_TEXT("append(): passed"));
    }

    // prepend()
    {
        String s(ATTO_TEXT("world"));
        String result = s.prepend(String(ATTO_TEXT("hello ")));
        REGISTER_TESTED(String_prepend);
        ASSERT_EQ(result, String(ATTO_TEXT("hello world")));
        Log(ATTO_TEXT("prepend(): passed"));
    }

    // insert()
    {
        String s(ATTO_TEXT("helloworld"));
        String result = s.insert(5, String(ATTO_TEXT(" ")));
        REGISTER_TESTED(String_insert);
        ASSERT_EQ(result, String(ATTO_TEXT("hello world")));
        Log(ATTO_TEXT("insert(): passed"));
    }

    // remove()
    {
        String s(ATTO_TEXT("hello world"));
        String result = s.remove(5, 11);
        REGISTER_TESTED(String_remove);
        ASSERT_EQ(result, String(ATTO_TEXT("hello")));
        Log(ATTO_TEXT("remove(): passed"));
    }

    // replace()
    {
        String s(ATTO_TEXT("hello world"));
        String result = s.replace(String(ATTO_TEXT("world")), String(ATTO_TEXT("there")));
        REGISTER_TESTED(String_replace);
        ASSERT_EQ(result, String(ATTO_TEXT("hello there")));
        Log(ATTO_TEXT("replace(): passed"));
    }

    // trim()
    {
        String s(ATTO_TEXT("  hello  "));
        String result = s.trim();
        REGISTER_TESTED(String_trim);
        ASSERT_EQ(result, String(ATTO_TEXT("hello")));
        Log(ATTO_TEXT("trim(): passed"));
    }

    // upper()
    {
        String s(ATTO_TEXT("hello"));
        String result = s.upper();
        REGISTER_TESTED(String_upper);
        ASSERT_EQ(result, String(ATTO_TEXT("HELLO")));
        Log(ATTO_TEXT("upper(): passed"));
    }

    // lower()
    {
        String s(ATTO_TEXT("HELLO"));
        String result = s.lower();
        REGISTER_TESTED(String_lower);
        ASSERT_EQ(result, String(ATTO_TEXT("hello")));
        Log(ATTO_TEXT("lower(): passed"));
    }

    // reverse()
    {
        String s(ATTO_TEXT("hello"));
        String result = s.reverse();
        REGISTER_TESTED(String_reverse);
        ASSERT_EQ(result, String(ATTO_TEXT("olleh")));
        Log(ATTO_TEXT("reverse(): passed"));
    }

    // repeat()
    {
        String s(ATTO_TEXT("ab"));
        String result = s.repeat(3);
        REGISTER_TESTED(String_repeat);
        ASSERT_EQ(result, String(ATTO_TEXT("ababab")));
        Log(ATTO_TEXT("repeat(): passed"));
    }

    // ========== COMPARISON ==========

    // equals()
    {
        String a(ATTO_TEXT("test"));
        String b(ATTO_TEXT("test"));
        String c(ATTO_TEXT("other"));
        REGISTER_TESTED(String_equals);
        ASSERT_TRUE(a.equals(b));
        ASSERT_FALSE(a.equals(c));
        Log(ATTO_TEXT("equals(): passed"));
    }

    // compare()
    {
        String a(ATTO_TEXT("apple"));
        String b(ATTO_TEXT("banana"));
        String c(ATTO_TEXT("apple"));
        REGISTER_TESTED(String_compare);
        ASSERT(a.compare(b) < 0); // apple < banana
        ASSERT(b.compare(a) > 0); // banana > apple
        ASSERT_EQ(a.compare(c), 0); // apple == apple
        Log(ATTO_TEXT("compare(): passed"));
    }

    // operator==
    {
        String a(ATTO_TEXT("test"));
        String b(ATTO_TEXT("test"));
        REGISTER_TESTED(String_operator_eq);
        ASSERT_TRUE(a == b);
        Log(ATTO_TEXT("operator==: passed"));
    }

    // operator!=
    {
        String a(ATTO_TEXT("test"));
        String b(ATTO_TEXT("other"));
        REGISTER_TESTED(String_operator_ne);
        ASSERT_TRUE(a != b);
        Log(ATTO_TEXT("operator!=: passed"));
    }

    // operator+
    {
        String a(ATTO_TEXT("hello"));
        String b(ATTO_TEXT(" world"));
        String result = a + b;
        REGISTER_TESTED(String_operator_plus);
        ASSERT_EQ(result, String(ATTO_TEXT("hello world")));
        Log(ATTO_TEXT("operator+: passed"));
    }

    // ========== HASHING ==========

    // hash()
    {
        String s(ATTO_TEXT("test"));
        int h = s.hash();
        REGISTER_TESTED(String_hash);
        ASSERT_NE(h, 0); // Should produce some hash
        Log(ATTO_TEXT("hash(): passed"));
    }

    // ========== ADVANCED OPERATIONS ==========

    // lines()
    {
        String s(ATTO_TEXT("line1\nline2\nline3"));
        List result = s.lines();
        REGISTER_TESTED(String_lines);
        ASSERT_EQ(result.length(), 3);
        ASSERT_EQ(result.at<String>(0), String(ATTO_TEXT("line1")));
        Log(ATTO_TEXT("lines(): passed"));
    }

    // join()
    {
        List list;
        list.append(ATTO_TEXT("a")).append(ATTO_TEXT("b")).append(ATTO_TEXT("c"));
        String sep(ATTO_TEXT(","));
        String result = sep.join(list);
        REGISTER_TESTED(String_join);
        ASSERT_EQ(result, String(ATTO_TEXT("a,b,c")));
        Log(ATTO_TEXT("join(): passed"));
    }

    // split(separator)
    {
        String s(ATTO_TEXT("a,b,c"));
        List result = s.split(String(ATTO_TEXT(",")), 10);
        REGISTER_TESTED(String_split_separator);
        ASSERT_EQ(result.length(), 3);
        ASSERT_EQ(result.at<String>(0), String(ATTO_TEXT("a")));
        ASSERT_EQ(result.at<String>(1), String(ATTO_TEXT("b")));
        ASSERT_EQ(result.at<String>(2), String(ATTO_TEXT("c")));
        Log(ATTO_TEXT("split(separator): passed"));
    }

    // split() [whitespace]
    {
        String s(ATTO_TEXT("one  two   three"));
        List result = s.split();
        REGISTER_TESTED(String_split_whitespace);
        ASSERT_EQ(result.length(), 3);
        ASSERT_EQ(result.at<String>(0), String(ATTO_TEXT("one")));
        ASSERT_EQ(result.at<String>(1), String(ATTO_TEXT("two")));
        ASSERT_EQ(result.at<String>(2), String(ATTO_TEXT("three")));
        Log(ATTO_TEXT("split() [whitespace]: passed"));
    }

    // format(list)
    {
        String template_str(ATTO_TEXT("Hello {0}, you are {1} years old"));
        List args;
        args.append(ATTO_TEXT("Alice")).append(30);
        String result = template_str.format(args);
        REGISTER_TESTED(String_format_list);
        ASSERT_TRUE(result.contains(String(ATTO_TEXT("Alice"))));
        ASSERT_TRUE(result.contains(String(ATTO_TEXT("30"))));
        Log(ATTO_TEXT("format(list): passed"));
    }

    // format(map)
    {
        String template_str(ATTO_TEXT("Hello {name}, you are {age} years old"));
        Map args;
        args.put(ATTO_TEXT("name"), ATTO_TEXT("Bob"));
        args.put(ATTO_TEXT("age"), 25);
        String result = template_str.format(args);
        REGISTER_TESTED(String_format_map);
        ASSERT_TRUE(result.contains(String(ATTO_TEXT("Bob"))));
        ASSERT_TRUE(result.contains(String(ATTO_TEXT("25"))));
        Log(ATTO_TEXT("format(map): passed"));
    }

    Log(ATTO_TEXT("=== All String Tests Passed ==="));
    TestFramework::DisplayCoverage();
    TestFramework::WriteCoverageData(ATTO_TEXT("test_string_comprehensive"));
    Exit(0);
}
