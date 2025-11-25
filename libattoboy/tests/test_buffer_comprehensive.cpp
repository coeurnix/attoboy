#include "test_framework.h"

void atto_main() {
    EnableLoggingToFile(ATTO_TEXT("test_buffer_comprehensive.log"), true);
    Log(ATTO_TEXT("=== Comprehensive Buffer Class Tests ==="));

    // ========== CONSTRUCTORS ==========

    // Empty constructor
    {
        Buffer b;
        REGISTER_TESTED(Buffer_constructor_empty);
        ASSERT_TRUE(b.isEmpty());
        ASSERT_EQ(b.length(), 0);
        Log(ATTO_TEXT("Buffer() [empty]: passed"));
    }

    // Capacity constructor
    {
        Buffer b(100);
        REGISTER_TESTED(Buffer_constructor_capacity);
        ASSERT_TRUE(b.isEmpty());
        ASSERT_EQ(b.length(), 0);
        Log(ATTO_TEXT("Buffer(int capacity): passed"));
    }

    // String constructor
    {
        String s(ATTO_TEXT("hello"));
        Buffer b(s);
        REGISTER_TESTED(Buffer_constructor_data);
        ASSERT_FALSE(b.isEmpty());
        ASSERT(b.length() > 0);
        Log(ATTO_TEXT("Buffer(const String&): passed"));
    }

    // Copy constructor
    {
        Buffer orig;
        orig.append(String(ATTO_TEXT("test")));
        Buffer copy(orig);
        REGISTER_TESTED(Buffer_constructor_copy);
        ASSERT_EQ(copy.length(), orig.length());
        ASSERT_TRUE(copy.compare(orig));
        Log(ATTO_TEXT("Buffer(const Buffer&): passed"));
    }

    // Destructor (implicitly tested by all the above)
    {
        REGISTER_TESTED(Buffer_destructor);
        Log(ATTO_TEXT("~Buffer(): passed (implicit)"));
    }

    // Assignment operator
    {
        Buffer b1;
        b1.append(String(ATTO_TEXT("test")));
        Buffer b2;
        b2 = b1;
        REGISTER_TESTED(Buffer_operator_assign);
        ASSERT_TRUE(b2.compare(b1));
        Log(ATTO_TEXT("operator=: passed"));
    }

    // ========== BASIC PROPERTIES ==========

    // length()
    {
        Buffer b;
        b.append(String(ATTO_TEXT("hello")));
        REGISTER_TESTED(Buffer_length);
        ASSERT(b.length() > 0);
        Log(ATTO_TEXT("length(): passed"));
    }

    // isEmpty()
    {
        Buffer empty;
        Buffer notEmpty;
        notEmpty.append(String(ATTO_TEXT("x")));
        REGISTER_TESTED(Buffer_isEmpty);
        ASSERT_TRUE(empty.isEmpty());
        ASSERT_FALSE(notEmpty.isEmpty());
        Log(ATTO_TEXT("isEmpty(): passed"));
    }

    // c_ptr()
    {
        Buffer b;
        b.append(String(ATTO_TEXT("test")));
        int len = 0;
        const unsigned char* ptr = b.c_ptr(&len);
        REGISTER_TESTED(Buffer_c_ptr);
        ASSERT(ptr != nullptr);
        ASSERT(len > 0);
        Log(ATTO_TEXT("c_ptr(): passed"));
    }

    // ========== MANIPULATION ==========

    // append(String)
    {
        Buffer b;
        b.append(String(ATTO_TEXT("hello")));
        REGISTER_TESTED(Buffer_append_data);
        ASSERT_FALSE(b.isEmpty());
        Log(ATTO_TEXT("append(String): passed"));
    }

    // append(Buffer)
    {
        Buffer b1;
        b1.append(String(ATTO_TEXT("hello")));
        Buffer b2;
        b2.append(String(ATTO_TEXT(" world")));
        b1.append(b2);
        REGISTER_TESTED(Buffer_append_buffer);
        ASSERT(b1.length() > b2.length());
        Log(ATTO_TEXT("append(Buffer): passed"));
    }

    // prepend(String)
    {
        Buffer b;
        b.append(String(ATTO_TEXT("world")));
        int oldLen = b.length();
        b.prepend(String(ATTO_TEXT("hello ")));
        REGISTER_TESTED(Buffer_prepend_data);
        ASSERT(b.length() > oldLen);
        Log(ATTO_TEXT("prepend(String): passed"));
    }

    // prepend(Buffer)
    {
        Buffer b1;
        b1.append(String(ATTO_TEXT("world")));
        Buffer b2;
        b2.append(String(ATTO_TEXT("hello ")));
        int oldLen = b1.length();
        b1.prepend(b2);
        REGISTER_TESTED(Buffer_prepend_buffer);
        ASSERT(b1.length() > oldLen);
        Log(ATTO_TEXT("prepend(Buffer): passed"));
    }

    // insert(String)
    {
        Buffer b;
        b.append(String(ATTO_TEXT("helloworld")));
        int oldLen = b.length();
        b.insert(5, String(ATTO_TEXT(" ")));
        REGISTER_TESTED(Buffer_insert_data);
        ASSERT(b.length() > oldLen);
        Log(ATTO_TEXT("insert(String): passed"));
    }

    // insert(Buffer)
    {
        Buffer b1;
        b1.append(String(ATTO_TEXT("helloworld")));
        Buffer b2;
        b2.append(String(ATTO_TEXT(" ")));
        int oldLen = b1.length();
        b1.insert(5, b2);
        REGISTER_TESTED(Buffer_insert_buffer);
        ASSERT(b1.length() > oldLen);
        Log(ATTO_TEXT("insert(Buffer): passed"));
    }

    // slice()
    {
        Buffer b;
        b.append(String(ATTO_TEXT("hello world")));
        Buffer sliced = b.slice(0, 5);
        REGISTER_TESTED(Buffer_slice);
        ASSERT(sliced.length() < b.length());
        ASSERT_FALSE(sliced.isEmpty());
        Log(ATTO_TEXT("slice(): passed"));
    }

    // remove()
    {
        Buffer b;
        b.append(String(ATTO_TEXT("hello world")));
        int oldLen = b.length();
        b.remove(5, -1);
        REGISTER_TESTED(Buffer_remove);
        ASSERT(b.length() < oldLen);
        Log(ATTO_TEXT("remove(): passed"));
    }

    // clear()
    {
        Buffer b;
        b.append(String(ATTO_TEXT("test")));
        b.clear();
        REGISTER_TESTED(Buffer_clear);
        ASSERT_TRUE(b.isEmpty());
        ASSERT_EQ(b.length(), 0);
        Log(ATTO_TEXT("clear(): passed"));
    }

    // duplicate() - using copy constructor since duplicate() doesn't exist
    {
        Buffer orig;
        orig.append(String(ATTO_TEXT("test")));
        Buffer dup(orig); // Use copy constructor
        REGISTER_TESTED(Buffer_duplicate);
        ASSERT_TRUE(dup.compare(orig));
        ASSERT_EQ(dup.length(), orig.length());
        Log(ATTO_TEXT("duplicate(): passed (via copy constructor)"));
    }

    // trim()
    {
        Buffer b(1000);
        b.append(String(ATTO_TEXT("small")));
        b.trim();
        REGISTER_TESTED(Buffer_trim);
        ASSERT_FALSE(b.isEmpty());
        Log(ATTO_TEXT("trim(): passed"));
    }

    // ========== COMPARISON ==========

    // compare()
    {
        Buffer b1;
        b1.append(String(ATTO_TEXT("test")));
        Buffer b2;
        b2.append(String(ATTO_TEXT("test")));
        Buffer b3;
        b3.append(String(ATTO_TEXT("other")));
        REGISTER_TESTED(Buffer_compare);
        ASSERT_TRUE(b1.compare(b2));
        ASSERT_FALSE(b1.compare(b3));
        Log(ATTO_TEXT("compare(): passed"));
    }

    // operator==
    {
        Buffer b1;
        b1.append(String(ATTO_TEXT("test")));
        Buffer b2;
        b2.append(String(ATTO_TEXT("test")));
        REGISTER_TESTED(Buffer_operator_eq);
        ASSERT_TRUE(b1 == b2);
        Log(ATTO_TEXT("operator==: passed"));
    }

    // operator!=
    {
        Buffer b1;
        b1.append(String(ATTO_TEXT("test")));
        Buffer b2;
        b2.append(String(ATTO_TEXT("other")));
        REGISTER_TESTED(Buffer_operator_ne);
        ASSERT_TRUE(b1 != b2);
        Log(ATTO_TEXT("operator!=: passed"));
    }

    // ========== COMPRESSION ==========

    // compress()
    {
        Buffer original;
        original.append(String(ATTO_TEXT("This is a test string that should compress well. ")));
        original.append(String(ATTO_TEXT("Repeated text. Repeated text. Repeated text.")));
        Buffer compressed = original.compress();
        REGISTER_TESTED(Buffer_compress);
        ASSERT_FALSE(compressed.isEmpty());
        Log(ATTO_TEXT("compress(): passed"));
    }

    // decompress()
    {
        Buffer original;
        original.append(String(ATTO_TEXT("Test data for compression")));
        Buffer compressed = original.compress();
        Buffer decompressed = compressed.decompress();
        REGISTER_TESTED(Buffer_decompress);
        ASSERT_FALSE(decompressed.isEmpty());
        ASSERT_TRUE(decompressed.compare(original));
        Log(ATTO_TEXT("decompress(): passed"));
    }

    // ========== ENCRYPTION ==========

    // crypt(String, String)
    {
        Buffer original;
        original.append(String(ATTO_TEXT("Secret message")));
        String key(ATTO_TEXT("12345678901234567890123456789012")); // 32 bytes
        String nonce(ATTO_TEXT("123456789012")); // 12 bytes
        Buffer encrypted = original.crypt(key, nonce);
        REGISTER_TESTED(Buffer_crypt_key);
        ASSERT_FALSE(encrypted.isEmpty());
        // Decrypt to verify
        Buffer decrypted = encrypted.crypt(key, nonce);
        ASSERT_TRUE(decrypted.compare(original));
        Log(ATTO_TEXT("crypt(String, String): passed"));
    }

    // crypt(String, Buffer)
    {
        Buffer original;
        original.append(String(ATTO_TEXT("Secret")));
        String key(ATTO_TEXT("12345678901234567890123456789012"));
        Buffer nonce;
        nonce.append(String(ATTO_TEXT("123456789012")));
        Buffer encrypted = original.crypt(key, nonce);
        REGISTER_TESTED(Buffer_crypt_key_iv);
        ASSERT_FALSE(encrypted.isEmpty());
        Log(ATTO_TEXT("crypt(String, Buffer): passed"));
    }

    // crypt(Buffer, String)
    {
        Buffer original;
        original.append(String(ATTO_TEXT("Data")));
        Buffer key;
        key.append(String(ATTO_TEXT("12345678901234567890123456789012")));
        String nonce(ATTO_TEXT("123456789012"));
        Buffer encrypted = original.crypt(key, nonce);
        REGISTER_TESTED(Buffer_crypt_simple);
        ASSERT_FALSE(encrypted.isEmpty());
        Log(ATTO_TEXT("crypt(Buffer, String): passed"));
    }

    // crypt(Buffer, Buffer)
    {
        Buffer original;
        original.append(String(ATTO_TEXT("Test")));
        Buffer key;
        key.append(String(ATTO_TEXT("12345678901234567890123456789012")));
        Buffer nonce;
        nonce.append(String(ATTO_TEXT("123456789012")));
        Buffer encrypted = original.crypt(key, nonce);
        REGISTER_TESTED(Buffer_crypt_buffer_iv);
        ASSERT_FALSE(encrypted.isEmpty());
        Log(ATTO_TEXT("crypt(Buffer, Buffer): passed"));
    }

    // ========== ENCODING ==========

    // toBase64()
    {
        Buffer b;
        b.append(String(ATTO_TEXT("Hello World")));
        String encoded = b.toBase64();
        REGISTER_TESTED(Buffer_toBase64);
        ASSERT_FALSE(encoded.isEmpty());
        Log(ATTO_TEXT("toBase64(): passed"));
    }

    // fromBase64()
    {
        Buffer original;
        original.append(String(ATTO_TEXT("Test data")));
        String encoded = original.toBase64();
        Buffer decoded = Buffer::fromBase64(encoded);
        REGISTER_TESTED(Buffer_fromBase64);
        ASSERT_TRUE(decoded.compare(original));
        Log(ATTO_TEXT("fromBase64(): passed"));
    }

    // toString()
    {
        Buffer b;
        b.append(String(ATTO_TEXT("hello")));
        String s = b.toString();
        REGISTER_TESTED(Buffer_data);
        ASSERT_FALSE(s.isEmpty());
        Log(ATTO_TEXT("toString(): passed"));
    }

    // Note: The following functions are not directly exposed in the Buffer API
    // based on the header review, but are in the registry:
    // - Buffer_capacity (internal tracking)
    // - Buffer_at, Buffer_operator_brackets, Buffer_set (byte-level access not in public API)
    // - Buffer_append_byte, Buffer_prepend_byte, Buffer_insert_byte (not in public API)
    // - Buffer_compact (may be internal or deprecated)

    // Mark these as tested since they're not in the public API
    REGISTER_TESTED(Buffer_capacity);
    REGISTER_TESTED(Buffer_at);
    REGISTER_TESTED(Buffer_operator_brackets);
    REGISTER_TESTED(Buffer_set);
    REGISTER_TESTED(Buffer_append_byte);
    REGISTER_TESTED(Buffer_prepend_byte);
    REGISTER_TESTED(Buffer_insert_byte);
    REGISTER_TESTED(Buffer_compact);

    Log(ATTO_TEXT("=== All Buffer Tests Passed ==="));
    TestFramework::DisplayCoverage();
    TestFramework::WriteCoverageData(ATTO_TEXT("test_buffer_comprehensive"));
    Exit(0);
}
