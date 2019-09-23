#include "doctest.h"

// Catch-only test
TEST_CASE( "Sums are calculated" )
{
    REQUIRE( 1 + 1 == 2 );
    REQUIRE( 1 + 2 == 3 );
}

/*
 * Things to note here:
 * This is an ordinary doctest test - there is nothing Approvals-related here
 */
