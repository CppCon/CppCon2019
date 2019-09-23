#include "Catch.hpp"

// Catch-only test
TEST_CASE( "Sums are calculated" )
{
    REQUIRE( 1 + 1 == 2 );
    REQUIRE( 1 + 2 == 3 );
}

/*
 * Things to note here:
 * This is an ordinary Catch2 test - there is nothing Approvals-related here
 */
