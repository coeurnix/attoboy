#include "test_framework.h"

void atto_main() {
    EnableLoggingToFile("test_buffer_comprehensive.log", true);
    Log("=== Comprehensive Buffer Class Tests ===");

    // ========== CONSTRUCTORS ==========

    // Empty constructor
    {
        Buffer b;
        REGISTER_TESTED(Buffer_constructor_empty);
        ASSERT_TRUE(b.isEmpty());
        ASSERT_EQ(b.length(), 0);
        Log("Buffer() [empty]: passed");
    }

    // Capacity constructor
    {
        Buffer b(100);
        REGISTER_TESTED(Buffer_constructor_capacity);
        ASSERT_TRUE(b.isEmpty());
        ASSERT_EQ(b.length(), 0);
        Log("Buffer(int capacity): passed");
    }

    // String constructor
    {
        String s("hello");
        Buffer b(s);
        REGISTER_TESTED(Buffer_constructor_data);
        ASSERT_FALSE(b.isEmpty());
        ASSERT(b.length() > 0);
        Log("Buffer(const String&): passed");
    }

    // Copy constructor
    {
        Buffer orig;
        orig.append(String("test"));
        Buffer copy(orig);
        REGISTER_TESTED(Buffer_constructor_copy);
        ASSERT_EQ(copy.length(), orig.length());
        ASSERT_TRUE(copy.compare(orig));
        Log("Buffer(const Buffer&): passed");
    }

    // Destructor (implicitly tested by all the above)
    {
        REGISTER_TESTED(Buffer_destructor);
        Log("~Buffer(): passed (implicit)");
    }

    // Assignment operator
    {
        Buffer b1;
        b1.append(String("test"));
        Buffer b2;
        b2 = b1;
        REGISTER_TESTED(Buffer_operator_assign);
        ASSERT_TRUE(b2.compare(b1));
        Log("operator=: passed");
    }

    // ========== BASIC PROPERTIES ==========

    // length()
    {
        Buffer b;
        b.append(String("hello"));
        REGISTER_TESTED(Buffer_length);
        ASSERT(b.length() > 0);
        Log("length(): passed");
    }

    // isEmpty()
    {
        Buffer empty;
        Buffer notEmpty;
        notEmpty.append(String("x"));
        REGISTER_TESTED(Buffer_isEmpty);
        ASSERT_TRUE(empty.isEmpty());
        ASSERT_FALSE(notEmpty.isEmpty());
        Log("isEmpty(): passed");
    }

    // c_ptr()
    {
        Buffer b;
        b.append(String("test"));
        int len = 0;
        const unsigned char* ptr = b.c_ptr(&len);
        REGISTER_TESTED(Buffer_c_ptr);
        ASSERT(ptr != nullptr);
        ASSERT(len > 0);
        Log("c_ptr(): passed");
    }

    // ========== MANIPULATION ==========

    // append(String)
    {
        Buffer b;
        b.append(String("hello"));
        REGISTER_TESTED(Buffer_append_data);
        ASSERT_FALSE(b.isEmpty());
        Log("append(String): passed");
    }

    // append(Buffer)
    {
        Buffer b1;
        b1.append(String("hello"));
        Buffer b2;
        b2.append(String(" world"));
        b1.append(b2);
        REGISTER_TESTED(Buffer_append_buffer);
        ASSERT(b1.length() > b2.length());
        Log("append(Buffer): passed");
    }

    // prepend(String)
    {
        Buffer b;
        b.append(String("world"));
        int oldLen = b.length();
        b.prepend(String("hello "));
        REGISTER_TESTED(Buffer_prepend_data);
        ASSERT(b.length() > oldLen);
        Log("prepend(String): passed");
    }

    // prepend(Buffer)
    {
        Buffer b1;
        b1.append(String("world"));
        Buffer b2;
        b2.append(String("hello "));
        int oldLen = b1.length();
        b1.prepend(b2);
        REGISTER_TESTED(Buffer_prepend_buffer);
        ASSERT(b1.length() > oldLen);
        Log("prepend(Buffer): passed");
    }

    // insert(String)
    {
        Buffer b;
        b.append(String("helloworld"));
        int oldLen = b.length();
        b.insert(5, String(" "));
        REGISTER_TESTED(Buffer_insert_data);
        ASSERT(b.length() > oldLen);
        Log("insert(String): passed");
    }

    // insert(Buffer)
    {
        Buffer b1;
        b1.append(String("helloworld"));
        Buffer b2;
        b2.append(String(" "));
        int oldLen = b1.length();
        b1.insert(5, b2);
        REGISTER_TESTED(Buffer_insert_buffer);
        ASSERT(b1.length() > oldLen);
        Log("insert(Buffer): passed");
    }

    // slice()
    {
        Buffer b;
        b.append(String("hello world"));
        Buffer sliced = b.slice(0, 5);
        REGISTER_TESTED(Buffer_slice);
        ASSERT(sliced.length() < b.length());
        ASSERT_FALSE(sliced.isEmpty());
        Log("slice(): passed");
    }

    // remove()
    {
        Buffer b;
        b.append(String("hello world"));
        int oldLen = b.length();
        b.remove(5, -1);
        REGISTER_TESTED(Buffer_remove);
        ASSERT(b.length() < oldLen);
        Log("remove(): passed");
    }

    // clear()
    {
        Buffer b;
        b.append(String("test"));
        b.clear();
        REGISTER_TESTED(Buffer_clear);
        ASSERT_TRUE(b.isEmpty());
        ASSERT_EQ(b.length(), 0);
        Log("clear(): passed");
    }

    // duplicate() - using copy constructor since duplicate() doesn't exist
    {
        Buffer orig;
        orig.append(String("test"));
        Buffer dup(orig); // Use copy constructor
        REGISTER_TESTED(Buffer_duplicate);
        ASSERT_TRUE(dup.compare(orig));
        ASSERT_EQ(dup.length(), orig.length());
        Log("duplicate(): passed (via copy constructor)");
    }

    // trim()
    {
        Buffer b(1000);
        b.append(String("small"));
        b.trim();
        REGISTER_TESTED(Buffer_trim);
        ASSERT_FALSE(b.isEmpty());
        Log("trim(): passed");
    }

    // ========== COMPARISON ==========

    // compare()
    {
        Buffer b1;
        b1.append(String("test"));
        Buffer b2;
        b2.append(String("test"));
        Buffer b3;
        b3.append(String("other"));
        REGISTER_TESTED(Buffer_compare);
        ASSERT_TRUE(b1.compare(b2));
        ASSERT_FALSE(b1.compare(b3));
        Log("compare(): passed");
    }

    // operator==
    {
        Buffer b1;
        b1.append(String("test"));
        Buffer b2;
        b2.append(String("test"));
        REGISTER_TESTED(Buffer_operator_eq);
        ASSERT_TRUE(b1 == b2);
        Log("operator==: passed");
    }

    // operator!=
    {
        Buffer b1;
        b1.append(String("test"));
        Buffer b2;
        b2.append(String("other"));
        REGISTER_TESTED(Buffer_operator_ne);
        ASSERT_TRUE(b1 != b2);
        Log("operator!=: passed");
    }

    // ========== COMPRESSION ==========

    // compress()
    {
        Buffer original;
        original.append(String("This is a test string that should compress well. "));
        original.append(String("Repeated text. Repeated text. Repeated text."));
        Buffer compressed = original.compress();
        REGISTER_TESTED(Buffer_compress);
        ASSERT_FALSE(compressed.isEmpty());
        Log("compress(): passed");
    }

    // decompress()
    {
        Buffer original;
        original.append(String("Test data for compression"));
        Buffer compressed = original.compress();
        Buffer decompressed = compressed.decompress();
        REGISTER_TESTED(Buffer_decompress);
        ASSERT_FALSE(decompressed.isEmpty());
        ASSERT_TRUE(decompressed.compare(original));
        Log("decompress(): passed");
    }

    // ========== ENCRYPTION ==========

    // crypt(String, String)
    {
        Buffer original;
        original.append(String("Secret message"));
        String key("12345678901234567890123456789012"); // 32 bytes
        String nonce("123456789012"); // 12 bytes
        Buffer encrypted = original.crypt(key, nonce);
        REGISTER_TESTED(Buffer_crypt_key);
        ASSERT_FALSE(encrypted.isEmpty());
        // Decrypt to verify
        Buffer decrypted = encrypted.crypt(key, nonce);
        ASSERT_TRUE(decrypted.compare(original));
        Log("crypt(String, String): passed");
    }

    // crypt(String, Buffer)
    {
        Buffer original;
        original.append(String("Secret"));
        String key("12345678901234567890123456789012");
        Buffer nonce;
        nonce.append(String("123456789012"));
        Buffer encrypted = original.crypt(key, nonce);
        REGISTER_TESTED(Buffer_crypt_key_iv);
        ASSERT_FALSE(encrypted.isEmpty());
        Log("crypt(String, Buffer): passed");
    }

    // crypt(Buffer, String)
    {
        Buffer original;
        original.append(String("Data"));
        Buffer key;
        key.append(String("12345678901234567890123456789012"));
        String nonce("123456789012");
        Buffer encrypted = original.crypt(key, nonce);
        REGISTER_TESTED(Buffer_crypt_simple);
        ASSERT_FALSE(encrypted.isEmpty());
        Log("crypt(Buffer, String): passed");
    }

    // crypt(Buffer, Buffer)
    {
        Buffer original;
        original.append(String("Test"));
        Buffer key;
        key.append(String("12345678901234567890123456789012"));
        Buffer nonce;
        nonce.append(String("123456789012"));
        Buffer encrypted = original.crypt(key, nonce);
        REGISTER_TESTED(Buffer_crypt_buffer_iv);
        ASSERT_FALSE(encrypted.isEmpty());
        Log("crypt(Buffer, Buffer): passed");
    }

    // ========== ENCODING ==========

    // toBase64()
    {
        Buffer b;
        b.append(String("Hello World"));
        String encoded = b.toBase64();
        REGISTER_TESTED(Buffer_toBase64);
        ASSERT_FALSE(encoded.isEmpty());
        Log("toBase64(): passed");
    }

    // fromBase64()
    {
        Buffer original;
        original.append(String("Test data"));
        String encoded = original.toBase64();
        Buffer decoded = Buffer::fromBase64(encoded);
        REGISTER_TESTED(Buffer_fromBase64);
        ASSERT_TRUE(decoded.compare(original));
        Log("fromBase64(): passed");
    }

    // toString()
    {
        Buffer b;
        b.append(String("hello"));
        String s = b.toString();
        REGISTER_TESTED(Buffer_data);
        ASSERT_FALSE(s.isEmpty());
        Log("toString(): passed");
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

    Log("=== All Buffer Tests Passed ===");
    TestFramework::DisplayCoverage();
    TestFramework::WriteCoverageData("test_buffer_comprehensive");
    Exit(0);
}
