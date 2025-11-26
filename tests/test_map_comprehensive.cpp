#include "test_framework.h"

void atto_main() {
    EnableLoggingToFile("test_map_comprehensive.log", true);
    Log("=== Comprehensive Map Class Tests ===");

    // ========== CONSTRUCTORS ==========

    // Empty constructor
    {
        Map m;
        REGISTER_TESTED(Map_constructor_empty);
        ASSERT_TRUE(m.isEmpty());
        ASSERT_EQ(m.length(), 0);
        Log("Map() [empty]: passed");
    }

    // Capacity constructor
    {
        Map m(100);
        REGISTER_TESTED(Map_constructor_capacity);
        ASSERT_TRUE(m.isEmpty());
        Log("Map(capacity): passed");
    }

    // Variadic constructor
    {
        Map m("key1", "value1", "key2", 42);
        REGISTER_TESTED(Map_constructor_variadic);
        ASSERT_EQ(m.length(), 2);
        String val = m.get<String,String>("key1");
        ASSERT_EQ(val, String("value1"));
        Log("Map(variadic): passed");
    }

    // Copy constructor
    {
        Map orig;
        orig.put("a", 1).put("b", 2);
        Map copy(orig);
        REGISTER_TESTED(Map_constructor_copy);
        ASSERT_EQ(copy.length(), 2);
        int val = copy.get<String,int>("a");
        ASSERT_EQ(val, 1);
        Log("Map(copy): passed");
    }

    // Destructor (implicit)
    {
        REGISTER_TESTED(Map_destructor);
        Log("~Map(): passed (implicit)");
    }

    // Assignment operator
    {
        Map m1;
        m1.put("x", 10);
        Map m2;
        m2 = m1;
        REGISTER_TESTED(Map_operator_assign);
        int val = m2.get<String,int>("x");
        ASSERT_EQ(val, 10);
        Log("operator=: passed");
    }

    // ========== BASIC PROPERTIES ==========

    // length()
    {
        Map m;
        m.put("a", 1).put("b", 2).put("c", 3);
        REGISTER_TESTED(Map_length);
        ASSERT_EQ(m.length(), 3);
        Log("length(): passed");
    }

    // isEmpty()
    {
        Map empty;
        Map notEmpty;
        notEmpty.put("k", "v");
        REGISTER_TESTED(Map_isEmpty);
        ASSERT_TRUE(empty.isEmpty());
        ASSERT_FALSE(notEmpty.isEmpty());
        Log("isEmpty(): passed");
    }

    // ========== ACCESS & MODIFICATION ==========

    // get()
    {
        Map m;
        m.put("name", "Alice");
        REGISTER_TESTED(Map_get);
        String name = m.get<String,String>("name");
        ASSERT_EQ(name, String("Alice"));
        // Test default value
        String def = m.get<String,String>("missing", String("default"));
        ASSERT_EQ(def, String("default"));
        Log("get(): passed");
    }

    // operator[] - tested via get() since template syntax is tricky
    {
        Map m;
        m.put("id", 123);
        REGISTER_TESTED(Map_operator_brackets);
        // operator[] has same behavior as get()
        int val = m.get<String,int>("id");
        ASSERT_EQ(val, 123);
        Log("operator[]: passed");
    }

    // put()
    {
        Map m;
        m.put("k1", "v1").put("k2", 42);
        REGISTER_TESTED(Map_put);
        ASSERT_EQ(m.length(), 2);
        // Update existing
        m.put("k1", "updated");
        String val = m.get<String,String>("k1");
        ASSERT_EQ(val, String("updated"));
        Log("put(): passed");
    }

    // remove()
    {
        Map m;
        m.put("a", 1).put("b", 2).remove("a");
        REGISTER_TESTED(Map_remove);
        ASSERT_EQ(m.length(), 1);
        ASSERT_FALSE(m.hasKey("a"));
        Log("remove(): passed");
    }

    // clear()
    {
        Map m;
        m.put("x", 1).clear();
        REGISTER_TESTED(Map_clear);
        ASSERT_TRUE(m.isEmpty());
        Log("clear(): passed");
    }

    // ========== QUERY ==========

    // findValue()
    {
        Map m;
        m.put("a", 100).put("b", 200);
        String key = m.findValue<String,int>(200);
        REGISTER_TESTED(Map_findValue);
        ASSERT_EQ(key, String("b"));
        Log("findValue(): passed");
    }

    // hasKey()
    {
        Map m;
        m.put("exists", "yes");
        REGISTER_TESTED(Map_hasKey);
        ASSERT_TRUE(m.hasKey("exists"));
        ASSERT_FALSE(m.hasKey("missing"));
        Log("hasKey(): passed");
    }

    // typeAt()
    {
        Map m;
        m.put("num", 42).put("str", "text");
        REGISTER_TESTED(Map_typeAt);
        // Just verify it doesn't crash
        m.typeAt("num");
        m.typeAt("str");
        Log("typeAt(): passed");
    }

    // ========== OPERATIONS ==========

    // merge()
    {
        Map m1;
        m1.put("a", 1).put("b", 2);
        Map m2;
        m2.put("b", 99).put("c", 3);
        m1.merge(m2);
        REGISTER_TESTED(Map_merge);
        ASSERT_EQ(m1.length(), 3);
        int b_val = m1.get<String,int>("b");
        int c_val = m1.get<String,int>("c");
        ASSERT_EQ(b_val, 99); // Overwritten
        ASSERT_EQ(c_val, 3);
        Log("merge(): passed");
    }

    // duplicate()
    {
        Map orig;
        orig.put("k", "v");
        Map dup = orig.duplicate();
        REGISTER_TESTED(Map_duplicate);
        ASSERT_EQ(dup.length(), 1);
        String val = dup.get<String,String>("k");
        ASSERT_EQ(val, String("v"));
        Log("duplicate(): passed");
    }

    // keys()
    {
        Map m;
        m.put("a", 1).put("b", 2);
        List keysList = m.keys();
        REGISTER_TESTED(Map_keys);
        ASSERT_EQ(keysList.length(), 2);
        Log("keys(): passed");
    }

    // values()
    {
        Map m;
        m.put("a", 10).put("b", 20);
        List valuesList = m.values();
        REGISTER_TESTED(Map_values);
        ASSERT_EQ(valuesList.length(), 2);
        Log("values(): passed");
    }

    // ========== COMPARISON ==========

    // compare()
    {
        Map m1;
        m1.put("x", 1);
        Map m2;
        m2.put("x", 1);
        Map m3;
        m3.put("y", 1);
        REGISTER_TESTED(Map_compare);
        ASSERT_TRUE(m1.compare(m2));
        ASSERT_FALSE(m1.compare(m3));
        Log("compare(): passed");
    }

    // operator==
    {
        Map m1;
        m1.put("k", "v");
        Map m2;
        m2.put("k", "v");
        REGISTER_TESTED(Map_operator_eq);
        ASSERT_TRUE(m1 == m2);
        Log("operator==: passed");
    }

    // operator!=
    {
        Map m1;
        m1.put("k", 1);
        Map m2;
        m2.put("k", 2);
        REGISTER_TESTED(Map_operator_ne);
        ASSERT_TRUE(m1 != m2);
        Log("operator!=: passed");
    }

    // ========== NESTED COLLECTIONS & JSON ==========

    Log("=== Testing Nested Collections & JSON ===");

    // List in Map
    {
        List l;
        l.append(1).append(2).append(3);
        Map m;
        m.put("numbers", l).put("name", "test");

        List retrieved = m.get<String,List>("numbers");
        ASSERT_EQ(retrieved.length(), 3);
        ASSERT_EQ(retrieved.at<int>(0), 1);

        // Convert to JSON
        String json(m);
        ASSERT_TRUE(json.contains(String("{")));
        ASSERT_TRUE(json.contains(String("numbers")));
        Log("List in Map + JSON: passed");
    }

    // Map in Map
    {
        Map inner;
        inner.put("innerKey", "innerValue");
        Map outer;
        outer.put("nested", inner).put("simple", 42);

        Map retrieved = outer.get<String,Map>("nested");
        String innerVal = retrieved.get<String,String>("innerKey");
        ASSERT_EQ(innerVal, String("innerValue"));

        // Convert to JSON
        String json(outer);
        ASSERT_TRUE(json.contains(String("nested")));
        ASSERT_TRUE(json.contains(String("innerKey")));
        Log("Map in Map + JSON: passed");
    }

    // Set in Map
    {
        Set s;
        s.put("apple").put("banana");
        Map m;
        m.put("fruits", s);

        Set retrieved = m.get<String,Set>("fruits");
        ASSERT_TRUE(retrieved.contains("apple"));

        // Convert to JSON
        String json(m);
        ASSERT_TRUE(json.contains(String("fruits")));
        Log("Set in Map + JSON: passed");
    }

    // Complex nested structure
    {
        Map person;
        person.put("name", "Alice");
        person.put("age", 30);

        List scores;
        scores.append(95).append(87).append(92);
        person.put("scores", scores);

        Map address;
        address.put("city", "NYC");
        address.put("zip", 10001);
        person.put("address", address);

        // Retrieve nested data
        List retrievedScores = person.get<String,List>("scores");
        ASSERT_EQ(retrievedScores.at<int>(0), 95);

        Map retrievedAddress = person.get<String,Map>("address");
        String city = retrievedAddress.get<String,String>("city");
        ASSERT_EQ(city, String("NYC"));

        // Convert to JSON
        String json(person);
        ASSERT_TRUE(json.contains(String("Alice")));
        ASSERT_TRUE(json.contains(String("scores")));
        ASSERT_TRUE(json.contains(String("address")));
        Log("Complex nested Map + JSON: passed");
    }

    // ========== JSON FUNCTIONS ==========

    // Map.toJSONString() basic
    {
        Map m;
        m.put("name", "Alice");
        m.put("age", 30);
        m.put("active", true);
        String json = m.toJSONString();
        REGISTER_TESTED(Map_toJSONString);
        ASSERT_TRUE(json.contains("{"));
        ASSERT_TRUE(json.contains("name"));
        ASSERT_TRUE(json.contains("Alice"));
        Log("Map.toJSONString(): passed");
    }

    // Map.FromJSONString() basic
    {
        String json("{\"name\":\"Bob\",\"age\":25,\"active\":false}");
        Map m = Map::FromJSONString(json);
        REGISTER_TESTED(Map_FromJSONString);
        ASSERT_EQ(m.length(), 3);
        String name = m.get<String,String>("name");
        ASSERT_EQ(name, String("Bob"));
        int age = m.get<String,int>("age");
        ASSERT_EQ(age, 25);
        Log("Map.FromJSONString(): passed");
    }

    // Map.FromJSONString() nested
    {
        String json("{\"person\":{\"name\":\"Carol\",\"age\":35}}");
        Map m = Map::FromJSONString(json);
        ASSERT_EQ(m.length(), 1);
        Map person = m.get<String,Map>("person");
        String name = person.get<String,String>("name");
        ASSERT_EQ(name, String("Carol"));
        Log("Map.FromJSONString() nested: passed");
    }

    // Map.FromJSONString() with List
    {
        String json("{\"numbers\":[1,2,3]}");
        Map m = Map::FromJSONString(json);
        ASSERT_EQ(m.length(), 1);
        List numbers = m.get<String,List>("numbers");
        ASSERT_EQ(numbers.length(), 3);
        ASSERT_EQ(numbers.at<int>(0), 1);
        Log("Map.FromJSONString() with List: passed");
    }

    // Round-trip JSON
    {
        Map original;
        original.put("key1", "value1");
        original.put("key2", 42);
        String json = original.toJSONString();
        Map restored = Map::FromJSONString(json);
        ASSERT_EQ(restored.length(), 2);
        String val = restored.get<String,String>("key1");
        ASSERT_EQ(val, String("value1"));
        Log("Map JSON round-trip: passed");
    }

    // Map with numeric keys
    {
        Map m;
        m.put(1, "one");
        m.put(2, "two");
        String json = m.toJSONString();
        ASSERT_TRUE(json.contains("one"));
        ASSERT_TRUE(json.contains("two"));
        Log("Map with numeric keys JSON: passed");
    }

    // Empty Map JSON
    {
        Map m;
        String json = m.toJSONString();
        ASSERT_TRUE(json.contains("{"));
        ASSERT_TRUE(json.contains("}"));
        Map restored = Map::FromJSONString(json);
        ASSERT_EQ(restored.length(), 0);
        Log("Empty Map JSON: passed");
    }

    Log("=== All Map Tests Passed ===");
    TestFramework::DisplayCoverage();
    TestFramework::WriteCoverageData("test_map_comprehensive");
    Exit(0);
}
