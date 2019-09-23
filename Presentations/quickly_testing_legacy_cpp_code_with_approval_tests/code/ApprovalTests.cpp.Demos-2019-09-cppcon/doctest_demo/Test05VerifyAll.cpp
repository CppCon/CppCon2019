#include "ApprovalTests.hpp"
#include "doctest.h"

#include <list>

using namespace ApprovalTests;

TEST_CASE("verifyAllWithVector")
{
    std::vector<int> numbers{ 0, 1, 2, 3};
    Approvals::verifyAll(
        numbers);
}
/*
 * Things to note here:
 * 1. Typically, we are testing the values that have been put
 *    into the container.
 * 2. The approved file will contain:

[0] = 0
[1] = 1
[2] = 2
[3] = 3
 * 3. We can also supply a Reporter as an extra argument.
 */


TEST_CASE("verifyAllWithHeaderAndVector")
{
    std::vector<int> numbers{ 0, 1, 2, 3};
    Approvals::verifyAll(
        "Some descriptive text, to help understand the output",
        numbers,
        DiffReporter{});
}
/*
 * Things to note here:
 * 
 * 1. Typically, we are testing the values that have been put
 *    into the container.
 * 2. The approved file will contain:

Some descriptive text, to help understand the output


[0] = 0
[1] = 1
[2] = 2
[3] = 3

 * 3. The descriptive text is useful when, during later maintenance, 
 *    the received output changes: if the text desribes the intention
 *    of the test, it can help make it clear whether the new output
 *    is now better, or is now broken.
 * 4. We can also supply a Reporter as an extra argument.
 */


TEST_CASE("verifyAllWithHeaderContainerAndLambda")
{
    std::deque<int> numbers{ 0, 1, 2, 3};
    Approvals::verifyAll(
        "Negate Values",
        numbers,
        [](int value, std::ostream& stream)
        {
            stream << value << " => " << -value;
        },
        DiffReporter{});
}
/*
 * Things to note here:
 * 
 * 1. Now the values container, which can be of any type, contains
 *    inputs to the lambda.
 * 2. The lambda will typically write the input value, do an operation
 *    on the input value, and then write the result of the operation.
 * 3. Here, the approved file will contain:

Negate Values


0 => 0
1 => -1
2 => -2
3 => -3
 * 4. We can also supply a Reporter as an extra argument.
 */

TEST_CASE("verifyAllWithHeaderBeginEndAndLambda")
{
    std::list<int> numbers{ 0, 1, 2, 3};
    Approvals::verifyAll(
        "Test Squares",
        numbers.begin(), numbers.end(),
        [](int value, std::ostream& stream)
        {
            stream << value << " => " << value * value;
        },
        DiffReporter{});
}
/*
 * Things to note here:
 * 
 * 1. The pair of iterators specifies the inputs to the lambda.
 * 2. The lambda will typically write the input value, do an operation
 *    on the input value, and then write the result of the operation.
 * 3. Here, the approved file will contain:

Test Squares


0 => 0
1 => 1
2 => 4
3 => 9
 * 4. We can also supply a Reporter as an extra argument.
 */