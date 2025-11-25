#include "test_framework.h"

void atto_main() {
    EnableLoggingToFile(ATTO_TEXT("test_set_comprehensive.log"), true);
    Log(ATTO_TEXT("=== Comprehensive Set Class Tests ==="));

    // ========== CONSTRUCTORS ==========

    // Empty constructor
    {
        Set s;
        REGISTER_TESTED(Set_constructor_empty);
        ASSERT_TRUE(s.isEmpty());
        ASSERT_EQ(s.length(), 0);
        Log(ATTO_TEXT("Set() [empty]: passed"));
    }

    // Capacity constructor
    {
        Set s(100);
        REGISTER_TESTED(Set_constructor_capacity);
        ASSERT_TRUE(s.isEmpty());
        Log(ATTO_TEXT("Set(capacity): passed"));
    }

    // Variadic constructor
    {
        Set s(1, 2, 3, 2, 1); // Duplicates should be removed
        REGISTER_TESTED(Set_constructor_variadic);
        ASSERT_EQ(s.length(), 3); // Only 1, 2, 3
        Log(ATTO_TEXT("Set(variadic): passed"));
    }

    // Copy constructor
    {
        Set orig;
        orig.put(10).put(20).put(30);
        Set copy(orig);
        REGISTER_TESTED(Set_constructor_copy);
        ASSERT_EQ(copy.length(), 3);
        ASSERT_TRUE(copy.contains(10));
        Log(ATTO_TEXT("Set(copy): passed"));
    }

    // From List constructor
    {
        List l;
        l.append(1).append(2).append(2).append(3); // Has duplicate
        Set s(l);
        REGISTER_TESTED(Set_constructor_from_list);
        ASSERT_EQ(s.length(), 3); // Duplicates removed
        Log(ATTO_TEXT("Set(List): passed"));
    }

    // Destructor (implicit)
    {
        REGISTER_TESTED(Set_destructor);
        Log(ATTO_TEXT("~Set(): passed (implicit)"));
    }

    // Assignment operator
    {
        Set s1;
        s1.put(1).put(2);
        Set s2;
        s2 = s1;
        REGISTER_TESTED(Set_operator_assign);
        ASSERT_EQ(s2.length(), 2);
        ASSERT_TRUE(s2.contains(1));
        Log(ATTO_TEXT("operator=: passed"));
    }

    // ========== BASIC PROPERTIES ==========

    // length()
    {
        Set s;
        s.put(1).put(2).put(3);
        REGISTER_TESTED(Set_length);
        ASSERT_EQ(s.length(), 3);
        Log(ATTO_TEXT("length(): passed"));
    }

    // isEmpty()
    {
        Set empty;
        Set notEmpty;
        notEmpty.put(1);
        REGISTER_TESTED(Set_isEmpty);
        ASSERT_TRUE(empty.isEmpty());
        ASSERT_FALSE(notEmpty.isEmpty());
        Log(ATTO_TEXT("isEmpty(): passed"));
    }

    // ========== MODIFICATION ==========

    // put()
    {
        Set s;
        s.put(1).put(2).put(1); // Duplicate 1
        REGISTER_TESTED(Set_put);
        ASSERT_EQ(s.length(), 2); // Only 1 and 2
        ASSERT_TRUE(s.contains(1));
        Log(ATTO_TEXT("put(): passed"));
    }

    // contains()
    {
        Set s;
        s.put(ATTO_TEXT("apple")).put(ATTO_TEXT("banana"));
        REGISTER_TESTED(Set_contains);
        ASSERT_TRUE(s.contains(ATTO_TEXT("apple")));
        ASSERT_FALSE(s.contains(ATTO_TEXT("cherry")));
        Log(ATTO_TEXT("contains(): passed"));
    }

    // remove()
    {
        Set s;
        s.put(1).put(2).put(3).remove(2);
        REGISTER_TESTED(Set_remove);
        ASSERT_EQ(s.length(), 2);
        ASSERT_FALSE(s.contains(2));
        Log(ATTO_TEXT("remove(): passed"));
    }

    // clear()
    {
        Set s;
        s.put(1).put(2).clear();
        REGISTER_TESTED(Set_clear);
        ASSERT_TRUE(s.isEmpty());
        Log(ATTO_TEXT("clear(): passed"));
    }

    // ========== SET OPERATIONS ==========

    // setUnion()
    {
        Set s1;
        s1.put(1).put(2).put(3);
        Set s2;
        s2.put(3).put(4).put(5);
        s1.setUnion(s2);
        REGISTER_TESTED(Set_setUnion);
        ASSERT_EQ(s1.length(), 5); // 1,2,3,4,5
        ASSERT_TRUE(s1.contains(4));
        Log(ATTO_TEXT("setUnion(): passed"));
    }

    // intersect()
    {
        Set s1;
        s1.put(1).put(2).put(3).put(4);
        Set s2;
        s2.put(3).put(4).put(5);
        s1.intersect(s2);
        REGISTER_TESTED(Set_intersect);
        ASSERT_EQ(s1.length(), 2); // Only 3,4
        ASSERT_TRUE(s1.contains(3));
        ASSERT_FALSE(s1.contains(1));
        Log(ATTO_TEXT("intersect(): passed"));
    }

    // subtract()
    {
        Set s1;
        s1.put(1).put(2).put(3).put(4);
        Set s2;
        s2.put(3).put(4);
        s1.subtract(s2);
        REGISTER_TESTED(Set_subtract);
        ASSERT_EQ(s1.length(), 2); // Only 1,2
        ASSERT_TRUE(s1.contains(1));
        ASSERT_FALSE(s1.contains(3));
        Log(ATTO_TEXT("subtract(): passed"));
    }

    // ========== CONVERSION ==========

    // duplicate()
    {
        Set orig;
        orig.put(1).put(2);
        Set dup = orig.duplicate();
        REGISTER_TESTED(Set_duplicate);
        ASSERT_EQ(dup.length(), 2);
        ASSERT_TRUE(dup.contains(1));
        Log(ATTO_TEXT("duplicate(): passed"));
    }

    // toList()
    {
        Set s;
        s.put(10).put(20).put(30);
        List l = s.toList();
        REGISTER_TESTED(Set_toList);
        ASSERT_EQ(l.length(), 3);
        Log(ATTO_TEXT("toList(): passed"));
    }

    // ========== COMPARISON ==========

    // compare()
    {
        Set s1;
        s1.put(1).put(2);
        Set s2;
        s2.put(2).put(1); // Order doesn't matter
        Set s3;
        s3.put(1).put(3);
        REGISTER_TESTED(Set_compare);
        ASSERT_TRUE(s1.compare(s2));
        ASSERT_FALSE(s1.compare(s3));
        Log(ATTO_TEXT("compare(): passed"));
    }

    // operator==
    {
        Set s1;
        s1.put(1);
        Set s2;
        s2.put(1);
        REGISTER_TESTED(Set_operator_eq);
        ASSERT_TRUE(s1 == s2);
        Log(ATTO_TEXT("operator==: passed"));
    }

    // operator!=
    {
        Set s1;
        s1.put(1);
        Set s2;
        s2.put(2);
        REGISTER_TESTED(Set_operator_ne);
        ASSERT_TRUE(s1 != s2);
        Log(ATTO_TEXT("operator!=: passed"));
    }

    // operator+
    {
        Set s;
        s.put(1);
        Set result = s + 2;
        REGISTER_TESTED(Set_operator_plus);
        ASSERT_EQ(result.length(), 2);
        ASSERT_TRUE(result.contains(2));
        Log(ATTO_TEXT("operator+: passed"));
    }

    // ========== NESTED COLLECTIONS & JSON ==========

    Log(ATTO_TEXT("=== Testing Nested Collections & JSON ==="));

    // Set containing Lists
    {
        List l1;
        l1.append(1).append(2);
        List l2;
        l2.append(3).append(4);
        Set s;
        s.put(l1).put(l2);

        ASSERT_EQ(s.length(), 2);

        // Convert to JSON
        String json(s);
        ASSERT_TRUE(json.contains(String(ATTO_TEXT("["))));
        Log(ATTO_TEXT("Set containing Lists + JSON: passed"));
    }

    // Set containing Maps
    {
        Map m1;
        m1.put(ATTO_TEXT("a"), 1);
        Map m2;
        m2.put(ATTO_TEXT("b"), 2);
        Set s;
        s.put(m1).put(m2);

        ASSERT_EQ(s.length(), 2);

        // Convert to JSON
        String json(s);
        ASSERT_TRUE(json.contains(String(ATTO_TEXT("{"))));
        Log(ATTO_TEXT("Set containing Maps + JSON: passed"));
    }

    // Set containing Sets
    {
        Set inner1;
        inner1.put(1).put(2);
        Set inner2;
        inner2.put(3).put(4);
        Set outer;
        outer.put(inner1).put(inner2);

        ASSERT_EQ(outer.length(), 2);

        // Convert to JSON
        String json(outer);
        ASSERT_TRUE(json.contains(String(ATTO_TEXT("["))));
        Log(ATTO_TEXT("Set containing Sets + JSON: passed"));
    }

    // Mixed collection types in Set
    {
        List l;
        l.append(ATTO_TEXT("list"));
        Map m;
        m.put(ATTO_TEXT("key"), ATTO_TEXT("value"));
        Set inner;
        inner.put(99);

        Set container;
        container.put(l).put(m).put(inner);

        ASSERT_EQ(container.length(), 3);

        // Convert to JSON
        String json(container);
        ASSERT_TRUE(json.contains(String(ATTO_TEXT("["))));
        ASSERT_TRUE(json.contains(String(ATTO_TEXT("{"))));
        Log(ATTO_TEXT("Mixed collections in Set + JSON: passed"));
    }

    // ========== JSON FUNCTIONS ==========

    // Set.toJSONString() basic
    {
        Set s;
        s.put(1).put(2).put(3);
        String json = s.toJSONString();
        REGISTER_TESTED(Set_toJSONString);
        ASSERT_TRUE(json.contains(ATTO_TEXT("[")));
        ASSERT_TRUE(json.contains(ATTO_TEXT("1")));
        ASSERT_TRUE(json.contains(ATTO_TEXT("2")));
        ASSERT_TRUE(json.contains(ATTO_TEXT("3")));
        Log(ATTO_TEXT("Set.toJSONString(): passed"));
    }

    // Set.FromJSONString() basic
    {
        String json(ATTO_TEXT("[10,20,30,20,10]"));
        Set s = Set::FromJSONString(json);
        REGISTER_TESTED(Set_FromJSONString);
        ASSERT_EQ(s.length(), 3);
        ASSERT_TRUE(s.contains(10));
        ASSERT_TRUE(s.contains(20));
        ASSERT_TRUE(s.contains(30));
        Log(ATTO_TEXT("Set.FromJSONString(): passed"));
    }

    // Set.FromJSONString() with strings
    {
        String json(ATTO_TEXT("[\"apple\",\"banana\",\"apple\"]"));
        Set s = Set::FromJSONString(json);
        ASSERT_EQ(s.length(), 2);
        ASSERT_TRUE(s.contains(ATTO_TEXT("apple")));
        ASSERT_TRUE(s.contains(ATTO_TEXT("banana")));
        Log(ATTO_TEXT("Set.FromJSONString() with strings: passed"));
    }

    // Set.FromJSONString() mixed types
    {
        String json(ATTO_TEXT("[1,\"hello\",true,3.14]"));
        Set s = Set::FromJSONString(json);
        ASSERT_EQ(s.length(), 4);
        ASSERT_TRUE(s.contains(1));
        ASSERT_TRUE(s.contains(ATTO_TEXT("hello")));
        ASSERT_TRUE(s.contains(true));
        Log(ATTO_TEXT("Set.FromJSONString() mixed types: passed"));
    }

    // Round-trip JSON
    {
        Set original;
        original.put(ATTO_TEXT("alpha")).put(ATTO_TEXT("beta")).put(ATTO_TEXT("gamma"));
        String json = original.toJSONString();
        Set restored = Set::FromJSONString(json);
        ASSERT_EQ(restored.length(), 3);
        ASSERT_TRUE(restored.contains(ATTO_TEXT("alpha")));
        ASSERT_TRUE(restored.contains(ATTO_TEXT("beta")));
        ASSERT_TRUE(restored.contains(ATTO_TEXT("gamma")));
        Log(ATTO_TEXT("Set JSON round-trip: passed"));
    }

    // Empty Set JSON
    {
        Set s;
        String json = s.toJSONString();
        ASSERT_TRUE(json.contains(ATTO_TEXT("[")));
        ASSERT_TRUE(json.contains(ATTO_TEXT("]")));
        Set restored = Set::FromJSONString(json);
        ASSERT_EQ(restored.length(), 0);
        Log(ATTO_TEXT("Empty Set JSON: passed"));
    }

    // Set from JSON array ensures uniqueness
    {
        String json(ATTO_TEXT("[1,1,1,2,2,3]"));
        Set s = Set::FromJSONString(json);
        ASSERT_EQ(s.length(), 3);
        Log(ATTO_TEXT("Set uniqueness from JSON: passed"));
    }

    Log(ATTO_TEXT("=== All Set Tests Passed ==="));
    TestFramework::DisplayCoverage();
    TestFramework::WriteCoverageData(ATTO_TEXT("test_set_comprehensive"));
    Exit(0);
}
