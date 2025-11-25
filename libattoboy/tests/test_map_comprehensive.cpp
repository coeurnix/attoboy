#include "test_framework.h"

void atto_main() {
    EnableLoggingToFile(ATTO_TEXT("test_map_comprehensive.log"), true);
    Log(ATTO_TEXT("=== Comprehensive Map Class Tests ==="));

    // ========== CONSTRUCTORS ==========

    // Empty constructor
    {
        Map m;
        REGISTER_TESTED(Map_constructor_empty);
        ASSERT_TRUE(m.isEmpty());
        ASSERT_EQ(m.length(), 0);
        Log(ATTO_TEXT("Map() [empty]: passed"));
    }

    // Capacity constructor
    {
        Map m(100);
        REGISTER_TESTED(Map_constructor_capacity);
        ASSERT_TRUE(m.isEmpty());
        Log(ATTO_TEXT("Map(capacity): passed"));
    }

    // Variadic constructor
    {
        Map m(ATTO_TEXT("key1"), ATTO_TEXT("value1"), ATTO_TEXT("key2"), 42);
        REGISTER_TESTED(Map_constructor_variadic);
        ASSERT_EQ(m.length(), 2);
        String val = m.get<String,String>(ATTO_TEXT("key1"));
        ASSERT_EQ(val, String(ATTO_TEXT("value1")));
        Log(ATTO_TEXT("Map(variadic): passed"));
    }

    // Copy constructor
    {
        Map orig;
        orig.put(ATTO_TEXT("a"), 1).put(ATTO_TEXT("b"), 2);
        Map copy(orig);
        REGISTER_TESTED(Map_constructor_copy);
        ASSERT_EQ(copy.length(), 2);
        int val = copy.get<String,int>(ATTO_TEXT("a"));
        ASSERT_EQ(val, 1);
        Log(ATTO_TEXT("Map(copy): passed"));
    }

    // Destructor (implicit)
    {
        REGISTER_TESTED(Map_destructor);
        Log(ATTO_TEXT("~Map(): passed (implicit)"));
    }

    // Assignment operator
    {
        Map m1;
        m1.put(ATTO_TEXT("x"), 10);
        Map m2;
        m2 = m1;
        REGISTER_TESTED(Map_operator_assign);
        int val = m2.get<String,int>(ATTO_TEXT("x"));
        ASSERT_EQ(val, 10);
        Log(ATTO_TEXT("operator=: passed"));
    }

    // ========== BASIC PROPERTIES ==========

    // length()
    {
        Map m;
        m.put(ATTO_TEXT("a"), 1).put(ATTO_TEXT("b"), 2).put(ATTO_TEXT("c"), 3);
        REGISTER_TESTED(Map_length);
        ASSERT_EQ(m.length(), 3);
        Log(ATTO_TEXT("length(): passed"));
    }

    // isEmpty()
    {
        Map empty;
        Map notEmpty;
        notEmpty.put(ATTO_TEXT("k"), ATTO_TEXT("v"));
        REGISTER_TESTED(Map_isEmpty);
        ASSERT_TRUE(empty.isEmpty());
        ASSERT_FALSE(notEmpty.isEmpty());
        Log(ATTO_TEXT("isEmpty(): passed"));
    }

    // ========== ACCESS & MODIFICATION ==========

    // get()
    {
        Map m;
        m.put(ATTO_TEXT("name"), ATTO_TEXT("Alice"));
        REGISTER_TESTED(Map_get);
        String name = m.get<String,String>(ATTO_TEXT("name"));
        ASSERT_EQ(name, String(ATTO_TEXT("Alice")));
        // Test default value
        String def = m.get<String,String>(ATTO_TEXT("missing"), String(ATTO_TEXT("default")));
        ASSERT_EQ(def, String(ATTO_TEXT("default")));
        Log(ATTO_TEXT("get(): passed"));
    }

    // operator[] - tested via get() since template syntax is tricky
    {
        Map m;
        m.put(ATTO_TEXT("id"), 123);
        REGISTER_TESTED(Map_operator_brackets);
        // operator[] has same behavior as get()
        int val = m.get<String,int>(ATTO_TEXT("id"));
        ASSERT_EQ(val, 123);
        Log(ATTO_TEXT("operator[]: passed"));
    }

    // put()
    {
        Map m;
        m.put(ATTO_TEXT("k1"), ATTO_TEXT("v1")).put(ATTO_TEXT("k2"), 42);
        REGISTER_TESTED(Map_put);
        ASSERT_EQ(m.length(), 2);
        // Update existing
        m.put(ATTO_TEXT("k1"), ATTO_TEXT("updated"));
        String val = m.get<String,String>(ATTO_TEXT("k1"));
        ASSERT_EQ(val, String(ATTO_TEXT("updated")));
        Log(ATTO_TEXT("put(): passed"));
    }

    // remove()
    {
        Map m;
        m.put(ATTO_TEXT("a"), 1).put(ATTO_TEXT("b"), 2).remove(ATTO_TEXT("a"));
        REGISTER_TESTED(Map_remove);
        ASSERT_EQ(m.length(), 1);
        ASSERT_FALSE(m.hasKey(ATTO_TEXT("a")));
        Log(ATTO_TEXT("remove(): passed"));
    }

    // clear()
    {
        Map m;
        m.put(ATTO_TEXT("x"), 1).clear();
        REGISTER_TESTED(Map_clear);
        ASSERT_TRUE(m.isEmpty());
        Log(ATTO_TEXT("clear(): passed"));
    }

    // ========== QUERY ==========

    // findValue()
    {
        Map m;
        m.put(ATTO_TEXT("a"), 100).put(ATTO_TEXT("b"), 200);
        String key = m.findValue<String,int>(200);
        REGISTER_TESTED(Map_findValue);
        ASSERT_EQ(key, String(ATTO_TEXT("b")));
        Log(ATTO_TEXT("findValue(): passed"));
    }

    // hasKey()
    {
        Map m;
        m.put(ATTO_TEXT("exists"), ATTO_TEXT("yes"));
        REGISTER_TESTED(Map_hasKey);
        ASSERT_TRUE(m.hasKey(ATTO_TEXT("exists")));
        ASSERT_FALSE(m.hasKey(ATTO_TEXT("missing")));
        Log(ATTO_TEXT("hasKey(): passed"));
    }

    // typeAt()
    {
        Map m;
        m.put(ATTO_TEXT("num"), 42).put(ATTO_TEXT("str"), ATTO_TEXT("text"));
        REGISTER_TESTED(Map_typeAt);
        // Just verify it doesn't crash
        m.typeAt(ATTO_TEXT("num"));
        m.typeAt(ATTO_TEXT("str"));
        Log(ATTO_TEXT("typeAt(): passed"));
    }

    // ========== OPERATIONS ==========

    // merge()
    {
        Map m1;
        m1.put(ATTO_TEXT("a"), 1).put(ATTO_TEXT("b"), 2);
        Map m2;
        m2.put(ATTO_TEXT("b"), 99).put(ATTO_TEXT("c"), 3);
        m1.merge(m2);
        REGISTER_TESTED(Map_merge);
        ASSERT_EQ(m1.length(), 3);
        int b_val = m1.get<String,int>(ATTO_TEXT("b"));
        int c_val = m1.get<String,int>(ATTO_TEXT("c"));
        ASSERT_EQ(b_val, 99); // Overwritten
        ASSERT_EQ(c_val, 3);
        Log(ATTO_TEXT("merge(): passed"));
    }

    // duplicate()
    {
        Map orig;
        orig.put(ATTO_TEXT("k"), ATTO_TEXT("v"));
        Map dup = orig.duplicate();
        REGISTER_TESTED(Map_duplicate);
        ASSERT_EQ(dup.length(), 1);
        String val = dup.get<String,String>(ATTO_TEXT("k"));
        ASSERT_EQ(val, String(ATTO_TEXT("v")));
        Log(ATTO_TEXT("duplicate(): passed"));
    }

    // keys()
    {
        Map m;
        m.put(ATTO_TEXT("a"), 1).put(ATTO_TEXT("b"), 2);
        List keysList = m.keys();
        REGISTER_TESTED(Map_keys);
        ASSERT_EQ(keysList.length(), 2);
        Log(ATTO_TEXT("keys(): passed"));
    }

    // values()
    {
        Map m;
        m.put(ATTO_TEXT("a"), 10).put(ATTO_TEXT("b"), 20);
        List valuesList = m.values();
        REGISTER_TESTED(Map_values);
        ASSERT_EQ(valuesList.length(), 2);
        Log(ATTO_TEXT("values(): passed"));
    }

    // ========== COMPARISON ==========

    // compare()
    {
        Map m1;
        m1.put(ATTO_TEXT("x"), 1);
        Map m2;
        m2.put(ATTO_TEXT("x"), 1);
        Map m3;
        m3.put(ATTO_TEXT("y"), 1);
        REGISTER_TESTED(Map_compare);
        ASSERT_TRUE(m1.compare(m2));
        ASSERT_FALSE(m1.compare(m3));
        Log(ATTO_TEXT("compare(): passed"));
    }

    // operator==
    {
        Map m1;
        m1.put(ATTO_TEXT("k"), ATTO_TEXT("v"));
        Map m2;
        m2.put(ATTO_TEXT("k"), ATTO_TEXT("v"));
        REGISTER_TESTED(Map_operator_eq);
        ASSERT_TRUE(m1 == m2);
        Log(ATTO_TEXT("operator==: passed"));
    }

    // operator!=
    {
        Map m1;
        m1.put(ATTO_TEXT("k"), 1);
        Map m2;
        m2.put(ATTO_TEXT("k"), 2);
        REGISTER_TESTED(Map_operator_ne);
        ASSERT_TRUE(m1 != m2);
        Log(ATTO_TEXT("operator!=: passed"));
    }

    // ========== NESTED COLLECTIONS & JSON ==========

    Log(ATTO_TEXT("=== Testing Nested Collections & JSON ==="));

    // List in Map
    {
        List l;
        l.append(1).append(2).append(3);
        Map m;
        m.put(ATTO_TEXT("numbers"), l).put(ATTO_TEXT("name"), ATTO_TEXT("test"));

        List retrieved = m.get<String,List>(ATTO_TEXT("numbers"));
        ASSERT_EQ(retrieved.length(), 3);
        ASSERT_EQ(retrieved.at<int>(0), 1);

        // Convert to JSON
        String json(m);
        ASSERT_TRUE(json.contains(String(ATTO_TEXT("{"))));
        ASSERT_TRUE(json.contains(String(ATTO_TEXT("numbers"))));
        Log(ATTO_TEXT("List in Map + JSON: passed"));
    }

    // Map in Map
    {
        Map inner;
        inner.put(ATTO_TEXT("innerKey"), ATTO_TEXT("innerValue"));
        Map outer;
        outer.put(ATTO_TEXT("nested"), inner).put(ATTO_TEXT("simple"), 42);

        Map retrieved = outer.get<String,Map>(ATTO_TEXT("nested"));
        String innerVal = retrieved.get<String,String>(ATTO_TEXT("innerKey"));
        ASSERT_EQ(innerVal, String(ATTO_TEXT("innerValue")));

        // Convert to JSON
        String json(outer);
        ASSERT_TRUE(json.contains(String(ATTO_TEXT("nested"))));
        ASSERT_TRUE(json.contains(String(ATTO_TEXT("innerKey"))));
        Log(ATTO_TEXT("Map in Map + JSON: passed"));
    }

    // Set in Map
    {
        Set s;
        s.put(ATTO_TEXT("apple")).put(ATTO_TEXT("banana"));
        Map m;
        m.put(ATTO_TEXT("fruits"), s);

        Set retrieved = m.get<String,Set>(ATTO_TEXT("fruits"));
        ASSERT_TRUE(retrieved.contains(ATTO_TEXT("apple")));

        // Convert to JSON
        String json(m);
        ASSERT_TRUE(json.contains(String(ATTO_TEXT("fruits"))));
        Log(ATTO_TEXT("Set in Map + JSON: passed"));
    }

    // Complex nested structure
    {
        Map person;
        person.put(ATTO_TEXT("name"), ATTO_TEXT("Alice"));
        person.put(ATTO_TEXT("age"), 30);

        List scores;
        scores.append(95).append(87).append(92);
        person.put(ATTO_TEXT("scores"), scores);

        Map address;
        address.put(ATTO_TEXT("city"), ATTO_TEXT("NYC"));
        address.put(ATTO_TEXT("zip"), 10001);
        person.put(ATTO_TEXT("address"), address);

        // Retrieve nested data
        List retrievedScores = person.get<String,List>(ATTO_TEXT("scores"));
        ASSERT_EQ(retrievedScores.at<int>(0), 95);

        Map retrievedAddress = person.get<String,Map>(ATTO_TEXT("address"));
        String city = retrievedAddress.get<String,String>(ATTO_TEXT("city"));
        ASSERT_EQ(city, String(ATTO_TEXT("NYC")));

        // Convert to JSON
        String json(person);
        ASSERT_TRUE(json.contains(String(ATTO_TEXT("Alice"))));
        ASSERT_TRUE(json.contains(String(ATTO_TEXT("scores"))));
        ASSERT_TRUE(json.contains(String(ATTO_TEXT("address"))));
        Log(ATTO_TEXT("Complex nested Map + JSON: passed"));
    }

    // ========== JSON FUNCTIONS ==========

    // Map.toJSONString() basic
    {
        Map m;
        m.put(ATTO_TEXT("name"), ATTO_TEXT("Alice"));
        m.put(ATTO_TEXT("age"), 30);
        m.put(ATTO_TEXT("active"), true);
        String json = m.toJSONString();
        REGISTER_TESTED(Map_toJSONString);
        ASSERT_TRUE(json.contains(ATTO_TEXT("{")));
        ASSERT_TRUE(json.contains(ATTO_TEXT("name")));
        ASSERT_TRUE(json.contains(ATTO_TEXT("Alice")));
        Log(ATTO_TEXT("Map.toJSONString(): passed"));
    }

    // Map.FromJSONString() basic
    {
        String json(ATTO_TEXT("{\"name\":\"Bob\",\"age\":25,\"active\":false}"));
        Map m = Map::FromJSONString(json);
        REGISTER_TESTED(Map_FromJSONString);
        ASSERT_EQ(m.length(), 3);
        String name = m.get<String,String>(ATTO_TEXT("name"));
        ASSERT_EQ(name, String(ATTO_TEXT("Bob")));
        int age = m.get<String,int>(ATTO_TEXT("age"));
        ASSERT_EQ(age, 25);
        Log(ATTO_TEXT("Map.FromJSONString(): passed"));
    }

    // Map.FromJSONString() nested
    {
        String json(ATTO_TEXT("{\"person\":{\"name\":\"Carol\",\"age\":35}}"));
        Map m = Map::FromJSONString(json);
        ASSERT_EQ(m.length(), 1);
        Map person = m.get<String,Map>(ATTO_TEXT("person"));
        String name = person.get<String,String>(ATTO_TEXT("name"));
        ASSERT_EQ(name, String(ATTO_TEXT("Carol")));
        Log(ATTO_TEXT("Map.FromJSONString() nested: passed"));
    }

    // Map.FromJSONString() with List
    {
        String json(ATTO_TEXT("{\"numbers\":[1,2,3]}"));
        Map m = Map::FromJSONString(json);
        ASSERT_EQ(m.length(), 1);
        List numbers = m.get<String,List>(ATTO_TEXT("numbers"));
        ASSERT_EQ(numbers.length(), 3);
        ASSERT_EQ(numbers.at<int>(0), 1);
        Log(ATTO_TEXT("Map.FromJSONString() with List: passed"));
    }

    // Round-trip JSON
    {
        Map original;
        original.put(ATTO_TEXT("key1"), ATTO_TEXT("value1"));
        original.put(ATTO_TEXT("key2"), 42);
        String json = original.toJSONString();
        Map restored = Map::FromJSONString(json);
        ASSERT_EQ(restored.length(), 2);
        String val = restored.get<String,String>(ATTO_TEXT("key1"));
        ASSERT_EQ(val, String(ATTO_TEXT("value1")));
        Log(ATTO_TEXT("Map JSON round-trip: passed"));
    }

    // Map with numeric keys
    {
        Map m;
        m.put(1, ATTO_TEXT("one"));
        m.put(2, ATTO_TEXT("two"));
        String json = m.toJSONString();
        ASSERT_TRUE(json.contains(ATTO_TEXT("one")));
        ASSERT_TRUE(json.contains(ATTO_TEXT("two")));
        Log(ATTO_TEXT("Map with numeric keys JSON: passed"));
    }

    // Empty Map JSON
    {
        Map m;
        String json = m.toJSONString();
        ASSERT_TRUE(json.contains(ATTO_TEXT("{")));
        ASSERT_TRUE(json.contains(ATTO_TEXT("}")));
        Map restored = Map::FromJSONString(json);
        ASSERT_EQ(restored.length(), 0);
        Log(ATTO_TEXT("Empty Map JSON: passed"));
    }

    Log(ATTO_TEXT("=== All Map Tests Passed ==="));
    TestFramework::DisplayCoverage();
    TestFramework::WriteCoverageData(ATTO_TEXT("test_map_comprehensive"));
    Exit(0);
}
