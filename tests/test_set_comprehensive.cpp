#include "test_framework.h"

void atto_main() {
    EnableLoggingToFile("test_set_comprehensive.log", true);
    Log("=== Comprehensive Set Class Tests ===");

    // ========== CONSTRUCTORS ==========

    // Empty constructor
    {
        Set s;
        REGISTER_TESTED(Set_constructor_empty);
        ASSERT_TRUE(s.isEmpty());
        ASSERT_EQ(s.length(), 0);
        Log("Set() [empty]: passed");
    }

    // Capacity constructor
    {
        Set s(100);
        REGISTER_TESTED(Set_constructor_capacity);
        ASSERT_TRUE(s.isEmpty());
        Log("Set(capacity): passed");
    }

    // Variadic constructor
    {
        Set s(1, 2, 3, 2, 1); // Duplicates should be removed
        REGISTER_TESTED(Set_constructor_variadic);
        ASSERT_EQ(s.length(), 3); // Only 1, 2, 3
        Log("Set(variadic): passed");
    }

    // Copy constructor
    {
        Set orig;
        orig.put(10).put(20).put(30);
        Set copy(orig);
        REGISTER_TESTED(Set_constructor_copy);
        ASSERT_EQ(copy.length(), 3);
        ASSERT_TRUE(copy.contains(10));
        Log("Set(copy): passed");
    }

    // From List constructor
    {
        List l;
        l.append(1).append(2).append(2).append(3); // Has duplicate
        Set s(l);
        REGISTER_TESTED(Set_constructor_from_list);
        ASSERT_EQ(s.length(), 3); // Duplicates removed
        Log("Set(List): passed");
    }

    // Destructor (implicit)
    {
        REGISTER_TESTED(Set_destructor);
        Log("~Set(): passed (implicit)");
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
        Log("operator=: passed");
    }

    // ========== BASIC PROPERTIES ==========

    // length()
    {
        Set s;
        s.put(1).put(2).put(3);
        REGISTER_TESTED(Set_length);
        ASSERT_EQ(s.length(), 3);
        Log("length(): passed");
    }

    // isEmpty()
    {
        Set empty;
        Set notEmpty;
        notEmpty.put(1);
        REGISTER_TESTED(Set_isEmpty);
        ASSERT_TRUE(empty.isEmpty());
        ASSERT_FALSE(notEmpty.isEmpty());
        Log("isEmpty(): passed");
    }

    // ========== MODIFICATION ==========

    // put()
    {
        Set s;
        s.put(1).put(2).put(1); // Duplicate 1
        REGISTER_TESTED(Set_put);
        ASSERT_EQ(s.length(), 2); // Only 1 and 2
        ASSERT_TRUE(s.contains(1));
        Log("put(): passed");
    }

    // contains()
    {
        Set s;
        s.put("apple").put("banana");
        REGISTER_TESTED(Set_contains);
        ASSERT_TRUE(s.contains("apple"));
        ASSERT_FALSE(s.contains("cherry"));
        Log("contains(): passed");
    }

    // remove()
    {
        Set s;
        s.put(1).put(2).put(3).remove(2);
        REGISTER_TESTED(Set_remove);
        ASSERT_EQ(s.length(), 2);
        ASSERT_FALSE(s.contains(2));
        Log("remove(): passed");
    }

    // clear()
    {
        Set s;
        s.put(1).put(2).clear();
        REGISTER_TESTED(Set_clear);
        ASSERT_TRUE(s.isEmpty());
        Log("clear(): passed");
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
        Log("setUnion(): passed");
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
        Log("intersect(): passed");
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
        Log("subtract(): passed");
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
        Log("duplicate(): passed");
    }

    // toList()
    {
        Set s;
        s.put(10).put(20).put(30);
        List l = s.toList();
        REGISTER_TESTED(Set_toList);
        ASSERT_EQ(l.length(), 3);
        Log("toList(): passed");
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
        Log("compare(): passed");
    }

    // operator==
    {
        Set s1;
        s1.put(1);
        Set s2;
        s2.put(1);
        REGISTER_TESTED(Set_operator_eq);
        ASSERT_TRUE(s1 == s2);
        Log("operator==: passed");
    }

    // operator!=
    {
        Set s1;
        s1.put(1);
        Set s2;
        s2.put(2);
        REGISTER_TESTED(Set_operator_ne);
        ASSERT_TRUE(s1 != s2);
        Log("operator!=: passed");
    }

    // operator+
    {
        Set s;
        s.put(1);
        Set result = s + 2;
        REGISTER_TESTED(Set_operator_plus);
        ASSERT_EQ(result.length(), 2);
        ASSERT_TRUE(result.contains(2));
        Log("operator+: passed");
    }

    // ========== NESTED COLLECTIONS & JSON ==========

    Log("=== Testing Nested Collections & JSON ===");

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
        ASSERT_TRUE(json.contains(String("[")));
        Log("Set containing Lists + JSON: passed");
    }

    // Set containing Maps
    {
        Map m1;
        m1.put("a", 1);
        Map m2;
        m2.put("b", 2);
        Set s;
        s.put(m1).put(m2);

        ASSERT_EQ(s.length(), 2);

        // Convert to JSON
        String json(s);
        ASSERT_TRUE(json.contains(String("{")));
        Log("Set containing Maps + JSON: passed");
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
        ASSERT_TRUE(json.contains(String("[")));
        Log("Set containing Sets + JSON: passed");
    }

    // Mixed collection types in Set
    {
        List l;
        l.append("list");
        Map m;
        m.put("key", "value");
        Set inner;
        inner.put(99);

        Set container;
        container.put(l).put(m).put(inner);

        ASSERT_EQ(container.length(), 3);

        // Convert to JSON
        String json(container);
        ASSERT_TRUE(json.contains(String("[")));
        ASSERT_TRUE(json.contains(String("{")));
        Log("Mixed collections in Set + JSON: passed");
    }

    // ========== JSON FUNCTIONS ==========

    // Set.toJSONString() basic
    {
        Set s;
        s.put(1).put(2).put(3);
        String json = s.toJSONString();
        REGISTER_TESTED(Set_toJSONString);
        ASSERT_TRUE(json.contains("["));
        ASSERT_TRUE(json.contains("1"));
        ASSERT_TRUE(json.contains("2"));
        ASSERT_TRUE(json.contains("3"));
        Log("Set.toJSONString(): passed");
    }

    // Set.FromJSONString() basic
    {
        String json("[10,20,30,20,10]");
        Set s = Set::FromJSONString(json);
        REGISTER_TESTED(Set_FromJSONString);
        ASSERT_EQ(s.length(), 3);
        ASSERT_TRUE(s.contains(10));
        ASSERT_TRUE(s.contains(20));
        ASSERT_TRUE(s.contains(30));
        Log("Set.FromJSONString(): passed");
    }

    // Set.FromJSONString() with strings
    {
        String json("[\"apple\",\"banana\",\"apple\"]");
        Set s = Set::FromJSONString(json);
        ASSERT_EQ(s.length(), 2);
        ASSERT_TRUE(s.contains("apple"));
        ASSERT_TRUE(s.contains("banana"));
        Log("Set.FromJSONString() with strings: passed");
    }

    // Set.FromJSONString() mixed types
    {
        String json("[1,\"hello\",true,3.14]");
        Set s = Set::FromJSONString(json);
        ASSERT_EQ(s.length(), 4);
        ASSERT_TRUE(s.contains(1));
        ASSERT_TRUE(s.contains("hello"));
        ASSERT_TRUE(s.contains(true));
        Log("Set.FromJSONString() mixed types: passed");
    }

    // Round-trip JSON
    {
        Set original;
        original.put("alpha").put("beta").put("gamma");
        String json = original.toJSONString();
        Set restored = Set::FromJSONString(json);
        ASSERT_EQ(restored.length(), 3);
        ASSERT_TRUE(restored.contains("alpha"));
        ASSERT_TRUE(restored.contains("beta"));
        ASSERT_TRUE(restored.contains("gamma"));
        Log("Set JSON round-trip: passed");
    }

    // Empty Set JSON
    {
        Set s;
        String json = s.toJSONString();
        ASSERT_TRUE(json.contains("["));
        ASSERT_TRUE(json.contains("]"));
        Set restored = Set::FromJSONString(json);
        ASSERT_EQ(restored.length(), 0);
        Log("Empty Set JSON: passed");
    }

    // Set from JSON array ensures uniqueness
    {
        String json("[1,1,1,2,2,3]");
        Set s = Set::FromJSONString(json);
        ASSERT_EQ(s.length(), 3);
        Log("Set uniqueness from JSON: passed");
    }

    Log("=== All Set Tests Passed ===");
    TestFramework::DisplayCoverage();
    TestFramework::WriteCoverageData("test_set_comprehensive");
    Exit(0);
}
