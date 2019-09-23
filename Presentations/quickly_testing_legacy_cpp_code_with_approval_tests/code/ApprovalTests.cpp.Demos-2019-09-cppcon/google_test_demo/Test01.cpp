#include <gtest/gtest.h>

// Google-only test
TEST( Test01, SumsAreCalculated )
{
    EXPECT_EQ( 1 + 1, 2 );
    EXPECT_EQ( 1 + 2, 3 );
}

/*
 * Things to note here:
 * This is an ordinary GoogleTest test - there is nothing Approvals-related here
 */
