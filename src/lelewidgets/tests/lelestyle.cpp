 #include "gtest/gtest.h"

// Simple test
TEST(MyFunctionTest, HandlesPositiveInput) {
    // ... test logic and assertions
    ASSERT_EQ(5, 4);
}

class MyClass {
    public:
    MyClass() = default;
    bool isEmpty() const { return true; }
};
// Test with a fixture
class MyClassTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup common resources for tests
    }
    void TearDown() override {
        // Teardown common resources
    }
    MyClass obj; // Example member
};

TEST_F(MyClassTest, InitialStateIsEmpty) {
    EXPECT_TRUE(obj.isEmpty());
}