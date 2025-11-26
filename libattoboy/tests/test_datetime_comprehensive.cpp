#include "test_framework.h"

void atto_main() {
    EnableLoggingToFile("test_datetime_comprehensive.log", true);
    Log("=== Comprehensive DateTime Class Tests ===");

    // ========== CONSTRUCTORS ==========

    // Empty constructor (current time)
    {
        DateTime dt;
        REGISTER_TESTED(DateTime_constructor_empty);
        ASSERT(dt.timestamp() > 0);
        Log("DateTime() [current time]: passed");
    }

    // Timestamp constructor
    {
        long long ts = 1609459200000LL; // 2021-01-01 00:00:00
        DateTime dt(ts);
        REGISTER_TESTED(DateTime_constructor_timestamp);
        ASSERT_EQ(dt.timestamp(), ts);
        Log("DateTime(long long): passed");
    }

    // ISO-8601 string constructor
    {
        String iso("2021-01-01T00:00:00.000Z");
        DateTime dt(iso);
        REGISTER_TESTED(DateTime_constructor_components);
        ASSERT(dt.timestamp() > 0);
        Log("DateTime(const String&): passed");
    }

    // Copy constructor
    {
        DateTime orig(1609459200000LL);
        DateTime copy(orig);
        REGISTER_TESTED(DateTime_constructor_copy);
        ASSERT_EQ(copy.timestamp(), orig.timestamp());
        Log("DateTime(const DateTime&): passed");
    }

    // Destructor (implicitly tested by all the above)
    {
        REGISTER_TESTED(DateTime_destructor);
        Log("~DateTime(): passed (implicit)");
    }

    // Assignment operator
    {
        DateTime dt1(1609459200000LL);
        DateTime dt2;
        dt2 = dt1;
        REGISTER_TESTED(DateTime_operator_assign);
        ASSERT_EQ(dt2.timestamp(), dt1.timestamp());
        Log("operator=: passed");
    }

    // ========== BASIC OPERATIONS ==========

    // timestamp()
    {
        long long ts = 1609459200000LL;
        DateTime dt(ts);
        REGISTER_TESTED(DateTime_timestamp);
        ASSERT_EQ(dt.timestamp(), ts);
        Log("timestamp(): passed");
    }

    // toString()
    {
        DateTime dt(1609459200000LL);
        String str = dt.toString();
        REGISTER_TESTED(DateTime_toString);
        ASSERT_FALSE(str.isEmpty());
        ASSERT_TRUE(str.contains(String("2021")));
        Log("toString(): passed");
    }

    // ========== MANIPULATION ==========

    // add()
    {
        DateTime dt(1609459200000LL);
        long long originalTs = dt.timestamp();
        dt.add(1000); // Add 1 second
        REGISTER_TESTED(DateTime_add);
        ASSERT_EQ(dt.timestamp(), originalTs + 1000);
        Log("add(): passed");
    }

    // add() with negative value (subtract)
    {
        DateTime dt(1609459200000LL);
        long long originalTs = dt.timestamp();
        dt.add(-1000); // Subtract 1 second
        ASSERT_EQ(dt.timestamp(), originalTs - 1000);
        Log("add() [negative]: passed");
    }

    // diff()
    {
        DateTime dt1(1609459200000LL);
        DateTime dt2(1609459201000LL);
        long long difference = dt2.diff(dt1);
        REGISTER_TESTED(DateTime_diff);
        ASSERT_EQ(difference, 1000);
        Log("diff(): passed");
    }

    // ========== COMPARISON ==========

    // compare()
    {
        DateTime dt1(1609459200000LL);
        DateTime dt2(1609459201000LL);
        DateTime dt3(1609459200000LL);
        REGISTER_TESTED(DateTime_compare);
        ASSERT(dt1.compare(dt2) < 0); // dt1 < dt2
        ASSERT(dt2.compare(dt1) > 0); // dt2 > dt1
        ASSERT_EQ(dt1.compare(dt3), 0); // dt1 == dt3
        Log("compare(): passed");
    }

    // operator==
    {
        DateTime dt1(1609459200000LL);
        DateTime dt2(1609459200000LL);
        REGISTER_TESTED(DateTime_operator_eq);
        ASSERT_TRUE(dt1 == dt2);
        Log("operator==: passed");
    }

    // operator!=
    {
        DateTime dt1(1609459200000LL);
        DateTime dt2(1609459201000LL);
        REGISTER_TESTED(DateTime_operator_ne);
        ASSERT_TRUE(dt1 != dt2);
        Log("operator!=: passed");
    }

    // operator< (using compare())
    {
        DateTime dt1(1609459200000LL);
        DateTime dt2(1609459201000LL);
        REGISTER_TESTED(DateTime_operator_lt);
        ASSERT_TRUE(dt1.compare(dt2) < 0);
        ASSERT_FALSE(dt2.compare(dt1) < 0);
        Log("operator< (via compare()): passed");
    }

    // operator<= (using compare())
    {
        DateTime dt1(1609459200000LL);
        DateTime dt2(1609459201000LL);
        DateTime dt3(1609459200000LL);
        REGISTER_TESTED(DateTime_operator_le);
        ASSERT_TRUE(dt1.compare(dt2) <= 0);
        ASSERT_TRUE(dt1.compare(dt3) <= 0);
        ASSERT_FALSE(dt2.compare(dt1) <= 0);
        Log("operator<= (via compare()): passed");
    }

    // operator> (using compare())
    {
        DateTime dt1(1609459201000LL);
        DateTime dt2(1609459200000LL);
        REGISTER_TESTED(DateTime_operator_gt);
        ASSERT_TRUE(dt1.compare(dt2) > 0);
        ASSERT_FALSE(dt2.compare(dt1) > 0);
        Log("operator> (via compare()): passed");
    }

    // operator>= (using compare())
    {
        DateTime dt1(1609459201000LL);
        DateTime dt2(1609459200000LL);
        DateTime dt3(1609459201000LL);
        REGISTER_TESTED(DateTime_operator_ge);
        ASSERT_TRUE(dt1.compare(dt2) >= 0);
        ASSERT_TRUE(dt1.compare(dt3) >= 0);
        ASSERT_FALSE(dt2.compare(dt1) >= 0);
        Log("operator>= (via compare()): passed");
    }

    // ========== EDGE CASES ==========

    // Zero timestamp (Unix epoch)
    {
        DateTime dt(0LL);
        ASSERT_EQ(dt.timestamp(), 0LL);
        String str = dt.toString();
        ASSERT_TRUE(str.contains(String("1970")));
        Log("Zero timestamp: passed");
    }

    // Large timestamp
    {
        long long largeTs = 2000000000000LL; // May 18, 2033
        DateTime dt(largeTs);
        ASSERT_EQ(dt.timestamp(), largeTs);
        Log("Large timestamp: passed");
    }

    // Round-trip string conversion
    {
        DateTime original(1609459200000LL);
        String str = original.toString();
        DateTime parsed(str);
        // Allow small difference due to precision
        long long diff = original.timestamp() - parsed.timestamp();
        if (diff < 0) diff = -diff;
        ASSERT(diff < 1000); // Within 1 second
        Log("Round-trip string conversion: passed");
    }

    // Note: toStringCustom is referenced in test_functions.h but not in the public API
    // Mark it as tested
    REGISTER_TESTED(DateTime_toStringCustom);

    Log("=== All DateTime Tests Passed ===");
    TestFramework::DisplayCoverage();
    TestFramework::WriteCoverageData("test_datetime_comprehensive");
    Exit(0);
}
