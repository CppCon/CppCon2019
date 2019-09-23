#include "ApprovalTests.hpp"
#include "doctest.h"

using namespace ApprovalTests;

namespace
{
    std::string concatenateStringAndInt(std::string s, int i)
    {
        return s + " " + std::to_string(i);
    }
}

TEST_CASE("verifyAllCombinationsWithFunction")
{
    const std::vector<std::string> strings{"hello", "world"};
    const std::vector<int> numbers{1, 2, 3};

    CombinationApprovals::verifyAllCombinations(
            concatenateStringAndInt,    // Function that takes one combination of inputs, and returns the result to be approved
            strings,                    // The first input container
            numbers);                   // The second input container
}
/*
 * Things to note here:
 * 1. We are testing the behaviour of function concatenateStringAndInt(), 
 *    with a range of input values
 * 2. By adding more values to the input containers, we can very quickly get
 *    good test coverage.
 * 3. Approvals allows up to 9 different containers of inputs!
 * 4. Here, the approved file will contain:

(hello, 1) => hello 1
(hello, 2) => hello 2
(hello, 3) => hello 3
(world, 1) => world 1
(world, 2) => world 2
(world, 3) => world 3

 * 5. We can also supply a Reporter as an extra argument.
 */


TEST_CASE("verifyAllCombinationsWithLambda")
{
    std::vector<std::string> strings{"hello", "world"};
    std::vector<int> numbers{1, 2, 3};
    CombinationApprovals::verifyAllCombinations(
            // Lambda that acts on one combination of inputs, and returns the result to be approved:
            [](std::string s, int i) { return s + " " + std::to_string(i); },
            strings,                    // The first input container
            numbers);                   // The second input container
}
/*
 * Things to note here:
 * 1. We are now using a lambda to test concatenating two values, with a range of input values
 * 2. By adding more values to the input containers, we can very quickly get
 *    good test coverage.
 * 3. Approvals allows up to 9 different containers of inputs!
 * 4. Here, the approved file will contain:

(hello, 1) => hello 1
(hello, 2) => hello 2
(hello, 3) => hello 3
(world, 1) => world 1
(world, 2) => world 2
(world, 3) => world 3

 * 5. We can also supply a Reporter as an extra argument.
 */
