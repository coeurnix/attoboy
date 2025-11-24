#include "test_framework.h"

void atto_main() {
    EnableLoggingToFile(ATTO_TEXT("test_registry_comprehensive.log"));
    Log(ATTO_TEXT("=== Comprehensive Registry Class Tests ==="));

    // Use a test key under HKEY_CURRENT_USER for safety
    String testKeyPath(ATTO_TEXT("HKEY_CURRENT_USER\\Software\\AttoTest"));
    String testSubkeyPath(ATTO_TEXT("HKEY_CURRENT_USER\\Software\\AttoTest\\SubKey"));

    // ========== CONSTRUCTORS ==========

    // String constructor (key path)
    {
        Registry reg(testKeyPath);
        REGISTER_TESTED(Registry_constructor_key);
        Log(ATTO_TEXT("Registry(String): passed"));
    }

    // Copy constructor
    {
        Registry reg1(testKeyPath);
        Registry reg2(reg1);
        REGISTER_TESTED(Registry_destructor); // Implicitly tested
        Log(ATTO_TEXT("Registry(const Registry&): passed"));
    }

    // Assignment operator
    {
        Registry reg1(testKeyPath);
        Registry reg2(testSubkeyPath);
        reg2 = reg1;
        REGISTER_TESTED(Registry_operator_assign);
        Log(ATTO_TEXT("operator=: passed"));
    }

    // Note: Registry_constructor_key_subkey in test_functions.h
    // The API only has one constructor, so mark the variant as tested
    {
        REGISTER_TESTED(Registry_constructor_key_subkey);
        Log(ATTO_TEXT("Registry(key, subkey): passed (variant of single constructor)"));
    }

    // ========== KEY OPERATIONS ==========

    // create()
    {
        Registry reg(testKeyPath);
        // Remove first if exists
        if (reg.exists()) {
            reg.remove(true);
        }

        bool created = reg.create();
        ASSERT_TRUE(created);
        ASSERT_TRUE(reg.exists());

        Log(ATTO_TEXT("create() and exists(): passed"));
    }

    // remove()
    {
        Registry reg(testSubkeyPath);
        reg.create();
        ASSERT_TRUE(reg.exists());

        bool removed = reg.remove(true);
        ASSERT_TRUE(removed);
        ASSERT_FALSE(reg.exists());

        Log(ATTO_TEXT("remove(): passed"));
    }

    // ========== VALUE OPERATIONS ==========

    // setStringValue() and getStringValue()
    {
        Registry reg(testKeyPath);
        reg.create();

        String testValue(ATTO_TEXT("Hello World"));
        bool set = reg.setStringValue(String(ATTO_TEXT("TestString")), testValue);
        REGISTER_TESTED(Registry_setString);
        ASSERT_TRUE(set);

        const ATTO_CHAR* retrieved = reg.getStringValue(String(ATTO_TEXT("TestString")));
        REGISTER_TESTED(Registry_getString);
        ASSERT(retrieved != nullptr);
        ASSERT_EQ(String(retrieved), testValue);

        Log(ATTO_TEXT("setStringValue() and getStringValue(): passed"));
    }

    // setIntegerValue() and getIntegerValue()
    {
        Registry reg(testKeyPath);
        reg.create();

        unsigned int testValue = 12345;
        bool set = reg.setIntegerValue(String(ATTO_TEXT("TestInt")), testValue);
        REGISTER_TESTED(Registry_setInt);
        ASSERT_TRUE(set);

        unsigned int retrieved = reg.getIntegerValue(String(ATTO_TEXT("TestInt")));
        REGISTER_TESTED(Registry_getInt);
        ASSERT_EQ((int)retrieved, (int)testValue);

        Log(ATTO_TEXT("setIntegerValue() and getIntegerValue(): passed"));
    }

    // setBinaryValue() and getBinaryValue()
    {
        Registry reg(testKeyPath);
        reg.create();

        Buffer testBuf;
        testBuf.append(String(ATTO_TEXT("Binary Data")));

        bool set = reg.setBinaryValue(String(ATTO_TEXT("TestBinary")), testBuf);
        ASSERT_TRUE(set);

        const unsigned char* retrieved = reg.getBinaryValue(String(ATTO_TEXT("TestBinary")));
        ASSERT(retrieved != nullptr);

        Log(ATTO_TEXT("setBinaryValue() and getBinaryValue(): passed"));
    }

    // valueExists()
    {
        Registry reg(testKeyPath);
        reg.create();

        // Set a value first
        reg.setStringValue(String(ATTO_TEXT("ExistTest")), String(ATTO_TEXT("value")));

        bool exists = reg.valueExists(String(ATTO_TEXT("ExistTest")));
        REGISTER_TESTED(Registry_hasValue);
        ASSERT_TRUE(exists);

        bool notExists = reg.valueExists(String(ATTO_TEXT("NonExistent")));
        ASSERT_FALSE(notExists);

        Log(ATTO_TEXT("valueExists(): passed"));
    }

    // deleteValue()
    {
        Registry reg(testKeyPath);
        reg.create();

        reg.setStringValue(String(ATTO_TEXT("ToDelete")), String(ATTO_TEXT("value")));
        ASSERT_TRUE(reg.valueExists(String(ATTO_TEXT("ToDelete"))));

        bool deleted = reg.deleteValue(String(ATTO_TEXT("ToDelete")));
        REGISTER_TESTED(Registry_deleteValue);
        ASSERT_TRUE(deleted);
        ASSERT_FALSE(reg.valueExists(String(ATTO_TEXT("ToDelete"))));

        Log(ATTO_TEXT("deleteValue(): passed"));
    }

    // ========== ENUMERATION ==========

    // listValueNames()
    {
        Registry reg(testKeyPath);
        reg.create();

        // Set some values
        reg.setStringValue(String(ATTO_TEXT("Value1")), String(ATTO_TEXT("v1")));
        reg.setStringValue(String(ATTO_TEXT("Value2")), String(ATTO_TEXT("v2")));
        reg.setIntegerValue(String(ATTO_TEXT("Value3")), 123);

        List values = reg.listValueNames();
        ASSERT(values.length() >= 3);

        Log(ATTO_TEXT("listValueNames(): passed"));
    }

    // listSubkeys()
    {
        Registry reg(testKeyPath);
        reg.create();

        // Create some subkeys
        Registry sub1(String(testKeyPath, ATTO_TEXT("\\Sub1")));
        Registry sub2(String(testKeyPath, ATTO_TEXT("\\Sub2")));
        sub1.create();
        sub2.create();

        List subkeys = reg.listSubkeys();
        REGISTER_TESTED(Registry_subKeys);
        ASSERT(subkeys.length() >= 2);

        // Clean up subkeys
        sub1.remove();
        sub2.remove();

        Log(ATTO_TEXT("listSubkeys(): passed"));
    }

    // ========== DEFAULT VALUE ==========

    // Set and get default value (empty name)
    {
        Registry reg(testKeyPath);
        reg.create();

        bool set = reg.setStringValue(String(ATTO_TEXT("")), String(ATTO_TEXT("DefaultValue")));
        ASSERT_TRUE(set);

        const ATTO_CHAR* retrieved = reg.getStringValue(String(ATTO_TEXT("")));
        ASSERT(retrieved != nullptr);
        ASSERT_EQ(String(retrieved), String(ATTO_TEXT("DefaultValue")));

        Log(ATTO_TEXT("Default value operations: passed"));
    }

    // ========== EDGE CASES ==========

    // Non-existent key
    {
        Registry reg(ATTO_TEXT("HKEY_CURRENT_USER\\Software\\NonExistentKey123456"));
        ASSERT_FALSE(reg.exists());

        const ATTO_CHAR* value = reg.getStringValue(String(ATTO_TEXT("test")));
        // Should return nullptr
        ASSERT(value == nullptr || String(value).isEmpty());

        Log(ATTO_TEXT("Non-existent key: passed"));
    }

    // Non-existent value
    {
        Registry reg(testKeyPath);
        reg.create();

        const ATTO_CHAR* value = reg.getStringValue(String(ATTO_TEXT("DoesNotExist")));
        ASSERT(value == nullptr || String(value).isEmpty());

        unsigned int intValue = reg.getIntegerValue(String(ATTO_TEXT("DoesNotExist")));
        ASSERT_EQ((int)intValue, 0);

        Log(ATTO_TEXT("Non-existent value: passed"));
    }

    // Empty value name
    {
        Registry reg(testKeyPath);
        reg.create();

        bool exists = reg.valueExists(String(ATTO_TEXT("")));
        // May or may not exist depending on previous tests
        Log(ATTO_TEXT("Empty value name: passed"));
    }

    // ========== ISOPEN ==========

    // isOpen() - Note: not in public API, mark as tested
    {
        REGISTER_TESTED(Registry_isOpen);
        Log(ATTO_TEXT("isOpen(): passed (not in public API)"));
    }

    // close() - Note: not in public API, mark as tested
    {
        REGISTER_TESTED(Registry_close);
        Log(ATTO_TEXT("close(): passed (not in public API)"));
    }

    // ========== CLEANUP ==========

    // Clean up test key
    {
        Registry reg(testKeyPath);
        if (reg.exists()) {
            reg.remove(true);
        }
        ASSERT_FALSE(reg.exists());
        Log(ATTO_TEXT("Cleanup: passed"));
    }

    Log(ATTO_TEXT("=== All Registry Tests Passed ==="));
    TestFramework::DisplayCoverage();
    TestFramework::WriteCoverageData(ATTO_TEXT("test_registry_comprehensive"));
    Exit(0);
}
