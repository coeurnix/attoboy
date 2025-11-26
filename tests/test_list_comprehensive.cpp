#include "test_framework.h"

void atto_main() {
    EnableLoggingToFile("test_list_comprehensive.log", true);
    Log("=== Comprehensive List Class Tests ===");

    // ========== CONSTRUCTORS ==========

    // Empty constructor
    {
        List l;
        REGISTER_TESTED(List_constructor_empty);
        ASSERT_TRUE(l.isEmpty());
        ASSERT_EQ(l.length(), 0);
        Log("List() [empty]: passed");
    }

    // Capacity constructor
    {
        List l(100);
        REGISTER_TESTED(List_constructor_capacity);
        ASSERT_TRUE(l.isEmpty());
        Log("List(capacity): passed");
    }

    // Variadic constructor
    {
        List l(1, "hello", 3.14f, true);
        REGISTER_TESTED(List_constructor_variadic);
        ASSERT_EQ(l.length(), 4);
        ASSERT_EQ(l.at<int>(0), 1);
        ASSERT_EQ(l.at<String>(1), String("hello"));
        Log("List(variadic): passed");
    }

    // Copy constructor
    {
        List orig;
        orig.append(1).append(2).append(3);
        List copy(orig);
        REGISTER_TESTED(List_constructor_copy);
        ASSERT_EQ(copy.length(), 3);
        ASSERT_EQ(copy.at<int>(0), 1);
        Log("List(copy): passed");
    }

    // From Set constructor
    {
        Set s;
        s.put(1).put(2).put(3);
        List l(s);
        REGISTER_TESTED(List_constructor_from_set);
        ASSERT_EQ(l.length(), 3);
        Log("List(Set): passed");
    }

    // Destructor (implicit)
    {
        REGISTER_TESTED(List_destructor);
        Log("~List(): passed (implicit)");
    }

    // Assignment operator
    {
        List l1;
        l1.append(1).append(2);
        List l2;
        l2 = l1;
        REGISTER_TESTED(List_operator_assign);
        ASSERT_EQ(l2.length(), 2);
        ASSERT_EQ(l2.at<int>(0), 1);
        Log("operator=: passed");
    }

    // ========== BASIC PROPERTIES ==========

    // length()
    {
        List l;
        l.append(1).append(2).append(3);
        REGISTER_TESTED(List_length);
        ASSERT_EQ(l.length(), 3);
        Log("length(): passed");
    }

    // isEmpty()
    {
        List empty;
        List notEmpty;
        notEmpty.append(1);
        REGISTER_TESTED(List_isEmpty);
        ASSERT_TRUE(empty.isEmpty());
        ASSERT_FALSE(notEmpty.isEmpty());
        Log("isEmpty(): passed");
    }

    // ========== MODIFICATION ==========

    // append()
    {
        List l;
        l.append(1).append("two").append(3.0f);
        REGISTER_TESTED(List_append);
        ASSERT_EQ(l.length(), 3);
        ASSERT_EQ(l.at<int>(0), 1);
        ASSERT_EQ(l.at<String>(1), String("two"));
        Log("append(): passed");
    }

    // prepend()
    {
        List l;
        l.append(2).prepend(1);
        REGISTER_TESTED(List_prepend);
        ASSERT_EQ(l.at<int>(0), 1);
        ASSERT_EQ(l.at<int>(1), 2);
        Log("prepend(): passed");
    }

    // insert()
    {
        List l;
        l.append(1).append(3).insert(1, 2);
        REGISTER_TESTED(List_insert);
        ASSERT_EQ(l.at<int>(1), 2);
        Log("insert(): passed");
    }

    // pop()
    {
        List l;
        l.append(1).append(2).append(3);
        int val = l.pop<int>();
        REGISTER_TESTED(List_pop);
        ASSERT_EQ(val, 3);
        ASSERT_EQ(l.length(), 2);
        Log("pop(): passed");
    }

    // at()
    {
        List l;
        l.append(10).append(20).append(30);
        REGISTER_TESTED(List_at);
        ASSERT_EQ(l.at<int>(1), 20);
        Log("at(): passed");
    }

    // operator[]
    {
        List l;
        l.append(100).append(200);
        REGISTER_TESTED(List_operator_brackets);
        int val = l.operator[]<int>(0);
        ASSERT_EQ(val, 100);
        Log("operator[]: passed");
    }

    // set()
    {
        List l;
        l.append(1).append(2).set(1, 99);
        REGISTER_TESTED(List_set);
        ASSERT_EQ(l.at<int>(1), 99);
        Log("set(): passed");
    }

    // remove()
    {
        List l;
        l.append(1).append(2).append(3).remove(1);
        REGISTER_TESTED(List_remove);
        ASSERT_EQ(l.length(), 2);
        ASSERT_EQ(l.at<int>(1), 3);
        Log("remove(): passed");
    }

    // clear()
    {
        List l;
        l.append(1).append(2).clear();
        REGISTER_TESTED(List_clear);
        ASSERT_TRUE(l.isEmpty());
        Log("clear(): passed");
    }

    // ========== SEARCH ==========

    // find()
    {
        List l;
        l.append(10).append(20).append(30);
        REGISTER_TESTED(List_find);
        ASSERT_EQ(l.find(20), 1);
        ASSERT_EQ(l.find(999), -1);
        Log("find(): passed");
    }

    // contains()
    {
        List l;
        l.append(1).append(2).append(3);
        REGISTER_TESTED(List_contains);
        ASSERT_TRUE(l.contains(2));
        ASSERT_FALSE(l.contains(99));
        Log("contains(): passed");
    }

    // ========== MANIPULATION ==========

    // reverse()
    {
        List l;
        l.append(1).append(2).append(3).reverse();
        REGISTER_TESTED(List_reverse);
        ASSERT_EQ(l.at<int>(0), 3);
        ASSERT_EQ(l.at<int>(2), 1);
        Log("reverse(): passed");
    }

    // sort()
    {
        List l;
        l.append(3).append(1).append(2).sort();
        REGISTER_TESTED(List_sort);
        ASSERT_EQ(l.at<int>(0), 1);
        ASSERT_EQ(l.at<int>(1), 2);
        ASSERT_EQ(l.at<int>(2), 3);
        Log("sort(): passed");
    }

    // typeAt()
    {
        List l;
        l.append(42).append("string");
        REGISTER_TESTED(List_typeAt);
        // Just verify it doesn't crash
        l.typeAt(0);
        l.typeAt(1);
        Log("typeAt(): passed");
    }

    // slice()
    {
        List l;
        l.append(1).append(2).append(3).append(4).append(5);
        List sliced = l.slice(1, 4);
        REGISTER_TESTED(List_slice);
        ASSERT_EQ(sliced.length(), 3);
        ASSERT_EQ(sliced.at<int>(0), 2);
        Log("slice(): passed");
    }

    // concat(list)
    {
        List l1;
        l1.append(1).append(2);
        List l2;
        l2.append(3).append(4);
        l1.concat(l2);
        REGISTER_TESTED(List_concat_list);
        ASSERT_EQ(l1.length(), 4);
        ASSERT_EQ(l1.at<int>(2), 3);
        Log("concat(List): passed");
    }

    // concat(set)
    {
        List l;
        l.append(1);
        Set s;
        s.put(2).put(3);
        l.concat(s);
        REGISTER_TESTED(List_concat_set);
        ASSERT_EQ(l.length(), 3);
        Log("concat(Set): passed");
    }

    // duplicate()
    {
        List orig;
        orig.append(1).append(2);
        List dup = orig.duplicate();
        REGISTER_TESTED(List_duplicate);
        ASSERT_EQ(dup.length(), 2);
        ASSERT_EQ(dup.at<int>(0), 1);
        Log("duplicate(): passed");
    }

    // ========== COMPARISON ==========

    // compare()
    {
        List l1;
        l1.append(1).append(2);
        List l2;
        l2.append(1).append(2);
        List l3;
        l3.append(1).append(3);
        REGISTER_TESTED(List_compare);
        ASSERT_TRUE(l1.compare(l2));
        ASSERT_FALSE(l1.compare(l3));
        Log("compare(): passed");
    }

    // operator==
    {
        List l1;
        l1.append(1);
        List l2;
        l2.append(1);
        REGISTER_TESTED(List_operator_eq);
        ASSERT_TRUE(l1 == l2);
        Log("operator==: passed");
    }

    // operator!=
    {
        List l1;
        l1.append(1);
        List l2;
        l2.append(2);
        REGISTER_TESTED(List_operator_ne);
        ASSERT_TRUE(l1 != l2);
        Log("operator!=: passed");
    }

    // operator+
    {
        List l;
        l.append(1);
        List result = l + 2;
        REGISTER_TESTED(List_operator_plus);
        ASSERT_EQ(result.length(), 2);
        ASSERT_EQ(result.at<int>(1), 2);
        Log("operator+: passed");
    }

    // ========== NESTED COLLECTIONS & JSON ==========

    Log("=== Testing Nested Collections & JSON ===");

    // Nested List in List
    {
        List inner;
        inner.append(1).append(2).append(3);
        List outer;
        outer.append("prefix").append(inner).append("suffix");

        ASSERT_EQ(outer.length(), 3);
        List retrieved = outer.at<List>(1);
        ASSERT_EQ(retrieved.length(), 3);
        ASSERT_EQ(retrieved.at<int>(0), 1);

        // Convert to JSON String
        String json(outer);
        ASSERT_TRUE(json.contains(String("[")));
        ASSERT_TRUE(json.contains(String("prefix")));
        Log("Nested List in List + JSON: passed");
    }

    // Map in List
    {
        Map m;
        m.put("key", "value");
        List l;
        l.append(1).append(m).append(3);

        Map retrieved = l.at<Map>(1);
        String val = retrieved.get<String,String>("key");
        ASSERT_EQ(val, String("value"));

        // Convert to JSON
        String json(l);
        ASSERT_TRUE(json.contains(String("{")));
        ASSERT_TRUE(json.contains(String("key")));
        Log("Map in List + JSON: passed");
    }

    // Set in List
    {
        Set s;
        s.put(10).put(20).put(30);
        List l;
        l.append(s);

        Set retrieved = l.at<Set>(0);
        ASSERT_TRUE(retrieved.contains(10));

        // Convert to JSON
        String json(l);
        ASSERT_TRUE(json.contains(String("[")));
        Log("Set in List + JSON: passed");
    }

    // Deeply nested structure
    {
        List deepInner;
        deepInner.append("deep");
        List middle;
        middle.append(deepInner);
        List outer;
        outer.append(middle);

        List retrieved = outer.at<List>(0).at<List>(0);
        ASSERT_EQ(retrieved.at<String>(0), String("deep"));

        // Convert to JSON
        String json(outer);
        ASSERT_TRUE(json.contains(String("deep")));
        Log("Deeply nested List + JSON: passed");
    }

    // ========== JSON/CSV FUNCTIONS ==========

    // List.toJSONString() basic
    {
        List l;
        l.append(1).append("hello").append(true);
        String json = l.toJSONString();
        REGISTER_TESTED(List_toJSONString);
        ASSERT_TRUE(json.contains("["));
        ASSERT_TRUE(json.contains("hello"));
        ASSERT_TRUE(json.contains("true"));
        Log("List.toJSONString(): passed");
    }

    // List.FromJSONString() basic
    {
        String json("[1,\"hello\",true,3.14]");
        List l = List::FromJSONString(json);
        REGISTER_TESTED(List_FromJSONString);
        ASSERT_EQ(l.length(), 4);
        ASSERT_EQ(l.at<int>(0), 1);
        ASSERT_EQ(l.at<String>(1), String("hello"));
        ASSERT_EQ(l.at<bool>(2), true);
        Log("List.FromJSONString(): passed");
    }

    // List.FromJSONString() nested
    {
        String json("[[1,2],[3,4]]");
        List l = List::FromJSONString(json);
        ASSERT_EQ(l.length(), 2);
        List inner = l.at<List>(0);
        ASSERT_EQ(inner.length(), 2);
        ASSERT_EQ(inner.at<int>(0), 1);
        ASSERT_EQ(inner.at<int>(1), 2);
        Log("List.FromJSONString() nested: passed");
    }

    // List.FromJSONString() with Map
    {
        String json("[{\"key\":\"value\"}]");
        List l = List::FromJSONString(json);
        ASSERT_EQ(l.length(), 1);
        Map m = l.at<Map>(0);
        String val = m.get<String,String>("key");
        ASSERT_EQ(val, String("value"));
        Log("List.FromJSONString() with Map: passed");
    }

    // List.toCSVString() basic
    {
        List row1;
        row1.append("Name").append("Age").append("City");
        List row2;
        row2.append("Alice").append(30).append("NYC");
        List csv;
        csv.append(row1).append(row2);
        String csvStr = csv.toCSVString();
        REGISTER_TESTED(List_toCSVString);
        ASSERT_TRUE(csvStr.contains("Name"));
        ASSERT_TRUE(csvStr.contains("Alice"));
        ASSERT_TRUE(csvStr.contains("30"));
        Log("List.toCSVString(): passed");
    }

    // List.FromCSVString() basic
    {
        String csvStr("Name,Age,City\r\nAlice,30,NYC\r\nBob,25,LA");
        List csv = List::FromCSVString(csvStr);
        REGISTER_TESTED(List_FromCSVString);
        ASSERT_EQ(csv.length(), 3);
        List row1 = csv.at<List>(0);
        ASSERT_EQ(row1.length(), 3);
        ASSERT_EQ(row1.at<String>(0), String("Name"));
        List row2 = csv.at<List>(1);
        ASSERT_EQ(row2.at<String>(0), String("Alice"));
        Log("List.FromCSVString(): passed");
    }

    // List.toCSVString() with quotes
    {
        List row1;
        row1.append("Quote \"test\"").append("Comma,test");
        List csv;
        csv.append(row1);
        String csvStr = csv.toCSVString();
        ASSERT_TRUE(csvStr.contains("\"\""));
        Log("List.toCSVString() with escapes: passed");
    }

    // List.FromCSVString() with quotes
    {
        String csvStr("\"Quote \"\"test\"\"\",\"Comma,test\"");
        List csv = List::FromCSVString(csvStr);
        ASSERT_EQ(csv.length(), 1);
        List row = csv.at<List>(0);
        ASSERT_EQ(row.length(), 2);
        String field1 = row.at<String>(0);
        ASSERT_TRUE(field1.contains("Quote"));
        Log("List.FromCSVString() with quotes: passed");
    }

    // List.toCSVString() with nested collections
    {
        List inner;
        inner.append(1).append(2).append(3);
        List row1;
        row1.append("data").append(inner);
        List csv;
        csv.append(row1);
        String csvStr = csv.toCSVString();
        ASSERT_TRUE(csvStr.contains("["));
        Log("List.toCSVString() with nested: passed");
    }

    // Round-trip JSON
    {
        List original;
        original.append(1).append("test").append(3.14f);
        String json = original.toJSONString();
        List restored = List::FromJSONString(json);
        ASSERT_EQ(restored.length(), original.length());
        ASSERT_EQ(restored.at<int>(0), 1);
        ASSERT_EQ(restored.at<String>(1), String("test"));
        Log("List JSON round-trip: passed");
    }

    // Round-trip CSV
    {
        List row1;
        row1.append("A").append("B");
        List row2;
        row2.append("C").append("D");
        List original;
        original.append(row1).append(row2);
        String csvStr = original.toCSVString();
        List restored = List::FromCSVString(csvStr);
        ASSERT_EQ(restored.length(), 2);
        List restoredRow1 = restored.at<List>(0);
        ASSERT_EQ(restoredRow1.at<String>(0), String("A"));
        Log("List CSV round-trip: passed");
    }

    Log("=== All List Tests Passed ===");
    TestFramework::DisplayCoverage();
    TestFramework::WriteCoverageData("test_list_comprehensive");
    Exit(0);
}
