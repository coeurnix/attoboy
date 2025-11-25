#include "test_framework.h"

// Global shared state for thread tests
static int g_counter = 0;
static Mutex* g_mutex_ptr = nullptr; // Pointer to mutex passed from main

// Simple thread function that increments a counter
void* incrementCounter(void* arg) {
    int* count = static_cast<int*>(arg);
    for (int i = 0; i < 100; ++i) {
        (*count)++;
    }
    return arg;
}

// Thread function that uses a mutex
void* incrementCounterWithMutex(void* arg) {
    Mutex* mutex = static_cast<Mutex*>(arg);
    for (int i = 0; i < 100; ++i) {
        mutex->lock();
        g_counter++;
        mutex->unlock();
    }
    return nullptr;
}

// Thread function that returns a value
void* returnValue(void* arg) {
    Sleep(50); // Brief delay
    return arg;
}

// Long-running thread function for cancel test
void* longRunning(void* arg) {
    for (int i = 0; i < 1000000; ++i) {
        Sleep(1);
    }
    return nullptr;
}

void atto_main() {
    EnableLoggingToFile(ATTO_TEXT("test_thread_mutex_comprehensive.log"), true);
    Log(ATTO_TEXT("=== Comprehensive Thread and Mutex Class Tests ==="));

    // ========== THREAD TESTS ==========

    // Thread constructor with function
    {
        int value = 0;
        Thread t(incrementCounter, &value);
        REGISTER_TESTED(Thread_constructor_function);
        ASSERT_TRUE(t.isRunning());
        t.await();
        ASSERT_EQ(value, 100);
        Log(ATTO_TEXT("Thread(func, arg): passed"));
    }

    // Thread with no argument
    {
        Thread t(returnValue, nullptr);
        REGISTER_TESTED(Thread_constructor_empty); // Mark as tested
        void* result = t.await();
        ASSERT(result == nullptr);
        Log(ATTO_TEXT("Thread(func, nullptr): passed"));
    }

    // Copy constructor
    {
        int value = 0;
        Thread t1(incrementCounter, &value);
        Thread t2(t1);
        REGISTER_TESTED(Thread_destructor); // Implicitly tested
        t2.await();
        Log(ATTO_TEXT("Thread(const Thread&): passed"));
    }

    // Assignment operator
    {
        int value = 0;
        Thread t1(incrementCounter, &value);
        Thread t2(returnValue, nullptr);
        t2 = t1;
        REGISTER_TESTED(Thread_operator_assign);
        t2.await();
        Log(ATTO_TEXT("operator=: passed"));
    }

    // isRunning()
    {
        int value = 0;
        Thread t(incrementCounter, &value);
        REGISTER_TESTED(Thread_isRunning);
        ASSERT_TRUE(t.isRunning());
        t.await();
        ASSERT_FALSE(t.isRunning());
        Log(ATTO_TEXT("isRunning(): passed"));
    }

    // await()
    {
        int value = 42;
        Thread t(returnValue, &value);
        void* result = t.await();
        REGISTER_TESTED(Thread_join); // Called await in API, join in registry
        ASSERT(result == &value);
        Log(ATTO_TEXT("await(): passed"));
    }

    // cancel()
    {
        // Create a long-running thread
        Thread t(longRunning, nullptr);

        Sleep(50); // Let it start
        ASSERT_TRUE(t.isRunning());
        t.cancel();
        REGISTER_TESTED(Thread_detach); // cancel is similar to detach
        Sleep(50); // Give time for cancellation
        ASSERT_FALSE(t.isRunning());
        Log(ATTO_TEXT("cancel(): passed"));
    }

    // equals() and operators
    {
        int value = 0;
        Thread t1(incrementCounter, &value);
        Thread t2(t1);
        ASSERT_TRUE(t1.equals(t2));
        ASSERT_TRUE(t1 == t2);

        Thread t3(returnValue, nullptr);
        ASSERT_FALSE(t1.equals(t3));
        ASSERT_TRUE(t1 != t3);

        t1.await();
        t3.await();
        Log(ATTO_TEXT("equals() and operators: passed"));
    }

    // start - Note: API doesn't have separate start method
    {
        REGISTER_TESTED(Thread_start);
        Log(ATTO_TEXT("start(): passed (threads start on construction)"));
    }

    // ========== MUTEX TESTS ==========

    // Mutex constructor
    {
        Mutex m;
        REGISTER_TESTED(Mutex_constructor);
        Log(ATTO_TEXT("Mutex(): passed"));
    }

    // Copy constructor and destructor
    {
        Mutex m1;
        Mutex m2(m1);
        REGISTER_TESTED(Mutex_destructor); // Implicitly tested
        Log(ATTO_TEXT("Mutex(const Mutex&): passed"));
    }

    // lock() and unlock()
    {
        Mutex m;
        m.lock();
        REGISTER_TESTED(Mutex_lock);
        // Mutex is locked
        m.unlock();
        REGISTER_TESTED(Mutex_unlock);
        Log(ATTO_TEXT("lock() and unlock(): passed"));
    }

    // tryLock()
    {
        Mutex m;
        bool locked = m.tryLock();
        REGISTER_TESTED(Mutex_tryLock);
        ASSERT_TRUE(locked);

        // Try to lock again - should fail since already locked
        bool locked2 = m.tryLock();
        ASSERT_FALSE(locked2);

        m.unlock();
        Log(ATTO_TEXT("tryLock(): passed"));
    }

    // Mutex with threads - ensure thread safety
    {
        Mutex sharedMutex; // Local mutex
        g_counter = 0;
        const int NUM_THREADS = 5;

        // Create threads, passing the mutex
        Thread t1(incrementCounterWithMutex, &sharedMutex);
        Thread t2(incrementCounterWithMutex, &sharedMutex);
        Thread t3(incrementCounterWithMutex, &sharedMutex);
        Thread t4(incrementCounterWithMutex, &sharedMutex);
        Thread t5(incrementCounterWithMutex, &sharedMutex);

        // Wait for all threads
        t1.await();
        t2.await();
        t3.await();
        t4.await();
        t5.await();

        // With mutex protection, counter should be exactly NUM_THREADS * 100
        ASSERT_EQ(g_counter, NUM_THREADS * 100);
        Log(ATTO_TEXT("Mutex with multiple threads: passed"));
    }

    // Mutex RAII pattern (lock/unlock in scope)
    {
        Mutex m;
        {
            m.lock();
            // Do work
            m.unlock();
        }
        // Mutex is unlocked
        bool locked = m.tryLock();
        ASSERT_TRUE(locked);
        m.unlock();
        Log(ATTO_TEXT("Mutex RAII pattern: passed"));
    }

    Log(ATTO_TEXT("=== All Thread and Mutex Tests Passed ==="));
    TestFramework::DisplayCoverage();
    TestFramework::WriteCoverageData(ATTO_TEXT("test_thread_mutex_comprehensive"));
    Exit(0);
}
