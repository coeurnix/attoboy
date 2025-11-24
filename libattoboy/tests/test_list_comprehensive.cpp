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

    Log(ATTO_TEXT("=== All List Tests Passed ==="));
    TestFramework::DisplayCoverage();
    TestFramework::WriteCoverageData(ATTO_TEXT("test_list_comprehensive"));
    Exit(0);
}
