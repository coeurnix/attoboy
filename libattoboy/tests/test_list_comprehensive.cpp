#include "test_framework.h"

void atto_main() {
    EnableLoggingToFile(ATTO_TEXT("test_list_comprehensive.log"));
    Log(ATTO_TEXT("=== Comprehensive List Class Tests ==="));

    // ========== CONSTRUCTORS ==========

    // Empty constructor
    {
        List l;
        REGISTER_TESTED(List_constructor_empty);
        ASSERT_TRUE(l.isEmpty());
        ASSERT_EQ(l.length(), 0);
        Log(ATTO_TEXT("List() [empty]: passed"));
    }

    // Capacity constructor
    {
        List l(100);
        REGISTER_TESTED(List_constructor_capacity);
        ASSERT_TRUE(l.isEmpty());
        Log(ATTO_TEXT("List(capacity): passed"));
    }

    // Variadic constructor
    {
        List l(1, ATTO_TEXT("hello"), 3.14f, true);
        REGISTER_TESTED(List_constructor_variadic);
        ASSERT_EQ(l.length(), 4);
        ASSERT_EQ(l.at<int>(0), 1);
        ASSERT_EQ(l.at<String>(1), String(ATTO_TEXT("hello")));
        Log(ATTO_TEXT("List(variadic): passed"));
    }

    // Copy constructor
    {
        List orig;
        orig.append(1).append(2).append(3);
        List copy(orig);
        REGISTER_TESTED(List_constructor_copy);
        ASSERT_EQ(copy.length(), 3);
        ASSERT_EQ(copy.at<int>(0), 1);
        Log(ATTO_TEXT("List(copy): passed"));
    }

    // From Set constructor
    {
        Set s;
        s.put(1).put(2).put(3);
        List l(s);
        REGISTER_TESTED(List_constructor_from_set);
        ASSERT_EQ(l.length(), 3);
        Log(ATTO_TEXT("List(Set): passed"));
    }

    // Destructor (implicit)
    {
        REGISTER_TESTED(List_destructor);
        Log(ATTO_TEXT("~List(): passed (implicit)"));
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
        Log(ATTO_TEXT("operator=: passed"));
    }

    // ========== BASIC PROPERTIES ==========

    // length()
    {
        List l;
        l.append(1).append(2).append(3);
        REGISTER_TESTED(List_length);
        ASSERT_EQ(l.length(), 3);
        Log(ATTO_TEXT("length(): passed"));
    }

    // isEmpty()
    {
        List empty;
        List notEmpty;
        notEmpty.append(1);
        REGISTER_TESTED(List_isEmpty);
        ASSERT_TRUE(empty.isEmpty());
        ASSERT_FALSE(notEmpty.isEmpty());
        Log(ATTO_TEXT("isEmpty(): passed"));
    }

    // ========== MODIFICATION ==========

    // append()
    {
        List l;
        l.append(1).append(ATTO_TEXT("two")).append(3.0f);
        REGISTER_TESTED(List_append);
        ASSERT_EQ(l.length(), 3);
        ASSERT_EQ(l.at<int>(0), 1);
        ASSERT_EQ(l.at<String>(1), String(ATTO_TEXT("two")));
        Log(ATTO_TEXT("append(): passed"));
    }

    // prepend()
    {
        List l;
        l.append(2).prepend(1);
        REGISTER_TESTED(List_prepend);
        ASSERT_EQ(l.at<int>(0), 1);
        ASSERT_EQ(l.at<int>(1), 2);
        Log(ATTO_TEXT("prepend(): passed"));
    }

    // insert()
    {
        List l;
        l.append(1).append(3).insert(1, 2);
        REGISTER_TESTED(List_insert);
        ASSERT_EQ(l.at<int>(1), 2);
        Log(ATTO_TEXT("insert(): passed"));
    }

    // pop()
    {
        List l;
        l.append(1).append(2).append(3);
        int val = l.pop<int>();
        REGISTER_TESTED(List_pop);
        ASSERT_EQ(val, 3);
        ASSERT_EQ(l.length(), 2);
        Log(ATTO_TEXT("pop(): passed"));
    }

    // at()
    {
        List l;
        l.append(10).append(20).append(30);
        REGISTER_TESTED(List_at);
        ASSERT_EQ(l.at<int>(1), 20);
        Log(ATTO_TEXT("at(): passed"));
    }

    // operator[]
    {
        List l;
        l.append(100).append(200);
        REGISTER_TESTED(List_operator_brackets);
        int val = l.operator[]<int>(0);
        ASSERT_EQ(val, 100);
        Log(ATTO_TEXT("operator[]: passed"));
    }

    // set()
    {
        List l;
        l.append(1).append(2).set(1, 99);
        REGISTER_TESTED(List_set);
        ASSERT_EQ(l.at<int>(1), 99);
        Log(ATTO_TEXT("set(): passed"));
    }

    // remove()
    {
        List l;
        l.append(1).append(2).append(3).remove(1);
        REGISTER_TESTED(List_remove);
        ASSERT_EQ(l.length(), 2);
        ASSERT_EQ(l.at<int>(1), 3);
        Log(ATTO_TEXT("remove(): passed"));
    }

    // clear()
    {
        List l;
        l.append(1).append(2).clear();
        REGISTER_TESTED(List_clear);
        ASSERT_TRUE(l.isEmpty());
        Log(ATTO_TEXT("clear(): passed"));
    }

    // ========== SEARCH ==========

    // find()
    {
        List l;
        l.append(10).append(20).append(30);
        REGISTER_TESTED(List_find);
        ASSERT_EQ(l.find(20), 1);
        ASSERT_EQ(l.find(999), -1);
        Log(ATTO_TEXT("find(): passed"));
    }

    // contains()
    {
        List l;
        l.append(1).append(2).append(3);
        REGISTER_TESTED(List_contains);
        ASSERT_TRUE(l.contains(2));
        ASSERT_FALSE(l.contains(99));
        Log(ATTO_TEXT("contains(): passed"));
    }

    // ========== MANIPULATION ==========

    // reverse()
    {
        List l;
        l.append(1).append(2).append(3).reverse();
        REGISTER_TESTED(List_reverse);
        ASSERT_EQ(l.at<int>(0), 3);
        ASSERT_EQ(l.at<int>(2), 1);
        Log(ATTO_TEXT("reverse(): passed"));
    }

    // sort()
    {
        List l;
        l.append(3).append(1).append(2).sort();
        REGISTER_TESTED(List_sort);
        ASSERT_EQ(l.at<int>(0), 1);
        ASSERT_EQ(l.at<int>(1), 2);
        ASSERT_EQ(l.at<int>(2), 3);
        Log(ATTO_TEXT("sort(): passed"));
    }

    // typeAt()
    {
        List l;
        l.append(42).append(ATTO_TEXT("string"));
        REGISTER_TESTED(List_typeAt);
        // Just verify it doesn't crash
        l.typeAt(0);
        l.typeAt(1);
        Log(ATTO_TEXT("typeAt(): passed"));
    }

    // slice()
    {
        List l;
        l.append(1).append(2).append(3).append(4).append(5);
        List sliced = l.slice(1, 4);
        REGISTER_TESTED(List_slice);
        ASSERT_EQ(sliced.length(), 3);
        ASSERT_EQ(sliced.at<int>(0), 2);
        Log(ATTO_TEXT("slice(): passed"));
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
        Log(ATTO_TEXT("concat(List): passed"));
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
        Log(ATTO_TEXT("concat(Set): passed"));
    }

    // duplicate()
    {
        List orig;
        orig.append(1).append(2);
        List dup = orig.duplicate();
        REGISTER_TESTED(List_duplicate);
        ASSERT_EQ(dup.length(), 2);
        ASSERT_EQ(dup.at<int>(0), 1);
        Log(ATTO_TEXT("duplicate(): passed"));
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
        Log(ATTO_TEXT("compare(): passed"));
    }

    // operator==
    {
        List l1;
        l1.append(1);
        List l2;
        l2.append(1);
        REGISTER_TESTED(List_operator_eq);
        ASSERT_TRUE(l1 == l2);
        Log(ATTO_TEXT("operator==: passed"));
    }

    // operator!=
    {
        List l1;
        l1.append(1);
        List l2;
        l2.append(2);
        REGISTER_TESTED(List_operator_ne);
        ASSERT_TRUE(l1 != l2);
        Log(ATTO_TEXT("operator!=: passed"));
    }

    // operator+
    {
        List l;
        l.append(1);
        List result = l + 2;
        REGISTER_TESTED(List_operator_plus);
        ASSERT_EQ(result.length(), 2);
        ASSERT_EQ(result.at<int>(1), 2);
        Log(ATTO_TEXT("operator+: passed"));
    }

    // ========== NESTED COLLECTIONS & JSON ==========

    Log(ATTO_TEXT("=== Testing Nested Collections & JSON ==="));

    // Nested List in List
    {
        List inner;
        inner.append(1).append(2).append(3);
        List outer;
        outer.append(ATTO_TEXT("prefix")).append(inner).append(ATTO_TEXT("suffix"));

        ASSERT_EQ(outer.length(), 3);
        List retrieved = outer.at<List>(1);
        ASSERT_EQ(retrieved.length(), 3);
        ASSERT_EQ(retrieved.at<int>(0), 1);

        // Convert to JSON String
        String json(outer);
        ASSERT_TRUE(json.contains(String(ATTO_TEXT("["))));
        ASSERT_TRUE(json.contains(String(ATTO_TEXT("prefix"))));
        Log(ATTO_TEXT("Nested List in List + JSON: passed"));
    }

    // Map in List
    {
        Map m;
        m.put(ATTO_TEXT("key"), ATTO_TEXT("value"));
        List l;
        l.append(1).append(m).append(3);

        Map retrieved = l.at<Map>(1);
        String val = retrieved.get<String,String>(ATTO_TEXT("key"));
        ASSERT_EQ(val, String(ATTO_TEXT("value")));

        // Convert to JSON
        String json(l);
        ASSERT_TRUE(json.contains(String(ATTO_TEXT("{"))));
        ASSERT_TRUE(json.contains(String(ATTO_TEXT("key"))));
        Log(ATTO_TEXT("Map in List + JSON: passed"));
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
        ASSERT_TRUE(json.contains(String(ATTO_TEXT("["))));
        Log(ATTO_TEXT("Set in List + JSON: passed"));
    }

    // Deeply nested structure
    {
        List deepInner;
        deepInner.append(ATTO_TEXT("deep"));
        List middle;
        middle.append(deepInner);
        List outer;
        outer.append(middle);

        List retrieved = outer.at<List>(0).at<List>(0);
        ASSERT_EQ(retrieved.at<String>(0), String(ATTO_TEXT("deep")));

        // Convert to JSON
        String json(outer);
        ASSERT_TRUE(json.contains(String(ATTO_TEXT("deep"))));
        Log(ATTO_TEXT("Deeply nested List + JSON: passed"));
    }

    // ========== JSON/CSV FUNCTIONS ==========

    // List.toJSONString() basic
    {
        List l;
        l.append(1).append(ATTO_TEXT("hello")).append(true);
        String json = l.toJSONString();
        REGISTER_TESTED(List_toJSONString);
        ASSERT_TRUE(json.contains(ATTO_TEXT("[")));
        ASSERT_TRUE(json.contains(ATTO_TEXT("hello")));
        ASSERT_TRUE(json.contains(ATTO_TEXT("true")));
        Log(ATTO_TEXT("List.toJSONString(): passed"));
    }

    // List.FromJSONString() basic
    {
        String json(ATTO_TEXT("[1,\"hello\",true,3.14]"));
        List l = List::FromJSONString(json);
        REGISTER_TESTED(List_FromJSONString);
        ASSERT_EQ(l.length(), 4);
        ASSERT_EQ(l.at<int>(0), 1);
        ASSERT_EQ(l.at<String>(1), String(ATTO_TEXT("hello")));
        ASSERT_EQ(l.at<bool>(2), true);
        Log(ATTO_TEXT("List.FromJSONString(): passed"));
    }

    // List.FromJSONString() nested
    {
        String json(ATTO_TEXT("[[1,2],[3,4]]"));
        List l = List::FromJSONString(json);
        ASSERT_EQ(l.length(), 2);
        List inner = l.at<List>(0);
        ASSERT_EQ(inner.length(), 2);
        ASSERT_EQ(inner.at<int>(0), 1);
        ASSERT_EQ(inner.at<int>(1), 2);
        Log(ATTO_TEXT("List.FromJSONString() nested: passed"));
    }

    // List.FromJSONString() with Map
    {
        String json(ATTO_TEXT("[{\"key\":\"value\"}]"));
        List l = List::FromJSONString(json);
        ASSERT_EQ(l.length(), 1);
        Map m = l.at<Map>(0);
        String val = m.get<String,String>(ATTO_TEXT("key"));
        ASSERT_EQ(val, String(ATTO_TEXT("value")));
        Log(ATTO_TEXT("List.FromJSONString() with Map: passed"));
    }

    // List.toCSVString() basic
    {
        List row1;
        row1.append(ATTO_TEXT("Name")).append(ATTO_TEXT("Age")).append(ATTO_TEXT("City"));
        List row2;
        row2.append(ATTO_TEXT("Alice")).append(30).append(ATTO_TEXT("NYC"));
        List csv;
        csv.append(row1).append(row2);
        String csvStr = csv.toCSVString();
        REGISTER_TESTED(List_toCSVString);
        ASSERT_TRUE(csvStr.contains(ATTO_TEXT("Name")));
        ASSERT_TRUE(csvStr.contains(ATTO_TEXT("Alice")));
        ASSERT_TRUE(csvStr.contains(ATTO_TEXT("30")));
        Log(ATTO_TEXT("List.toCSVString(): passed"));
    }

    // List.FromCSVString() basic
    {
        String csvStr(ATTO_TEXT("Name,Age,City\r\nAlice,30,NYC\r\nBob,25,LA"));
        List csv = List::FromCSVString(csvStr);
        REGISTER_TESTED(List_FromCSVString);
        ASSERT_EQ(csv.length(), 3);
        List row1 = csv.at<List>(0);
        ASSERT_EQ(row1.length(), 3);
        ASSERT_EQ(row1.at<String>(0), String(ATTO_TEXT("Name")));
        List row2 = csv.at<List>(1);
        ASSERT_EQ(row2.at<String>(0), String(ATTO_TEXT("Alice")));
        Log(ATTO_TEXT("List.FromCSVString(): passed"));
    }

    // List.toCSVString() with quotes
    {
        List row1;
        row1.append(ATTO_TEXT("Quote \"test\"")).append(ATTO_TEXT("Comma,test"));
        List csv;
        csv.append(row1);
        String csvStr = csv.toCSVString();
        ASSERT_TRUE(csvStr.contains(ATTO_TEXT("\"\"")));
        Log(ATTO_TEXT("List.toCSVString() with escapes: passed"));
    }

    // List.FromCSVString() with quotes
    {
        String csvStr(ATTO_TEXT("\"Quote \"\"test\"\"\",\"Comma,test\""));
        List csv = List::FromCSVString(csvStr);
        ASSERT_EQ(csv.length(), 1);
        List row = csv.at<List>(0);
        ASSERT_EQ(row.length(), 2);
        String field1 = row.at<String>(0);
        ASSERT_TRUE(field1.contains(ATTO_TEXT("Quote")));
        Log(ATTO_TEXT("List.FromCSVString() with quotes: passed"));
    }

    // List.toCSVString() with nested collections
    {
        List inner;
        inner.append(1).append(2).append(3);
        List row1;
        row1.append(ATTO_TEXT("data")).append(inner);
        List csv;
        csv.append(row1);
        String csvStr = csv.toCSVString();
        ASSERT_TRUE(csvStr.contains(ATTO_TEXT("[")));
        Log(ATTO_TEXT("List.toCSVString() with nested: passed"));
    }

    // Round-trip JSON
    {
        List original;
        original.append(1).append(ATTO_TEXT("test")).append(3.14f);
        String json = original.toJSONString();
        List restored = List::FromJSONString(json);
        ASSERT_EQ(restored.length(), original.length());
        ASSERT_EQ(restored.at<int>(0), 1);
        ASSERT_EQ(restored.at<String>(1), String(ATTO_TEXT("test")));
        Log(ATTO_TEXT("List JSON round-trip: passed"));
    }

    // Round-trip CSV
    {
        List row1;
        row1.append(ATTO_TEXT("A")).append(ATTO_TEXT("B"));
        List row2;
        row2.append(ATTO_TEXT("C")).append(ATTO_TEXT("D"));
        List original;
        original.append(row1).append(row2);
        String csvStr = original.toCSVString();
        List restored = List::FromCSVString(csvStr);
        ASSERT_EQ(restored.length(), 2);
        List restoredRow1 = restored.at<List>(0);
        ASSERT_EQ(restoredRow1.at<String>(0), String(ATTO_TEXT("A")));
        Log(ATTO_TEXT("List CSV round-trip: passed"));
    }

    Log(ATTO_TEXT("=== All List Tests Passed ==="));
    TestFramework::DisplayCoverage();
    TestFramework::WriteCoverageData(ATTO_TEXT("test_list_comprehensive"));
    Exit(0);
}
